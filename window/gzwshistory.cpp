#include "gzwshistory.h"
#include "ui_gzwshistory.h"

GzwsHistory::GzwsHistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GzwsHistory)
{
    ui->setupUi(this);
}

GzwsHistory::~GzwsHistory()
{
    delete ui;
}
