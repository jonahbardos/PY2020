#pragma once

#include <opencv2/core.hpp>

namespace AR
{
/**
  Camera parameters are obtained from camera calibration and are unique to and constant for
  every camera.
*/
class CameraParams
{
	private:
		cv::Mat _camera_params;
		cv::Mat _dist_coeff;
	public:
		CameraParams(cv::Mat camera_params, cv::Mat dist_coeff);
		cv::Mat getCameraParams() const;
		cv::Mat getDistCoeff() const;
};

const CameraParams DEFAULT_PARAMS(cv::Mat::eye(3,3,CV_64F),
								  cv::Mat::zeros(3,3,CV_64F));

constexpr double webcam_params[] = {5.5929841506515140e+02, 0., 3.3632998221165434e+02,
									0.,5.5929841506515140e+02, 2.4560280586167062e+02,
									0., 0., 1.};
constexpr double webcam_dist[] = {2.2319725483552814e-02, -2.3138101190867111e-01,
								  3.6220766734074462e-03, 3.8852893952725500e-03,
								  5.4773015987500950e-01};
const CameraParams WEBCAM_PARAMS(cv::Mat(3, 3, CV_64F, *webcam_params),
								 cv::Mat(5, 1, CV_64F, *webcam_dist));

constexpr double laptop_params[] = {5.6534586568739849e+02, 0., 3.5588060437029822e+02,
									0.,5.6534586568739849e+02, 2.9541081641775287e+02,
									0., 0., 1.};
constexpr double laptop_dist[] = {3.4425270669197583e-01, -4.5627505509852968e+00,
								  1.4729564760154447e-04, -1.2416402052553264e-02,
								  1.5720190712074883e+01};
const CameraParams LAPTOP_PARAMS(cv::Mat(3, 3, CV_64F, *laptop_params),
								 cv::Mat(5, 1, CV_64F, *laptop_dist));
}
