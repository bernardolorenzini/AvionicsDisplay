#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "symbologyUploading.hpp"
#include "draw_display.hpp"
#include "horizon_detector.hpp"


const int POOLING_KERNEL_SIZE = 3;

// Define the Position struct for absolute and relative positioning
struct Position {
    int x; // X position
    int y; // Y position
};


// Function to overlay an image on the frame at a specified position
void overlayImageAtPosition(cv::Mat& frame, const cv::Mat& overlayImage, const Position& position) {
    int overlayX = (frame.cols - overlayImage.cols) / 2 + position.x;
    int overlayY = (frame.rows - overlayImage.rows) / 2 + position.y;
    
    
    for (int y = 0; y < overlayImage.rows; y++) {
        for (int x = 0; x < overlayImage.cols; x++) {
            uchar alpha = overlayImage.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, overlayY + y);
                if (framePos.x >= 0 && framePos.x < frame.cols && framePos.y >= 0 && framePos.y < frame.rows) {
                    for (int c = 0; c < 3; c++) {
                        frame.at<cv::Vec3b>(framePos.y, framePos.x)[c] =
                            (alpha * overlayImage.at<cv::Vec4b>(y, x)[c] +
                             (255 - alpha) * frame.at<cv::Vec3b>(framePos.y, framePos.x)[c]) / 255;
                    }
                }
            }
        }
    }
}


int main() {
    // Open camera object
    cv::VideoCapture cap(0);
    // Decrease frame size
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);  //1000
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1024);  //600

    cv::Mat frame = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");
    cv::Mat inputImage;
    
    cv::Mat bgr2gray_frame;

    cv::Mat hsv_frame;

    cv::Mat hsv_mask;

    cv::Mat blue_filtered_greyscale;

    cv::Mat blur;

    cv::Mat mask;

    cv::Mat edges;

    cv::Mat pooled_edges;

    cv::Mat edges_max_pooled;

    cv::Mat edges_final;



    cv::Scalar lower(109, 0, 116);
    cv::Scalar upper(153, 255, 255);


    while (true) {
        // Capture frames from the camera
        cap.read(inputImage);

        if (inputImage.empty())
            break;
        
        
        //cv::Mat
        inputImage = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");

            // Convert the input image to grayscale
            cv::Mat bgr2gray;
            cv::cvtColor(inputImage, bgr2gray, cv::COLOR_BGR2GRAY);

            // Apply bilateral filter
            cv::Mat blur;
            cv::bilateralFilter(bgr2gray, blur, 9, 50, 50);

            // Apply Otsu's thresholding
            cv::Mat mask;
            cv::threshold(blur, mask, 250, 255, cv::THRESH_OTSU);

            // Apply Canny edge detection
            cv::Mat edges;
            cv::Canny(bgr2gray, edges, 200, 250);

            // Downsample using OpenCV's resize function

            cv::Mat downsampled;
            cv::resize(edges, downsampled, cv::Size(), 1.0 / POOLING_KERNEL_SIZE, 1.0 / POOLING_KERNEL_SIZE);
        
        // Find contours
            std::vector<std::vector<cv::Point>> contours;
            cv::Mat contourHierarchy;

            int chain = cv::CHAIN_APPROX_NONE;

            cv::findContours(mask.clone(), contours, contourHierarchy, cv::RETR_TREE, chain);
            

            // Draw contours on the original image
            cv::Mat resultImage = inputImage.clone();
            cv::drawContours(resultImage, contours, -1, cv::Scalar(0, 0, 255), 2);


//            // Display or save the results
//            cv::imshow("Mask", mask);
//            cv::imshow("Edges", edges);
//            cv::imshow("Downsampled Edges", downsampled);

 /*       cv::cvtColor(frame, bgr2gray_frame, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);
        cv::inRange(hsv_frame, lower, upper, hsv_mask);
        cv::add(bgr2gray_frame, hsv_mask, blue_filtered_greyscale);

        cv::bilateralFilter(blue_filtered_greyscale, blur, 9, 50, 50);

        cv::threshold(blur, mask, 250, 255, cv::THRESH_OTSU);

        cv::Canny(bgr2gray_frame, edges, 200, 250);



        cv::resize(edges, pooled_edges, cv::Size(edges.cols / POOLING_KERNEL_SIZE, edges.rows / POOLING_KERNEL_SIZE));

        cv::reduce(edges, edges_max_pooled, 1, cv::REDUCE_MAX);

        cv::repeat(edges_max_pooled, 1, POOLING_KERNEL_SIZE, edges_final);


*/


//        overlayImageAtPosition(frame, fpvGround, {0, 0});
//        overlayImageAtPosition(bgr2gray_frame, fpvGround, {0, 0});
//        overlayImageAtPosition(hsv_frame, fpvGround, {0, 0});
//        overlayImageAtPosition(hsv_mask, fpvGround, {0, 0});
//        overlayImageAtPosition(blue_filtered_greyscale, fpvGround, {0, 0});



//         Show the final image
//        cv::imshow("Overlay", frame);
//        cv::imshow("GRAY", bgr2gray_frame);
//        cv::imshow("hsv_frame", hsv_frame);
//        cv::imshow("hsv_mask", hsv_mask);
//        cv::imshow("blue_filtered_greyscale", blue_filtered_greyscale);

        cv::imshow("reultado", resultImage);



        // Close the output video by pressing 'ESC'
        int keyEsc = cv::waitKey(2);
        if (keyEsc == 27)
            break;
    }

    // Release the camera capture and destroy windows
    cap.release();
    cv::destroyAllWindows();

    return 0;

}
