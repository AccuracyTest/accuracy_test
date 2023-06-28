#include "PhoxiControl.h"
#define LOCAL_CROSS_SLEEP(Millis) usleep(Millis * 1000)
#define DELIMITER "/"

PhoxiControl::PhoxiControl()
{

}

PhoxiControl::~PhoxiControl()
{

}

void PhoxiControl::GetAvailableDevice()
{
    // Wait for the PhoxiControl
    while (!Factory.isPhoXiControlRunning())
    {
        LOCAL_CROSS_SLEEP(100);
    }

    DeviceList = Factory.GetDeviceList();
}

int PhoxiControl::ConnectPhoXiDevice(const std::string &HardwareIdentification)
{
    pho::api::PhoXiTimeout Timeout = pho::api::PhoXiTimeout::ZeroTimeout;
    PhoXiDevice = Factory.CreateAndConnect(HardwareIdentification, Timeout);
    if (PhoXiDevice)
    {
        std::cout << "Connection to the device " << HardwareIdentification << " was Successful!" << std::endl;
        SetCalibrationSettings();
        SetCameraMatrix();
        SetDistortionCoef();
        return 0;
    }
    else
    {
        std::cout << "Connection to the device " << HardwareIdentification << " was Unsuccessful!" << std::endl;
        return -1;
    }
}

int PhoxiControl::DisconnectPhoXiDevice()
{
    if (PhoXiDevice->isConnected())
    {
        if(!PhoXiDevice->StopAcquisition()){
            return -1;
        }
        if(PhoXiDevice->Disconnect()){
            return 0;
        } else {
            return -2;
        }

    } else {
        std::cout << "No device is connected" << std::endl;
        return -3;
    }

}

void PhoxiControl::SoftwareTrigger()
{
    if (!PhoXiDevice || !PhoXiDevice->isConnected())
    {
        std::cout << "Device is not created, or not connected!" << std::endl;
        return;
    }

    //If it is not in Software trigger mode, we need to switch the modes
    if (PhoXiDevice->TriggerMode != pho::api::PhoXiTriggerMode::Software)
    {
        std::cout << "Device is not in Software trigger mode" << std::endl;
        if (PhoXiDevice->isAcquiring())
        {
            std::cout << "Stopping acquisition" << std::endl;
            //If the device is in Acquisition mode, we need to stop the acquisition
            if (!PhoXiDevice->StopAcquisition())
            {
                throw std::runtime_error("Error in StopAcquistion");
            }
        }

        std::cout << "Switching to Software trigger mode " << std::endl;

        //Switching the mode is as easy as assigning of a value, it will call the appropriate calls in the background
        PhoXiDevice->TriggerMode = pho::api::PhoXiTriggerMode::Software;

        //Just check if did everything run smoothly
        if (!PhoXiDevice->TriggerMode.isLastOperationSuccessful())
        {
            throw std::runtime_error(PhoXiDevice->TriggerMode.GetLastErrorMessage().c_str());
        }
    }

    //Start the device acquisition, if necessary
    if (!PhoXiDevice->isAcquiring())
    {
        if (!PhoXiDevice->StartAcquisition())
        {
            throw std::runtime_error("Error in StartAcquisition");
        }
    }

    //We can clear the current Acquisition buffer -- This will not clear Frames that arrives to the PC after the Clear command is performed
        int ClearedFrames = PhoXiDevice->ClearBuffer();
        std::cout << ClearedFrames << " frames were cleared from the cyclic buffer" << std::endl;

        //While we checked the state of the StartAcquisition call, this check is not necessary, but it is a good practice
        if (!PhoXiDevice->isAcquiring())
        {
            std::cout << "Device is not acquiring" << std::endl;
            return;
        }

        for (std::size_t i = 0; i < 1; ++i)
        {
            std::cout << "Triggering the " << i << "-th frame" << std::endl;
            int FrameID = PhoXiDevice->TriggerFrame(/*If false is passed here, the device will reject the frame if it is not ready to be triggered, if true us supplied, it will wait for the trigger*/);
            if (FrameID < 0)
            {
                //If negative number is returned trigger was unsuccessful
                std::cout << "Trigger was unsuccessful!" << std::endl;
                continue;
            }
            else
            {
                std::cout << "Frame was triggered, Frame Id: " << FrameID << std::endl;
            }

            std::cout << "Waiting for frame " << i << std::endl;


            //Wait for a frame with specific FrameID. There is a possibility, that frame triggered before the trigger will arrive after the trigger call, and will be retrieved before requested frame
            //  Because of this, the TriggerFrame call returns the requested frame ID, so it can than be retrieved from the Frame structure. This call is doing that internally in background
            pho::api::PFrame Frame = PhoXiDevice->GetSpecificFrame(FrameID/*, You can specify Timeout here - default is the Timeout stored in Timeout Feature -> Infinity by default*/);
            if (Frame)
            {
                SampleFrame = Frame;

                std::cout << "Arrived Frame Resolution: "
                    << Frame->GetResolution().Width << " x "
                    << Frame->GetResolution().Height << std::endl;
            }
            else
            {
                std::cout << "Failed to retrieve the frame!";
            }
        }
}

bool PhoxiControl::GetConnectionStatus()
{
    if (PhoXiDevice){
        return PhoXiDevice->isConnected();
    } else {
        std::cout << "no PhoxiDevice created" << std::endl;
        return false;
    }

}

pho::api::PFrame PhoxiControl::GetSampleFrame()
{
    return SampleFrame;
}

std::vector<pho::api::PhoXiDeviceInformation> PhoxiControl::GetDeviceList()
{
    std::cout << "GetDeviceList" << std::endl;
    return DeviceList;
}

pho::api::CameraMatrix64f PhoxiControl::GetCameraMatrix()
{
    return CameraMatrix;
}

std::vector<double> PhoxiControl::GetDistortionCoef()
{
    return DistortionCoef;
}

void PhoxiControl::SetCalibrationSettings()
{
    if (PhoXiDevice){
        CalibrationSettings = PhoXiDevice->CalibrationSettings;
    } else {
        std::cout << "no PhoxiDevice created" << std::endl;
        return;
    }
}

void PhoxiControl::SetCameraMatrix()
{
    if (!CalibrationSettings.CameraMatrix.Empty()){
        CameraMatrix = CalibrationSettings.CameraMatrix;
    } else {
        std::cout << "empty Calibration Settings" << std::endl;
        return;
    }
}

void PhoxiControl::SetDistortionCoef()
{
    if (!CalibrationSettings.DistortionCoefficients.empty()){
        DistortionCoef = CalibrationSettings.DistortionCoefficients;
    } else {
        std::cout << "empty Calibration Settings" << std::endl;
    }
}

