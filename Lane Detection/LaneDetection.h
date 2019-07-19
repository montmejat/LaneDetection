#pragma once

#include <opencv2/core/types.hpp>
#include "Line.h"

Line getLeftLane(std::vector<Line> lines);
Line getRightLane(std::vector<Line> lines);
void findLanes(cv::Mat image);
void crop(cv::Mat original, cv::Mat out, std::vector<std::vector<cv::Point>> vertices);