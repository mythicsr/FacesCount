#include "facescount.h"
#include "ui_facescount.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QByteArray>
#include <QDateTime>

#include <QTimer>
#include <opencv/cv.h>
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

FacesCount::FacesCount(QWidget *parent) :
    QMainWindow(parent),
    isAsked(false),
    isalive(false),
    ui(new Ui::FacesCount)
{
    ui->setupUi(this);
    camera = new VideoCapture();
    timer = new QTimer(this);
    imgthread = new ImgThread(this);
    ui->info->setFaceCount(this);
    qDebug()<<"my objname = "<<objectName();
    isalive = true;
    camera->open(0);
    if(camera->isOpened()==false)
    {
        qDebug("camera open failed");
    }
    qDebug("camera open ok");
    qRegisterMetaType<vector<Rect>>("vector<Rect>");
    connect(timer,SIGNAL(timeout()), this, SLOT(cameraRead()));
    connect(imgthread, SIGNAL(onFacesFound(vector<Rect>)), this, SLOT(updateFacesPosition(vector<Rect>)));
    connect(ui->info, SIGNAL(onUpdatefacesInfo(int)),this, SLOT(updateFacesInfo(int)));

    if(imgthread == nullptr)
    {
        qDebug()<<"imgthread new failed!";
    }

    imgthread->start();

    timer->start(10);

    qDebug()<<"set alive == true";
}

FacesCount::~FacesCount()
{
    delete ui;
    delete camera;
}



QImage FacesCount::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}
Mat FacesCount::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

void FacesCount::triggerSrcImgFlag()
{
    isAsked = true;
}

vector<Rect> FacesCount::getRectVector()
{
    return rectVector;
}

void FacesCount::cameraRead()
{
    Mat imgdata;
    QImage qimg;
    camera->read(imgdata);
    if(imgdata.empty()==false)
    {
        qimg = cvMat2QImage(imgdata);
        if(isAsked==true)
        {
            imgthread->m_imgQueue.push(qimg);
            isAsked=false;
        }
        ui->viewer->setPixmap(QPixmap::fromImage(qimg));

    }
    imgdata.release();
}

void FacesCount::updateFacesPosition(vector<Rect> r)//slot
{
    rectVector = r;
    qDebug()<<"faces count ="<<r.size()<<" "<<QDateTime::currentDateTime().toString("mm:ss");
    ui->info->update();
}

void FacesCount::updateFacesInfo(int count)
{
    ui->faceslabel->setText(QString::number(count)+" Faces");
}

//ImgThread

ImgThread::ImgThread(QObject *parent):
    QThread(parent)
{
    m_parent = parent;
    cvface = new Cvfaces;
    cvface->setCascade("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");

}

void ImgThread::run()
{
    while(true)
    {
        askForSrcImg(m_parent);
        while(m_imgQueue.empty()==true);
        QImage src = m_imgQueue.front();
        m_imgQueue.pop();
        vector<Rect> rectSet = cvface->getFacesPoints(FacesCount::QImage2cvMat(src));
        if(rectSet.size()>0)
        {
            qDebug()<<"faceCount="<<rectSet.size()<<"  ";
            for(vector<Rect>::iterator r=rectSet.begin(); r!=rectSet.end(); r++)
            {
                qDebug()<<"("<<(*r).x<<" , "<<(*r).y<<")"<<"   ";
            }
        }

        emit onFacesFound(rectSet);
    }
}


void ImgThread::askForSrcImg(QObject* p)
{
    ((FacesCount*)p)->triggerSrcImgFlag();
}

ImgThread::~ImgThread(){


}

//myLable


myLabel::myLabel(QWidget *p):
    QLabel(p)
{

}



myLabel::~myLabel()
{

}

void myLabel::paintEvent(QPaintEvent *e)
{
//    if((m_fc->isalive)==false) //  judge that if parent-instance has been constructed
//    {
//        qDebug()<<objectName()<<":alive==false";
//        QLabel::paintEvent(e);
//    }
//    {

        QPainter painter(this);

        QPen pen;
        pen.setWidth(2);
        pen.setColor(QColor(Qt::green));
        painter.setPen(pen);
        vector<Rect> rectVect = m_fc->getRectVector();
        float x,y,w,h;

        for(vector<Rect>::iterator r=rectVect.begin(); r!=rectVect.end(); r++)
        {
            w = (*r).width;
            h = (*r).height;
            x = (*r).x;
            y = (*r).y-h/6.;
            painter.drawRect(x,y,w,h);
        }
        emit onUpdatefacesInfo(rectVect.size());

//    }
}

void myLabel::setFaceCount(FacesCount *fc)
{
    m_fc = fc;
}




