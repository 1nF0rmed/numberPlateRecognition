#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "classifier.h"

using namespace cv;

int str_ends_with(const char * str, const char * suffix) {

  if( str == NULL || suffix == NULL )
    return 0;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  if(suffix_len > str_len)
    return 0;

  return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}

std::string runClassification(Mat img) {
    NumberPlateLocator nbp = NumberPlateLocator(img);
    // Get the threshold image
    Mat img_pre = nbp.generateThresholdImage(img);
    //waitKey(0);
    // get the  contours
    std::vector<std::vector<Point>> cont = nbp.extractContours(img_pre);

    Mat tmp;
    img.copyTo(tmp);
    //waitKey(0);

    // Get the number plate
    Plate pobj = nbp.cleanAndFindPlate(cont);

    // Perform OCR
    PlateClassifier plateClassifier = PlateClassifier(pobj.plate, pobj.sub_contours, pobj.r_plate);

    std::string plateData = plateClassifier.recognizePlate(plateClassifier.extractCharacters());

    std::cout << plateData << "\n";

    return plateData;
}

Mat generateMatFromImagePath(const char *path) {
    int read_image = open(path, O_RDONLY);

    if (read_image == NULL)
    {
        printf("Image Not Found\n");
    }

    int fileLen = lseek(read_image, 0, SEEK_END);
    lseek(read_image, 0, SEEK_SET);

    unsigned char* pre_image = (unsigned char *)malloc(fileLen);
    size_t data = read(read_image, pre_image, fileLen);

    // Printed and verify the values
    //printf("File Size %d\n", fileLen);
    //printf("Read bytes %d\n", data);

    //close(read_image);

    std::vector<unsigned char> buffer(pre_image, pre_image + data);
    Mat img = imdecode(buffer, IMREAD_ANYCOLOR);


    return img;

}


void parseFiles(const char *dirPath) {
    DIR *curDIR = opendir(dirPath);
    struct dirent *curDirent;

    if(curDIR == NULL) {
        printf("[ERR] Cannot open directory!!!");
        perror("opendir");
        return;
    }
    while((curDirent = readdir(curDIR))) {
        if(str_ends_with(curDirent->d_name, ".jpg")) {
            // Call classifier
            Mat image = generateMatFromImagePath(curDirent->d_name);
            printf("Image: %s Number Plate: ", curDirent->d_name);
            // Get prediction
            std::string pred = runClassification(image);
            // Print prediction
            printf("\n");
        }
            //printf("%s\n", curDirent->d_name);
    }

    closedir(curDIR);
}

int main(int argc, char *argv[])
{

    char path[2048];

    std::cout << "Enter the path to the files: ";
    std::cin >> path;

    parseFiles(path);

    return 0;

}



