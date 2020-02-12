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
	std::vector<Tag> findTags(cv::Mat left_frame, cv::Mat right_frame);
	std::vector<Tag> findTags(cv::Mat unified_frame);
};

}
