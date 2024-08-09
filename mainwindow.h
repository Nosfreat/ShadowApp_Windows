#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <opencv2/core/mat.hpp> // Make sure to include the necessary header for cv::Mat
#include <QListWidgetItem>
#include "helpdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct Shadow {
    cv::Mat image;  // The shadow image
    bool isEssential; // Flag to indicate if the shadow is essential
    std::string text; // Text representation of the shadow
    int number;
    int sliceNumber;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnClear_clicked();

    void on_btnMoveBack_clicked();

    void on_btnSelectShadows_clicked();

    void on_btnGenerateShadows_clicked();

    void on_btnSelectImage_clicked();

    void on_btnDecode_clicked();

    void on_listGeneratedSh_itemDoubleClicked(QListWidgetItem *item);

    void on_listSelectedSh_itemDoubleClicked(QListWidgetItem *item);

    void on_actionHelp_triggered();

    void on_actionAbout_Gui_triggered();

    void on_actionAuthor_triggered();

    void on_actionAbout_Schemes_triggered();

private:
    QPixmap loadingImg = QPixmap(":/images/loading.jpg");

    Ui::MainWindow *ui;
    HelpDialog *helpDialog;                             // Doing it this way (pointer then creating new instance) allows for using main window and help window at the same time
                                                        // if it was created simply by HelpDialog helpDialog and then exec(), only main window would not be enabled when help is open

    cv::Mat loadedImage;

    std::vector<Shadow> generatedShadows;               // Hold the generated shadow images and their importance
    std::vector<Shadow> selectedShadows;                // Hold the selected shadow images and their importance
    std::vector<Shadow> partitionedShadows;             // Hold the partitioned shadow images (Wang-Lin)
    std::vector<Shadow> copiedPartitions;               // Hold the partitions shadow images (Wang-Lin)
    std::vector<cv::Mat> decodedPartitions;             // Hold the decoded partitions (Wang-Ling)
    std::vector<std::string> generatedShadowsStrings;   // Hold the generated shadow images as strings
    std::vector<std::string> selectedShadowsStrings;    // Hold the selected shadow images as strings

    int shadowsThreshold;
    int shadowsAmount;
    int essentialThreshold;
    int essentialNumber;
};
#endif // MAINWINDOW_H
