#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;

class NumberPlateLocator {
private:
    Mat base_img;
public:
    NumberPlateLocator(Mat img);
    Mat getBaseImage();
    Mat generateThresholdImage(Mat img);
    std::vector<std::vector<Point>> extractContours(Mat img);
    bool verifySizes(RotatedRect mr);
    void cleanAndFindPlate(Mat img, std::vector<std::vector<Point>> contours);
};

NumberPlateLocator::NumberPlateLocator(Mat img) {
    // Set base_img as the original image to be processed
    base_img = img;
}

Mat NumberPlateLocator::getBaseImage(){
    return this->base_img;
}

Mat NumberPlateLocator::generateThresholdImage(Mat img) {
    /*
    // General Process for preprocessing:
    //
    // Convert image matrix to grayscale
    // https://docs.opencv.org/4.0.1/d8/d01/group__imgproc__color__conversions.html#ga397ae87e1288a81d2363b61574eb8cab
    //
    // Apply Gaussian Blur 5x5 - use Size(5, 5)
    // ex: cvtColor(img, img_gray, COLOR_BGR2GRAY);
    // https://docs.opencv.org/4.0.1/d4/d86/group__imgproc__filter.html#ga8c45db9afe636703801b0b2e440fce37
    //
    // Apply Adaptive Threshold - 255 max - ADAPTIVE_THRESH_GAUSSIAN_C - THRESH_BINARY - 55 block size - 5 constant
    // https://docs.opencv.org/4.0.1/d7/d1b/group__imgproc__misc.html#ga72b913f352e4a1b1b397736707afcde3
    //
    */

    Mat img_gray, img_sobel, img_thres;

    // Grayscale conversion


    // Blur the image to remove any noise


    /*
    // This is some experimentation I was doing. Kindly skip this section

    // Apply a Sobel filter with a kernel size of 3
    Sobel(img_gray,img_sobel, CV_8U, 1, 0, 3);

    // Apply threshold
    threshold(img_sobel, img_thres, 0, 255, THRESH_OTSU+THRESH_BINARY);
    */

    /*Continue from here*/

    // Apply Adaptive Threshold



    return img_thres;
}

std::vector<std::vector<Point>> NumberPlateLocator::extractContours(Mat img) {
    /*
    // General Process for extracting contours
    //
    // Define the structural elements with dimension 17x3
    // Perform morphological operations on the elements
    // Extract the contours using findContours
    // Return the extracted contours from the image
    //
    */

    std::vector<std::vector<Point>> contours;
    Mat img_morph;

    // define the max structural element
    Mat element = getStructuringElement(MORPH_RECT, Size(17, 3));
    // perform morphological operation on the image
    morphologyEx(img, img_morph, MORPH_CLOSE, element);

    // Store the morphed image as the base image for visual representation
    this->base_img = img_morph;

    // find the contours
    findContours(img_morph, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    return contours;
}

bool NumberPlateLocator::verifySizes(RotatedRect mr){

    float error=0.4;
    //Spain car plate size: 52x11 aspect 4,7272
    float aspect=4.7272;
    //Set a min and max area. All other patchs are discarded
    int min= 15*aspect*15; // minimum area
    int max= 125*aspect*125; // maximum area
    //Get only patchs that match to a respect ratio.
    float rmin= aspect-aspect*error;
    float rmax= aspect+aspect*error;

    int area= mr.size.height * mr.size.width;
    float r= (float)mr.size.width / (float)mr.size.height;
    if(r<1)
        r= (float)mr.size.height / (float)mr.size.width;

    if(( area < min || area > max ) || ( r < rmin || r > rmax )){
    }else{
        return false;
        return true;
    }

}

void NumberPlateLocator::cleanAndFindPlate(Mat img, std::vector<std::vector<Point>> contours) {
    std::vector<std::vector<Point>>::iterator itc = contours.begin();
    std::vector<RotatedRect> rects;

    // Loop through the contours
    while(itc!=contours.end()) {
        std::cout << "Contours" << std::endl;
        RotatedRect mr = minAreaRect(Mat(*itc));
        if(!verifySizes(mr)) {
            itc = contours.erase(itc);
        } else {
            ++itc;
            rects.push_back(mr);
        }
    }

    std::cout << "Number of plate regions" << rects.size() << std::endl;
}



