#ifndef THIEN_LIN_SHADOWS_H
#define THIEN_LIN_SHADOWS_H

#include "mainwindow.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "Eigen/Dense"

std::vector<Shadow> generateShadowsTL(const cv::Mat& inputImage, int K, int N);
std::vector<Shadow> generateShadowsTL(const cv::Mat& inputImage, int K, int N, int sliceNumber);
cv::Mat decodeShadowsTL(const std::vector<Shadow>& selectedShadows, int K);
cv::Mat decodeShadowsTLdebug(const std::vector<Shadow>& selectedShadows, int K);

#endif // THIEN_LIN_SHADOWS_H
