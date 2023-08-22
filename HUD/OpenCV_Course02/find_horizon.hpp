//
//  find_horizon.hpp
//  OpenCV_Course02
//
//  Created by Bernardo Lorenzini on 15/08/23.
//

#ifndef FIND_HORIZON_HPP
#define FIND_HORIZON_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>

const int FULL_ROTATION = 360;
const int POOLING_KERNEL_SIZE = 5;

class HorizonDetector {
public:
    HorizonDetector(float exclusion_thresh, float fov, float acceptable_variance, cv::Size frame_shape);
    std::tuple<float, float, float, int, cv::Mat> findHorizon(cv::Mat frame, bool diagnostic_mode = false);

private:
    float exclusion_thresh;
    int exclusion_thresh_pixels;
    float fov;
    float acceptable_variance;
    float predicted_roll;
    float predicted_pitch;
    std::vector<float> recent_horizons;

    void _predict_next_horizon(float roll = 0.0, float pitch = 0.0, int is_good_horizon = 0);
    float _adjust_roll(float roll, int sky_is_up);
};

#endif


