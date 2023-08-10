#include <iostream>
#include <opencv2/opencv.hpp>

#include "symbologyUploading.hpp"



cv::Mat overlayImage = fpvGround; // Default symbology (uaRing)


void overlayImageOnFrame(cv::Mat& frame, const cv::Mat& overlayImage) {
    // Calculate the position of the top-left corner of the overlay image
    int overlayX = (frame.cols - overlayImage.cols) / 2;
    int overlayY = (frame.rows - overlayImage.rows) / 2;

    // Create a region of interest (ROI) on the webcam frame for overlaying the image
    cv::Rect roi(overlayX, overlayY, overlayImage.cols, overlayImage.rows);

    // Ensure the ROI stays within the webcam frame boundaries
    roi &= cv::Rect(0, 0, frame.cols, frame.rows);

    // Extract the ROI from the webcam frame
    cv::Mat frameROI = frame(roi);

    // Overlay the image on the webcam frame using alpha blending
    for (int y = 0; y < frameROI.rows; y++) {
        for (int x = 0; x < frameROI.cols; x++) {
            // Get the alpha value of the overlay pixel
            uchar alpha = overlayImage.at<cv::Vec4b>(y, x)[3];

            // Perform alpha blending
            for (int c = 0; c < 3; c++) {
                frameROI.at<cv::Vec3b>(y, x)[c] =
                    (alpha * overlayImage.at<cv::Vec4b>(y, x)[c] +
                    (255 - alpha) * frameROI.at<cv::Vec3b>(y, x)[c]) / 255;
            }
        }
    }
}

void FPV_Excessive_Deviation(cv::Mat& frame){
    if(fpvExcessiveDeviation.empty()){
        std::cerr << "Error: Failed to load FPV_Excessive_Deviation.png" << std::endl;
    }
    
    int overlayX = (frame.cols - fpvExcessiveDeviation.cols) / 2;
    int overlayY = (frame.rows - fpvExcessiveDeviation.rows) / 2;
    
    for (int y = 0; y < fpvExcessiveDeviation.rows; y++){
        for (int x = 0; x < fpvExcessiveDeviation.cols; x++){
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

void RollScaleTicksDraw(cv::Mat& frame){
    if(RollScaleTicks.empty()){
        std::cerr << "Error: Failed to load RollScaleTicks.png" << std::endl;
    }
    
    int overlayX = (frame.cols - RollScaleTicks.cols) / 2;
    int overlayY = (frame.rows - RollScaleTicks.rows) / 2;
    
    for (int y = 0; y < RollScaleTicks.rows; y++){
        for (int x = 0; x < RollScaleTicks.cols; x++){
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

void Gearslockeddown(cv::Mat& frame) {
    if (Gears_locked_down.empty()) {
        std::cerr << "Error: Failed to load Airspeed-Limits-Ladder-Middle.png" << std::endl;
        return;
    }
    
    int overlayX = (frame.cols - Gears_locked_down.cols) / 2; // Adjust the offset as needed
    int overlayY = (frame.rows - Gears_locked_down.rows) / 2;

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

void overlayAirspeedLimitsLadder(cv::Mat& frame) {
    if (airspeedLadder.empty()) {
        std::cerr << "Error: Failed to load Airspeed-Limits-Ladder-Middle.png" << std::endl;
        return;
    }
    
    int overlayX = (frame.cols - airspeedLadder.cols) / 4; // Adjust the offset as needed
    int overlayY = (frame.rows - airspeedLadder.rows) / 2;

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


int overlaySpeedIndicatorWithMovement(cv::Mat& frame, int indicatorY) {
    if (SpeedScaleIndicator.empty()) {
        std::cerr << "Error: Failed to load Speed_Scale-Indicated_AirSpeed.png" << std::endl;
        return -1;
    }
    static bool isWKeyPressed = false;
    static bool isSKeyPressed = false;

    int key = cv::waitKey(10);

    
    if (key == 'w') {
        isWKeyPressed = true;
    }
    else if (key == 's') {
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
    int overlayX = ((frame.cols - SpeedScaleIndicator.cols) / 4) - SpeedScaleIndicator.cols / 2;

    // Overlay the speed indicator image on the frame using the updated indicatorY
    for (int y = 0; y < SpeedScaleIndicator.rows; y++) {
        for (int x = 0; x < SpeedScaleIndicator.cols; x++) {
            uchar alpha = SpeedScaleIndicator.at<cv::Vec4b>(y, x)[3];
            if (alpha > 0) {
                cv::Point framePos(overlayX + x, indicatorY + y); // Use updated indicatorY
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

    return indicatorY; // Return the updated indicatorY value
}

int main() {
    // Load pre-trained Haar Cascade classifier for face detection
   /* cv::CascadeClassifier faceCascade;
    faceCascade.load(cv::samples::findFile("/Users/bernardolorenzini/Documents/estudos_teste/OpenCV_Course01/OpenCV_Course01/Resources/haarcascade_frontalface_default.xml"));
    */
    // Open camera object
    cv::VideoCapture cap(0);
    // Decrease frame size
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1000);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
    

    cv::Mat frame;
        int indicatorY = (frame.rows - SpeedScaleIndicator.rows) / 2; // Initial position

    while (true) {
        
        // Capture frames from the camera
        cap.read(frame);
        if (frame.empty())
            break;

        // Detect faces using Haar Cascade classifier
       /* std::vector<cv::Rect> faces;
        cv::Mat frameGray;
        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
        faceCascade.detectMultiScale(frameGray, faces, 1.3, 5);

        // Draw rectangles around detected faces and connect to uaRing center
        for (const auto& faceRect : faces) {
            cv::Point center_face(faceRect.x + faceRect.width / 2, faceRect.y + faceRect.height / 2);
            cv::Point center_uaRing(frame.cols / 2, frame.rows / 2);
            cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
            cv::line(frame, center_uaRing, center_face, cv::Scalar(255, 0, 0), 2);
        }
        */

        // Overlay uaRing on the frame
        overlayImageOnFrame(frame, uaRing);
        
        RollScaleTicksDraw(frame);
        
        overlayAirspeedLimitsLadder(frame);
        // CHANGED ON 08/08overlaySpeedIndicator(frame);
        indicatorY = overlaySpeedIndicatorWithMovement(frame, indicatorY);
        // Detect keyboard input
        int key = cv::waitKey(1);
        switch (key) {
            case '1':
                overlayImage = uaRing;
                break;
            case '2':
                overlayImage = pitchArrow;
                break;
            case '3':
                overlayImage = fpvGround;
                break;
            case '4':
                overlayImage = fpvDashed;
                break;
            case '5':
                overlayImage = fpvExcessiveDeviation;
                break;
            default:
                break;
        }

        // Overlay the selected symbology on uaRing background
        overlayImageOnFrame(frame, overlayImage);
        
        
        Gearslockeddown(frame);
        
        FPV_Excessive_Deviation(frame);

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
