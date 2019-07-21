#pragma once

#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>
#include "Line.h"

Line getLeftLane(std::vector<Line> lines);
Line getRightLane(std::vector<Line> lines);
cv::Mat findLanes(cv::Mat image);
void findLanes(cv::Mat image, Line& leftLane, Line& rightLane);
void findLanes(cv::VideoCapture video, cv::VideoWriter& videoOut);
void crop(cv::Mat original, cv::Mat& out, std::vector<std::vector<cv::Point>> vertices);
void smoothLanes(std::vector<Line>& lines);
cv::Mat drawLanes(cv::Mat image, Line leftLane, Line rightLane);