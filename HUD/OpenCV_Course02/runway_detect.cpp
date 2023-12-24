#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "symbologyUploading.hpp"
#include "draw_display.hpp"
#include "horizon_detector.hpp"


using namespace std;
using namespace cv;
int main() {
    VideoCapture cap(0); // Open the default camera (change to your camera index if needed)
    
    if (!cap.isOpened()) {
        std::cerr << "Error opening the camera." << std::endl;
        return -1;
    }

    Mat frame, edges;
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;

    namedWindow("Runway Detection", WINDOW_NORMAL);
    resizeWindow("Runway Detection", 640, 480);

    while (waitKey(1) != 'q') {
        cap >> frame; // Capture a frame from the camera

        if (frame.empty()) {
            std::cerr << "Error capturing frame." << std::endl;
            break;
        }

        // Convert the frame to grayscale
        cvtColor(frame, edges, COLOR_BGR2GRAY);

        // Apply Gaussian blur to reduce noise
        GaussianBlur(edges, edges, Size(5, 5), 0);

        // Apply Canny edge detection
        Canny(edges, edges, 50, 150);

        // Find contours
        findContours(edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // Filter contours based on approximate rectangle shape
        std::vector<std::vector<Point>> approxContours;

        for (const auto& contour : contours) {
            std::vector<Point> approx;
            approxPolyDP(contour, approx, arcLength(contour, true) * 0.02, true);

            if (approx.size() == 4 && isContourConvex(approx)) {
                approxContours.push_back(approx);
            }
        }

        // Find the contour most resembling a rectangular area (runway)
        double maxArea = 0;
        std::vector<Point> bestContour;

        for (const auto& approxContour : approxContours) {
            double area = contourArea(approxContour);

            if (area > maxArea) {
                maxArea = area;
                bestContour = approxContour;
            }
        }

        // Draw the best contour (runway) on the frame
        if (!bestContour.empty()) {
            std::vector<std::vector<Point>> bestContourVec;
            bestContourVec.push_back(bestContour);
            drawContours(frame, bestContourVec, -1, Scalar(0, 255, 0), 2);
        }

        // Display the result
        imshow("Runway Detection", frame);
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
