#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "preprocess.h"

using namespace cv;

int main(int argc, char *argv[])
{
    Mat img = imread("test.JPG", cv::IMREAD_COLOR);
    if(img.empty())
       return -1;

    NumberPlateLocator nbp = NumberPlateLocator(img);
    // Get the threshold image
    Mat img_pre = nbp.generateThresholdImage(img);
    imshow("Threshold Image", img_pre);
    waitKey(0);
    // get the  contours
    std::vector<std::vector<Point>> cont = nbp.extractContours(img_pre);
    imshow("Morphed Image", nbp.getBaseImage());
    waitKey(0);

    Mat tmp;
    img.copyTo(tmp);
    drawContours(tmp, cont, -1, Scalar(255,0,0), 1);

    imshow("Contours", tmp);
    waitKey(0);

    nbp.cleanAndFindPlate(img, cont);

    // Get the number plate

    // Perform OCR

    // Display the final extracted plate number

    return 0;
}



