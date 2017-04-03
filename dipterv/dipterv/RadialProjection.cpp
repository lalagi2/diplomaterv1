#include "stdafx.h"
#include "RadialProjection.h"

std::vector<cv::Point> RadialProjection::bresenham(cv::Point p0, cv::Point p1)
{
	std::vector<cv::Point> pixelCoordinates;

	float x1 = (float)p0.x;
	float x2 = (float)p1.x;
	float y1 = (float)p0.y;
	float y2 = (float)p1.y;

	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x<maxX; x++)
	{
		if (steep)
		{
			cv::Point p(y, x);
			pixelCoordinates.push_back(p);
		}
		else
		{

			cv::Point p(x, y);
			pixelCoordinates.push_back(p);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}

	return pixelCoordinates;
}

void RadialProjection::printFingerPrint()
{
}

void RadialProjection::calculateFingerPrint(int median)
{
}

int RadialProjection::calculateMedian()
{
	return 0;
}

void RadialProjection::run(std::vector<cv::Mat> keyFrames)
{

}


