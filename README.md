
# Number Plate Recognition
A simple number plate recognition application that uses OpenCV C++ and support vector machine for plate characters recognition



## How to run the project

1. Download and setup Code::Blocks from [here](http://www.codeblocks.org/downloads/26)
2. Download the souce of OpenCV 4.0 from [here](https://opencv.org/releases.html)
3. Build OpenCV for C++ using this [link](https://medium.com/@sourabhjigjinni/install-opencv-4-0-0-for-c-windows-7-10-code-blocks-tdm-gcc-64-dff65addf162)
4. Clone the repository
5. Open as a project in Code::Blocks
6. Click "Run" to start the terminal interface

## How to recognize a number plate
1. In the CLI, enter the path of the directory containing the image file
2. Press Enter

## User Stories
1. A user can provide the path to their image file directory
2. A user can use any image format
3. A user can provide image with the number plate shown in any position.

## Working
1. User provides the path to the image files directory
2. The application loads the image using POSIX API and converts into a Mat format (OpenCV Basic Image container)
3. The image matrix is converted to grayscale
4. Gaussian Blur 5x5 is applied to the grayscale image
5. Applying Adaptive Threshold - 255 max - 55 block size - 5 constant on the resulting image
6. Define the structural elements for the image
7. Then perform morphological operations on the elements
8. Use findContours to get all the contours in the image.
9. Using the contours, find if there is a rectangular object in the image matrix.
10. In the found section, the contours of each of the  characters on the number plate is extracted.
11. The contours are padded to ensure that it doesn't cause any confusion to the classifier.
12. The image matrix sub divisions for the characters are sent the Support Vector Machine.
13. The Support Vector Machine classifies the sub section of the matrix as to any character from A-Z and 0-9 and provides the final class.
14. The results of all the classification is accumulated as a single string and shown to the user.

## Dependencies

- OpenCV 4.0.0
- OpenCV 4.0.0 Contrib

## What the app looks like

![Running on a few test images](https://github.com/1nF0rmed/numberPlateRecognition/blob/master/test1.jpg)
