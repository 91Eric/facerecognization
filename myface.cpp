#include "myface.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <opencv2/opencv.hpp>

int face_detection(const char* fd_model_path,const char* img_path,std::vector<seeta::FaceInfo>& faces_known)
{
    cv::Mat img_origin = cv::imread(img_path, cv::IMREAD_UNCHANGED);
    cv::Mat img_gray;

    if(img_origin.empty())
    {
        fprintf(stderr,"img read error!\n");
        return -1;
    }

    if (img_origin.channels() != 1)
        cv::cvtColor(img_origin, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img_origin;

    seeta::ImageData img_data(img_gray.cols,img_gray.rows,img_gray.channels());
    img_data.data = img_gray.data;

    seeta::FaceDetection detector(fd_model_path);

    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    faces_known = detector.Detect(img_data);

    return 0;
}
