#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;

class Plate {
public:
    Mat plate;
    Rect r_plate;
    std::vector<std::vector<Point>> sub_contours;
};

class NumberPlateLocator {
private:
    Mat base_img;
    Mat binary_img;
public:
    NumberPlateLocator(Mat img);
    Mat getBaseImage();
    Mat generateThresholdImage(Mat img);
    std::vector<std::vector<Point>> extractContours(Mat img);
    bool verifySizes(Rect r, Mat image);
    Plate cleanAndFindPlate(std::vector<std::vector<Point>> contours);
};

NumberPlateLocator::NumberPlateLocator(Mat img) {
    // Set base_img as the original image to be processed
    base_img = img.clone();
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
    cvtColor(img, img_gray, COLOR_BGR2GRAY);

    // Blur the image to remove any noise
    blur(img_gray, img_gray, Size(5, 5));

    /*
    // This is some experimentation I was doing. Kindly skip this section

    // Apply a Sobel filter with a kernel size of 3
    Sobel(img_gray,img_sobel, CV_8U, 1, 0, 3);

    // Apply threshold
    threshold(img_sobel, img_thres, 0, 255, THRESH_OTSU+THRESH_BINARY);
    */

    /*Continue from here*/

    // Apply Adaptive Threshold
    adaptiveThreshold(img_gray, img_thres, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);



    return img_thres;
}

std::vector<std::vector<Point>> NumberPlateLocator::extractContours(Mat img) {
    /*
    // General Process for extracting contours
    //
    // Define the structural elements - shape MORPH_RECT - size 3x3
    // https://docs.opencv.org/4.0.1/d4/d86/group__imgproc__filter.html#gac342a1bb6eabf6f55c803b09268e36dc
    //
    // Perform morphological operations on the elements
    // i) erode - img --> img_morph use the structuring element
    // https://docs.opencv.org/4.0.1/d4/d86/group__imgproc__filter.html#gaeb1e0c1033e3f6b891a25d0511362aeb
    //
    // ii) dilate - img_morph --> img_morph use the structuring element
    // https://docs.opencv.org/4.0.1/d4/d86/group__imgproc__filter.html#ga4ff0f3318642c4f469d0e11f242f3b6c
    //
    // Extract the contours using findContours
    // https://docs.opencv.org/4.0.1/d3/dc0/group__imgproc__shape.html#gadf1ad6a0b82947fa1fe3c3d497f260e0
    //
    // Return the extracted contours from the image
    //
    */

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    Mat img_morph;

    // define the max structural element
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

    // perform morphological operation on the image
    erode(img, img_morph, element);

    // perform the dilate morphological operation on the image
    dilate(img_morph, img_morph, element);

    this->binary_img = img_morph.clone();

    // find the contours
    findContours(img_morph, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));

    return contours;
}

bool NumberPlateLocator::verifySizes(Rect r, Mat image){

    // Original Expression
    // (r.width > image.cols / 2 || r.height > image.cols / 2 || r.width < 120 || r.height < 20 || (double)r.width / r.height > 4.5 || (double)r.width / r.height < 3.5)

    return (r.width > image.cols / 2 || r.height > image.cols / 2 || r.width < 120 || r.height < 60);

}

Plate NumberPlateLocator::cleanAndFindPlate(std::vector<std::vector<Point>> contours) {
    /*
    // General Process for plate localization
    //
    // Loop through the contours
    //
    // Get the bouding rectangle for the contour
    // use boundingRect and store in a Rect object
    //
    // use checkSizes member function to verify that the Rect object
    */

    Mat tmp;
    Rect r;
    Plate pobj;
    if(contours.size()<=0){
        std::cout << "Error: No contours were found in the image.\n" ;
        return pobj;
    }

    for(int i=0;i<contours.size();++i) {
        // Create a bounding rectangle shape for the extracted contours
        r = boundingRect(contours.at(i));

        // Verify the dimensions of the rectangle
        if(verifySizes(r, this->base_img))continue;


        // Get the rectangle region from the morphed image
        Mat sub_binary = this->binary_img(r);
        Mat _plate = sub_binary.clone();

        std::vector<std::vector<Point>> sub_contours;
        std::vector<Vec4i> sub_hierarchy;

        findContours(sub_binary, sub_contours, sub_hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));

        // Check if there are any contours
        if(sub_contours.size()<8) continue;

        tmp = this->base_img.clone();
        rectangle(tmp, r, Scalar(0, 255, 0), 2, 8, 0);
        imshow("Plate Final", tmp);
        //waitKey(0);

        pobj.sub_contours = sub_contours;
        pobj.plate = _plate;
        pobj.r_plate = r;

        break;

    }



    return pobj;
}



