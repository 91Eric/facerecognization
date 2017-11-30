///*

//实现了人脸检测，关键点计算，使用未切割的图片计算相似度
//*/

//#define MAIN
//#ifdef MAIN

//#include <string>
//#include <iostream>
//#include <fstream>
//#include <cstdio>
//#include <face_detection.h>
//#include <face_alignment.h>
//#include <face_identification.h>
//#include <opencv2/opencv.hpp>

//#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/gui_widgets.h>
//#include <dlib/image_io.h>



//#define LOG
////#undef LOG

//using namespace std;

//const std::string fd_model_path = "../face/model/seeta_fd_frontal_v1.0.bin";
//const std::string fp_model_path = "../face/model/seeta_fa_v1.1.bin";
//const std::string fr_model_path = "../face/model/seeta_fr_v1.0.bin";

//int main(int argc, char** argv)
//{
//    if (argc > 1)
//    {
//      cout << "Usage: " << argv[0]
//          << " image_path"
//          << endl;
//      return -1;
//    }

//    const char* img_path_known = "../face/data/hg1.jpg";
//    cv::Mat img_origin_known = cv::imread(img_path_known, cv::IMREAD_UNCHANGED);
//    cv::Mat img_gray_known;

//    const char * img_path_unknown = "../face/data/hg2.jpg";
//    cv::Mat img_origin_unknown = cv::imread(img_path_unknown, cv::IMREAD_UNCHANGED);
//    cv::Mat img_gray_unknown;

//    if(img_origin_known.empty() || img_origin_unknown.empty())
//    {
//        fprintf(stderr,"img read error!\n");
//        return -1;
//    }

//    if (img_origin_known.channels() != 1)
//        cv::cvtColor(img_origin_known, img_gray_known, cv::COLOR_BGR2GRAY);
//    else
//        img_gray_known = img_origin_known;
//    if (img_origin_unknown.channels() != 1)
//        cv::cvtColor(img_origin_unknown, img_gray_unknown, cv::COLOR_BGR2GRAY);
//    else
//        img_gray_unknown = img_origin_unknown;

//    #ifdef LOG
//        cout<<"img read finished!"<<endl;
//    #endif

//    seeta::ImageData img_data_know(img_gray_known.cols,img_gray_known.rows,img_gray_known.channels());
//    img_data_know.data = img_gray_known.data;
//    seeta::ImageData img_data_unknow(img_gray_unknown.cols,img_gray_unknown.rows,img_gray_unknown.channels());
//    img_data_unknow.data = img_gray_unknown.data;

//    #ifdef LOG
//        cout<<"detection start!"<<endl;
//    #endif

//    seeta::FaceDetection detector(fd_model_path.c_str());

//    detector.SetMinFaceSize(40);
//    detector.SetScoreThresh(2.f);
//    detector.SetImagePyramidScaleFactor(0.8f);
//    detector.SetWindowStep(4, 4);

//    long t0 = cv::getTickCount();
//    std::vector<seeta::FaceInfo> faces_known = detector.Detect(img_data_know);
//    std::vector<seeta::FaceInfo> faces_unknown = detector.Detect(img_data_unknow);
//    long t1 = cv::getTickCount();
//    double secs = (t1 - t0)/cv::getTickFrequency();

//    cout << "Detections takes " << secs << " seconds " << endl;

//    #ifdef INFO
//    cout << "Image size (wxh): " << img_data_know.width << "x"
//      << img_data_know.height << endl;
//    #endif

//    seeta::FaceAlignment point_detector(fp_model_path.c_str());
//    seeta::FacialLandmark points_known[5];
//    seeta::FacialLandmark points_unknown[5];
//    int32_t num_face = static_cast<int32_t>(faces_known.size());

//    for (int32_t i = 0; i < num_face; i++)
//    {
//        point_detector.PointDetectLandmarks(img_data_know, faces_known[i], points_known);

//        // Detect unknown people 5 facial landmarks: two eye centers, nose tip and two mouth corners

//        point_detector.PointDetectLandmarks(img_data_unknow, faces_unknown[i], points_unknown);
//    }

//    seeta::FaceIdentification face_recognizer(fr_model_path.c_str());

//    seeta::ImageData img_origin_data_known(img_origin_known.cols,img_origin_known.rows,img_origin_known.channels());
//    img_origin_data_known.data = img_origin_known.data;
//    float fea_known[2048];
//    face_recognizer.ExtractFeatureWithCrop(img_origin_data_known,points_known,fea_known);


//    seeta::ImageData img_origin_data_unknown(img_origin_unknown.cols,img_origin_unknown.rows,img_origin_unknown.channels());
//    img_origin_data_unknown.data = img_origin_unknown.data;
//    float fea_unknown[2048];
//    face_recognizer.ExtractFeatureWithCrop(img_origin_data_unknown,points_unknown,fea_unknown);

//    fstream fea_out("re.txt",ios_base::app);
//    for(auto i : fea_unknown)
//    {
//        fea_out<<i<<" ";
//    }
//    fea_out.close();

//    // Caculate similarity of two faces
//    float sim = face_recognizer.CalcSimilarity(fea_known, fea_unknown);
//    std::cout << sim <<endl;

//    cv::namedWindow("known", cv::WINDOW_AUTOSIZE);
//    cv::imshow("known", img_origin_known);
//    cv::namedWindow("unkown", cv::WINDOW_AUTOSIZE);
//    cv::imshow("unkown", img_origin_unknown);
//    cv::waitKey(0);
//    cv::destroyAllWindows();

//    return 0;
//}

//#endif
