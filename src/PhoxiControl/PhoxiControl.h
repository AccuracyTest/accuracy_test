#ifndef PHOXICONTROL_H
#define PHOXICONTROL_H
#include <vector>
#include <string>
#include <iostream>
#include <atomic>
#include <unistd.h>


// PhoXi Control
#include "PhoXi.h"

class PhoxiControl{
public:
    PhoxiControl();
    ~PhoxiControl();

    void GetAvailableDevice();

    int ConnectPhoXiDevice(const std::string &HardwareIdentification);

    int DisconnectPhoXiDevice();

    void SoftwareTrigger();

    bool GetConnectionStatus();

    pho::api::PFrame GetSampleFrame();

    std::vector <pho::api::PhoXiDeviceInformation> GetDeviceList();

    pho::api::CameraMatrix64f GetCameraMatrix();

    std::vector<double> GetDistortionCoef();

private:
    void SetCalibrationSettings();

    void SetCameraMatrix();

    void SetDistortionCoef();

private:
    pho::api::PhoXiFactory Factory;
    pho::api::PPhoXi PhoXiDevice;
    pho::api::PFrame SampleFrame;
    pho::api::PhoXiCalibrationSettings CalibrationSettings;
    pho::api::CameraMatrix64f CameraMatrix;
    std::vector<double> DistortionCoef;

    std::vector <pho::api::PhoXiDeviceInformation> DeviceList;
    std::string FileCameraFolder = "";
    std::string OutputFolder = "";

};


#endif // PHOXICONTROL_H
