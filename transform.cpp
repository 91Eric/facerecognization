/*
实现人脸的检测，关键点 检测，人脸剪切及矫正

*/
//#define TRANSF
#ifdef TRANSF

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <face_detection.h>
#include <face_alignment.h>
#include <face_identification.h>
#include <opencv2/opencv.hpp>

//#define LOG
#define SHOW

using namespace std;

const std::string fd_model_path = "../face/model/seeta_fd_frontal_v1.0.bin";
const std::string fp_model_path = "../face/model/seeta_fa_v1.1.bin";
const std::string fr_model_path = "../face/model/seeta_fr_v1.0.bin";

const string src_path = "../face/data/hg2.jpg";
const string algiment_path = "../face/data/hg2_algiment.jpg";
const string crop_path = "../face/data/hg2_crop.jpg";
const string transf_path = "../face/data/hg2_transform.jpg";

int main()
{
    cv::Mat img_origin_known = cv::imread(src_path, cv::IMREAD_UNCHANGED);
    cv::Mat img_gray_known;


    if(img_origin_known.empty())
    {
        fprintf(stderr,"img read error!\n");
        return -1;
    }

    if (img_origin_known.channels() != 1)
        cv::cvtColor(img_origin_known, img_gray_known, cv::COLOR_BGR2GRAY);
    else
        img_gray_known = img_origin_known;


    #ifdef LOG
        cout<<"img read finished!"<<endl;
    #endif

    seeta::ImageData img_data_know(img_gray_known.cols,img_gray_known.rows,img_gray_known.channels());
    img_data_know.data = img_gray_known.data;

    #ifdef LOG
        cout<<"detection start!"<<endl;
    #endif

    seeta::FaceDetection detector(fd_model_path.c_str());

    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    long t0 = cv::getTickCount();
    std::vector<seeta::FaceInfo> faces_known = detector.Detect(img_data_know);

    long t1 = cv::getTickCount();
    double secs = (t1 - t0)/cv::getTickFrequency();

    cout << "Detections takes " << secs << " seconds " << endl;


    #ifdef INFO
    cout << "Image size (wxh): " << img_data_know.width << "x"
      << img_data_know.height << endl;
    #endif

    seeta::FaceAlignment point_detector(fp_model_path.c_str());
    seeta::FacialLandmark points_known[5];

    cv::Rect face_rect;
    int32_t num_face = static_cast<int32_t>(faces_known.size());

    for (int32_t i = 0; i < num_face; i++)
    {
        //bbox of known people
        face_rect.x = faces_known[i].bbox.x;
        face_rect.y = faces_known[i].bbox.y;
        face_rect.width = faces_known[i].bbox.width;
        face_rect.height = faces_known[i].bbox.height;

        cv::rectangle(img_origin_known, face_rect, CV_RGB(0, 0, 255), 4, 8, 0);

        // Detect known people 5 facial landmarks: two eye centers, nose tip and two mouth corners
        /*
            0: left eye
            1: right eye
            2: nose tip
            3: left mouth corner
            4: right mouth corner
        */

        point_detector.PointDetectLandmarks(img_data_know, faces_known[i], points_known);

        // draw 5 facial landmarks
        for( int  i = 0;i < 5;i++)
        {

            if(i == 0 || i ==1)
            cv::circle(img_origin_known, cv::Point(static_cast<int>(points_known[i].x), static_cast<int>(points_known[i].y)), 2, cv::Scalar(0, 0, 255), 2);
        }

        cv::imwrite(algiment_path,img_origin_known);
    }

    //Crop Face
    fprintf(stderr, "start process image: %s\n", src_path.c_str());

    cv::Mat src_img = cv::imread(src_path, 1);
    if (src_img.data == nullptr)
    {
        fprintf(stderr, "Load image error: %s\n", src_path.c_str());
        return -1;
    }

    seeta::FaceIdentification face_recognizer(fr_model_path.c_str());

    cout<<"face_recognizer.crop_channels() : "<<face_recognizer.crop_channels() <<endl;
    if (face_recognizer.crop_channels() != src_img.channels())
    {
        fprintf(stderr, "channels dismatch: %d, %d\n", face_recognizer.crop_channels(), src_img.channels());
        return -1;
    }

    // ImageData store data of an image without memory alignment.
    seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
    src_img_data.data = src_img.data;

    // Create a image to store crop face.
    cv::Mat dst_img(face_recognizer.crop_height(), face_recognizer.crop_width(), CV_8UC(face_recognizer.crop_channels()));
    seeta::ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
    dst_img_data.data = dst_img.data; //shiyong = yinwei liangzhe gongxiang shujuqu !!!!!!
    // Crop Face
    face_recognizer.CropFace(src_img_data, points_known, dst_img_data);

    cv::imwrite(crop_path, dst_img);

    // transfrom
    cv::Mat src = cv::imread(crop_path);
    cv::Mat dst;

    //旋转角度
    //  这里不需要重新定位剪切后的眼睛位置，因为使用的是差值，也就是相对位置，因此不同坐标下相对位置不变

    double dx = static_cast<double>(points_known[1].x - points_known[0].x);
    double dy = static_cast<double>(points_known[1].y - points_known[0].y);
    double angle = atan2(dy,dx)* 180.0/CV_PI;
    cout <<"angle " <<angle<<endl;

    cv::Size src_sz = src.size();
    cv::Size dst_sz(src_sz.height, src_sz.width);
    int len = std::max(src.cols, src.rows);

    //指定旋转中心
    cv::Point2f center(len / 2., len / 2.);

    //获取旋转矩阵（2x3矩阵）
    cv::Mat rot_mat = cv::getRotationMatrix2D(center, -angle, 1.0);

    //根据旋转矩阵进行仿射变换
    cv::warpAffine(src, dst, rot_mat, dst_sz);

    cv::imwrite(transf_path,dst);

#ifdef SHOW
    //显示旋转效果
    cv::imshow("crop_image", src);
    cv::imshow("trans_crop_face", dst);

    cv::namedWindow("known", cv::WINDOW_AUTOSIZE);
    cv::imshow("known", img_origin_known);

    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

    return 0;
}

#endif
