#include "StereoDetector.h"

namespace AR
{

StereoDetector::StereoDetector(CameraParams left_params, CameraParams right_params)
	: left_detector(left_params), right_detector(right_params)
{}

std::vector<Tag> StereoDetector::findTags(cv::Mat left_frame, cv::Mat right_frame,
										  cv::Mat &left_grayscale, cv::Mat &right_grayscale,
										  cv::Mat &left_edges, cv::Mat &right_edges,
										  int canny_thresh_1, int canny_thresh_2,
										  int blur_size)
{
	std::vector<Tag> leftTags =
		left_detector.findTags(left_frame, left_grayscale, left_edges,
							   canny_thresh_1, canny_thresh_2, blur_size);
	std::vector<Tag> rightTags =
		right_detector.findTags(right_frame, right_grayscale, right_edges,
								canny_thresh_1, canny_thresh_2, blur_size);
	if (leftTags.size() == 0)
	{
		return rightTags;
	}
	else
	{
		return leftTags;
	}
}

std::vector<Tag> StereoDetector::findTags(cv::Mat unified_frame, cv::Mat &grayscale,
										  cv::Mat &edges, int canny_thresh_1,
										  int canny_thresh_2, int blur_size)
{
	int half_width = unified_frame.cols / 2;
	int height = unified_frame.rows;
	cv::Mat left_frame = unified_frame(cv::Rect(0,0,half_width,height));
	cv::Mat right_frame = unified_frame(cv::Rect(half_width,0,half_width,height));
	cv::Mat left_grayscale;
	cv::Mat right_grayscale;
	cv::Mat left_edges;
	cv::Mat right_edges;
	std::vector<Tag> results = findTags(left_frame, right_frame, left_grayscale,
										right_grayscale, left_edges, right_edges,
										canny_thresh_1, canny_thresh_2, blur_size);
	cv::hconcat(left_grayscale, right_grayscale, grayscale);
	cv::hconcat(left_edges, right_edges, edges);
	return results;
}

std::vector<Tag> StereoDetector::findTags(cv::Mat left_frame, cv::Mat right_frame,
										  int canny_thresh_1, int canny_thresh_2,
										  int blur_size)
{
	cv::Mat junk;
	return findTags(left_frame, right_frame, junk, junk, junk, junk, canny_thresh_1,
					canny_thresh_2, blur_size);
}

std::vector<Tag> StereoDetector::findTags(cv::Mat unified_frame, int canny_thresh_1,
										  int canny_thresh_2, int blur_size)
{
	cv::Mat junk;
	return findTags(unified_frame, junk, junk, canny_thresh_1, canny_thresh_2,
					blur_size);
}

}
