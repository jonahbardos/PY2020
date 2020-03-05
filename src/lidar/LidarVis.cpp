#include "LidarVis.h"

#include <iostream>

namespace lidar
{
LidarVis::LidarVis(int win_width, int win_height, cv::Scalar bgr, int max_range)
	: view(win_height, win_width, CV_8UC3, cv::Scalar(bgr[0], bgr[1], bgr[2])), bg_color(bgr),
	  win_width(win_width), win_height(win_height), lidar_max_range(max_range)
{
}

cv::Point LidarVis::worldToCvPoint(PointXY p)
{
	int x = (p.x / (2 * this->lidar_max_range) + 0.5) * this->win_width;
	int y = 0.5 * this->win_width * (1 - p.y / this->lidar_max_range);
	return cv::Point(x, y);
}

void LidarVis::drawPoints(std::vector<PointXY> &pts, cv::Scalar bgr, int pt_radius)
{
	for (PointXY p : pts)
	{
		cv::circle(this->view, worldToCvPoint(p), pt_radius, bgr, -1);
	}
}

void LidarVis::outlinePolygon(std::vector<PointXY> &vertices, cv::Scalar bgr)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		cv::Point p1 = worldToCvPoint(vertices[i]);
		cv::Point p2 = worldToCvPoint(vertices[(i + 1) % vertices.size()]);
		cv::line(this->view, p1, p2, bgr);
	}
}

void LidarVis::drawLidar(cv::Scalar bgr, int symb_px_size)
{
	cv::Point bot_left =
		cv::Point((this->win_width - symb_px_size) / 2, (this->win_height - symb_px_size) / 2);
	cv::Point bot_right = cv::Point((this->win_width + symb_px_size) / 2, bot_left.y);
	cv::Point top_mid = cv::Point((bot_left.x + bot_right.x) / 2, bot_left.y + symb_px_size);
}

void LidarVis::clear()
{
	this->view.setTo(this->bg_color);
}

cv::Mat LidarVis::getView()
{
	return this->view;
}
} // namespace lidar

int main(int argc, char **argv)
{
	using namespace lidar;
	URGLidar lidar;
	if (!lidar.open())
	{
		std::cout << "failed to open lidar" << std::endl;
		return lidar.getError();
	}

	LidarVis vis(vis_win_width, vis_win_height, vis_bg_color, lidar_max_range);
	cv::namedWindow(vis_win_name);
	while (true)
	{
		vis.clear();
		if (!lidar.createFrame())
		{
			std::cout << "failed to create frame" << std::endl;
			return lidar.getError();
		}
		std::vector<Polar2D> polarPts = lidar.getLastFrame();
		std::vector<PointXY> pts(polarPts.size());
		for (Polar2D p : polarPts)
		{
			pts.push_back(lidar::polarToCartesian(p));
		}

		vis.drawPoints(pts, vis_pt_color, vis_pt_radius);
		std::vector<std::vector<PointXY>> clusters = clusterPoints(pts, cluster_sep_threshold);
		for (std::vector<PointXY> cluster : clusters)
		{
			std::vector<PointXY> bounds = convexHull(cluster);
			vis.outlinePolygon(bounds, vis_conv_hull_color);
		}

		cv::imshow(vis_win_name, vis.getView());
		if (cv::waitKey(5) == vis_win_esc)
		{
			break;
		}
	}

	if (!lidar.close())
	{
		std::cout << "failed to close lidar device" << std::endl;
		return lidar.getError();
	}
	return 0;
}
