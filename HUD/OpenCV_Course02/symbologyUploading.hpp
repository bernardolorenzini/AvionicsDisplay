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
cv::Mat fpvExcessiveDeviation = cv::imread("/Users/bernardolorenzini/Documents/MyGitRepositories/Textures/FPV_Excessive_Deviation.png", cv::IMREAD_UNCHANGED);

cv::Mat selectSymbology(int key);


#endif /* symbologyUploading */
