//
//  find_horizon.cpp
//  OpenCV_Course02
//
//  Created by Bernardo Lorenzini on 15/08/23.
//

#include "find_horizon.hpp"
#include <numeric>
#include <cmath>


const int FULL_ROTATION = 360;
const int POOLING_KERNEL_SIZE = 5;

HorizonDetector::HorizonDetector(float exclusion_thresh, float fov, float acceptable_variance, cv::Size frame_shape) {
    this->exclusion_thresh = exclusion_thresh;
    this->exclusion_thresh_pixels = static_cast<int>(exclusion_thresh * frame_shape.height / fov);
    this->fov = fov;
    this->acceptable_variance = acceptable_variance;
    this->predicted_roll = 0.0;
    this->predicted_pitch = 0.0;
    this->recent_horizons = {0.0, 0.0};
}

double radians(double degrees) {
        return degrees * M_PI / 180.0;
    }


std::tuple<float, float, float, int, cv::Mat> HorizonDetector::findHorizon(cv::Mat frame, bool diagnostic_mode) {
    float roll = 0.0, pitch = 0.0, variance = 0.0;
    int is_good_horizon = 0;

    // Convert frame to grayscale
        cv::Mat bgr2gray;
        cv::cvtColor(frame, bgr2gray, cv::COLOR_BGR2GRAY);

        // Filter out blue from the sky
        cv::Mat hsv, hsv_mask, blue_filtered_greyscale;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, cv::Scalar(109, 0, 116), cv::Scalar(153, 255, 255), hsv_mask);
        cv::add(bgr2gray, hsv_mask, blue_filtered_greyscale);

        // Generate mask
        cv::Mat blur, mask, edges;
        cv::bilateralFilter(blue_filtered_greyscale, blur, 9, 50, 50);
        cv::threshold(blur, mask, 250, 255, cv::THRESH_OTSU);
        cv::Canny(bgr2gray, edges, 200, 250);

        // Downsample edges using skimage.measure.block_reduce
        cv::Mat pooled_edges = cv::Mat::zeros(edges.size() / POOLING_KERNEL_SIZE, edges.type());
        for (int r = 0; r < pooled_edges.rows; ++r) {
            for (int c = 0; c < pooled_edges.cols; ++c) {
                int max_val = 0;
                for (int i = 0; i < POOLING_KERNEL_SIZE; ++i) {
                    for (int j = 0; j < POOLING_KERNEL_SIZE; ++j) {
                        int val = edges.at<uchar>(r * POOLING_KERNEL_SIZE + i, c * POOLING_KERNEL_SIZE + j);
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
                pooled_edges.at<uchar>(r, c) = max_val;
            }
        }

        // Find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    if (contours.size() == 0) {
        // predict the next horizon
        this->_predict_next_horizon();

        // convert the diagnostic image to color
        if (diagnostic_mode) {
            cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);
        }
        return std::make_tuple(roll, pitch, variance, is_good_horizon, mask);
    }
    
    
    // Find the contour with the largest area
      size_t largest_contour_idx = 0;
      double largest_contour_area = 0.0;

      for (size_t i = 0; i < contours.size(); ++i) {
          double contour_area = cv::contourArea(contours[i]);
          if (contour_area > largest_contour_area) {
              largest_contour_area = contour_area;
              largest_contour_idx = i;
          }
      }

      std::vector<cv::Point> largest_contour = contours[largest_contour_idx];
      std::vector<int> x_original, y_original;

      for (size_t i = 0; i < largest_contour.size(); ++i) {
          x_original.push_back(largest_contour[i].x);
          y_original.push_back(largest_contour[i].y);
      }

        // Separate the points that lie on the edge of the frame from all other points.
        // Edge points will be used to find sky_is_up.
        // All other points will be used to find the horizon.
        std::vector<int> x_abbr, y_abbr, x_edge_points, y_edge_points;
        for (size_t n = 0; n < x_original.size(); ++n) {
            int x_point = x_original[n];
            int y_point = y_original[n];
            if (x_point == 0 || x_point == frame.cols - 1 ||
                y_point == 0 || y_point == frame.rows - 1) {
                x_edge_points.push_back(x_point);
                y_edge_points.push_back(y_point);
            } else {
                x_abbr.push_back(x_point);
                y_abbr.push_back(y_point);
            }
        }
    
    // Filter out points that don't lie on an edge.
    std::vector<int> x_filtered, y_filtered;
    for (size_t idx = 0; idx < x_abbr.size(); ++idx) {
        int x_point = x_abbr[idx];
        int y_point = y_abbr[idx];

        // Evaluate if the point exists on an edge
        if (pooled_edges.at<uchar>(y_point / POOLING_KERNEL_SIZE, x_point / POOLING_KERNEL_SIZE) == 0) {
            continue; // Do not append the point
        }

        // If there is no predicted horizon, perform no further
        // filtering on this point and accept it as valid.
        if (predicted_roll == 0.0) {
            x_filtered.push_back(x_point);
            y_filtered.push_back(y_point);
            continue;
        }

        // If there is a predicted horizon, check if the current point
        // is reasonably close to it.
        int p3[2] = { x_point, y_point };
        int distance = cv::norm(cv::Mat(p2_minus_p1), cv::Mat(p1 - p3)) / cv::norm(cv::Mat(p2_minus_p1));
        if (distance < exclusion_thresh_pixels) {
            x_filtered.push_back(x_point);
            y_filtered.push_back(y_point);
        }
    }
    // Convert filtered points to cv::Mat
    cv::Mat x_filtered_mat(x_filtered.size(), 1, CV_32F, x_filtered.data());
    cv::Mat y_filtered_mat(y_filtered.size(), 1, CV_32F, y_filtered.data());

    // Perform polyfit
    cv::Vec4f line_params; // [vx, vy, x0, y0]
    cv::fitLine(x_filtered_mat, y_filtered_mat, line_params, cv::DIST_L2, 0, 0.01, 0.01);

    // Calculate roll from line parameters
    double m = line_params[1] / line_params[0];
    roll = std::atan(m);
    roll = roll * 180.0 / M_PI;

    // Determine the direction of the sky (above or below)
    int sky_is_up;
    if (m * avg_x + line_params[2] > avg_y) {
        sky_is_up = 1; // Above
    } else {
        sky_is_up = 0; // Below
    }

    // Calculate pitch
    double distance_to_horizon = cv::norm(cv::Point2d(line_params[2], line_params[3]) - cv::Point2d(p1[0], p1[1]));
    int plane_pointing_up = 0;
    if (p3[1] < m * frame.cols / 2 + line_params[2] && sky_is_up == 1) {
        plane_pointing_up = 1;
    } else if (p3[1] > m * frame.cols / 2 + line_params[2] && sky_is_up == 0) {
        plane_pointing_up = 1;
    }

    pitch = distance_to_horizon / frame.rows * fov;
    if (!plane_pointing_up) {
        pitch *= -1;
    }

    // Calculate variance
    double sum_distance = 0.0;
    for (size_t n = 0; n < x_filtered.size(); ++n) {
        int x_point = x_filtered[n];
        int y_point = y_filtered[n];
        double p3[2] = { x_point, y_point };
        double distance = cv::norm(cv::Point2d(line_params[2], line_params[3]) - cv::Point2d(p1[0], p1[1]));
        sum_distance += distance;
    }

    double average_distance = sum_distance / x_filtered.size();
    variance = average_distance / frame.rows * 100.0;

    // Adjust the roll within the range of 0 - 360 degrees
    roll = _adjust_roll(roll, sky_is_up);

    // Determine if the horizon is acceptable
    if (variance < acceptable_variance) {
        is_good_horizon = 1;
    }

    // Predict the approximate position of the next horizon
    _predict_next_horizon(roll, pitch, is_good_horizon);

    // Return the calculated values for horizon
    return std::make_tuple(roll, pitch, variance, is_good_horizon, mask);
}

float HorizonDetector::_adjust_roll(float roll, int sky_is_up) {
    roll = std::abs(roll) * FULL_ROTATION / 360.0;
    bool in_sky_is_up_sector = (roll >= FULL_ROTATION * 0.75 || (roll > 0 && roll <= FULL_ROTATION * 0.25));
    
    if (sky_is_up == in_sky_is_up_sector) {
        return roll;
    }
    
    if (roll < FULL_ROTATION / 2) {
        roll += FULL_ROTATION / 2;
    } else {
        roll -= FULL_ROTATION / 2;
    }
    
    return roll;
}

// Implement the remaining private methods if needed...
