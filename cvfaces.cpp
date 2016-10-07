#include "cvfaces.h"
#include <opencv2/core.hpp>
#include <objdetect.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio/videoio_c.h"
#include <iostream>

using namespace std;
using namespace cv;

std::vector<Rect> Cvfaces::getFacesPoints(const Mat &srcImg)
{
    int scale = 1;
    double t = 0;
    vector<Rect> faces;
    vector<Rect> outputfacesRects;

    if(isCascadeSet == false)
    {
        if(cascade.load(cascadePath)==false)
        {
            faces.clear();
            return faces;
        }
    }


    Mat gray;
    Mat smallImg(cvRound(srcImg.rows / scale), cvRound(srcImg.cols / scale), CV_8UC1);

    cvtColor(srcImg, gray, COLOR_BGR2GRAY);
    resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    t = (double)cvGetTickCount();
    cascade.detectMultiScale(smallImg, faces,
                            1.1, 2, 0
                            //|CASCADE_FIND_BIGGEST_OBJECT
                            //|CASCADE_DO_ROUGH_SEARCH
                            | CASCADE_SCALE_IMAGE,
                            Size(30, 30));

    t = (double)cvGetTickCount() - t;
//    printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency()*1000.));


    for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++)
    {
        double aspect_ratio = (double)r->width / r->height;//  width:height
        if (0.75 < aspect_ratio && aspect_ratio < 1.3)
        {
            outputfacesRects.push_back(*r);
        }
    }

    return outputfacesRects;
}


bool Cvfaces::setCascade(const string &_cascadePath)
{
    if(cascade.load(_cascadePath))
    {
        isCascadeSet = true;
        return true;
    }
    else
    {
        isCascadeSet = false;
        return false;
    }
}

Cvfaces::Cvfaces()
{

}

Cvfaces::Cvfaces(const string &_cascadePath="/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml")
{
    if(cascade.load(_cascadePath))
        isCascadeSet = true;
    else
        isCascadeSet = false;
}

Cvfaces::~Cvfaces()
{

}




