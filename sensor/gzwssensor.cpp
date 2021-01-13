#include "gzwssensor.h"
#include <QDebug>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

GzwsSensor::GzwsSensor()
{
}

void GzwsSensor::test()
{
    puts("");
    while (1) {
        qreal* res = this->getTempAndRh();
        printf("\r%.1fC %.1f%% %ulux", res[0], res[1], this->getLux());
        sleep(1);
        fflush(stdout);
    }
}

bool GzwsSensor::init()
{
    fd = open(this->dev.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        qDebug() << "Can't Open Serial Port";
        exit(1);
    }
    fcntl(fd, F_SETFL, 0);
    //判断串口的状态是否为阻塞状态
    //    if (fcntl(fd, F_SETFL, 0) < 0) {
    //        printf("fcntl failed!\n");
    //    } else {
    //        printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
    //    }
    //测试是否为终端设备
    //    if (0 == isatty(STDIN_FILENO)) {
    //        printf("standard input is not a terminal device\n");
    //    } else {
    //        printf("isatty success!\n");
    //    }

    termios& options = *(new termios);
    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    //修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;

    options.c_cflag &= ~CRTSCTS;

    options.c_cflag &= ~CSIZE; //屏蔽其他标志位

    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_iflag &= ~INPCK;

    // raw mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    options.c_oflag &= ~OPOST; /*Output*/

    options.c_cflag &= ~CSTOPB;

    options.c_oflag &= ~OPOST;

    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("com set error!\n");
    }

    inited = true;

    return true;
}

void GzwsSensor::startRefresh()
{
    if (!inited)
        init();
    connect(&this->timer, &QTimer::timeout, this, &GzwsSensor::refresh);
    this->timer.start(200);
}

qreal* GzwsSensor::getTempAndRh()
{
    this->locker.lockForRead();
    qreal* ret = new qreal[2] { this->tempAndRh[0], this->tempAndRh[1] };
    this->locker.unlock();
    return ret;
}

qreal* GzwsSensor::getTempAndRhMoc()
{
    this->now = QTime::currentTime();
    return new qreal[2] { (qrand() % 200 + 200) * 0.1, (qrand() % 200 + 200) * 0.1 };
}

quint32 GzwsSensor::getLux()
{
    quint32 lux;
    this->locker.lockForRead();
    lux = this->lux;
    this->locker.unlock();

    return lux;
};

void GzwsSensor::refresh()
{
    // send query code
    write(fd, code_humi_temp_lux, sizeof(code_humi_temp_lux));
    quint8 buf[19];
    for (int i = 0; i < 19; ++i) {
        read(fd, &buf[i], 1);
    }

    // parse temperature
    quint16 temp = buf[3];
    temp <<= 8;
    temp += buf[4];
    qreal rh = static_cast<qint16>(temp) * 1.0 / 10;
    // parse humidity
    temp = buf[5];
    temp <<= 8;
    temp += buf[6];
    qreal t = static_cast<qint16>(temp) * 1.0 / 10;
    // parse lux
    quint32 lux = buf[7];
    lux <<= 8;
    lux += buf[8];
    lux <<= 8;
    lux += buf[9];
    lux <<= 8;
    lux += buf[10];

    QTime now = QTime::currentTime();
    // update data
    this->locker.lockForWrite();
    this->now = now;
    this->tempAndRh[0] = t;
    this->tempAndRh[1] = rh;
    this->lux = lux;
    this->locker.unlock();
}
