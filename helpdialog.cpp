#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    ui->picHelpImg->setPixmap(helpImg.scaled(ui->picHelpImg->width(), ui->picHelpImg->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QString helpText =
        "This is help window for program for encoding and decoding of pictures created by <b>inż. Mikołaj Osiecki</b> <i>mdosiecki@gmail.com</i> for the purpose of the master thesis"
        "<br>"
        "<b>Scalable and multi-level algorithms for secret image data sharing</b>"
        "<br>"
        "(for more information go to &quot;About&quot; section)."
        "<br>"
        "Explanation on how to use the program:"
        "<br>"
        "<b>1.</b> Select image from hard drive. It will be loaded as grayscale image and displayed on the left side."
        "<br>"
        "<b>2.</b> Select one out of three possible schemes for encoding and decoding of image (more about schemes in &quot;About&quot; section):"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>Thien-Lin scheme</b>"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>Lin-Wang scheme</b>"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>Liu-Yang scheme</b>"
        "<br>"
        "<b>3.</b> Input values for algorithms. After generating shadow, they should not be changed unless to generate new shadows."
        "Also note that <b>t</b> and <b>s</b> values are only used in Liu-Yang scheme."
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>t</b> - threshold value for essential shadows (Liu-yang)"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>s</b> - amount of essential shadows (Liu-yang)"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>k</b> - threshold value for all shadows (all schemes)"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>n</b> - amount of all shadows (all schemes)"
        "<br>"
        "<b>4.</b> Cropping/Padding of input image (Ticked off = crop, on = pad). Since algorithms for scalable secret image sharing  were designed for shadow of equal size,"
        "images have to cropped/padded to achieve equal sizes. Since this programs slices input image vertically, cropping and padding will impact right side edge of image."
        "In Liu-Yang scheme this functionality differs slightly - when in Cropping mode (ticked off), width of image will be modified to closest value that satisfies mutiple "
        "divisibility conditions while prioritzing cropping. If resulting required width would be less than 90% of original width, image will be padded instead (most of the time this results in padded image). "
        "In Padding mode (ticked on) image will always be padded."
        "<br>"
        "<b>5.</b> Buttons for generating shadows and decoding. Generating shadows will instantly clear both the generated and selected shadows lists."
        " Decoding is possible only when atleast one shadow has been selected."
        "<br>"
        "<b>6.</b> List of generated shadows. Shadows with color background are <b>essential shadows</b> (they are generated only in Liu-Yang scheme)."
        "It is possible do double-click on shadow to display it in separate window."
        "<br>"
        "<b>7.</b> Buttons for moving of schadows between the generated shadows and selected shadows lists. Buttons operate on shadows selected on lists."
        "Select shadows by left-clicking on them, it is possible to select multiple at once by holding Ctrl or Shift."
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>Select</b> - moves all selected shadows from generated list (left) to selected list (right)"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>Move back</b> - moves all selected shadows back from selected list (right) to generated list (left)"
                       "<br>"
                       "&nbsp;&nbsp;&nbsp;<b>Clear</b> - moves all shadows from generated list (left) to selected list (right), regardless of selection"
        "<br>"
        "<b>8.</b> List of selected shadows. Shadows with color background are <b>essential shadows</b> (they are generated only in Liu-Yang scheme)."
        "It is possible do double-click on shadow to display it in separate window."
        ;
    ui->textBrowser->setText(helpText);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
