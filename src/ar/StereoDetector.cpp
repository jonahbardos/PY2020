#include "StereoDetector.h"

namespace AR
{

StereoDetector::StereoDetector(CameraParams left_params, CameraParams right_params)
	: left_detector(left_params), right_detector(right_params)
{}

std::vector<Tag> StereoDetector::findTags(cv::Mat left_frame, cv::Mat right_frame)
{
	std::vector<Tag> leftTags = left_detector.findTags(left_frame);
	std::vector<Tag> rightTags = right_detector.findTags(right_frame);
	if (leftTags.size() == 0)
	{
		return rightTags;
	}
	else
	{
		return leftTags;
	}
}

std::vector<Tag> StereoDetector::findTags(cv::Mat unified_frame)
{
	int half_width = unified_frame.cols / 2;
	int height = unified_frame.rows;
	cv::Mat left_frame = unified_frame(cv::Rect(0,0,half_width,height));
	cv::Mat right_frame = unified_frame(cv::Rect(half_width,0,half_width,height));
	return findTags(left_frame, right_frame);
}

}
