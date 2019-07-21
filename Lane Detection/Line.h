#pragma once

#include <opencv2/core/mat.hpp>

class Line
{
private:
	int m_x1;
	int m_y1;
	int m_x2;
	int m_y2;
	float m_slope;
	float m_bias;

public:

	Line();
	Line(int x1, int y1, int x2, int y2);

	void draw(cv::Mat& image);
	float slope();
	float bias();
	int x1();
	int x2();
	int y1();
	int y2();
};