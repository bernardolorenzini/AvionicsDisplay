#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <numeric>


#include "symbologyUploading.hpp"
#include "draw_display.hpp"
//#include "horizon_detector.hpp"


const int POOLING_KERNEL_SIZE = 3;

cv::Mat draw_hud(cv::Mat frame, double roll, double pitch, double fps, bool is_good_horizon, bool recording);

void draw_roi(cv::Mat& frame, const cv::Rect& roi_rect, const cv::Scalar& color) {
    cv::rectangle(frame, roi_rect, color, 1);
}

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
   // cv::VideoCapture cap(0);
    // Decrease frame size
  //  cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);  //1000
  //  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1024);  //600

    cv::Mat inputImage = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");
   // cv::Mat inputImage;
    
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
       // cap.read(inputImage);

        if (inputImage.empty())
            break;
        
        
        //cv::Mat
      //  frame = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");

            // Convert the input image to grayscale
            cv::Mat bgr2gray;
            cv::cvtColor(inputImage, bgr2gray, cv::COLOR_BGR2GRAY);


            cv::cvtColor(inputImage, hsv_frame, cv::COLOR_BGR2HSV);
        
        
            cv::inRange(hsv_frame, lower, upper, hsv_mask);
        
            cv::add(bgr2gray, hsv_mask, blue_filtered_greyscale);
            cv::Mat blur;
            cv::bilateralFilter(blue_filtered_greyscale, blur, 9, 50, 50);


        
            // Apply Otsu's thresholding
            cv::Mat mask;
            cv::threshold(blur, mask, 250, 255, cv::THRESH_OTSU);

        
            // Apply Canny edge detection
            cv::Mat edges;
            cv::Canny(mask, edges, 200, 250);
            cv::imshow("mask", mask);
            cv::imshow("edges", edges);


            
            // Find contours
            std::vector<std::vector<cv::Point>> contours;
            cv::Mat contourHierarchy;

            int chain = cv::CHAIN_APPROX_NONE;

            cv::findContours(edges.clone(), contours, contourHierarchy, cv::RETR_TREE, chain);
            
            cv::Mat resultImage = inputImage.clone();
        
        
            cv::drawContours(resultImage, contours, -1, cv::Scalar(0, 0, 255), 2);

            cv::imshow("resultImage with all countors", resultImage);

            // Find the contour with the largest area
            double largest_area = -1;
            int largest_contour_index = -1;

            for (int i = 0; i < contours.size(); i++) {
                double area = cv::contourArea(contours[i]);
                if (area > largest_area) {
                    largest_area = area;
                    largest_contour_index = i;
                }
            }
        std::vector<int> x_edge_points, y_edge_points;
        std::vector<int> x_abbr, y_abbr;

            if (largest_contour_index != -1) {
                // The largest contour is now in contours[largest_contour_index]
                std::vector<cv::Point> largest_contour = contours[largest_contour_index];

                // Initialize vectors for points on the edge and all other points

                       cv::Size frameSize = mask.size();

                       // Iterate through the points of the largest contour
                       for (const cv::Point& point : largest_contour) {
                           int x_point = point.x;
                           int y_point = point.y;

                           // Check if the point is on the edge of the frame
                           if (x_point == 0 || x_point == frameSize.width - 1 ||
                               y_point == 0 || y_point == frameSize.height - 1) {
                               x_edge_points.push_back(x_point);
                               y_edge_points.push_back(y_point);
                           } else {
                               x_abbr.push_back(x_point);
                               y_abbr.push_back(y_point);
                           }
                       }
                
            }
        // Find the average position of the edge points
        double average_x = 0.0;
        double average_y = 0.0;

        for (size_t i = 0; i < x_edge_points.size(); i++) {
            average_x += x_edge_points[i];
            average_y += y_edge_points[i];
        }

        if (!x_edge_points.empty()) {
            average_x /= x_edge_points.size();
            average_y /= y_edge_points.size();
        }

        // Reduce the number of edge points
        int maximum_number_of_points = 20;
        if (x_edge_points.size() > maximum_number_of_points) {
            int step_size = x_edge_points.size(); // maximum_number_of_points;
            std::vector<int> reduced_x_edge_points;
            std::vector<int> reduced_y_edge_points;

            for (size_t i = 0; i < x_edge_points.size(); i += step_size) {
                reduced_x_edge_points.push_back(x_edge_points[i]);
                reduced_y_edge_points.push_back(y_edge_points[i]);
            }

            // Update x_edge_points and y_edge_points with reduced points
            x_edge_points = reduced_x_edge_points;
            y_edge_points = reduced_y_edge_points;
        }
        
        double avg_x, avg_y;

            // Check if there are any edge points
            if (!x_edge_points.empty()) {
                // Take the average of edge points
                double sum_x = std::accumulate(x_edge_points.begin(), x_edge_points.end(), 0.0);
                double sum_y = std::accumulate(y_edge_points.begin(), y_edge_points.end(), 0.0);
                avg_x = sum_x / x_edge_points.size();
                avg_y = sum_y / y_edge_points.size();
            } else {
                // Take the average of abbreviated points
                double sum_x = std::accumulate(x_abbr.begin(), x_abbr.end(), 0.0);
                double sum_y = std::accumulate(y_abbr.begin(), y_abbr.end(), 0.0);
                avg_x = sum_x / x_abbr.size();
                avg_y = sum_y / y_abbr.size();
            }
        
        
        if (x_abbr.size() > maximum_number_of_points) {
            int step_size = x_abbr.size() / maximum_number_of_points;
            std::vector<int> reduced_x_abbr;
            std::vector<int> reduced_y_abbr;

            for (size_t i = 0; i < x_abbr.size(); i += step_size) {
                reduced_x_abbr.push_back(x_abbr[i]);
                reduced_y_abbr.push_back(y_abbr[i]);
            }

            // Update x_abbr and y_abbr with reduced points
            x_abbr = reduced_x_abbr;
            y_abbr = reduced_y_abbr;
        }
        
        
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

        cv::Mat frame_hud;

        frame_hud = draw_hud(resultImage, 1, 1, 1, true, false);
        cv::imshow("reultado", frame_hud);



        // Close the output video by pressing 'ESC'
        int keyEsc = cv::waitKey(2);
        if (keyEsc == 27)
            break;
    }

    // Release the camera capture and destroy windows
    //cap.release();
    cv::destroyAllWindows();

    return 0;

}

// Function to draw the heads-up display (HUD) on the frame
cv::Mat draw_hud(cv::Mat frame, double roll, double pitch, double fps, bool is_good_horizon, bool recording) {
    // Convert roll and pitch to integers if they are valid and good_horizon is true
    if (roll && is_good_horizon) {
        roll = std::round(roll);
        pitch = std::round(pitch);
    } else {
        roll = std::numeric_limits<double>::quiet_NaN();
        pitch = std::numeric_limits<double>::quiet_NaN();
    }
    
    // Round fps to 2 decimal places
    fps = std::round(fps * 100.0) / 100.0;
    
    // Define text properties
    int font = cv::FONT_HERSHEY_COMPLEX_SMALL;
    double font_scale = 1.0;
    int thickness = 1;
    cv::Scalar text_color = is_good_horizon ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 0, 255);
    
    // Display roll, pitch, and FPS
    cv::putText(frame, "Roll: " + std::to_string(static_cast<int>(roll)), cv::Point(20, 40), font, font_scale, text_color, thickness);
    cv::putText(frame, "Pitch: " + std::to_string(static_cast<int>(pitch)), cv::Point(20, 80), font, font_scale, text_color, thickness);
    cv::putText(frame, "FPS: " + std::to_string(fps), cv::Point(20, 120), font, font_scale, cv::Scalar(255, 0, 0), thickness);
    
    return frame;
}
