#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

#include "FlyCapture2.h"
#include <iostream>
#include <sstream>
#include <vector>


#include <opencv2/opencv.hpp>

using namespace FlyCapture2;


std::vector<cv::Point> HoughDetector(cv::Mat image)
{  
    const int cannyThreshold = 200;
    cv::pyrDown(image, image);
    cv::pyrUp(image, image);

    double resolution = 1;
    double minDist = 15;
    int minRadius = 0;
    int maxRadius = 5;

	
    std::vector<cv::Vec3f> circles;
    try {
        cv::HoughCircles(image, circles, cv::HOUGH_GRADIENT, resolution, minDist, cannyThreshold, 4, minRadius, maxRadius);
    }
    catch (cv::Exception& e) {
        std::cout << e.what() << std::endl;
    }

    std::vector<cv::Point> result(circles.size());
    for (int i = 0; i < circles.size(); ++i) {
		result[i] = cv::Point(circles[i][0], circles[i][1]);
		
    }

    return result;
}


std::vector<cv::Point> CannyDetector(cv::Mat image)
{
    cv::Mat canny;

    double cannyThreshold = 100;
    cv::pyrDown(image, image);
    cv::pyrUp(image, image);

    cv::Canny(image, canny, cannyThreshold, cannyThreshold * 2);

    cv::imshow("Canny", canny);
	
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(canny, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::drawContours(image, contours, -1, cv::Scalar(0,0,255), 2);
	cv::imshow("Contours", image);

    std::vector<cv::Point> markers(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        try {
            auto ellipse = cv::fitEllipse(contours[i]);
            markers[i] = ellipse.center;
        }
        catch (cv::Exception e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return markers;
}

void PrintError(Error error) 
{ 
    error.PrintErrorTrace(); 
}

int RunSingleCamera(PGRGuid guid)
{
    const int k_numImages = 3000;

    FlyCapture2::Error error;
    FlyCapture2::Camera cam;
    if ((error = cam.Connect(&guid)) != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    FlyCapture2::CameraInfo camInfo;
    if ((error = cam.GetCameraInfo(&camInfo)) != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    std::cout << std::endl;
    std::cout << "*** CAMERA INFORMATION ***" << std::endl;
    std::cout << "Serial number - " << camInfo.serialNumber << std::endl;
    std::cout << "Camera model - " << camInfo.modelName << std::endl;
    std::cout << "Camera vendor - " << camInfo.vendorName << std::endl;
    std::cout << "Sensor - " << camInfo.sensorInfo << std::endl;
    std::cout << "Resolution - " << camInfo.sensorResolution << std::endl;
    std::cout << "Firmware version - " << camInfo.firmwareVersion << std::endl;
    std::cout << "Firmware build time - " << camInfo.firmwareBuildTime << std::endl << std::endl;

    FlyCapture2::FC2Config config;
    if ((error != cam.GetConfiguration(&config)) != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    config.numBuffers = 300;

    error = cam.SetConfiguration(&config);
    if (error != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    error = cam.StartCapture();
    if (error != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }
       
    cv::namedWindow("FlyCapture");

    FlyCapture2::Image rawImage, convertedImage;
    cv::Mat toDraw;

	auto start = std::chrono::high_resolution_clock::now();
    for (int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
        if ((error = cam.RetrieveBuffer(&rawImage)) != PGRERROR_OK) {
            PrintError(error);
            continue;
        }

        if ((rawImage.Convert(PIXEL_FORMAT_MONO8, &convertedImage)) != PGRERROR_OK) {
            PrintError(error);
            return -1;
        }

        cv::Mat image = cv::Mat(convertedImage.GetRows(), convertedImage.GetCols(), CV_8UC1, convertedImage.GetData());
        cv::resize(image, toDraw, cv::Size(), 0.5, 0.5);

        image = toDraw;
        auto circles = HoughDetector(toDraw);

        cv::resize(image, image, cv::Size(), 0.5, 0.5);

        for (size_t i = 0; i < circles.size(); i++) {
            cv::Point center = circles[i];
            cv::circle(toDraw, center, 1, cv::Scalar(0, 100, 100), 3, cv::LINE_AA);
            circle(toDraw, center, 10, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
        }

        cv::imshow("FlyCapture", toDraw);
        int key = cv::waitKey(1);
		
        auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Time: " << duration.count() << " microseconds" << std::endl;
		std::cout << "FPS: " << (double)1000000 / duration.count() << std::endl;
		start = std::chrono::high_resolution_clock::now();
		
        if(key == 27 || key == 'q') {
            break;
		}
    }

    cv::destroyWindow("FlyCapture");

    if ((error = cam.StopCapture()) != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    if ((error = cam.Disconnect()) != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    return 0;
}

int main()
{
    Error error;
    BusManager busMgr;
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK) {
        PrintError(error);
        return -1;
    }

    std::cout << "Number of cameras detected: " << numCameras << std::endl;

    for (unsigned int i = 0; i < numCameras; i++) {
        PGRGuid guid;
        error = busMgr.GetCameraFromIndex(i, &guid);
        if (error != PGRERROR_OK) {
            PrintError(error);
            return -1;
        }

        RunSingleCamera(guid);
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.ignore();
    return 0;
}
