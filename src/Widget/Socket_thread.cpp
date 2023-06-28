#include "Socket_thread.h"

Socket_thread::Socket_thread(QObject *parent) : QThread(parent)
{
    m_sockerserver_ = new SocketServer;
    m_sockerserver_->Start();
}

Socket_thread::~Socket_thread()
{
    delete m_sockerserver_;
}

void Socket_thread::slotisAutoMode(bool status)
{
    qDebug() << "slotisAutoMode";
    m_mutex_.lock();
    isAutoMode = status;
    m_mutex_.unlock();
    qDebug() << "isAutoMode: " << isAutoMode;
}

void Socket_thread::slotisMeasurementFinish(bool status)
{
    qDebug() << "slotisMeasurementFinish";
    m_mutex_.lock();
    isMeasurementFinish = status;
    m_mutex_.unlock();
    qDebug() << "isMeasurementFinish: " << isMeasurementFinish;
}

void Socket_thread::run()
{
    qDebug() << "auto mode start";

    while (isAutoMode) {
        m_mutex_.lock();
        isMeasurementFinish = false;
        m_mutex_.unlock();
        m_sockerserver_->Accept();
        m_sockerserver_->Recv();
        std::string recv = m_sockerserver_->Get_Server_Recv();
        if (recv.empty()){
            qDebug() << "recv is empty";
        }else{
            if (recv == "ready"){
                qDebug() << "ready for measure";
                emit sig_scanning();
                emit sig_detecting();
                emit sig_save_data();
                m_sockerserver_->Set_Server_Recv("");
            } else {
                qDebug() << "invalid recv";
                emit sig_invalid_recv();
                emit sig_save_data();
                m_sockerserver_->Set_Server_Recv("");
            }
            while(!isMeasurementFinish){
                qDebug() << "thread isMeasurementFinish: " << isMeasurementFinish;
                usleep(500);
            }
            m_sockerserver_->Send();
            m_mutex_.lock();
            isMeasurementFinish = false;
            m_mutex_.unlock();
        }
    }
}
