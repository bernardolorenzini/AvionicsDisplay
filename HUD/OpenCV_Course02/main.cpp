#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "symbologyUploading.hpp"
#include "draw_display.hpp"
#include "horizon_detector.hpp"


const int POOLING_KERNEL_SIZE = 5;

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
    cv::Mat frame1;
    
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
        cap.read(frame1);

        if (frame.empty())
            break;
        
        
        cv::Mat inputImage = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");

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
            int POOLING_KERNEL_SIZE = 2; // You can adjust this value
            cv::Mat downsampled;
            cv::resize(edges, downsampled, cv::Size(), 1.0 / POOLING_KERNEL_SIZE, 1.0 / POOLING_KERNEL_SIZE);

            // Display or save the results
            cv::imshow("Mask", mask);
            cv::imshow("Edges", edges);
            cv::imshow("Downsampled Edges", downsampled);

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

        cv::imshow("edges", blur);



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




/*
 
############### THIS WORKS WITH SIMBOLOGY ############

#include <iostream>
#include <opencv2/opencv.hpp>
#include "symbologyUploading.hpp"


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

void overlayAirspeedLimitsLadder(cv::Mat& frame, const Position& position) {
    if (airspeedLadder.empty()) {
        std::cerr << "Error: Failed to load Airspeed-Limits-Ladder-Middle.png" << std::endl;
        return;
    }

    int overlayX = (frame.cols - airspeedLadder.cols) / 4 + position.x;
    int overlayY = (frame.rows - airspeedLadder.rows) / 2 + position.y;

    for (int y = 0; y < airspeedLadder.rows; y++) {
        for (int x = 0; x < airspeedLadder.cols; x++) {
            uchar alpha = airspeedLadder.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, overlayY + y);
                if (framePos.x >= 0 && framePos.x < frame.cols && framePos.y >= 0 && framePos.y < frame.rows) {
                    for (int c = 0; c < 3; c++) {
                        frame.at<cv::Vec3b>(framePos.y, framePos.x)[c] =
                            (alpha * airspeedLadder.at<cv::Vec4b>(y, x)[c] +
                             (255 - alpha) * frame.at<cv::Vec3b>(framePos.y, framePos.x)[c]) / 255;
                    }
                }
            }
        }
    }
}

void overlaySpeedIndicatorWithMovement(cv::Mat& frame, int& indicatorY, const Position& position) {
    if (SpeedScaleIndicator.empty()) {
        std::cerr << "Error: Failed to load Speed_Scale-Indicated_AirSpeed.png" << std::endl;
        return;
    }
    static bool isWKeyPressed = false;
    static bool isSKeyPressed = false;

    int key = cv::waitKey(10);

    if (key == 'w') {
        isWKeyPressed = true;
    } else if (key == 's') {
        isSKeyPressed = true;
    } else if (key == -1) {  // No key pressed
        isWKeyPressed = false;
        isSKeyPressed = false;
    }

    if (isWKeyPressed) {
        indicatorY -= 15; // Move indicator up by adjusting the position
    } else if (isSKeyPressed) {
        indicatorY += 15; // Move indicator down by adjusting the position
    }

    // Calculate the overlay position with the updated indicatorY
    int overlayX = ((frame.cols - SpeedScaleIndicator.cols) / 4) - SpeedScaleIndicator.cols / 2 + position.x;

    // Overlay the speed indicator image on the frame using the updated indicatorY
    for (int y = 0; y < SpeedScaleIndicator.rows; y++) {
        for (int x = 0; x < SpeedScaleIndicator.cols; x++) {
            uchar alpha = SpeedScaleIndicator.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, indicatorY + y + position.y); // Use updated indicatorY
                if (framePos.x >= 0 && framePos.x < frame.cols && framePos.y >= 0 && framePos.y < frame.rows) {
                    for (int c = 0; c < 3; c++) {
                        frame.at<cv::Vec3b>(framePos.y, framePos.x)[c] =
                            (alpha * SpeedScaleIndicator.at<cv::Vec4b>(y, x)[c] +
                             (255 - alpha) * frame.at<cv::Vec3b>(framePos.y, framePos.x)[c]) / 255;
                    }
                }
            }
        }
    }
}

void RollScaleTicksDraw(cv::Mat& frame, const Position& position) {
    if (RollScaleTicks.empty()) {
        std::cerr << "Error: Failed to load RollScaleTicks.png" << std::endl;
        return;
    }

    int overlayX = (frame.cols - RollScaleTicks.cols) / 2 + position.x;
    int overlayY = (frame.rows - RollScaleTicks.rows) / 4.66 + position.y;

    for (int y = 0; y < RollScaleTicks.rows; y++) {
        for (int x = 0; x < RollScaleTicks.cols; x++) {
            uchar alpha = RollScaleTicks.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, overlayY + y);
                if (framePos.x >= 0 && framePos.x < frame.cols && framePos.y >= 0 && framePos.y < frame.rows) {
                    for (int c = 0; c < 3; c++) {
                        frame.at<cv::Vec3b>(framePos.y, framePos.x)[c] =
                            (alpha * RollScaleTicks.at<cv::Vec4b>(y, x)[c] +
                             (255 - alpha) * frame.at<cv::Vec3b>(framePos.y, framePos.x)[c]) / 255;
                    }
                }
            }
        }
    }
}

void Gearslockeddown(cv::Mat& frame, const Position& position) {
    if (Gears_locked_down.empty()) {
        std::cerr << "Error: Failed to load Airspeed-Limits-Ladder-Middle.png" << std::endl;
        return;
    }

    int overlayX = (frame.cols - Gears_locked_down.cols) / 2 + position.x; // Adjust the offset as needed
    int overlayY = (frame.rows - Gears_locked_down.rows) / 3 + position.y;

    for (int y = 0; y < Gears_locked_down.rows; y++) {
        for (int x = 0; x < Gears_locked_down.cols; x++) {
            uchar alpha = Gears_locked_down.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, overlayY + y);
                if (framePos.x >= 0 && framePos.x < frame.cols && framePos.y >= 0 && framePos.y < frame.rows) {
                    for (int c = 0; c < 3; c++) {
                        frame.at<cv::Vec3b>(framePos.y, framePos.x)[c] =
                            (alpha * Gears_locked_down.at<cv::Vec4b>(y, x)[c] +
                             (255 - alpha) * frame.at<cv::Vec3b>(framePos.y, framePos.x)[c]) / 255;
                    }
                }
            }
        }
    }
}

void FPV_Excessive_Deviation(cv::Mat& frame, const Position& position) {
    if (fpvExcessiveDeviation.empty()) {
        std::cerr << "Error: Failed to load FPV_Excessive_Deviation.png" << std::endl;
    }

    int overlayX = (frame.cols - fpvExcessiveDeviation.cols) / 2 + position.x;
    int overlayY = (frame.rows - fpvExcessiveDeviation.rows) / 2 + position.y;

    for (int y = 0; y < fpvExcessiveDeviation.rows; y++) {
        for (int x = 0; x < fpvExcessiveDeviation.cols; x++) {
            uchar alpha = fpvExcessiveDeviation.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, overlayY + y);
                if (framePos.x >= 0 && framePos.x < frame.cols && framePos.y >= 0 && framePos.y < frame.rows) {
                    for (int c = 0; c < 3; c++) {
                        frame.at<cv::Vec3b>(framePos.y, framePos.x)[c] =
                            (alpha * fpvExcessiveDeviation.at<cv::Vec4b>(y, x)[c] +
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
    
    cv::Mat frame;
    int indicatorY = (frame.rows - SpeedScaleIndicator.rows) / 2; // Initial position
    Position indicatorPosition = {0, 0};
    Position ladderPosition = {0, 0};
    Position rollScalePosition = {0, 0};
    Position gearsLockedDownPosition = {0, 0};
    Position excessiveDeviationPosition = {0, 0};
    
    while (true) {
        // Capture frames from the camera
        cap.read(frame);
        if (frame.empty())
            break;
        
        // Overlay uaRing on the frame at center position
        overlayImageAtPosition(frame, uaRing, {0, 0});
        
        // Draw RollScaleTicks at its specified position
        RollScaleTicksDraw(frame, rollScalePosition);
        
        // Overlay Airspeed Limits Ladder at its specified position
        overlayAirspeedLimitsLadder(frame, ladderPosition);
        
        // Overlay Speed Indicator with movement at its specified position
        overlaySpeedIndicatorWithMovement(frame, indicatorY, indicatorPosition);
        
        // Detect keyboard input and update positions
        int key = cv::waitKey(1);
        switch (key) {
            case '1':
                indicatorPosition = {0, 0};
                ladderPosition = {0, 0};
                rollScalePosition = {0, 0};
                excessiveDeviationPosition = {0, 0};
                overlayImageAtPosition(frame, uaRing, {0, 0});
                break;
            case '2':
                overlayImageAtPosition(frame, pitchArrow, {0, 0});
                break;
            case '3':
                overlayImageAtPosition(frame, fpvGround, {0, 0});
                break;
            case '4':
                overlayImageAtPosition(frame, fpvDashed, {0, 0});
                break;
            case '5':
                FPV_Excessive_Deviation(frame, excessiveDeviationPosition);
                break;
            case 'w':
                indicatorPosition.y -= 15;
                break;
            case 's':
                indicatorPosition.y += 15;
                break;
            case 'a':
                indicatorPosition.x -= 15;
                break;
            case 'd':
                indicatorPosition.x += 15;
                break;
            default:
                break;
        }
        
        // Overlay Gears Locked Down at its specified position
        Gearslockeddown(frame, gearsLockedDownPosition);
        
        // Overlay FPV Excessive Deviation at its specified position
        FPV_Excessive_Deviation(frame, excessiveDeviationPosition);
        
        // Show the final image
        cv::imshow("Overlay", frame);
        
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

*/
