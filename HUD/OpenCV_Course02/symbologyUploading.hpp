//
//  simbologyUploading.hpp
//  OpenCV_Course02
//
//  Created by Bernardo Lorenzini on 05/08/23.
//

#ifndef symbologyUploading_hpp
#define symbologyUploading_hpp
#include <opencv2/opencv.hpp>

cv::Mat uaRing = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/UA_Ring.png", cv::IMREAD_UNCHANGED);
cv::Mat pitchArrow = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/Pitch_Arrow.png", cv::IMREAD_UNCHANGED);
cv::Mat fpvGround = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/FPV_Ground.png", cv::IMREAD_UNCHANGED);
cv::Mat fpvDashed = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/FPV_Dashed.png", cv::IMREAD_UNCHANGED);
cv::Mat SpeedScaleIndicator = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/Speed_Scale-Indicated_AirSpeed.png", cv::IMREAD_UNCHANGED);
cv::Mat airspeedLadder = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/Airspeed-Limits-Ladder-Middle.png", cv::IMREAD_UNCHANGED);
cv::Mat fpvExcessiveDeviation = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/FPV_Excessive_Deviation.png", cv::IMREAD_UNCHANGED);

cv::Mat RollScaleTicks = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/Roll_Scale-Ticks.png", cv::IMREAD_UNCHANGED);

cv::Mat Gears_locked_down = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/Gears_locked_down.png", cv::IMREAD_UNCHANGED);



cv::Mat selectSymbology(int key);


#endif /* symbologyUploading */
