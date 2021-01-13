#ifndef GZWSHISTORY_H
#define GZWSHISTORY_H

#include <QDialog>

namespace Ui {
class GzwsHistory;
}

class GzwsHistory : public QDialog
{
    Q_OBJECT

public:
    explicit GzwsHistory(QWidget *parent = nullptr);
    ~GzwsHistory();

private:
    Ui::GzwsHistory *ui;
};

#endif // GZWSHISTORY_H
