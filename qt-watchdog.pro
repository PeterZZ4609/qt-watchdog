INCLUDEPATH += /opt/opencv4.5.1/include/opencv4
LIBS += \
-L/opt/opencv4.5.1/lib \
-lopencv_core \
-lopencv_imgcodecs \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_flann \
-lopencv_video \
-lopencv_videoio \
-lopencv_dnn

QT       += core gui sql quick qml quickcontrols2 quickwidgets virtualkeyboard
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
static {
    QTPLUGIN += qtvirtualkeyboardplugin
    QT += svg
}
CONFIG += c++11
#CONFIG += diable-desktop lang-zh_CN

#disable-desktop|android-embedded|!isEmpty(CROSS_COMPILE)|qnx {
#    DEFINES += MAIN_QML=\\\"basic-b2qt.qml\\\" # 采用了自定义的虚拟键盘的InputPanel，可以依附窗口。
#} else {
#    DEFINES += MAIN_QML=\\\"Basic.qml\\\"       # 独立于窗口
#}
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    db/clouddb.cpp \
    db/localdb.cpp \
    main.cpp \
    mainwindow.cpp \
    sensor/gzwssensor.cpp \
    window/gzwshistorywindow.cpp \
    window/util/axistag.cpp \
    window/util/customtabstyle.cpp \
    window/util/qcustomplot.cpp

HEADERS += \
    db/clouddb.h \
    db/localdb.h \
    mainwindow.h \
    sensor/gzwssensor.h \
    window/gzwshistorywindow.h \
    window/util/axistag.h \
    window/util/customtabstyle.h \
    window/util/qcustomplot.h

FORMS += \
    mainwindow.ui \
    window/gzwshistorywindow.ui \
    window/logwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /root/sdcard
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/main.qrc \
    resources/qss/qss.qrc

