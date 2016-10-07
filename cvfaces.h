#ifndef CVFACES_H
#define CVFACES_H
#include <opencv/cv.h>
#include <opencv2/core.hpp>
#include <objdetect.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio/videoio_c.h"

#include <vector>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;
class Cvfaces
{
public:
    Cvfaces();
    Cvfaces(const string &cascadePath);
    ~Cvfaces();
public:
    bool setCascade(const string &cascadePath);
    std::vector<Rect> getFacesPoints(const Mat &srcImg);

private:
    Point postion;
    Point center;
    string cascadePath;
    bool isCascadeSet;
    CascadeClassifier cascade;
};

#endif // CVFACES_H

