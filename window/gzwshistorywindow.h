#ifndef GZWSHISTORY_H
#define GZWSHISTORY_H

#include "db/localdb.h"
#include "window/util/axistag.h"
#include <QDialog>

namespace Ui {
class GzwsHistoryWindow;
}

class GzwsHistoryWindow : public QDialog {
    Q_OBJECT

public:
    explicit GzwsHistoryWindow(QWidget* parent = nullptr);
    ~GzwsHistoryWindow();
public slots:
    /* setters */
    GzwsHistoryWindow& setLocalDb(LocalDb* db) { return this->db = db, *this; }
    void refreshHistoryData();

private slots:

    void on_refreshButton_clicked();

    void on_slider_valueChanged(int value);

    void checkBoxClicked(int value);

private:
    Ui::GzwsHistoryWindow* ui;
    QCustomPlot* mPlot;
    QPointer<QCPGraph> mGraphTemperature;
    QPointer<QCPGraph> mGraphHumidity;
    QPointer<QCPGraph> mGraphLux;
    QSharedPointer<QCPAxisTickerDateTime> timeTicker;
    double xMid;
    double xMaxSize;
    /* db */
    LocalDb* db;
};

#endif // GZWSHISTORY_H
