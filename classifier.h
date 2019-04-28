#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "preprocess.h"
#include <iostream>

using namespace cv;

class PlateClassifier{
private:
    const int number_of_feature = 32;
public:
    std::vector<std::vector<Point>> sub_contours;
    Mat _plate;
    Rect r_plate;
    PlateClassifier(Mat _plate, std::vector<std::vector<Point>> sub_contours, Rect r_plate);
    // TODO methods to train the classifier
    std::vector<float> calculate_feature(Mat src);
    // TODO methods to get all the characters
    int count_pixel(Mat img, bool black_pixel=true);
    std::vector<std::vector<Mat>> extractCharacters();
    // TODO methods to classify the characters
    std::string recognizePlate(std::vector<std::vector<Mat>> characters);
    char svmClassifier(Mat img_character);
};

PlateClassifier::PlateClassifier(Mat _plate, std::vector<std::vector<Point>> sub_contours, Rect r_plate) {
    this->_plate = _plate.clone();
    this->sub_contours = sub_contours;
    this->r_plate = r_plate;
}

std::vector<float> PlateClassifier::calculate_feature(Mat src)
{
	Mat img;
	if(src.channels() == 3)
	{
		cvtColor(src, img, COLOR_BGR2GRAY);
		threshold(img, img, 100, 255, THRESH_BINARY);
	}
	else
	{
		threshold(src, img, 100, 255, THRESH_BINARY);
	}

	std::vector<float> r;
	//vector<int> cell_pixel;
	resize(img, img, Size(40, 40));
	int h = img.rows/4;
	int w = img.cols/4;
	int S = count_pixel(img);
	int T = img.cols * img.rows;
	for(int i = 0; i < img.rows; i += h)
	{
		for(int j = 0; j < img.cols; j += w)
		{
			Mat cell = img(Rect(i,j, h , w));
			int s = count_pixel(cell);
			float f = (float)s/S;
			r.push_back(f);
		}
	}

	for(int i = 0; i < 16; i+= 4)
	{
		float f = r[i] + r[i+1] + r[i+2] + r[i+3];
		r.push_back(f);
	}

	for(int i = 0; i < 4; ++i)
	{
		float f = r[i] + r[i+4] + r[i+8] + r[i+ 12];
		r.push_back(f);
	}

	r.push_back(r[0] + r[5] + r[10] + r[15]);
	r.push_back(r[3] + r[6] + r[9] + r[12]);
	r.push_back(r[0] + r[1] + r[4] + r[5]);
	r.push_back(r[2] + r[3] + r[6] + r[7]);
	r.push_back(r[8] + r[9] + r[12] + r[13]);
	r.push_back(r[10] + r[11] + r[14] + r[15]);
	r.push_back(r[5] + r[6] + r[9] + r[10]);
	r.push_back(r[0] + r[1] + r[2] + r[3] + r[4] + r[7] + r[8] + r[11] + r[12] + r[13] + r[14] + r[15]);

	return r; //32 feature
}

int PlateClassifier::count_pixel(Mat img, bool black_pixel) {
	int black = 0;
	int white = 0;
	for(int i = 0; i < img.rows; ++i)
		for(int j = 0; j < img.cols; ++j)
		{
				if(img.at<uchar>(i,j) == 0)
					black++;
				else
					white++;
		}
	if(black_pixel)
		return black;
	else
		return white;
}

std::vector<std::vector<Mat>> PlateClassifier::extractCharacters() {
    std::cout << "Hello\n";
    std::vector<Rect> r_characters;
    std::vector<std::vector<Mat> > characters;
    std::vector<Mat> c;

    for(int i=0;i<this->sub_contours.size();i++) {
        Rect sub_r = boundingRect(sub_contours.at(i));

        if (sub_r.height > this->r_plate.height / 2 && sub_r.width < this->r_plate.width / 8 && sub_r.width > 5 && this->r_plate.width > 15 && sub_r.x > 5)
			{
				Mat cj = this->_plate(sub_r);
				double ratio = (double)count_pixel(cj) / (cj.cols*cj.rows);
				if (ratio > 0.2 && ratio < 0.7)
				{
					r_characters.push_back(sub_r);
				}
            }

    }

    if (r_characters.size() >= 7)
		{
			// sap xep
			for (int i = 0; i < r_characters.size() - 1; ++i)
			{
				for (int j = i + 1; j < r_characters.size(); ++j)
				{
					Rect temp;
					if (r_characters.at(j).x < r_characters.at(i).x)
					{
						temp = r_characters.at(j);
						r_characters.at(j) = r_characters.at(i);
						r_characters.at(i) = temp;
					}
				}
			}
			for (int i = 0; i < r_characters.size(); ++i)
			{
				Mat cj = this->_plate(r_characters.at(i));
				c.push_back(cj);
			}
			characters.push_back(c);
        }

    return characters;

}

char PlateClassifier::svmClassifier(Mat img_character) {
    //Load SVM training file OpenCV 3.1
	Ptr<ml::SVM> svmNew = ml::SVM::create();
	svmNew = ml::SVM::load("svm.txt");
	char c = '*';

	std::vector<float> feature = calculate_feature(img_character);
	// Open CV3.1
	Mat m = Mat(1, number_of_feature, CV_32FC1);
	for (size_t i = 0; i < feature.size(); ++i)
	{
		float temp = feature[i];
		m.at<float>(0, i) = temp;
	}

	int ri = int(svmNew->predict(m)); // Open CV 3.1
									  /*int ri = int(svmNew.predict(m));*/
	if (ri >= 0 && ri <= 9)
		c = (char)(ri + 48); //ma ascii 0 = 48
	if (ri >= 10 && ri < 18)
		c = (char)(ri + 55); //ma accii A = 5, --> tu A-H
	if (ri >= 18 && ri < 22)
		c = (char)(ri + 55 + 2); //K-N, bo I,J
	if (ri == 22) c = 'P';
	if (ri == 23) c = 'S';
	if (ri >= 24 && ri < 27)
		c = (char)(ri + 60); //T-V,
	if (ri >= 27 && ri < 30)
		c = (char)(ri + 61); //X-Z

    return c;
}

std::string PlateClassifier::recognizePlate(std::vector<std::vector<Mat>> characters) {

    std::string licenseRecog;
    for (int i = 0; i < characters.size(); i++)
	{
		std::string result;
		for (size_t j = 0; j < characters.at(i).size(); ++j)
		{

			char cs = svmClassifier(characters.at(i).at(j));
			result.push_back(cs);

		}
		licenseRecog += result;
	}
    return licenseRecog;
}
