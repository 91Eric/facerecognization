/*

实现了使用切割矫正过的图片计算相似度
*/

//#define REC
#ifdef REC

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <face_detection.h>
#include <face_alignment.h>
#include <face_identification.h>
#include <opencv2/opencv.hpp>

#define LOG
//#undef LOG

using namespace std;

const std::string fd_model_path = "../face/model/seeta_fd_frontal_v1.0.bin";
const std::string fp_model_path = "../face/model/seeta_fa_v1.1.bin";
const std::string fr_model_path = "../face/model/seeta_fr_v1.0.bin";

int main()
{


    const char* img_path_known = "../face/data/hg1_transform.jpg";
    cv::Mat img_origin_known = cv::imread(img_path_known, cv::IMREAD_UNCHANGED);
    cv::Mat img_gray_known;

    const char * img_path_unknown = "../face/data/hg2_transform.jpg";
    cv::Mat img_origin_unknown = cv::imread(img_path_unknown, cv::IMREAD_UNCHANGED);
    cv::Mat img_gray_unknown;

    if(img_origin_known.empty() || img_origin_unknown.empty())
    {
        fprintf(stderr,"img read error!\n");
        return -1;
    }

    if (img_origin_known.channels() != 1)
        cv::cvtColor(img_origin_known, img_gray_known, cv::COLOR_BGR2GRAY);
    else
        img_gray_known = img_origin_known;
    if (img_origin_unknown.channels() != 1)
        cv::cvtColor(img_origin_unknown, img_gray_unknown, cv::COLOR_BGR2GRAY);
    else
        img_gray_unknown = img_origin_unknown;

    #ifdef LOG
        cout<<"img read finished!"<<endl;
    #endif


    seeta::FaceIdentification face_recognizer(fr_model_path.c_str());

    seeta::ImageData img_origin_data_known(img_origin_known.cols,img_origin_known.rows,img_origin_known.channels());
    img_origin_data_known.data = img_origin_known.data;
    float fea_known[2048];
    face_recognizer.ExtractFeature(img_origin_data_known,fea_known);


    seeta::ImageData img_origin_data_unknown(img_origin_unknown.cols,img_origin_unknown.rows,img_origin_unknown.channels());
    img_origin_data_unknown.data = img_origin_unknown.data;
    float fea_unknown[2048];
    face_recognizer.ExtractFeature(img_origin_data_unknown,fea_unknown);

    fstream fea_out("re.txt",ios_base::app);
    for(auto i : fea_unknown)
    {
        fea_out<<i<<" ";
    }
    fea_out.close();

    // Caculate similarity of two faces
    float sim = face_recognizer.CalcSimilarity(fea_known, fea_unknown);
    std::cout << sim <<endl;

    cv::namedWindow("known", cv::WINDOW_AUTOSIZE);
    cv::imshow("known", img_origin_known);
    cv::namedWindow("unkown", cv::WINDOW_AUTOSIZE);
    cv::imshow("unkown", img_origin_unknown);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}

#endif
