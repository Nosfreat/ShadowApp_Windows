#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "mainwindow.h"
#include "thien_lin_shadows.h"
#include <QByteArray>
#include <QBuffer>
#include <QString>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>

// Function to process a vector of Shadow objects and update their text fields
void convertShadowsToStr(std::vector<Shadow>& shadows);
void changeShadowEssentialValue(std::vector<Shadow>& shadows, bool isEssential);
std::vector<cv::Mat> sliceImageVertically(const cv::Mat& image, int n, bool usePadding);
std::vector<cv::Mat> sliceExtremeImageVertically(const cv::Mat& image, int n, int sktWangLingAmount, int shadowsThreshold, bool usePadding);
cv::Mat mergeSubImages(const std::vector<cv::Mat>& subImages);
std::vector<Shadow> composeShadows(const std::vector<Shadow>& allSubShadows, int shadowsAmount, int shadowsThreshold);
std::vector<Shadow> decomposeShadows(const std::vector<Shadow>& composedShadows, int shadowsAmount, int shadowsThreshold);
std::vector<Shadow> copyShadowsWithNumber(const std::vector<Shadow>& shadows, int specifiedNumber);
std::vector<Shadow> copyShadowsWithSliceNumber(const std::vector<Shadow>& shadows, int specifiedNumber);
std::vector<Shadow> copyEssentialShadows (const std::vector<Shadow>& shadows, bool returnEssential);
cv::Mat decodeLiuYang (const std::vector<Shadow>& shadows, int essentialThreshold, int essentialNumber, int shadowsThreshold, int shadowsAmount);
bool LinYangValueCheck(int essentialThreshold, int essentialNumber, int shadowsThreshold, int shadowsAmount);

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);
    void setImage(const cv::Mat &image);

private:
    QLabel *imageLabel;
};


#endif // FUNCTIONS_H
