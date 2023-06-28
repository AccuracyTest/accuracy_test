#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <string>
#include "PhoxiControl.h"
#include "DataProcess.h"
#include "SocketServer.h"
#include "Socket_thread.h"

namespace Ui {
class Widget;
}


class Widget : public QWidget{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    bool get_isMeasurementFinish();


signals:
    void sigisAutoMode(bool status);
    void sigisMeasurementFinish(bool status);

private:
    void connect_device();

    void saveData(const QByteArray &data, const QString filename);


private slots:
    void on_pbn_refresh_clicked();
    void on_pbn_connect_clicked();
    void on_pbn_disconnect_clicked();
    void on_pbn_scanning_clicked();
    void on_pbn_detect_clicked();
    void on_pbn_save_data_clicked();
    void on_pbn_start_auto_mode_clicked();
    void on_pbn_stop_auto_mode_clicked();
    void slt_invalid_recv();

private:
    bool isMeasurementFinish {false};
    QString _vector_file_name;
    QString _euler_file_name;
    Ui::Widget *ui;
    PhoxiControl *m_phoxicontrol_;
    DataProcess *m_dataprocess_;
    Socket_thread *m_socket_thread_;
};

#endif // WIDGET_H
