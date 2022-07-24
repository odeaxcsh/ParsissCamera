#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

#include "FlyCapture2.h"
#include <iostream>
#include <sstream>

#include <opencv2/opencv.hpp>

using namespace FlyCapture2;


cv::Mat findCircles(cv::Mat image)
{
    double cannyThreshold = 200;
    cv::Mat canny;
	cv::Canny(image, canny, cannyThreshold, cannyThreshold * 2);
    return canny;
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
    cv::Mat canny;

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
        canny = findCircles(image);
		
        cv::imshow("FlyCapture", canny);
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
