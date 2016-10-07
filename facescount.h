#ifndef FACESCOUNT_H
#define FACESCOUNT_H
#include <QMainWindow>
#include <QCamera>
#include <QCameraViewfinder>
#include <QLabel>
#include <QWidget>
#include <QThread>
#include <QImage>

#include <queue>

#include <opencv/cv.h>
#include <opencv2/core.hpp>
#include <objdetect.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio/videoio_c.h"
#include "cvfaces.h"
namespace Ui {
class FacesCount;
}

using namespace cv;
using namespace std;

class ImgThread;

class FacesCount : public QMainWindow
{
    Q_OBJECT

public:
    explicit FacesCount(QWidget *parent = 0);
    ~FacesCount();

private:
    Ui::FacesCount *ui;
    VideoCapture *camera;
    QTimer *timer;
    ImgThread *imgthread;
    vector<Rect> rectVector;
    bool isAsked;
public:
    bool isalive;

public:
    static QImage cvMat2QImage(const Mat& mat);
    static Mat QImage2cvMat(QImage image);
    void triggerSrcImgFlag();
    vector<Rect> getRectVector();
public slots:
    void cameraRead();
    void updateFacesPosition(vector<Rect> r);
    void updateFacesInfo(int count);
};

//ImgThread
class ImgThread:public QThread{
    Q_OBJECT
public:
    ImgThread(QObject* parent);
    ~ImgThread();
    void run();

    void askForSrcImg(QObject *p);

public:
    queue<QImage> m_imgQueue;

private:
    Cvfaces *cvface;
    QObject* m_parent;
    QImage tempImg;
signals:
    void onFacesFound(vector<Rect> r);
};


//myLabel
class myLabel:public QLabel{
    Q_OBJECT
public:
    myLabel(QWidget *p);
    virtual ~myLabel();
    void paintEvent(QPaintEvent* e);
    void setFaceCount(FacesCount *);
private:
    FacesCount* m_fc;
signals:
    void onUpdatefacesInfo(int);
};


#endif // FACESCOUNT_H
