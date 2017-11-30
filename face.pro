TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    recognization.cpp \
    crop.cpp \
    transform.cpp \
    recongnizationwithcrop.cpp \
    source1.cpp \
    myface.cpp

INCLUDEPATH += /usr/local/include/opencv \
/home/eric/SeetaFaceEngine/FaceDetection/include \
/home/eric/SeetaFaceEngine/FaceAlignment/include \
/home/eric/SeetaFaceEngine/FaceIdentification/include \
/home/eric/dlib-19.4

LIBS += -L../face/lib \
-lseeta_facedet_lib \
-lseeta_fa_lib \
-lviplnet \
-ldlib

LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann \
-lopencv_imgcodecs

HEADERS += \
    myface.h
