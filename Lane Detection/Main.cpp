#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "LaneDetection.h"

using namespace cv;
using namespace std;

int main(int argc, char * argv[])
{
	string file;
	if (argc == 2)
	{
		file = string(argv[1]);
	}
	else
	{
		file = string("data/test_images/solidYellowLeft.jpg");
	}

	Mat image;
	image = imread(file.c_str(), IMREAD_COLOR);

	if (image.empty())
	{
		printf("Coulnd't find file %s.", file.c_str());
		return -1;
	}

	cvtColor(image, image, COLOR_BGR2RGB);
	findLanes(image);

	return 0;
}