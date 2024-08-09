#include <iostream>
#include <filesystem>
#include "thien_lin_shadows.h"


std::vector<std::vector<std::vector<uint8_t>>> initializeB(const cv::Mat& A, int H, int W, int K);
std::vector<std::vector<std::vector<uint8_t>>> initializeC(const std::vector<std::vector<std::vector<uint8_t>>>& B, int H, int W, int K, int N);
// void saveImages(const std::vector<std::vector<std::vector<uint8_t>>>& C, int H, int W, int N, std::vector<Shadow>& shadows);
// void saveImages(const std::vector<std::vector<std::vector<uint8_t>>>& C, int H, int W, int N, std::vector<Shadow>& shadows, int sliceNumber);
void saveImages(const std::vector<Shadow>& shadows, const std::string& outputDir);



// Polynomial class for evaluating polynomials at given points
class Polynomial {
public:
    Polynomial(const std::vector<int>& coeffs) : coefficients(coeffs) {}

    int operator()(int x) const {
        int result = 0;
        int power = 1;
        for (int coeff : coefficients) {
            result = (result + coeff * power) % 251;
            power = (power * x) % 251;
        }
        return result;
    }

private:
    std::vector<int> coefficients;
};

// Function to process the image and generate shadows
std::vector<Shadow> generateShadowsTL(const cv::Mat& inputImage, int K, int N) {
    // Ensure the input image is grayscale
    cv::Mat A = inputImage.clone();
    if (A.channels() > 1) {
        cv::cvtColor(A, A, cv::COLOR_BGR2GRAY);
    }
    cv::min(A, 250, A);
    int H = A.rows;
    int W = A.cols / K;

    // Initialize the B matrix
    std::vector<Eigen::MatrixXi> B(H, Eigen::MatrixXi(W, K));
    for (int P = 0; P < H; ++P) {
        for (int Q = 0; Q < W; ++Q) {
            for (int R = 0; R < K; ++R) {
                int colIndex = Q + (W * R);
                if (colIndex >= A.cols) {
                    break;
                }
                B[P](Q, R) = A.at<uchar>(P, colIndex);
            }
        }
    }

    // Initialize the C matrix
    std::vector<Eigen::MatrixXi> C(H, Eigen::MatrixXi(W, N));
    for (int P = 0; P < H; ++P) {
        for (int Q = 0; Q < W; ++Q) {
            std::vector<int> Y(K);
            for (int R = 0; R < K; ++R) {
                Y[R] = B[P](Q, R);
            }
            std::reverse(Y.begin(), Y.end()); // Reversing Y for correct order
            Polynomial poly(Y);
            for (int R = 1; R <= N; ++R) {
                C[P](Q, R - 1) = poly(R) % 251;
            }
        }
    }

    // Create shadows
    std::vector<Shadow> shadows;
    for (int R = 1; R <= N; ++R) {
        cv::Mat img(H, W, CV_8UC1);
        for (int P = 0; P < H; ++P) {
            for (int Q = 0; Q < W; ++Q) {
                img.at<uchar>(P, Q) = C[P](Q, R - 1);
            }
        }
        Shadow shadow;
        shadow.image = img;
        shadow.isEssential = false;
        shadow.text = "Generated";
        shadow.number = R;
        // std::cout << "R: " << R << std::endl;
        shadow.sliceNumber = -1;
        shadows.push_back(shadow);
    }

    // saveImages(shadows, "KEYS_CPP");
    return shadows;
}


// Function to process the image and generate shadows
std::vector<Shadow> generateShadowsTL(const cv::Mat& inputImage, int K, int N, int sliceNumber) {
    // Ensure the input image is grayscale
    cv::Mat A = inputImage.clone();
    if (A.channels() > 1) {
        cv::cvtColor(A, A, cv::COLOR_BGR2GRAY);
    }
    cv::min(A, 250, A);
    int H = A.rows;
    int W = A.cols / K;

    // Initialize the B matrix
    std::vector<Eigen::MatrixXi> B(H, Eigen::MatrixXi(W, K));
    for (int P = 0; P < H; ++P) {
        for (int Q = 0; Q < W; ++Q) {
            for (int R = 0; R < K; ++R) {
                int colIndex = Q + (W * R);
                if (colIndex >= A.cols) {
                    break;
                }
                B[P](Q, R) = A.at<uchar>(P, colIndex);
            }
        }
    }

    // Initialize the C matrix
    std::vector<Eigen::MatrixXi> C(H, Eigen::MatrixXi(W, N));
    for (int P = 0; P < H; ++P) {
        for (int Q = 0; Q < W; ++Q) {
            std::vector<int> Y(K);
            for (int R = 0; R < K; ++R) {
                Y[R] = B[P](Q, R);
            }
            std::reverse(Y.begin(), Y.end()); // Reversing Y for correct order
            Polynomial poly(Y);
            for (int R = 1; R <= N; ++R) {
                C[P](Q, R - 1) = poly(R) % 251;
            }
        }
    }

    // Create shadows
    std::vector<Shadow> shadows;
    for (int R = 1; R <= N; ++R) {
        cv::Mat img(H, W, CV_8UC1);
        for (int P = 0; P < H; ++P) {
            for (int Q = 0; Q < W; ++Q) {
                img.at<uchar>(P, Q) = C[P](Q, R - 1);
            }
        }
        Shadow shadow;
        shadow.image = img;
        shadow.isEssential = false;
        shadow.text = "Generated";
        shadow.number = R;
        // std::cout << "R: " << R << std::endl;
        shadow.sliceNumber = sliceNumber;
        shadows.push_back(shadow);
    }

    return shadows;
}


// Function to save shadows to disk
void saveImages(const std::vector<Shadow>& shadows, const std::string& outputDir) {
    std::filesystem::create_directories(outputDir);
    for (const auto& shadow : shadows) {
        std::string fileName = outputDir + "/K" + std::to_string(shadow.number) + ".bmp";
        cv::imwrite(fileName, shadow.image);
        std::cout << shadow.text << " saved as " << fileName << std::endl;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////

// std::vector<Shadow> generateShadowsTL(const cv::Mat& inputImage, int K, int N) {

//     int H = inputImage.rows;
//     int W = inputImage.cols / K;

//     auto B = initializeB(inputImage, H, W, K);
//     auto C = initializeC(B, H, W, K, N);

//     std::vector<Shadow> shadows;
//     saveImages(C, H, W, N, shadows);

//     return shadows;
// }


// std::vector<Shadow> generateShadowsTL(const cv::Mat& inputImage, int K, int N, int sliceNumber) {
//     // std::cout << "generateShadowsTL input Size: " << inputImage.size() << std::endl;

//     int H = inputImage.rows;
//     int W = inputImage.cols / K;

//     auto B = initializeB(inputImage, H, W, K);
//     auto C = initializeC(B, H, W, K, N);

//     std::vector<Shadow> shadows;
//     saveImages(C, H, W, N, shadows, sliceNumber);
//     // for (const auto& shadow : shadows) {
//         // std::cout << "generateShadowsTL shadow Size: " << shadow.image.size() << std::endl;
//     // }
//     return shadows;
// }


// std::vector<std::vector<std::vector<uint8_t>>> initializeB(const cv::Mat& A, int H, int W, int K) {
//     std::vector<std::vector<std::vector<uint8_t>>> B(H, std::vector<std::vector<uint8_t>>(W, std::vector<uint8_t>(K, 0)));
//     for (int P = 0; P < H; ++P) {
//         for (int Q = 0; Q < W; ++Q) {
//             for (int R = 0; R < K; ++R) {
//                 B[P][Q][R] = A.at<uint8_t>(P, Q + (W * R));
//             }
//         }
//     }
//     return B;
// }


// std::vector<std::vector<std::vector<uint8_t>>> initializeC(const std::vector<std::vector<std::vector<uint8_t>>>& B, int H, int W, int K, int N) {
//     std::vector<std::vector<std::vector<uint8_t>>> C(H, std::vector<std::vector<uint8_t>>(W, std::vector<uint8_t>(N, 0)));
//     for (int P = 0; P < H; ++P) {
//         for (int Q = 0; Q < W; ++Q) {
//             std::vector<uint8_t> Y(K);
//             for (int R = 0; R < K; ++R) {
//                 Y[R] = B[P][Q][R];
//             }

//             for (int R = 0; R < N; ++R) {
//                 int poly_val = 0;
//                 for (int i = 0; i < K; ++i) {
//                     poly_val = poly_val * (R + 1) + Y[i];
//                 }
//                 C[P][Q][R] = poly_val % 251;
//             }
//         }
//     }
//     return C;
// }

// void saveImages(const std::vector<std::vector<std::vector<uint8_t>>>& C, int H, int W, int N, std::vector<Shadow>& shadows) {
//     std::filesystem::create_directory("KEYS_CPP");

//     for (int R = 0; R < N; ++R) {
//         cv::Mat img(H, W, CV_8UC1);
//         for (int P = 0; P < H; ++P) {
//             for (int Q = 0; Q < W; ++Q) {
//                 img.at<uint8_t>(P, Q) = C[P][Q][R];
//             }
//         }

//         std::string fname = "KEYS_CPP/K" + std::to_string(R + 1) + ".bmp";
//         if (!cv::imwrite(fname, img)) {
//             std::cerr << "Error saving image: " << fname << std::endl;
//         }

//         shadows.push_back({img, false, "", R + 1});

//         // Print the Shadow struct
//         // std::cout << "Shadow " << R + 1 << " added: "
//         //           << "isEssential=" << true
//         //           << ", text=\"" << "" << "\""
//         //           << ", number=" << R + 1
//         //           << std::endl;
//     }
// }

// void saveImages(const std::vector<std::vector<std::vector<uint8_t>>>& C, int H, int W, int N, std::vector<Shadow>& shadows, int sliceNumber) {
//     std::filesystem::create_directory("KEYS_CPP");
//     for (int R = 0; R < N; ++R) {
//         cv::Mat img(H, W, CV_8UC1);
//         for (int P = 0; P < H; ++P) {
//             for (int Q = 0; Q < W; ++Q) {
//                 img.at<uint8_t>(P, Q) = C[P][Q][R];
//             }
//         }

//         std::string fname = "KEYS_CPP/K" + std::to_string(R + 1) + ".bmp";
//         if (!cv::imwrite(fname, img)) {
//             std::cerr << "Error saving image: " << fname << std::endl;
//         }

//         shadows.push_back({img, false, "", R + 1, sliceNumber});

//         // Print the Shadow struct
//         // std::cout << "Shadow " << R + 1 << " added: "
//         //           << "isEssential=" << true
//         //           << ", text=\"" << "" << "\""
//         //           << ", number=" << R + 1
//         //           << ", sliceNumber=" << sliceNumber
//         //           << std::endl;
//     }
// }

////////////////////////////////////////////////
////////////////////DECODE//////////////////////
////////////////////////////////////////////////

std::vector<double> lj_num(const std::vector<double>& x, long long j, long long m) {
    std::vector<double> f = {1.0};
    for (long long p = 1; p <= m; ++p) {
        if (p == j) {
            continue;
        }
        std::vector<double> temp = {1.0, -x[p-1]};
        std::vector<double> result(f.size() + temp.size() - 1, 0.0);

        for (size_t i = 0; i < f.size(); ++i) {
            for (size_t k = 0; k < temp.size(); ++k) {
                result[i + k] += f[i] * temp[k];
            }
        }
        f = result;
    }
    return f;
}

double lj_den(const std::vector<double>& x, long long j, long long m) {
    double f = 1.0;
    for (long long p = 1; p <= m; ++p) {
        if (p == j) {
            continue;
        }
        f *= (x[j-1] - x[p-1]);
    }
    return f;
}

long long invMod(long long x, long long y, long long m) {
    if (y == 0) {
        return 1;
    }

    long long p = invMod(x, y / 2, m) % m;
    p = (p * p) % m;

    if (y % 2 == 0) {
        return p;
    } else {
        return (x * p) % m;
    }
}

long long modFrac(long long a, long long b, long long m) {
    if (b < 0) {
        b = -b;
        a = -a;
    }
    b = invMod(b, m - 2, m);
    long long a_mod_m = (a % m + m) % m;
    long long retval = (a_mod_m * b % m) % m;
    return retval;
}

std::vector<long long> modLagPol(const std::vector<long long>& y, const std::vector<double>& x, long long v) {
    long long m = y.size();
    std::vector<std::vector<double>> a(m);
    std::vector<double> b(m);

    for (long long p = 1; p <= m; ++p) {
        a[p-1] = lj_num(x, p, m);
        b[p-1] = lj_den(x, p, m);
    }

    std::vector<long long> num(m, 0);
    double den = *std::max_element(b.begin(), b.end());

    for (long long p = 1; p <= m; ++p) {
        std::vector<double> t(a[p-1].size());
        std::transform(a[p-1].begin(), a[p-1].end(), t.begin(), [den, &b, p](double val) { return val * (den / b[p-1]); });
        if((den / b[p-1]) == 0 || den == 0 || b[p-1] == 0){
            // std::cout << "Division by 0!!!" << std::endl;
        }
        for (size_t i = 0; i < t.size(); ++i) {
            num[i] += y[p-1] * t[i];
        }
    }

    std::vector<long long> f(m);
    for (long long p = 1; p <= m; ++p) {
        f[p-1] = modFrac(num[p-1], den, v);
    }

    return f;
}

cv::Mat decodeShadowsTL(const std::vector<Shadow>& selectedShadows, int K) {
    long long R = selectedShadows.size();
    if (R < K) {
        // std::cout << "-- Insufficient Number of Keys" << std::endl;
        // return cv::Mat();
    }

    std::vector<long long> X(R);
    for (long long i = 0; i < R; ++i) {
        X[i] = selectedShadows[i].number;  // Use the number field from the Shadow struct
    }

    // Load the first image to get the dimensions
    cv::Mat firstImage = selectedShadows[0].image;
    long long H = firstImage.rows;
    long long W = firstImage.cols;

    // Create a 3D matrix to store all images
    std::vector<cv::Mat> A(R, cv::Mat(H, W, CV_8UC1));

    // Load all images into the 3D matrix
    for (long long P = 0; P < R; ++P) {
        A[P] = selectedShadows[P].image;
    }

    // Process the images
    cv::Mat I = cv::Mat::zeros(H, W * K, CV_64FC1);

    for (long long M = 0; M < H; ++M) {
        for (long long N = 0; N < W; ++N) {
            std::vector<long long> Y(R);
            for (long long O = 0; O < R; ++O) {
                Y[O] = A[O].at<uchar>(M, N);
            }

            std::vector<long long> Z = modLagPol(Y, std::vector<double>(X.begin(), X.end()), 251);
            Z = std::vector<long long>(Z.begin() + (R - K), Z.end());

            for (long long O = 0; O < K; ++O) {
                I.at<double>(M, N + (W * O)) = Z[O];
            }
        }
    }

    I.convertTo(I, CV_8UC1);
    // imwrite("Message_new.jpg", I);

    // std::cout << "-- Message Successfully Recovered" << std::endl;

    return I;
}


