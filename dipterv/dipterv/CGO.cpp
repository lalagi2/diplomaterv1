#include "CGO.h"

void CGO::run(cv::Mat keyFrame)
{
	#pragma omp parallel for 
	for (int n = 0; n < 8; n++)
	{
		double szamlaloosszeg = 0;
		double sumM = 0;

		cv::Rect part;
		part.x = 320 * (n % 4);
		if (n < 4)
		{
			part.y = 0;
		}
		else
		{
			part.y = 360;
		}

		part.width = 320;
		part.height = 360;
		cv::Mat partMat = keyFrame(part);
		//cv::imshow("video", keyFrame);
		//cv::imshow("video2", partMat);

		for (int row = 1; row < partMat.rows - 1; row++)
		{
			for (int col = 1; col < partMat.cols - 1; col++)
			{
				char Gx = partMat.at<char>(cv::Point(col + 1, row)) - partMat.at<char>(cv::Point(col - 1, row));
				char Gy = partMat.at<char>(cv::Point(col, row + 1)) - partMat.at<char>(cv::Point(col, row - 1));

				double M = sqrt(Gx * Gx + Gy * Gy);
				double theta;
				if (Gx == 0)
				{
					theta = CV_PI / 2.0;
				}
				else
				{
					theta = atan(Gy / Gx);
				}

				szamlaloosszeg += M * theta;
				sumM += M;
			}
		}

		fingerPrint[n] = szamlaloosszeg / sumM;
	}
}