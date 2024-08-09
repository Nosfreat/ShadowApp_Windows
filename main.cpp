#include "mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QApplication>
#include<QMenuBar>

int globalTempShadowSize = 0; // in a perfect world, a variable like me would not exist
void clear_directory(const std::filesystem::path& dir_path) {
    if (!std::filesystem::exists(dir_path)) {
        std::cerr << "Directory does not exist: " << dir_path << std::endl;
        return;
    }

    if (!std::filesystem::is_directory(dir_path)) {
        std::cerr << "Path is not a directory: " << dir_path << std::endl;
        return;
    }

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        std::filesystem::remove_all(entry.path(), ec);
        if (ec) {
            std::cerr << "Error deleting " << entry.path() << ": " << ec.message() << std::endl;
        }
    }

    // std::cout << "Directory cleared: " << dir_path << std::endl;
}


int main(int argc, char *argv[])
{
    // std::filesystem::path KEYS_CPP = "KEYS_CPP";
    // std::filesystem::path DECOMPOSED_SHADOWS = "DECOMPOSED_SHADOWS";
    // clear_directory(KEYS_CPP);
    // clear_directory(DECOMPOSED_SHADOWS);

    // std::cout << "Maximum value of int: " << std::numeric_limits<int>::max() << std::endl;
    // std::cout << "Maximum value of long long: " << std::numeric_limits<long long>::max() << std::endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
