#include <iostream>
#include <OpenGL/gl.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>  // For sleep function

#include <chrono>  // For measuring frame rate
#include <thread>  // For sleep function

#include <cmath>
#include <numeric>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "loadSymbology.hpp"

#include "draw_display.hpp"
#include "overlayImageAtPosition.hpp"
#include "HUD_Mode.hpp"
#include "select_mode.hpp"



#include "Settings.hpp"

//#include "horizon_detector.hpp"




const double FULL_ROTATION_RADIANS = 2.0f * CV_PI;
const int POOLING_KERNEL_SIZE = 3;



cv::Mat draw_horizon(cv::Mat frame, double roll, double pitch, double fov, cv::Scalar color, bool draw_groundline);

cv::Mat draw_roi(cv::Mat frame, const std::map<std::string, int>& crop_and_scale_parameters);

void draw_hud(cv::Mat &frame, double roll, double pitch, double fps, bool is_good_horizon, bool recording);

std::vector<cv::Point> findPoints(double m, double b, const cv::Size& frame_shape);

void draw_roi(cv::Mat& frame, const cv::Rect& roi_rect, const cv::Scalar& color) {
    cv::rectangle(frame, roi_rect, color, 1);
}







int main() {
    std::cout << "----------STARTING HORIZON DETECTOR----------" << std::endl;

    
    // Open camera object
    std::string path = "/Users/bernardolorenzini/Documents/estudos_teste/OpenCV_Course02 copy/OpenCV_Course02/settings.txt";
    Settings settings(path);
    
    
    std::string SOURCE = "0";  // Change to your desired video source
    cv::Size RESOLUTION(1200, 1000);  // Change to your desired resolution
    cv::Size INFERENCE_RESOLUTION(100, 100);  // Change to your desired inference resolution
    double fps = 30.0f;  // Change to your desired FPS
    double ACCEPTABLE_VARIANCE = 1.3f;  // Change to your desired value
    double EXCLUSION_THRESH = 4.0f;  // Change to your desired value
    double FOV = 48.8f;  // Change to your desired FOV
    std::string OPERATING_SYSTEM = "macOS";  // Adjust as needed
    
    // Validate INFERENCE_RESOLUTION
    if (INFERENCE_RESOLUTION.height > RESOLUTION.height) {
        std::cout << "Specified inference resolution is taller than the resolution. This is not allowed." << std::endl;
        INFERENCE_RESOLUTION = cv::Size(100, 100);  // Recommended inference resolution
        std::cout << "Inference resolution has been adjusted to the recommended resolution." << std::endl;
    }

    double inference_aspect_ratio = static_cast<double>(INFERENCE_RESOLUTION.width) / INFERENCE_RESOLUTION.height;
    double aspect_ratio = static_cast<double>(RESOLUTION.width) / RESOLUTION.height;

    if (inference_aspect_ratio > aspect_ratio) {
        std::cout << "The specified inference aspect ratio is wider than the aspect ratio. This is not allowed." << std::endl;
        int inference_height = INFERENCE_RESOLUTION.height;
        int inference_width = static_cast<int>(INFERENCE_RESOLUTION.height * aspect_ratio);
        INFERENCE_RESOLUTION = cv::Size(inference_width, inference_height);
        std::cout << "The inference resolution has been adjusted." << std::endl;
    }

    // Global variables
    double fps_toHUD = 0.0f;
    bool render_image = true;  // Set to true for macOS
    
    cv::Mat paused_frame(500, 500, CV_8U, cv::Scalar(0));
    cv::putText(paused_frame, "Real-time display is paused.", cv::Point(20, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    cv::putText(paused_frame, "Press 'd' to enable real-time display.", cv::Point(20, 60), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    cv::putText(paused_frame, "Press 'r' to record.", cv::Point(20, 90), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    cv::putText(paused_frame, "Press 'q' to quit.", cv::Point(20, 120), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    cv::imshow("Real-time Display", paused_frame);

    // Define VideoCapture
    cv::VideoCapture video_capture(0);  // Change the index (0) to your desired video source

    // Start video stream
    if (!video_capture.isOpened()) {
        std::cerr << "Error: Failed to open video capture device." << std::endl;
        return 1;
    }

    // Sleep for 1 second
    sleep(1);

      
    
   // cv::VideoCapture cap(SOURCE);
    // Decrease frame size
    video_capture.set(cv::CAP_PROP_FRAME_WIDTH, 1200);  //1000
    video_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 1000);  //600

    cv::Mat inputImage = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");
    
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

    cv::Mat frame;
    cv::Mat frame_hud;

    cv::Mat bgr2gray;
    
    cv::Mat contourHierarchy;
    cv::Mat resultImage;
    

    
    cv::Scalar lower(109, 0, 116);
    cv::Scalar upper(153, 255, 255);

    // Variables for FPS calculation
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Position excessiveDeviationPosition;
    
    HUD hud;

    while (true) {
        // Capture frames from the camera
        video_capture.read(inputImage);

        if (inputImage.empty())
            break;

        // Get a frame from the webcam or video
        video_capture >> frame;
        
        //cv::Mat
      //  frame = cv::imread("/Users/bernardolorenzini/Desktop/testh.png");

            // Convert the input image to grayscale
        cv::cvtColor(inputImage, bgr2gray, cv::COLOR_BGR2GRAY);


        cv::cvtColor(inputImage, hsv_frame, cv::COLOR_BGR2HSV);
        
        
        cv::inRange(hsv_frame, lower, upper, hsv_mask);
        
        cv::add(bgr2gray, hsv_mask, blue_filtered_greyscale);
        cv::bilateralFilter(blue_filtered_greyscale, blur, 9, 50, 50);


        
            // Apply Otsu's thresholding
        
        cv::threshold(blur, mask, 250, 255, cv::THRESH_OTSU);

        
            // Apply Canny edge detection
        cv::Canny(mask, edges, 200, 250);
        cv::imshow("mask", mask);
        cv::imshow("edges", edges);


            
            // Find contours
            std::vector<std::vector<cv::Point>> contours;

            int chain = cv::CHAIN_APPROX_NONE;

            cv::findContours(edges.clone(), contours, contourHierarchy, cv::RETR_TREE, chain);
            
            resultImage = inputImage.clone();
        
        
            cv::drawContours(resultImage, contours, -1, cv::Scalar(0, 0, 255), 2);

            //cv::imshow("resultImage with all countors", resultImage);

            // Find the contour with the largest area
            double largest_area = -1.0f;
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
        double average_x = 0.0f;
        double average_y = 0.0f;

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
            int step_size = x_edge_points.size() / maximum_number_of_points;
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

        // Calculate FPS
        frameCount++;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

        if (elapsedTime >= 1) {
            double fps = static_cast<double>(frameCount) / elapsedTime;
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            startTime = endTime;
            fps_toHUD = fps;
        }
        
        draw_hud(resultImage, 1.0f, 1.0f, fps_toHUD, true, false);
        
        frame_hud = resultImage;

    
    drawFPV(frame_hud, excessiveDeviationPosition);
    
                // Detect keyboard input and update positions
        int key = cv::waitKey(1);
        switch (key) {
            case '1':
            {
                selectMode(hud, key);

                excessiveDeviationPosition = {0, 0};
                FPV_Excessive_Deviation(frame_hud, excessiveDeviationPosition);
                
                break;
            }
            case '2':
            {
                selectMode(hud, key);

                overlayImageAtPosition(frame_hud, pitchArrow, {0, 0});
                break;
            }
            case '3':
            {
                selectMode(hud, key);

                overlayImageAtPosition(frame_hud, fpvGround, {0, 0});
                break;
            }
            case '4':
            {
                selectMode(hud, key);

                overlayImageAtPosition(frame_hud, fpvDashed, {0, 0});
                break;
            }
            case '5':
            {
                selectMode(hud, key);

                overlayImageAtPosition(frame_hud, lateralDeviationScale, {0,0});
                break;
            }
//                    case 'w':
//                        indicatorPosition.y -= 15;
//                        break;
//                    case 's':
//                        indicatorPosition.y += 15;
//                        break;
//                    case 'a':
//                        indicatorPosition.x -= 15;
//                        break;
//                    case 'd':
//                        indicatorPosition.x += 15;
//                        break;
//                    default:
//                        break;
        }
        
        cv::imshow("FRAME WITH HUD AND CENTER CIRCLE", frame_hud);

        
/*        int key = cv::waitKey(1);
                // Do things based on detected user input
        if (key == 'q' || key == 'Q' || key == 27) {
            //cv::destroyAllWindows();
            //cv::imshow("Real-time Display", paused_frame);
            //render_image = !render_image;
            //std::cout << "Real-time display: " << render_image << std::endl;
            break;
        }
*/
        // Close the output video by pressing 'ESC'
        
    }

    // Release the camera capture and destroy windows
    video_capture.release();
    cv::destroyAllWindows();

    return 0;

}

// Function to draw the heads-up display (HUD) on the frame
void draw_hud(cv::Mat &frame, double roll, double pitch, double fps, bool is_good_horizon, bool recording) {
    // Convert roll and pitch to integers if they are valid and good_horizon is true
        if (roll && is_good_horizon) {
            roll = std::round(roll);
            pitch = std::round(pitch);
        } else {
            roll = std::numeric_limits<double>::quiet_NaN();
            pitch = std::numeric_limits<double>::quiet_NaN();
        }
    // Define text properties
        int font = cv::FONT_HERSHEY_COMPLEX_SMALL;
        double font_scale = 1.0f;
        int thickness = 1;
        cv::Scalar text_color = is_good_horizon ? cv::Scalar(111, 238, 38) : cv::Scalar(111, 238, 38);
        
        // Display roll, pitch
        cv::putText(frame, "Roll: " + std::to_string(static_cast<int>(roll)), cv::Point(20, 40), font, font_scale, text_color, thickness);
        cv::putText(frame, "Pitch: " + std::to_string(static_cast<int>(pitch)), cv::Point(20, 80), font, font_scale, text_color, thickness);

    // Check if fps is a valid numeric value
    if (!std::isnan(fps)) {
        // Round fps to 2 decimal places using std::setprecision
        std::ostringstream fps_str;
        fps_str << std::fixed << std::setprecision(2) << fps;
        std::string fps_text = "FPS: " + fps_str.str();
        cv::putText(frame, fps_text, cv::Point(20, 120), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0f, cv::Scalar(111, 238, 38), 1);
    } else {
        // Handle the case where fps is not a valid numeric value
        cv::putText(frame, "FPS: N/A", cv::Point(20, 120), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0f, cv::Scalar(111, 238, 38), 1);
    }
            
    cv::Point center(frame.cols / 2, frame.rows / 2);
    int radius = frame.rows / 50;

    // Draw the center circle
    cv::circle(frame, center, radius, cv::Scalar(111, 238, 38), 2); // Using blue color (BGR format)
    
}

std::vector<cv::Point> findPoints(double m, double b, const cv::Size& frame_shape) {
    std::vector<cv::Point> points_to_return;

    // Special condition if slope is 0
    if (m == 0) {
        int bx = cvRound(b);
        cv::Point p1(0, bx);
        cv::Point p2(frame_shape.width, bx);
        points_to_return.push_back(p1);
        points_to_return.push_back(p2);
        return points_to_return;
    }

    // Left
    if (0 < b && b <= frame_shape.height) {
        int px = 0;
        int py = cvRound(b);
        points_to_return.emplace_back(px, py);
    }

    // Top
    if (0 < -b / m && -b / m <= frame_shape.width) {
        int px = cvRound(-b / m);
        int py = 0;
        points_to_return.emplace_back(px, py);
    }

    // Right
    if (0 < m * frame_shape.width + b && m * frame_shape.width + b <= frame_shape.height) {
        int px = frame_shape.width;
        int py = cvRound(m * frame_shape.width + b);
        points_to_return.emplace_back(px, py);
    }

    // Bottom
    if (0 < (frame_shape.height - b) / m && (frame_shape.height - b) / m <= frame_shape.width) {
        int px = cvRound((frame_shape.height - b) / m);
        int py = frame_shape.height;
        points_to_return.emplace_back(px, py);
    }

    return points_to_return;
}

cv::Mat draw_roi(cv::Mat frame, const std::map<std::string, int>& crop_and_scale_parameters) {
    // Extract relevant values from the dictionary
    int cropping_start = crop_and_scale_parameters.at("cropping_start");
    int cropping_end = crop_and_scale_parameters.at("cropping_end");

    // Define the points for the ROI boundaries
    cv::Point p1(cropping_start, 0);
    cv::Point p2(cropping_start, frame.rows);
    cv::Point p3(cropping_end, 0);
    cv::Point p4(cropping_end, frame.rows);

    // Define the color (off-white in BGR format)
    cv::Scalar off_white(215, 215, 215);

    // Draw the lines on the frame
    cv::line(frame, p1, p2, off_white, 1);
    cv::line(frame, p3, p4, off_white, 1);

    return frame;
}


cv::Mat draw_horizon(cv::Mat frame, double roll, double pitch, double fov, cv::Scalar color, bool draw_groundline) {
    // If no horizon data is provided, terminate the function early and return the input frame
    if (std::isnan(roll)) {
        return frame;
    }

    // Convert roll to radians
    roll = CV_PI * roll / 180.0;

    // Determine if the sky is up or down based on the roll
    bool sky_is_up = (roll >= 0.75 * FULL_ROTATION_RADIANS || (roll > 0 && roll <= 0.25 * FULL_ROTATION_RADIANS));

    // Find the distance
    double distance = pitch / fov * frame.rows;

    // Define the line perpendicular to the horizon
    double angle_perp = roll + CV_PI / 2;
    double x_perp = distance * cos(angle_perp) + frame.cols / 2.0;
    double y_perp = distance * sin(angle_perp) + frame.rows / 2.0;

    // Define the horizon line
    double run = cos(roll);
    double rise = sin(roll);
    double m, b;
    
    if (run != 0) {
        m = rise / run;
        b = y_perp - m * x_perp;
        std::vector<cv::Point> points = findPoints(m, b, frame.size());
        if (points.empty()) {
            return frame;
        }
        cv::Point p1 = points[0];
        cv::Point p2 = points[1];
        cv::line(frame, p1, p2, color, 2);
    } else {
        cv::Point p1(static_cast<int>(round(x_perp)), 0);
        cv::Point p2(static_cast<int>(round(x_perp)), frame.rows);
        cv::line(frame, p1, p2, color, 2);
    }

    // Draw the ground line if specified
    if (draw_groundline && run != 0) {
        double m_perp = -1.0f / m;
        double b_perp = y_perp - m_perp * x_perp;
        std::vector<cv::Point> points = findPoints(m_perp, b_perp, frame.size());
        if (!points.empty()) {
            bool above_line = m * points[0].x + b < points[0].y;
            cv::Point p2;
            if ((sky_is_up && above_line) || (!sky_is_up && !above_line)) {
                p2 = points[0];
            } else {
                p2 = points[1];
            }
            int p1x = static_cast<int>(round(x_perp));
            int p1y = static_cast<int>(round(y_perp));
            cv::Point p1(p1x, p1y);
            cv::line(frame, p1, p2, cv::Scalar(0, 191, 255), 1);
        }
    }

    return frame;
}

