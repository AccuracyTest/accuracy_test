#include "Widget.h"
#include "ui_Widget.h"
#include <QDebug>
#include <QDateTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_phoxicontrol_ = new PhoxiControl;
    m_dataprocess_ = new DataProcess;
    m_socket_thread_ = new Socket_thread;

    connect(m_socket_thread_, &Socket_thread::sig_scanning, this , &Widget::on_pbn_scanning_clicked);
    connect(m_socket_thread_, &Socket_thread::sig_detecting, this , &Widget::on_pbn_detect_clicked);
    connect(m_socket_thread_, &Socket_thread::sig_save_data, this , &Widget::on_pbn_save_data_clicked);
    connect(m_socket_thread_, &Socket_thread::sig_invalid_recv, this, &Widget::slt_invalid_recv);
    connect(this, SIGNAL(sigisAutoMode(bool)), m_socket_thread_, SLOT(slotisAutoMode(bool)));
    connect(this, SIGNAL(sigisMeasurementFinish(bool)), m_socket_thread_, SLOT(slotisMeasurementFinish(bool)));

    _vector_file_name = QString("Euler_data_%1").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"));
    _euler_file_name = QString("Vec_data_%1").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"));
}

Widget::~Widget()
{
    delete ui;
    delete m_phoxicontrol_;
    delete m_dataprocess_;
    delete m_socket_thread_;
}

bool Widget::get_isMeasurementFinish()
{
    return isMeasurementFinish;
}

void Widget::connect_device()
{
    qDebug() << ui->cmb_device->currentText();
    int ret = 0;
    ret = m_phoxicontrol_->ConnectPhoXiDevice(ui->cmb_device->currentText().toStdString());
    if (ret == 0){
        ui->lbl_status->setText(QString("Connection to the device %1 was Successful!").arg(ui->cmb_device->currentText()));
    } else {
        ui->lbl_status->setText(QString("Connection to the device %1 Failed!").arg(ui->cmb_device->currentText()));
    }
}

void Widget::saveData(const QByteArray &data, const QString filename)
{
    bool exist;
    QString fileName;
    QDir *folder = new QDir;
    exist = folder->exists("/home/kenny/data");

    if (!exist){
        bool ok = folder->mkdir("/home/kenny/data");
        if (ok) {
            qDebug() << "directory created";
        } else {
            qDebug() << "directory creation failed";
        }
    }
    fileName = tr("/home/kenny/data/%1.txt").arg(filename);

    QFile f(fileName);
    if(!f.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    } else {
        f.write(data);
        qDebug() << "Data Saved to " << fileName;
        ui->lbl_status->setText(QString("Data Saved to %1").arg(filename));
    }
        f.close();
}

void Widget::on_pbn_refresh_clicked()
{
    m_phoxicontrol_->GetAvailableDevice();
    QStringList devicelist;
    for (std::size_t i = 0; i < m_phoxicontrol_->GetDeviceList().size(); ++i)
    {
        devicelist << QString(m_phoxicontrol_->GetDeviceList().at(i).HWIdentification.data());
    }
    ui->cmb_device->addItems(devicelist);
}

void Widget::on_pbn_connect_clicked()
{

    if (!m_phoxicontrol_->GetConnectionStatus())
    {
//        qDebug() << "Start Connection";
        connect_device();
    } else {
//        qDebug() << "Device already connected";
        ui->lbl_status->setText("Device already connected");
    }
}

void Widget::on_pbn_disconnect_clicked()
{
    int ret = 0;
    if(m_phoxicontrol_->GetConnectionStatus())
    {
        ret = m_phoxicontrol_->DisconnectPhoXiDevice();
        if (ret == 0) {
            ui->lbl_status->setText("Device is disconnected");
            return;
        } else if ( ret == -1){
            ui->lbl_status->setText("StopAcquisition Failed");
        } else if (ret == -2){
            ui->lbl_status->setText("Disconnect PhoXiDevice Failed");
        }
    } else {
        qDebug() << "No Device connected";
        ui->lbl_status->setText("No Device connected");
        return;
    }
}

void Widget::on_pbn_scanning_clicked()
{
    isMeasurementFinish = false;
    emit sigisMeasurementFinish(isMeasurementFinish);
    if (m_phoxicontrol_->GetConnectionStatus())
    {
        m_phoxicontrol_->SoftwareTrigger();
        ui->lbl_status->setText("Trigger Scanning");
        if(!m_phoxicontrol_->GetSampleFrame()->Empty()){
            m_dataprocess_->Set_SampleFrame(m_phoxicontrol_->GetSampleFrame());
            m_dataprocess_->Generate_Gray_Value_Image();
            cv::Mat src = m_dataprocess_->Get_Gray_Value_Image();
            cv::resize(src, src, cv::Size(768, 432));
            cv::Mat temp;
            if(src.channels()==4)
                cv::cvtColor(src,temp,cv::COLOR_BGRA2RGB);
            else if (src.channels()==3)
                cv::cvtColor(src,temp,cv::COLOR_BGR2RGB);
            else
                cv::cvtColor(src,temp,cv::COLOR_GRAY2RGB);
            QImage img = QImage((uchar*)temp.data,temp.cols,temp.rows,temp.step,QImage::Format_RGB888);
            ui->lbl_fig->setPixmap(QPixmap::fromImage(img));
            ui->lbl_fig->resize(ui->lbl_fig->pixmap()->size());
            ui->lbl_status->setText("Scan Finished");
        } else {
//            std::cout << "Empty SampleFrame!!!" << std::endl;
            ui->lbl_status->setText("Empty SampleFrame!!!");
            return;
        }
    } else {
//        qDebug() << "No Device connected";
        ui->lbl_status->setText("No Device connected");
        return;
    }
}

void Widget::on_pbn_detect_clicked()
{
    int ret = m_dataprocess_->Aruco_Detection();

    if (ret == 0){
        cv::Mat src = m_dataprocess_->Get_Aruco_Detection_Image();
        cv::resize(src, src, cv::Size(768, 432));
        cv::Mat temp;
        if(src.channels()==4)
            cv::cvtColor(src,temp,cv::COLOR_BGRA2RGB);
        else if (src.channels()==3)
            cv::cvtColor(src,temp,cv::COLOR_BGR2RGB);
        else
            cv::cvtColor(src,temp,cv::COLOR_GRAY2RGB);
        QImage img = QImage((uchar*)temp.data,temp.cols,temp.rows,temp.step,QImage::Format_RGB888);
        ui->lbl_fig->setPixmap(QPixmap::fromImage(img));
        ui->lbl_fig->resize(ui->lbl_fig->pixmap()->size());

        std::vector<float> Position = m_dataprocess_->Get_Position();
        ui->lbl_x->setText(QString("%1").arg(Position.at(0)));
        ui->lbl_y->setText(QString("%1").arg(Position.at(1)));
        ui->lbl_z->setText(QString("%1").arg(Position.at(2)));

        std::vector<float> Orienrtation = m_dataprocess_->Get_Euler();
        ui->lbl_Rx->setText(QString("%1").arg(Orienrtation.at(0)));
        ui->lbl_Ry->setText(QString("%1").arg(Orienrtation.at(1)));
        ui->lbl_Rz->setText(QString("%1").arg(Orienrtation.at(2)));

        std::vector<float> Vec_X = m_dataprocess_->Get_X_Axis();
        ui->lbl_vec_x->setText(QString("%1").arg(Vec_X.at(0)));
        ui->lbl_vec_y->setText(QString("%1").arg(Vec_X.at(1)));
        ui->lbl_vec_z->setText(QString("%1").arg(Vec_X.at(2)));
    } else {
        ui->lbl_x->setText(QString("%1").arg("NAN"));
        ui->lbl_y->setText(QString("%1").arg("NAN"));
        ui->lbl_z->setText(QString("%1").arg("NAN"));
        ui->lbl_Rx->setText(QString("%1").arg("NAN"));
        ui->lbl_Ry->setText(QString("%1").arg("NAN"));
        ui->lbl_Rz->setText(QString("%1").arg("NAN"));
        ui->lbl_vec_x->setText(QString("%1").arg("NAN"));
        ui->lbl_vec_y->setText(QString("%1").arg("NAN"));
        ui->lbl_vec_z->setText(QString("%1").arg("NAN"));
    }
}

void Widget::on_pbn_save_data_clicked()
{
    QString data_str = QString("Rx: %1, Ry: %2, Rz: %3, x: %4, y: %5, z: %6\n")
            .arg(ui->lbl_Rx->text(), ui->lbl_Ry->text(), ui->lbl_Rz->text(),
                 ui->lbl_x->text(), ui->lbl_y->text(), ui->lbl_z->text());

    QByteArray bytes = data_str.toLatin1();
    saveData(bytes, _euler_file_name);

    QString data_vec = QString("x: %1, y: %2, z: %3\n").arg(ui->lbl_vec_x->text(), ui->lbl_vec_y->text(), ui->lbl_vec_z->text());
    QByteArray bytes_vec = data_vec.toLatin1();
    saveData(bytes_vec, _vector_file_name);
    isMeasurementFinish = true;
    emit sigisMeasurementFinish(isMeasurementFinish);
}

void Widget::on_pbn_start_auto_mode_clicked()
{
//    m_socket_thread_->set_isAutoMode(true);
    emit sigisAutoMode(true);
    emit sigisMeasurementFinish(false);
    m_socket_thread_->start();
}

void Widget::on_pbn_stop_auto_mode_clicked()
{
//    m_socket_thread_->set_isAutoMode(false);
    emit sigisAutoMode(false);
    m_socket_thread_->exit();
}

void Widget::slt_invalid_recv()
{
    ui->lbl_x->setText(QString("%1").arg("invalid"));
    ui->lbl_y->setText(QString("%1").arg("invalid"));
    ui->lbl_z->setText(QString("%1").arg("invalid"));
    ui->lbl_Rx->setText(QString("%1").arg("invalid"));
    ui->lbl_Ry->setText(QString("%1").arg("invalid"));
    ui->lbl_Rz->setText(QString("%1").arg("invalid"));
    ui->lbl_vec_x->setText(QString("%1").arg("invalid"));
    ui->lbl_vec_y->setText(QString("%1").arg("invalid"));
    ui->lbl_vec_z->setText(QString("%1").arg("invalid"));
}
