
//#define CROP
#ifdef CROP

#include <string>
#include <fstream>
#include <cstdio>
#include <face_detection.h>
#include <face_alignment.h>
#include <face_identification.h>
#include <opencv2/opencv.hpp>

using namespace std;

const std::string fd_model_path = "../face/model/seeta_fd_frontal_v1.0.bin";
const std::string fp_model_path = "../face/model/seeta_fa_v1.1.bin";
const std::string fr_model_path = "../face/model/seeta_fr_v1.0.bin";
const std::string path_images = "../face/data/" ;

int main()
{

    seeta::FaceDetection detector(fd_model_path.c_str());
    seeta::FaceAlignment alignment(fp_model_path.c_str());
    seeta::FaceIdentification face_recognizer(fr_model_path.c_str());

    detector.SetMinFaceSize(20);
    detector.SetMaxFaceSize(200);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    std::vector<std::vector<seeta::FacialLandmark>> landmards;

    // detect and alignment
    for (int i = 0; i < 2; i++)
    {
        std::string image = path_images + std::to_string(i) + ".jpg";
        //fprintf(stderr, "start process image: %s\n", image.c_str());

        cv::Mat src_ = cv::imread(image, 1);
        if (src_.empty()) {
            fprintf(stderr, "read image error: %s\n", image.c_str());
            continue;
        }

        cv::Mat src;
        cv::cvtColor(src_, src, CV_BGR2GRAY);

        seeta::ImageData img_data;
        img_data.data = src.data;
        img_data.width = src.cols;
        img_data.height = src.rows;
        img_data.num_channels = 1;

        std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);
        if (faces.size() == 0) {
            fprintf(stderr, "%s don't detect face\n", image.c_str());
            continue;
        }

        // Detect 5 facial landmarks: two eye centers, nose tip and two mouth corners
        std::vector<seeta::FacialLandmark> landmard(5);
        alignment.PointDetectLandmarks(img_data, faces[0], &landmard[0]);

        landmards.push_back(landmard);

        cv::rectangle(src_, cv::Rect(faces[0].bbox.x, faces[0].bbox.y,
            faces[0].bbox.width, faces[0].bbox.height), cv::Scalar(0, 255, 0), 2);

        for (auto point : landmard) {
            cv::circle(src_, cv::Point(point.x, point.y), 2, cv::Scalar(0, 0, 255), 2);
        }

        std::string save_result = path_images + "_" + std::to_string(i) + ".jpg";
        cv::imwrite(save_result, src_);
    }


    ////////////////////////////////////////////////////////////////
 //zheyibufen zhishi ba jieguo fangzai yizhangdatulimian
//    int width = 200;
//    int height = 200;
//    cv::Mat dst(height * 5, width * 4, CV_8UC3);
//    for (int i = 0; i < 2; i++) {
//        std::string input_image = path_images + "_" + std::to_string(i) + ".jpg";
//        cv::Mat src = cv::imread(input_image, 1);
//        if (src.empty()) {
//            fprintf(stderr, "read image error: %s\n", input_image.c_str());
//            return -1;
//        }

//        cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
//        int x = (i * width) % (width * 4);
//        int y = (i / 4) * height;
//        cv::Mat part = dst(cv::Rect(x, y, width, height));
//        src.copyTo(part);
//    }
//    std::string output_image = path_images + "result_alignment.png";
//    cv::imwrite(output_image, dst);

    // crop image
    for (int i = 0; i < 2; i++) {
        std::string image = path_images + std::to_string(i) + ".jpg";
        //fprintf(stderr, "start process image: %s\n", image.c_str());

        cv::Mat src_img = cv::imread(image, 1);
        if (src_img.data == nullptr) {
            fprintf(stderr, "Load image error: %s\n", image.c_str());
            return -1;
        }

        if (face_recognizer.crop_channels() != src_img.channels()) {
            fprintf(stderr, "channels dismatch: %d, %d\n", face_recognizer.crop_channels(), src_img.channels());
            return -1;
        }

        // ImageData store data of an image without memory alignment.
        seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
        src_img_data.data = src_img.data;

        // Create a image to store crop face.
        cv::Mat dst_img(face_recognizer.crop_height(), face_recognizer.crop_width(), CV_8UC(face_recognizer.crop_channels()));
        seeta::ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
        dst_img_data.data = dst_img.data;
        // Crop Face
        face_recognizer.CropFace(src_img_data, &landmards[i][0], dst_img_data);

        std::string save_image_name = path_images + "crop_" + std::to_string(i) + ".jpg";
        cv::imwrite(save_image_name, dst_img);
    }

    dst = cv::Mat(height * 5, width * 4, CV_8UC3);
    for (int i = 0; i < 2; i++) {
        std::string input_image = path_images + "crop_" + std::to_string(i) + ".jpg";
        cv::Mat src_img = cv::imread(input_image, 1);
        if (src_img.empty()) {
            fprintf(stderr, "read image error: %s\n", input_image.c_str());
            return -1;
        }

        cv::resize(src_img, src_img, cv::Size(width, height), 0, 0, 4);
        int x = (i * width) % (width * 4);
        int y = (i / 4) * height;
        cv::Mat part = dst(cv::Rect(x, y, width, height));
        src_img.copyTo(part);
    }
    output_image = path_images + "result_crop.png";
    cv::imwrite(output_image, dst);

    // extract feature
    int feat_size = face_recognizer.feature_size();
    if (feat_size != 2048) {
        fprintf(stderr, "feature size mismatch: %d\n", feat_size);
        return -1;
    }

    float* feat_sdk = new float[feat_size * 20];

    for (int i = 0; i < 2; i++) {
        std::string input_image = path_images + "crop_" + std::to_string(i) + ".jpg";
        cv::Mat src_img = cv::imread(input_image, 1);
        if (src_img.empty()) {
            fprintf(stderr, "read image error: %s\n", input_image.c_str());
            return -1;
        }

        cv::resize(src_img, src_img, cv::Size(face_recognizer.crop_height(), face_recognizer.crop_width()));

        // ImageData store data of an image without memory alignment.
        seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
        src_img_data.data = src_img.data;

        // Extract feature
        face_recognizer.ExtractFeature(src_img_data, feat_sdk + i * feat_size);
    }

    float* feat1 = feat_sdk;
    // varify(recognize)
    for (int i = 1; i < 2; i++) {
        std::string image = std::to_string(i) + ".jpg";
        float* feat_other = feat_sdk + i * feat_size;

        // Caculate similarity
        float sim = face_recognizer.CalcSimilarity(feat1, feat_other);
        fprintf(stdout, "0.jpg -- %s similarity: %f\n", image.c_str(), sim);

    }

    delete[] feat_sdk;

    return 0;
}

#endif


