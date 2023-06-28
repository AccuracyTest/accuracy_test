#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <atomic>
#include <unistd.h>

// OpenCv
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/aruco.hpp"

// Eigen
#include "eigen3/Eigen/Dense"
// Phoxi
#include "PhoXi.h"

struct Vector3D
{
    float x = 0;
    float y = 0;
    float z = 0;
};

using namespace cv;

class DataProcess
{
public:
    DataProcess();
    ~DataProcess();

    void Set_SampleFrame(const pho::api::PFrame Frame);

    void Generate_Maximum_Gray_Value();

    void Generate_Gray_Value_Image();

    int Aruco_Detection();

    cv::Mat CameraMatrix_Trans(pho::api::CameraMatrix64f cameraMatrix);

    cv::Mat DistCoeffs_Trans(std::vector<double> distCoeffs);

    cv::Mat Get_Gray_Value_Image();

    cv::Mat Get_Aruco_Detection_Image();

    std::vector<float> Get_Euler();

    std::vector<float> Get_Position();

    std::vector<float> Get_X_Axis();

    bool Get_isArucoMarkerDetected();

private:
    void Init_DataProcess();

    void normalize(Vector3D& vector);

    Vector3D crossProduct(const Vector3D &a, const Vector3D &b);

private:
    pho::api::PFrame SampleFrame;

    double gray_value_max;

    cv::Mat gray_value_image;
    cv::Mat aruco_detection_image;

    // Define vec_cor0_to_cor1 as X-axis
    Vector3D vec_cor0_to_cor1;

    // Define vec_cor0_to_cor3 as Y-axis
    Vector3D vec_cor0_to_cor3;

    // Define vec_z as vec_cor0_to_cor1 x vec_cor0_to_cor3
    Vector3D vec_z;

    // Euler Angle of Marker
    Eigen::Vector3f euler;

    // Position of Marker
    Vector3D Position;

    std::vector<int> marker_id;
    std::vector<std::vector<cv::Point2f>> marker_corners;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_250);
    float marker_length = 0.05;

    bool isArucoMarkerDetected {false};


};

#endif // DATAPROCESS_H
