#include <stdio.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "LaneDetection.h"

#define WIDTH 960
#define HEIGHT 540

using namespace cv;
using namespace std;

int main(int argc, char * argv[])
{
	string file;
	string outDestination;

	if (argc == 2)
	{
		file = string(argv[1]);
		if (file.find(".jpg") != string::npos)
		{
			outDestination = "out.jpg";
		}
		else
		{
			outDestination = "out.mp4";
		}
	}
	else if (argc == 3)
	{
		file = string(argv[1]);
		outDestination = string(argv[2]);
	}
	else
	{
		/*file = string("data/test_images/night2.jpg");
		outDestination = "out/out.jpg";*/
		file = string("data/test_videos/night.mp4");
		outDestination = "out/out.avi";
	}

	if (file.find(".jpg") != string::npos)
	{
		Mat image;
		image = imread(file.c_str(), IMREAD_COLOR);

		if (image.empty())
		{
			printf("Coulnd't find file %s.", file.c_str());
			return -1;
		}

		cvtColor(image, image, COLOR_BGR2RGB);
		resize(image, image, Size(WIDTH, HEIGHT));

		Mat out = findLanes(image);
		cvtColor(out, out, COLOR_RGB2BGR);
		imwrite(outDestination, out);
	}
	else
	{
		VideoCapture video = VideoCapture(file);
		VideoWriter videoOut = VideoWriter(outDestination, VideoWriter::fourcc('D', 'I', 'V', 'X'), 20, Size(WIDTH, HEIGHT));

		if (video.isOpened())
		{
			findLanes(video, videoOut);
		}

		video.release();
		videoOut.release();
	}

	return 0;
}