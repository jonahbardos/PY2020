#pragma once

#include "Detector.h"

#include <opencv2/core.hpp>

#include <vector>

namespace AR
{

class StereoDetector
{
private:
	Detector left_detector;
	Detector right_detector;

public:
	StereoDetector(CameraParams left_params, CameraParams right_params);
	std::vector<Tag> findTags(cv::Mat left_frame, cv::Mat right_frame, cv::Mat &left_grayscale,
							  cv::Mat &right_grayscale, cv::Mat &left_edges,
							  cv::Mat &right_edges, int canny_thresh_1 = THRESH_1_DEF,
							  int canny_thresh_2 = THRESH_2_DEF, int blur_size = BLUR_DEF);
	
	std::vector<Tag> findTags(cv::Mat left_frame, cv::Mat right_frame,
							  int canny_thresh_1 = THRESH_1_DEF,
							  int canny_thresh_2 = THRESH_2_DEF,
							  int blur_size = BLUR_DEF);
	
	std::vector<Tag> findTags(cv::Mat unified_frame, cv::Mat &grayscale, cv::Mat &edges,
							  int canny_thresh_1 = THRESH_1_DEF,
							  int canny_thresh_2 = THRESH_2_DEF, int blur_size = BLUR_DEF);
	
    std::vector<Tag> findTags(cv::Mat unified_frame, int canny_thresh_1 = THRESH_1_DEF,
							  int canny_thresh_2 = THRESH_2_DEF, int blur_size = BLUR_DEF);
};

}
