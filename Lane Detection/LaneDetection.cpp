#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cstdlib>
#include "LaneDetection.h"
#include "Line.h"

#define WIDTH 960
#define HEIGHT 540

using namespace cv;
using namespace std;

Line getLeftLane(vector<Line> lines)
{
	float medianSlope = 0;
	float medianBias = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		medianSlope += lines[i].slope;
		medianBias += lines[i].bias;
	}

	medianSlope /= lines.size();
	medianBias /= lines.size();

	return Line(0, medianBias, -(medianBias / medianSlope), 0);
}

Line getRightLane(vector<Line> lines)
{
	float medianSlope = 0;
	float medianBias = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		medianSlope += lines[i].slope;
		medianBias += lines[i].bias;
	}

	medianSlope /= lines.size();
	medianBias /= lines.size();

	return Line(0, medianBias, (WIDTH - medianBias) / medianSlope, WIDTH);
}

void findLanes(Mat image)
{
	Mat original = image.clone();
	Mat out = image.clone();

	resize(image, image, Size(WIDTH, HEIGHT));
	cvtColor(image, image, COLOR_BGR2GRAY);

	// bluring image to avoid detecting small and not usefull lines
	GaussianBlur(image, image, Size(17, 17), 0);

	// edges detection
	Mat edges;
	Canny(image, edges, 50, 80);

	// hough transform
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 2, CV_PI / 180, 1, 15, 5);

	vector<Line> leftLanes;
	vector<Line> rightLanes;
	for (int i = 0; i < lines.size(); i++)
	{
		Line currentLine = Line(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);

		// filter lines that have a slope going from 30 to 60 degrees
		if (abs(currentLine.slope) >= 0.5 && abs(currentLine.slope) <= 2)
		{
			if (currentLine.slope <= 0)
			{
				leftLanes.push_back(currentLine);
			}
			else
			{
				rightLanes.push_back(currentLine);
			}
		}
	}

	Line leftLane = getLeftLane(leftLanes);
	Line rightLane = getRightLane(rightLanes);

	leftLane.draw(out);
	rightLane.draw(out);
	
	vector<Point> points;
	points.push_back(Point(50, HEIGHT));
	points.push_back(Point(440, 325));
	points.push_back(Point(500, 325));
	points.push_back(Point(WIDTH - 50, HEIGHT));
	vector<vector<Point>> verticles;
	verticles.push_back(points);
	crop(original, out, verticles);

	cvtColor(out, out, COLOR_RGB2BGR);
	imwrite("out.jpg", out);
}

void crop(Mat original, Mat out, vector<vector<Point>> vertices)
{
	Mat temp = out.clone();
	Mat mask = Mat::zeros(original.rows, original.cols, original.type());
	fillPoly(mask, vertices, Scalar(255, 255, 255));

	// keep only the lanes polygone
	bitwise_and(out, mask, out);

	// mask the lanes
	bitwise_not(mask, mask);
	bitwise_and(original, mask, temp);

	// return the sum of both
	addWeighted(out, 1, temp, 1, 0, out);
}