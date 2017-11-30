#ifndef _MYFACE_H_
#define _MYFACE_H_

#include <face_detection.h>
#include <face_alignment.h>
#include <face_identification.h>
#include <opencv2/opencv.hpp>

extern int face_detection(const char* fd_model_path ,const char* img_path,std::vector<seeta::FaceInfo>& faces_known);
//extern int face_aligenment(const char* fd_model_path ,cv::mat,std::vector<seeta::FaceInfo>& faces_known);


#endif // MYFACE_H
