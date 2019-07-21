#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
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
		medianSlope += lines[i].slope();
		medianBias += lines[i].bias();
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
		medianSlope += lines[i].slope();
		medianBias += lines[i].bias();
	}

	medianSlope /= lines.size();
	medianBias /= lines.size();

	return Line(0, medianBias, (WIDTH - medianBias) / medianSlope, WIDTH);
}

Mat findLanes(Mat image)
{
	Line rightLane;
	Line leftLane;

	findLanes(image, rightLane, leftLane);
	return drawLanes(image, leftLane, rightLane);
}

void findLanes(Mat image, Line& leftLane, Line& rightLane)
{
	Mat original = image.clone();

	cvtColor(image, image, COLOR_BGR2GRAY);

	// bluring image to avoid detecting small and not usefull lines
	GaussianBlur(image, image, Size(17, 17), 0);

	// edges detection
	Mat edges;
	Canny(image, edges, 55, 75);

	vector<Point> points;
	points.push_back(Point(50, HEIGHT));
	points.push_back(Point(340, 325));
	points.push_back(Point(560, 325));
	points.push_back(Point(WIDTH - 50, HEIGHT));
	vector<vector<Point>> verticles;
	verticles.push_back(points);

	//imwrite("out/out-edge-before.jpg", edges);
	crop(edges, edges, verticles);
	//imwrite("out/out-edge-after.jpg", edges);

	// hough transform
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 2, CV_PI / 180, 1, 15, 5);

	vector<Line> leftLanes;
	vector<Line> rightLanes;
	for (int i = 0; i < lines.size(); i++)
	{
		Line currentLine = Line(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);

		// filter lines that have a slope going from 30 to 60 degrees
		if (abs(currentLine.slope()) >= 0.5 && abs(currentLine.slope()) <= 2)
		{
			if (currentLine.slope() <= 0)
			{
				leftLanes.push_back(currentLine);
			}
			else
			{
				rightLanes.push_back(currentLine);
			}
		}
	}

	leftLane = getLeftLane(leftLanes);
	rightLane = getRightLane(rightLanes);
}

void findLanes(VideoCapture video, VideoWriter& videoOut)
{
	int counter = 0;
	int framesCount = video.get(CAP_PROP_FRAME_COUNT);
	Mat currentImage;

	vector<Line> rightLanes;
	vector<Line> leftLanes;

	printf("Getting lanes...\n");
	while (video.isOpened())
	{
		if (video.read(currentImage))
		{
			printf("%d/%d\n", counter, framesCount);

			Line rightLane;
			Line leftLane;

			cvtColor(currentImage, currentImage, COLOR_BGR2RGB);
			resize(currentImage, currentImage, Size(WIDTH, HEIGHT));
			findLanes(currentImage, rightLane, leftLane);

			rightLanes.push_back(rightLane);
			leftLanes.push_back(leftLane);
		}
		else
		{
			break;
		}

		counter++;
	}

	printf("Smoothing the lanes...\n");
	smoothLanes(leftLanes);
	smoothLanes(rightLanes);

	video.set(CAP_PROP_POS_AVI_RATIO, 0);
	counter = 0;

	printf("Applying the lanes on each frame...\n");
	while (video.isOpened())
	{
		if (video.read(currentImage) && counter < rightLanes.size() && counter < leftLanes.size())
		{
			printf("%d/%d\n", counter, framesCount);

			Line rightLane = rightLanes.at(counter);
			Line leftLane = leftLanes.at(counter);

			cvtColor(currentImage, currentImage, COLOR_BGR2RGB);
			resize(currentImage, currentImage, Size(WIDTH, HEIGHT));
			currentImage = drawLanes(currentImage, leftLane, rightLane);

			cvtColor(currentImage, currentImage, COLOR_RGB2BGR);
			videoOut.write(currentImage);
		}
		else
		{
			break;
		}

		counter++;
	}
}

void crop(Mat original, Mat& out, vector<vector<Point>> vertices)
{
	Mat temp = out.clone();
	Mat mask = Mat::zeros(original.rows, original.cols, original.type());
	fillPoly(mask, vertices, Scalar(255, 255, 255));

	// keep only the lanes polygone
	bitwise_and(out, mask, out);

	// mask the lanes polygone
	bitwise_not(mask, mask);
	bitwise_and(original, mask, temp);

	// return the sum of both
	addWeighted(out, 1, temp, 1, 0, out);
}

void smoothLanes(vector<Line>& lines)
{
	/*if (!lines.size() % 2 == 0)
	{
		lines.push_back(Line(lines.at(lines.size())));
	}*/

	for (int i = 0; i < lines.size() - 2; i += 3)
	{
		Line smoothLine = Line((lines[i].x1() + lines[i + 1].x1() + lines[i + 2].x1()) / 3,
			(lines[i].y1() + lines[i + 1].y1() + lines[i + 2].y1()) / 3,
			(lines[i].x2() + lines[i + 1].x2() + lines[i + 2].x2()) / 3,
			(lines[i].y2() + lines[i + 1].y2() + lines[i + 2].y2()) / 3);
		lines[i] = smoothLine;
		lines[i + 1] = smoothLine;
		lines[i + 2] = smoothLine;
	}
}

Mat drawLanes(cv::Mat image, Line leftLane, Line rightLane)
{
	vector<Point> points;
	points.push_back(Point(50, HEIGHT));
	points.push_back(Point(340, 325));
	points.push_back(Point(560, 325));
	points.push_back(Point(WIDTH - 50, HEIGHT));
	vector<vector<Point>> verticles;
	verticles.push_back(points);

	Mat temp = image.clone();;
	leftLane.draw(temp);
	rightLane.draw(temp);

	crop(image, temp, verticles);

	return temp;
}