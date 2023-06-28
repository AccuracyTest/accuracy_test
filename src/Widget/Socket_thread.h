#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <QThread>
#include <QDebug>
#include <QMutex>
#include "SocketServer.h"

class Socket_thread : public QThread
{
    Q_OBJECT
public:
    explicit Socket_thread(QObject *parent = nullptr);
    ~Socket_thread();
public slots:
    void slotisAutoMode(bool status);
    void slotisMeasurementFinish(bool status);

//public:
//    void set_isMeasurementFinish(bool status);
//    void set_isAutoMode (bool status);

private:
    void run() override;

signals:
    void sig_scanning();
    void sig_detecting();
    void sig_save_data();
    void sig_invalid_recv();

private:
    SocketServer *m_sockerserver_;
    bool isMeasurementFinish {false};
    bool isAutoMode {false};
    QMutex m_mutex_;

};


#endif // SOCKET_THREAD_H
