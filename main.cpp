#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "classifier.h"

using namespace cv;

int main(int argc, char *argv[])
{
    Mat img = imread("test3.jpg", cv::IMREAD_COLOR);
    if(img.empty())
       return -1;

    NumberPlateLocator nbp = NumberPlateLocator(img);
    // Get the threshold image
    Mat img_pre = nbp.generateThresholdImage(img);
    imshow("Threshold Image", img_pre);
    //waitKey(0);
    // get the  contours
    std::vector<std::vector<Point>> cont = nbp.extractContours(img_pre);

    Mat tmp;
    img.copyTo(tmp);
    drawContours(tmp, cont, -1, Scalar(255,0,0), 1);

    imshow("Contours", tmp);
    //waitKey(0);

    // Get the number plate
    Plate pobj = nbp.cleanAndFindPlate(cont);

    // Perform OCR
    PlateClassifier plateClassifier = PlateClassifier(pobj.plate, pobj.sub_contours, pobj.r_plate);

    std::string plateData = plateClassifier.recognizePlate(plateClassifier.extractCharacters());
    std::cout << "Number Plate: " << plateData << "\n";
    // Display the final extracted plate number
    putText(tmp, plateData, Point(pobj.r_plate.x, pobj.r_plate.y-10), FONT_HERSHEY_PLAIN, 3.0, Scalar(0, 0, 250), 3);
    rectangle(tmp, pobj.r_plate, Scalar(0, 255, 0), 2, 8, 0);
    imshow("Number Plate", tmp);
    waitKey(0);

    return 0;
}



