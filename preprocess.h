#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>
#include "Line.h"
#include "cvutils.h"
#include "typedefs.h"

class Preprocess
{
public:
    Preprocess(cv::Mat&);
    void getLines(Lines&);

    void showCanny();
    void showHoughlines();

    cv::Mat getCanny(){return canny;}
    cv::Mat getHough();
    cv::Mat getBlurred(){return blurred;}

private:
    int houghThreshold;
    int minLineLength;
    int maxLineGap;
    cv::Size gaussianBlurSize;
    int gaussianBlurSigma;

    Lines lines;
    std::vector<cv::Vec4i> houghlines;
    cv::Mat image, blurred, canny, imgHough;
    void edgeDetection(bool doBlur = true);
    void lineDetection();
};

#endif // PREPROCESS_H
