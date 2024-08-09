#include "functions.h"
#include "mainwindow.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
// #include <iostream>

extern int globalTempShadowSize; // in a perfect world, a variable like me would not exist
extern std::vector<Shadow> globaltemporaryShadows;


std::vector<std::string> generatePlaceholder(int amount, int selected_encoding) {
    std::vector<std::string> generated_shadows_list;

    for (int i = 1; i <= amount; ++i) {
        generated_shadows_list.push_back("Type: " + std::to_string(selected_encoding+1) + " Number: " + std::to_string(i));
    }

    return generated_shadows_list;
}


// Function to convert a shadow image to a string of size 50
std::string convertImageToString(const cv::Mat& image) {
    const std::string charset = "0123456789abcdefghijklmnopqrstuvwxyz";
    const int charsetSize = charset.size();
    std::string result;
    result.reserve(50);

    // We will take the first 50 pixels from the image in row-major order
    int count = 0;
    for (int i = 0; i < image.rows && count < 100; ++i) {
        for (int j = 0; j < image.cols && count < 100; ++j) {
            int pixelValue = image.at<uchar>(i, j);
            char charValue = charset[pixelValue * charsetSize / 256];
            result += charValue;
            ++count;
        }
    }

    // If the image has less than 50 pixels, pad the result with '0'
    while (result.size() < 50) {
        result += '0';
    }

    return result;
}


// Function to process a vector of Shadow objects and update their text fields
void convertShadowsToStr(std::vector<Shadow>& shadows) {
    for (auto& shadow : shadows) {
        shadow.text = convertImageToString(shadow.image);
    }
}


ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent)
{
    imageLabel = new QLabel(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    setLayout(layout);
}


void ImageViewer::setImage(const cv::Mat &image)
{
    // Check if the image is grayscale
    if (image.channels() == 1) {
        // Convert cv::Mat to QImage (grayscale)
        QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8);
        QPixmap pixmap = QPixmap::fromImage(qImage);
        imageLabel->setPixmap(pixmap);
        imageLabel->adjustSize();
    } else {
        // Handle the case where the image is not grayscale (optional)

    }
}


cv::Mat padImage(const cv::Mat& image, int n) {
    int newWidth = ((image.cols + n - 1) / n) * n;  // Find the next multiple of n
    cv::Mat paddedImage;
    int rightPadding = newWidth - image.cols;
    cv::copyMakeBorder(image, paddedImage, 0, 0, 0, rightPadding, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    return paddedImage;
}


cv::Mat cropImage(const cv::Mat& image, int n) {
    int newWidth = (image.cols / n) * n;  // Largest multiple of n less than image.cols
    cv::Rect cropRegion(0, 0, newWidth, image.rows);
    return image(cropRegion);
}


// Function to calculate the Least Common Multiple (LCM)
int lcm(int a, int b) {
    return (a * (b / std::gcd(a, b)));
}


// cv::Mat smartPadAndCrop(const cv::Mat &image, int X, int sktWangLingAmount, int shadowsThreshold, bool usePadding) {
//     int rows = image.rows;
//     int cols = image.cols;

//     // Step 1: Ensure that the image width is divisible by X
//     int initialCols = (cols % X == 0) ? cols : cols + (X - (cols % X));

//     // Step 2: Ensure that the resulting width after dividing by X is also divisible by X
//     int intermediateWidth = initialCols / X;
//     intermediateWidth = (intermediateWidth % X == 0) ? intermediateWidth : intermediateWidth + (X - (intermediateWidth % X));
//     int finalIntermediateWidth = intermediateWidth * X;

//     // Step 3: Ensure that the final `temporaryShadow` is divisible by shadowsThreshold
//     int temporaryShadowWidth = finalIntermediateWidth * sktWangLingAmount;
//     temporaryShadowWidth = (temporaryShadowWidth % shadowsThreshold == 0) ? temporaryShadowWidth : temporaryShadowWidth + (shadowsThreshold - (temporaryShadowWidth % shadowsThreshold));
//     int finalTemporaryShadowWidth = (temporaryShadowWidth / sktWangLingAmount);


//     // std::cout << "finalTemporaryShadowWidth size: " << finalTemporaryShadowWidth << std::endl;
//     // Step 4: Calculate the LCM of both widths to ensure both conditions are met
//     int finalCols = lcm(finalIntermediateWidth, finalTemporaryShadowWidth);
//     // std::cout << "lcm size: " << finalCols << std::endl;
//     // Step 5: Ensure finalCols is divisible by X
//     finalCols = (finalCols % X == 0) ? finalCols : finalCols + (X - (finalCols % X));
//     std::cout << "finalCols size: " << finalCols << std::endl;

//     cv::Mat resultImage;

//     if (usePadding) {
//         // Always pad the image to the calculated finalCols
//         cv::copyMakeBorder(image, resultImage, 0, 0, 0, finalCols - cols, cv::BORDER_CONSTANT, cv::Scalar(0));
//     } else {
//         // Crop the image to the calculated finalCols
//         if (finalCols > cols) {
//             // Pad if the calculated width is greater than the current width
//             cv::copyMakeBorder(image, resultImage, 0, 0, 0, finalCols - cols, cv::BORDER_CONSTANT, cv::Scalar(0));
//         } else {
//             // Crop if the calculated width is less than or equal to the current width
//             resultImage = image(cv::Rect(0, 0, finalCols, rows)).clone();
//         }
//     }

//     return resultImage;
// }


cv::Mat smartPadAndCrop(const cv::Mat& img, int X, int sktWangLingAmount, int shadowsThreshold, bool usePadding) {
    int originalWidth = img.cols;
    int newWidth = originalWidth;
    cv::Mat adjustedImg;

    if (usePadding) {
        while (newWidth % X != 0 || (newWidth / X) % X != 0 || ((newWidth / X) / X) * sktWangLingAmount % shadowsThreshold != 0) {
            newWidth++;
        }
        adjustedImg = cv::Mat(img.rows, newWidth, img.type(), cv::Scalar::all(0));
        img.copyTo(adjustedImg(cv::Rect(0, 0, img.cols, img.rows)));
    } else {
        while (newWidth % X != 0 || (newWidth / X) % X != 0 || ((newWidth / X) / X) * sktWangLingAmount % shadowsThreshold != 0) {
            newWidth--;
        }
        adjustedImg = img(cv::Rect(0, 0, newWidth, img.rows)).clone();
        // Failsafe mechanism: Switch to padding if newWidth is less than 90% of original width
        if (newWidth < 0.9 * originalWidth) {
            // std::cout << "Failsafe, width is too small!!! newWidth: " << newWidth << std::endl;
            newWidth = originalWidth;
            while (newWidth % X != 0 || (newWidth / X) % X != 0 || ((newWidth / X) / X) * sktWangLingAmount % shadowsThreshold != 0) {
                newWidth++;
            }
            adjustedImg = cv::Mat(img.rows, newWidth, img.type(), cv::Scalar::all(0));
            img.copyTo(adjustedImg(cv::Rect(0, 0, img.cols, img.rows)));
        }
    }
    // std::cout << "newWidth: " << newWidth << std::endl;


    // // Adjust the image dimensions based on newWidth
    // cv::Mat adjustedImg;
    // if (newWidth > img.cols) {
    //     // Padding the image
    //     adjustedImg = cv::Mat(img.rows, newWidth, img.type(), cv::Scalar::all(0));
    //     img.copyTo(adjustedImg(cv::Rect(0, 0, img.cols, img.rows)));
    // } else if (newWidth < img.cols) {
    //     // Cropping the image
    //     adjustedImg = img(cv::Rect(0, 0, newWidth, img.rows)).clone();
    // } else {
    //     // No change needed
    //     adjustedImg = img.clone();
    // }

    return adjustedImg;
}



std::vector<cv::Mat> sliceExtremeImageVertically(const cv::Mat& image, int n, int sktWangLingAmount, int shadowsThreshold, bool usePadding = false) {
    cv::Mat processedImage = smartPadAndCrop(image, n, sktWangLingAmount, shadowsThreshold, usePadding);
    // std::cout << "processedImage size: " << processedImage.size() << std::endl;
    // cv::Mat dump = padRight(image, n, sktWangLingAmount, shadowsThreshold, usePadding);

    std::vector<cv::Mat> slices;
    int sliceWidth = processedImage.cols / n;

    for (int i = 0; i < n; ++i) {
        int startCol = i * sliceWidth;
        int endCol = startCol + sliceWidth;

        cv::Rect sliceRegion(startCol, 0, endCol - startCol, processedImage.rows);
        cv::Mat slice = processedImage(sliceRegion);
        slices.push_back(slice);

        // std::string fname = "KEYS_CPP/I" + std::to_string(i + 1) + ".bmp";
        // cv::imwrite(fname, slice);
    }

    return slices;
}


std::vector<cv::Mat> sliceImageVertically(const cv::Mat& image, int n, bool usePadding = false) {
    cv::Mat processedImage;
    if (usePadding) {
        processedImage = padImage(image, n);
    } else {
        processedImage = cropImage(image, n);
    }
    // std::cout << "processedImage size: " << processedImage.cols << "x" << processedImage.rows << std::endl;

    std::vector<cv::Mat> slices;
    int sliceWidth = processedImage.cols / n;

    for (int i = 0; i < n; ++i) {
        int startCol = i * sliceWidth;
        int endCol = startCol + sliceWidth;

        cv::Rect sliceRegion(startCol, 0, endCol - startCol, processedImage.rows);
        cv::Mat slice = processedImage(sliceRegion);
        slices.push_back(slice);

        // std::string fname = "KEYS_CPP/I" + std::to_string(i + 1) + ".bmp";
        // cv::imwrite(fname, slice);
    }

    return slices;
}


cv::Mat mergeSubImages(const std::vector<cv::Mat>& subImages) {
    // Assuming all sub-images are of the same size and should be concatenated vertically
    cv::Mat composedImage;
    cv::hconcat(subImages, composedImage);
    return composedImage;
}


/**
 * Function to compose shadows from all sub-shadows
 * @param allSubShadows: A vector containing all sub-shadows
 * @param shadowsAmount: The total number of shadows
 * @param shadowsThreshold: The threshold for decoding shadows
 * @return A vector containing the composed shadows (shadowsAmount in total)
 * @note Function assumes that all sub-shadows are of the same size, and that the shadows are concatenated vertically
 *
 * The function will compose shadows based on Wang-Lin's method (2013).
*/
std::vector<Shadow> composeShadows(const std::vector<Shadow>& allSubShadows, int shadowsAmount, int shadowsThreshold) {
    std::vector<Shadow> composedShadows(shadowsAmount);

    for (int i = 0; i < shadowsAmount; ++i) {
        // std::cout << "Now processing i == " << i << std::endl;
        Shadow composedShadow;
        composedShadow.isEssential = false;
        composedShadow.number = i + 1;
        composedShadow.sliceNumber = -1; // This is a composed shadow, not from a specific slice
        int WangLinAmount = 2 * shadowsAmount - shadowsThreshold;
        std::vector<cv::Mat> subImages;
        for (int j = 0; j < shadowsAmount; ++j) {
            // std::cout << "Now processing j == " << j << std::endl;

                if (j == i) {
                    for (int k = j; k < j + shadowsAmount - shadowsThreshold + 1; ++k) {
                        // std::cout << "Adding j==i, k == " << k << std::endl;
                        subImages.push_back(allSubShadows[j*WangLinAmount + k].image);
                        // std::cout << "Shadow Number: " << allSubShadows[j*WangLinAmount + k].number << " , slice number: " << allSubShadows[j*WangLinAmount + k].sliceNumber << std::endl;
                    }
                } else if (j > i) {
                    // std::cout << "Adding j>i, i == " << i << std::endl;
                    subImages.push_back(allSubShadows[j * WangLinAmount + i].image);
                    // std::cout << "Shadow Number: " << allSubShadows[j * WangLinAmount + i].number << " , slice number: " << allSubShadows[j * WangLinAmount + i].sliceNumber << std::endl;
                } else {
                    // std::cout << "Adding j<i, i+n-t == " << i+shadowsAmount-shadowsThreshold << std::endl;
                    subImages.push_back(allSubShadows[j * WangLinAmount + (i + shadowsAmount - shadowsThreshold)].image);
                    // std::cout << "Shadow Number: " << allSubShadows[j * WangLinAmount + (i + shadowsAmount - shadowsThreshold)].number << " , slice number: " << allSubShadows[j * WangLinAmount + (i + shadowsAmount - shadowsThreshold)].sliceNumber << std::endl;
                }
        }
        // std::cout << "=================" << std::endl;
        // Merge all selected sub-images into one composed shadow image
        composedShadow.image = mergeSubImages(subImages);
        // composedShadow.text = "Composed Shadow " + std::to_string(i + 1);
        composedShadows[i] = composedShadow;

        // std::string fname = "KEYS_CPP/K" + std::to_string(i + 1) + ".bmp";
        // if (!cv::imwrite(fname, composedShadow.image)) {
        //     std::cout << "Error saving image: " << fname << std::endl;
        // }
    }

    return composedShadows;
}


std::vector<Shadow> decomposeShadows(const std::vector<Shadow>& composedShadows, int shadowsAmount, int shadowsThreshold) {
    std::vector<Shadow> allSubShadows;
    int WangLinAmount = 2 * shadowsAmount - shadowsThreshold;

    for (const auto& composedShadow : composedShadows) {
        std::vector<cv::Mat> slices = sliceImageVertically(composedShadow.image, WangLinAmount, false);
        // std::cout << "composedShadow.number: " << composedShadow.number << std::endl;

        int i = composedShadow.number - 1;
        for (int j = 0; j < shadowsAmount; ++j) {
            if (j == i) {
                for (int k = j; k < j +  shadowsAmount - shadowsThreshold + 1; ++k) {
                    Shadow subShadow;
                    subShadow.isEssential = false;
                    subShadow.sliceNumber = j + 1;
                    subShadow.number = k + 1;
                    subShadow.image = slices[k];
                    // std::cout << "slice.number j==i: " << k << std::endl;
                    subShadow.text = "Sub Shadow " + std::to_string(j + 1) + "-" + std::to_string(k + 1);
                    allSubShadows.push_back(subShadow);
                }
            // i have no idea why these 2 cases have to have selectors (shadow.image) like that
            // j==i has same as in composeShadows minus the offset and these two dont. it just works
            } else if (j > i) {
                Shadow subShadow;
                subShadow.isEssential = false;
                subShadow.sliceNumber = j + 1;
                subShadow.number = i + 1;
                subShadow.image = slices[j+ shadowsAmount - shadowsThreshold];
                // std::cout << "slice.number j > i: " << j+ shadowsAmount - shadowsThreshold << std::endl;
                subShadow.text = "Sub Shadow " + std::to_string(j + 1) + "-" + std::to_string(i + 1);
                allSubShadows.push_back(subShadow);
            } else {
                Shadow subShadow;
                subShadow.isEssential = false;
                subShadow.sliceNumber = j + 1;
                subShadow.number = i + 1 + shadowsAmount - shadowsThreshold;
                subShadow.image = slices[j];
                // std::cout << "slice.number j < i: " << j << std::endl;
                subShadow.text = "Sub Shadow " + std::to_string(j + 1) + "-" + std::to_string(i + 1);
                allSubShadows.push_back(subShadow);
            }
        }
    }

    return allSubShadows;
}


std::vector<Shadow> copyShadowsWithSliceNumber(const std::vector<Shadow>& shadows, int specifiedNumber) {
    std::vector<Shadow> result;

    for (const auto& shadow : shadows) {
        if (shadow.sliceNumber == specifiedNumber) {
            // std::cout << "Copied Shadow Number: " << shadow.number << ", Slice Number: " << shadow.sliceNumber << std::endl;
            result.push_back(shadow);
        }
    }

    return result;
}


std::vector<Shadow> copyShadowsWithNumber(const std::vector<Shadow>& shadows, int specifiedNumber) {
    std::vector<Shadow> result;

    for (const auto& shadow : shadows) {
        if (shadow.number == specifiedNumber) {
            // std::cout << "Copied Shadow Number: " << shadow.number << ", Slice Number: " << shadow.sliceNumber << std::endl;
            result.push_back(shadow);
        }
    }

    return result;
}

/**
 * @param shadows: A vector containing shadows
 * @param returnEssential: If true return only essential, otherwise only non-essential
 * @return A vector containing the selected shadows (shadowsAmount in total)
*/
std::vector<Shadow> copyEssentialShadows (const std::vector<Shadow>& shadows, bool returnEssential) {
    std::vector<Shadow> result;

    for (const auto& shadow : shadows) {
        if (shadow.isEssential == returnEssential) {
            result.push_back(shadow);
        }
    }

    return result;
}


void changeShadowEssentialValue(std::vector<Shadow>& shadows, bool isEssential) {
    for (auto& shadow : shadows) {
        shadow.isEssential = isEssential;
    }
    return;
}

/**
 * This function is a hack to calculate size of part of essential shadow which is temporary shadow
 * not one of the subtemo shadows.
 * We can calculate size diff between essential and nonessential shadow, since we know that essential shadow
 * is bigger than essential by the size of temporary shadow Wi.
 *
 * From the formula of size of essential sahdow we solve for size of original image I. Then we plug that I into
 * equation for size diff between essential and non essential shadow.
 *
 * In the end we get size diff is equal to: |Se|*k/(2k-t) where |Se| is size of essential shadow
 * @param Se: Essential shadow
 * @param k: Shadow threshold value
 * @param t: Essential threshold value
 * @return Value of DeltaS (size diff between essential and non essential shadow)
*/
int calculateDeltaS(const cv::Mat& Se, int k, int t) {
    // Calculate total number of pixels in the image
    // std::cout << "deltaS calc k: "<< k << std::endl;
    // std::cout << "deltaS calc t: "<< t  << std::endl;

    int totalPixels = Se.rows * Se.cols;
    // std::cout << "deltaS calc totalPixels: "<< totalPixels << std::endl;
    // Calculate delta S using the provided formula
    int deltaS = std::abs(totalPixels * k) / (2 * k - t);
    // std::cout << "deltaS calc deltaS: "<< deltaS << std::endl;
    return deltaS;
}


int calculateNonEssentialSize(const cv::Mat& Se, int k, int t) {
    // Calculate total number of pixels in the image
    // std::cout << "deltaS calc k: "<< k << std::endl;
    // std::cout << "deltaS calc t: "<< t  << std::endl;

    int totalPixels = Se.rows * Se.cols;
    // std::cout << "deltaS calc totalPixels: "<< totalPixels << std::endl;
    // Calculate delta S using the provided formula
    int deltaS = (totalPixels * (k - t)) / (2 * k - t);
    // std::cout << "deltaS calc deltaS: "<< deltaS << std::endl;
    return deltaS;
}

/**
 * Functions that are commented out below are function that would work in perfect world (see globalTempShadowSize)
*/
// cv::Mat extractTempShadowFromEssential(const cv::Mat& inputImage, int deltaS) {
//     int rows = inputImage.rows;
//     int targetCols = deltaS / rows;

//     std::cout << "targetCols value: " << targetCols << std::endl;

//     cv::Rect roi(0, 0, targetCols, rows);
//     cv::Mat croppedImage = inputImage(roi).clone();

//     return croppedImage;
// }


cv::Mat extractTempShadowFromEssential(const cv::Mat& inputImage, int deltaS) {
    int rows = inputImage.rows;

    // std::cout << "targetCols value: " << deltaS << std::endl;

    cv::Rect roi(0, 0, deltaS, rows);
    cv::Mat croppedImage = inputImage(roi).clone();

    return croppedImage;
}


// cv::Mat extractAllSubTempShadowFromEssential(const cv::Mat& inputImage, int deltaS) {
//     int rows = inputImage.rows;
//     int targetCols = deltaS / rows;
//     int remainingCols = inputImage.cols - targetCols;

//     std::cout << "remainingCols value: " << remainingCols << std::endl;

//     cv::Rect roi(targetCols, 0, remainingCols, rows);
//     cv::Mat croppedImage = inputImage(roi).clone();
//     std::cout << "croppedImage size: " << croppedImage.cols << "x" << croppedImage.rows << std::endl;

//     return croppedImage;
// }


cv::Mat extractAllSubTempShadowFromEssential(const cv::Mat& inputImage, int deltaS) {
    int rows = inputImage.rows;
    int remainingCols = inputImage.cols - deltaS;

    // std::cout << "remainingCols value: " << remainingCols << std::endl;

    cv::Rect roi(deltaS, 0, remainingCols, rows);
    cv::Mat croppedImage = inputImage(roi).clone();
    // std::cout << "croppedImage size: " << croppedImage.cols << "x" << croppedImage.rows << std::endl;

    return croppedImage;
}


// cv::Mat extractAllSubTempShadowFromEssentialRight(const cv::Mat& inputImage, int deltaS) {
//     int rows = inputImage.rows;
//     int targetCols = deltaS / rows;
//     int remainingCols = inputImage.cols - targetCols;

//     std::cout << "remainingCols value: " << remainingCols << std::endl;

//     // Calculate the starting column for the ROI from the right edge
//     int startCol = inputImage.cols - remainingCols;

//     cv::Rect roi(startCol, 0, remainingCols, rows);
//     cv::Mat croppedImage = inputImage(roi).clone();
//     std::cout << "croppedImage size: " << croppedImage.cols << "x" << croppedImage.rows << std::endl;

//     return croppedImage;
// }


cv::Mat extractAllSubTempShadowFromEssentialRight(const cv::Mat& inputImage, int deltaS) {
    int rows = inputImage.rows;
    int remainingCols = inputImage.cols - deltaS;

    // std::cout << "remainingCols value: " << remainingCols << std::endl;

    // Calculate the starting column for the ROI from the right edge
    int startCol = inputImage.cols - remainingCols;

    cv::Rect roi(startCol, 0, remainingCols, rows);
    cv::Mat croppedImage = inputImage(roi).clone();
    // std::cout << "croppedImage size: " << croppedImage.cols << "x" << croppedImage.rows << std::endl;

    return croppedImage;
}


cv::Mat decodeLiuYang (const std::vector<Shadow>& shadows, int essentialThreshold, int essentialNumber, int shadowsThreshold, int shadowsAmount) {
    int sktAmount = essentialNumber + shadowsThreshold - essentialThreshold;
    int sktWangLingAmount = 2*sktAmount - shadowsThreshold;
    std::vector<Shadow> result;
    std::vector<Shadow> extractedAllTempShadow;
    std::vector<Shadow> extractedAllSubTempShadow;

    // std::cout << "Partitioning "  << std::endl;
    for(const auto& shadow : shadows){
        if(shadow.isEssential == true){
            // std::cout << "Slicing "  << std::endl;
            // int deltaS = calculateDeltaS(shadow.image, shadowsThreshold, essentialThreshold);
            // std::cout << "deltaS: " << deltaS << std::endl;

            // std::cout << "shadowImage size: " << shadow.image.cols << "x" << shadow.image.rows << std::endl;

            cv::Mat extractedTempShadow = extractTempShadowFromEssential(shadow.image, globalTempShadowSize);
            extractedAllTempShadow.push_back({extractedTempShadow, false, "", shadow.number, shadow.sliceNumber });
            // cv::imshow("extracted Temp", extractedTempShadow);
            // std::cout << "extractedTempShadow size: " << extractedTempShadow.cols << "x" << extractedTempShadow.rows << std::endl;


            cv::Mat extractedSubTempShadow = extractAllSubTempShadowFromEssentialRight(shadow.image, globalTempShadowSize);
            std::vector<cv::Mat> slicedSubTempShadows = sliceImageVertically(extractedSubTempShadow, (shadowsThreshold  - essentialThreshold));
            for(int i = 0; i < slicedSubTempShadows.size(); i++){
                // std::string windowName = cv::format("slicedSubTempShadows %d", i);
                // cv::imshow(windowName, slicedSubTempShadows[i]);
                int sliceNumber = -1;    //dont even ask why, if i knew why them this wouldnt be here
                if (shadowsThreshold == essentialNumber){
                    sliceNumber = (shadowsThreshold  - essentialThreshold + 1 + i) +1;
                }
                else {
                    sliceNumber = (shadowsThreshold  - essentialThreshold + 1 + i);
                }
                extractedAllSubTempShadow.push_back({slicedSubTempShadows[i], false, "", shadow.number, essentialNumber + i +1});
            }
            // std::string windowName = cv::format("Essential SubTemp %d", shadow.number);
            // cv::imshow(windowName, extractedSubTempShadow);
            // std::cout << "extractedSubTempShadow size: " << extractedSubTempShadow.cols << "x" << extractedSubTempShadow.rows << std::endl;
        }
        else{
            std::vector<cv::Mat> slicedSubTempShadows = sliceImageVertically(shadow.image, (shadowsThreshold  - essentialThreshold));
            for(int i = 0; i < slicedSubTempShadows.size(); i++){
                int sliceNumber = -1;   //dont even ask why, if i knew why them this wouldnt be here
                if (shadowsThreshold == essentialNumber){
                    sliceNumber = (shadowsThreshold  - essentialThreshold + 1 + i) +1;
                }
                else {
                    sliceNumber = (shadowsThreshold  - essentialThreshold + 1 + i);
                }
                extractedAllSubTempShadow.push_back({slicedSubTempShadows[i], false, "", shadow.number, essentialNumber + i +1});
            }
        }
    }
    // int k = 1;
    // // std::cout << "slcies size= "<< slices.size()  << std::endl;
    // for(const auto& shadow : extractedAllSubTempShadow){
    //     std::cout << "shadow number=  "  << shadow.number << " shadow slice: "<< shadow.sliceNumber << std::endl;
    //     std::string fname = "KEYS_CPP/SUBTEMP_" + std::to_string(shadow.number) + "_SL_" + std::to_string(shadow.sliceNumber) + ".bmp";
    //     cv::imwrite(fname, shadow.image);
    //     // std::string windowName = cv::format("SUBTEMP %d", shadow.number);
    //     // cv::imshow(windowName, shadow.image);
    //     // k++;
    // }

    // int m = 1;
    // // std::cout << "slcies size= "<< slices.size()  << std::endl;
    // for(const auto& shadow : extractedAllTempShadow){
    //     std::cout << "shadow number=  "  << shadow.number << " shadow slice: "<< shadow.sliceNumber << std::endl;
    //     std::string fname = "KEYS_CPP/TEMP_" + std::to_string(shadow.number) + "_SL_" + std::to_string(shadow.sliceNumber) + ".bmp";
    //     cv::imwrite(fname, shadow.image);
    // //     std::string windowName = cv::format("TEMP %d", m);
    // //     cv::imshow(windowName, shadow.image);
    // //     m++;
    // }
    std::vector<Shadow> copiedPartitions;
    std::vector<Shadow> wanglinPartitions;
    // return result;

    // again, dont even ask why
    if (shadowsThreshold == essentialNumber){
        // std::cout << "Override" << std::endl;
        for (int i = essentialNumber +1; i < (essentialNumber + shadowsThreshold - essentialThreshold +1); i++){
            // std::cout << "(shadowsThreshold - essentialThreshold +1): " << (shadowsThreshold - essentialThreshold +1)<< " (essentialNumber + shadowsThreshold - essentialThreshold +1): " << (essentialNumber + shadowsThreshold - essentialThreshold +1) << std::endl;
            // std::cout << "i: " << i << std::endl;

            copiedPartitions.clear();
            copiedPartitions = copyShadowsWithSliceNumber(extractedAllSubTempShadow, i);
            // std::cout << "copiedPartitions amount: " << copiedPartitions.size() << std::endl;
            // for(const auto& shadow : copiedPartitions){
                // std::cout << "shadow number=  "  << shadow.number << " shadow slice: "<< shadow.sliceNumber << std::endl;
            // }

            cv::Mat reconstructedPartition = decodeShadowsTL(copiedPartitions, shadowsThreshold);
            // decodedPartitions.push_back(reconstructedPartition);
            wanglinPartitions.push_back({reconstructedPartition, false, "", i, -1});
            // std::string fname = "KEYS_CPP/RECON_" + std::to_string(i) + "_SL_" + std::to_string(0) + ".bmp";
            // cv::imwrite(fname, reconstructedPartition);

            // std::cout << "reconstructedPartition size: " << reconstructedPartition.cols << "x" << reconstructedPartition.rows << std::endl;
            // std::string windowName = cv::format("reconstructedPartition %d", i);
            // cv::imshow(windowName, reconstructedPartition);
        }
    }
    else {
        // std::cout << "No Override" << std::endl;
        for (int i = essentialNumber +1; i < (essentialNumber + shadowsThreshold - essentialThreshold +1); i++){
            // std::cout << "(shadowsThreshold - essentialThreshold +1): " << (shadowsThreshold - essentialThreshold +1)<< " (essentialNumber + shadowsThreshold - essentialThreshold +1): " << (essentialNumber + shadowsThreshold - essentialThreshold +1) << std::endl;
            // std::cout << "i: " << i << std::endl;

            copiedPartitions.clear();
            copiedPartitions = copyShadowsWithSliceNumber(extractedAllSubTempShadow, i);
            // std::cout << "copiedPartitions amount: " << copiedPartitions.size() << std::endl;
            // for(const auto& shadow : copiedPartitions){
                // std::cout << "shadow number=  "  << shadow.number << " shadow slice: "<< shadow.sliceNumber << std::endl;
            // }

            cv::Mat reconstructedPartition = decodeShadowsTL(copiedPartitions, shadowsThreshold);
            // decodedPartitions.push_back(reconstructedPartition);
            wanglinPartitions.push_back({reconstructedPartition, false, "", i, -1});
            // std::string fname = "KEYS_CPP/RECON_" + std::to_string(i) + "_SL_" + std::to_string(0) + ".bmp";
            // cv::imwrite(fname, reconstructedPartition);

            // std::cout << "reconstructedPartition size: " << reconstructedPartition.cols << "x" << reconstructedPartition.rows << std::endl;
            // std::string windowName = cv::format("reconstructedPartition %d", i);
            // cv::imshow(windowName, reconstructedPartition);
        }
    }





    // int n = 1;
    // std::cout << "slcies size= "<< slices.size()  << std::endl;
    for(const auto& shadow : extractedAllTempShadow){
        // std::cout << "shadow number=  "  << shadow.number << " shadow slice: "<< shadow.sliceNumber << std::endl;
        wanglinPartitions.push_back(shadow);
        // std::string windowName = cv::format("TEMP %d", n);
        // cv::imshow(windowName, shadow.image);
        // n++;
    }

    // for(const auto& shadow : wanglinPartitions){
        // std::cout << "wanglinPartitions number=  "  << shadow.number << " wanglinPartitions slice: "<< shadow.sliceNumber << std::endl;
        // std::string windowName = cv::format("wanglinPartitions %d", shadow.number);
        // cv::imshow(windowName, shadow.image);
    // }

    std::vector<Shadow> partitionedShadows = decomposeShadows(wanglinPartitions, sktAmount, shadowsThreshold);
    // std::vector<Shadow> partitionedShadows = decomposeShadows(globaltemporaryShadows, sktAmount, shadowsThreshold);
    std::vector<cv::Mat> decodedPartitions;


    // for(const auto& shadow : partitionedShadows){
    //     std::cout << "partitionedShadows number=  "  << shadow.number << " partitionedShadows slice: "<< shadow.sliceNumber << std::endl;
    //     std::string windowName = cv::format("partitionedShadows %d", shadow.number);
    //     cv::imshow(windowName, shadow.image);
    // }

    for(int i = 0; i < sktAmount; i++){
        std::vector<Shadow> copiedPartitions;
        copiedPartitions = copyShadowsWithSliceNumber(partitionedShadows, i+1);

        cv::Mat reconstructedPartition = decodeShadowsTL(copiedPartitions, sktAmount);
        decodedPartitions.push_back(reconstructedPartition);
        // std::string fname = "DECOMPOSED_SHADOWS/RECON_" + std::to_string(i) + "_SL_" + std::to_string(0) + ".bmp";
        // cv::imwrite(fname, reconstructedPartition);

        // if (i == 3){
        // for (const auto& shadow : copiedPartitions) {
        //     std::cout << "Shadow Number: " << shadow.number << ", Slice Number: " << shadow.sliceNumber << std::endl;
        //     // std::string fname = "DECOMPOSED_SHADOWS/DS" + std::to_string(shadow.number) + "_" + std::to_string(shadow.sliceNumber) + ".bmp";
        //     // std::string windowName = cv::format("Partition %d", k);
        // }
        // std::cout << "======================== " << std::endl;

        // k+=1;
        // cv::imshow(windowName, shadow.image);
        // if (!cv::imwrite(fname, shadow.image)) {
        // std::cout << "Error saving image: " << fname << std::endl;
        // }
        // }
        // std::string windowName = cv::format("Partition %d", i + 1);
        // cv::imshow(windowName, reconstructedPartition);
        // cv::waitKey(0);
        // }
    }

    // int p =1;
    // for(const auto& img : decodedPartitions){
    //     // std::cout << "decodedPartitions number=  "  << shadow.number << " decodedPartitions slice: "<< shadow.sliceNumber << std::endl;
    //     std::string windowName = cv::format("decodedPartitions %d", p);
    //     cv::imshow(windowName, img);
    //     p++;
    // }

    cv::Mat reconstructed = mergeSubImages(decodedPartitions);
    // cv::imshow("reconstructed",reconstructed);

    return reconstructed;
}


bool LinYangValueCheck(int essentialThreshold = 0, int essentialNumber =0 , int shadowsThreshold = 0, int shadowsAmount = 0){
    // std::cout << "essentialThreshold: " << essentialThreshold << ", essentialNumber: " << essentialNumber << ", shadowsThreshold: " << shadowsThreshold << ", shadowsAmount: " << shadowsAmount << std::endl;
    if (essentialThreshold < 1 && essentialNumber < 1 && shadowsThreshold < 1 && shadowsAmount < 1 ){
        return false;
    }

    if ( !(essentialThreshold <= essentialNumber && essentialNumber <= shadowsAmount) ){
        return false;
    }
    else if ( !(essentialThreshold < shadowsThreshold && shadowsThreshold <= shadowsAmount) ){
        return false;
    }
    else if ( !(shadowsThreshold+essentialNumber <= shadowsAmount+essentialThreshold) ){
        return false;
    }
    else {
        return true;
    }
}
