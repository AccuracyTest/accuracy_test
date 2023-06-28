#include "DataProcess.h"

DataProcess::DataProcess()
{
    Init_DataProcess();
}

DataProcess::~DataProcess()
{

}

void DataProcess::Set_SampleFrame(const pho::api::PFrame Frame)
{
    SampleFrame = Frame;
}
void DataProcess::Init_DataProcess()
{
    gray_value_max = 0;
}

void DataProcess::normalize(Vector3D& vector)
{
    float magSp = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;

    if (magSp > 0.0f){
        float oneOverMag = 1.0f/sqrt(magSp);
        vector.x *= oneOverMag;
        vector.y *= oneOverMag;
        vector.z *= oneOverMag;
    }

}

Vector3D DataProcess::crossProduct(const Vector3D &a, const Vector3D &b)
{
    Vector3D cross_vector;

    cross_vector.x = a.y*b.z - a.z*b.y;
    cross_vector.y = a.z*b.x - a.x*b.z;
    cross_vector.z = a.x*b.y - a.y*b.x;

    return  cross_vector;

}

void DataProcess::Generate_Maximum_Gray_Value()
{
    if (SampleFrame->Empty())
    {
        std::cout << "Sample Frame has no content!" << std::endl;
        return;
    }

    double grayValue_max = 0;

    for (int h = 0; h < SampleFrame->PointCloud.Size.Height; ++h)
    {
        for (int w = 0; w < SampleFrame->PointCloud.Size.Width; ++w)
        {
            double grayValue = SampleFrame->Texture[h][w];

            if (grayValue_max < grayValue)
            {
                grayValue_max = grayValue;
            }
        }
    }

    std::cout << "grayValue_max: " << grayValue_max << std::endl;

    gray_value_max = grayValue_max;

}

void DataProcess::Generate_Gray_Value_Image()
{
    Generate_Maximum_Gray_Value();
    int Width = SampleFrame->PointCloud.Size.Width;
    int Height = SampleFrame->PointCloud.Size.Height;
    cv::Mat image(Height, Width, CV_8UC3, Scalar(0,0,255));

    if (SampleFrame->Empty())
    {
        std::cout << "Sample Frame has no content!" << std::endl;
        return;
    }

    for (int h = 0; h < SampleFrame->PointCloud.Size.Height; ++h)
    {
        for (int w =0; w < SampleFrame->PointCloud.Size.Width; ++w)
        {
            int grayValue = int (SampleFrame->Texture[h][w] / gray_value_max * 255.);

            image.at<cv::Vec3b>(h, w)[0] = grayValue;
            image.at<cv::Vec3b>(h, w)[1] = grayValue;
            image.at<cv::Vec3b>(h, w)[2] = grayValue;
        }
    }
    gray_value_image = image;
}


int DataProcess::Aruco_Detection()
{
    cv::Mat image = gray_value_image.clone();
    aruco_detection_image = image.clone();
    cv::aruco::detectMarkers(image, dictionary, marker_corners, marker_id);
    if (marker_id.size() > 0){
        std::cout << "Aruco Marker Detected" << std::endl;
        std::cout << "Marker id: " << marker_id.at(0) << std::endl;
        std::cout << "Marker corner: ";
        std::cout << "[ " << marker_corners[0][0].x << ", " << marker_corners[0][0].y << " ]"<< std::endl;
        std::cout << "[ " << marker_corners[0][1].x << ", " << marker_corners[0][1].y << " ]"<< std::endl;
        std::cout << "[ " << marker_corners[0][2].x << ", " << marker_corners[0][2].y << " ]"<< std::endl;
        std::cout << "[ " << marker_corners[0][3].x << ", " << marker_corners[0][3].y << " ]"<< std::endl;

        pho::api::Point3<float> corner0 =  SampleFrame->PointCloud.At(marker_corners[0][0].y, marker_corners[0][0].x);
        pho::api::Point3<float> corner1 =  SampleFrame->PointCloud.At(marker_corners[0][1].y, marker_corners[0][1].x);
        pho::api::Point3<float> corner2 =  SampleFrame->PointCloud.At(marker_corners[0][2].y, marker_corners[0][2].x);
        pho::api::Point3<float> corner3 =  SampleFrame->PointCloud.At(marker_corners[0][3].y, marker_corners[0][3].x);

        std::cout << "corner 0: [x, y, z] " << corner0.x  << ", " << corner0.y << ", " << corner0.z << std::endl;
        std::cout << "corner 1: [x, y, z] " << corner1.x  << ", " << corner1.y << ", " << corner1.z << std::endl;
        std::cout << "corner 2: [x, y, z] " << corner2.x  << ", " << corner2.y << ", " << corner2.z << std::endl;
        std::cout << "corner 3: [x, y, z] " << corner3.x  << ", " << corner3.y << ", " << corner3.z << std::endl;

        Position.x = corner0.x;
        Position.y = corner0.y;
        Position.z = corner0.z;

        vec_cor0_to_cor1.x = corner1.x - corner0.x;
        vec_cor0_to_cor1.y = corner1.y - corner0.y;
        vec_cor0_to_cor1.z = corner1.z - corner0.z;
        normalize(vec_cor0_to_cor1);
        std::cout << "vec_cor0_to_cor1: [x, y, z] " << vec_cor0_to_cor1.x << ", " << vec_cor0_to_cor1.y << ", " << vec_cor0_to_cor1.z << std::endl;

        vec_cor0_to_cor3.x = corner3.x - corner0.x;
        vec_cor0_to_cor3.y = corner3.y - corner0.y;
        vec_cor0_to_cor3.z = corner3.z - corner0.z;
        normalize(vec_cor0_to_cor3);
        std::cout << "vec_cor0_to_cor3: [x, y, z] " << vec_cor0_to_cor3.x << ", " << vec_cor0_to_cor3.y << ", " << vec_cor0_to_cor3.z << std::endl;

        vec_z = crossProduct(vec_cor0_to_cor1, vec_cor0_to_cor3);
        std::cout << "vec_z: [x, y, z] " << vec_z.x << ", " << vec_z.y << ", " << vec_z.z << std::endl;

        Eigen::Matrix3f Rotation_Matrix;
        Rotation_Matrix.col(0).x() = vec_cor0_to_cor1.x;
        Rotation_Matrix.col(0).y() = vec_cor0_to_cor1.y;
        Rotation_Matrix.col(0).z() = vec_cor0_to_cor1.z;

        Rotation_Matrix.col(1).x() = vec_cor0_to_cor3.x;
        Rotation_Matrix.col(1).y() = vec_cor0_to_cor3.y;
        Rotation_Matrix.col(1).z() = vec_cor0_to_cor3.z;

        Rotation_Matrix.col(2).x() = vec_z.x;
        Rotation_Matrix.col(2).y() = vec_z.y;
        Rotation_Matrix.col(2).z() = vec_z.z;
        // pitch, yaw, roll XYZ
        euler = Rotation_Matrix.eulerAngles(0, 1, 2);

        std::cout << "euler: " << euler << std::endl;

        cv::aruco::drawDetectedMarkers(aruco_detection_image, marker_corners, marker_id);
        isArucoMarkerDetected = true;
        return 0;
    } else {
        std::cout << "No Aruco Marker Found" << std::endl;
        isArucoMarkerDetected = false;
        return 1;
    }
}

Mat DataProcess::CameraMatrix_Trans(pho::api::CameraMatrix64f cameraMatrix)
{
    cv::Mat temp;
    std::vector<float> camera;
    camera.push_back(cameraMatrix[0][0]);
    camera.push_back(cameraMatrix[0][1]);
    camera.push_back(cameraMatrix[0][2]);
    camera.push_back(cameraMatrix[1][0]);
    camera.push_back(cameraMatrix[1][1]);
    camera.push_back(cameraMatrix[1][2]);
    camera.push_back(cameraMatrix[2][0]);
    camera.push_back(cameraMatrix[2][1]);
    camera.push_back(cameraMatrix[2][2]);

    temp = Mat(camera);
    temp = temp.reshape(1, 3);
    return temp;
}

Mat DataProcess::DistCoeffs_Trans(std::vector<double> distCoeffs)
{
    cv::Mat temp = Mat(distCoeffs);
    temp = temp.reshape(1,1);
    return temp;
}


cv::Mat DataProcess::Get_Gray_Value_Image()
{
    return gray_value_image;
}

Mat DataProcess::Get_Aruco_Detection_Image()
{
    return aruco_detection_image;
}

std::vector<float> DataProcess::Get_Euler()
{
    std::vector<float> euler_degree;
    euler_degree.push_back(euler.x() /3.14159265 * 180);
    euler_degree.push_back(euler.y() /3.14159265 * 180);
    euler_degree.push_back(euler.z() /3.14159265 * 180);
    return euler_degree;
}

std::vector<float> DataProcess::Get_Position()
{
    return {Position.x, Position.y, Position.z};
}

std::vector<float> DataProcess::Get_X_Axis()
{
    return {vec_cor0_to_cor1.x, vec_cor0_to_cor1.y, vec_cor0_to_cor1.z};
}

bool DataProcess::Get_isArucoMarkerDetected()
{
    return isArucoMarkerDetected;
}


