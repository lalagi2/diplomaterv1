#include "stdafx.h"
#include "Corner.h"

void Corner::printFingerPrint()
{

}

void Corner::run(std::vector<cv::Mat> keyFrames)
{
	for (int frame = 0; frame < keyFrames.size(); frame++)
	{
		cv::Mat SobelX;
		cv::Mat SobelY;
		cv::Mat SobelXY;

		cv::Sobel(keyFrames[frame], SobelX, CV_8UC1, 1, 0);
		cv::Sobel(keyFrames[frame], SobelY, CV_8UC1, 0, 1);
		cv::Sobel(SobelX, SobelXY, CV_8UC1, 0, 1);

		for (int row = 5; row < keyFrames[frame].rows - 5; row += 5)
		{
			for (int column = 5; column < keyFrames[frame].cols - 5; column += 5)
			{
				cv::Rect part;

				part.x = column - 5;
				part.width = 11;
				part.y = row - 5;
				part.height = 11;

				cv::Mat dx = SobelX(part);
				cv::Mat dy = SobelY(part);

				cv::Mat dxx = dx.mul(dx);
				cv::Mat dyy = dy.mul(dy);

				float dxsum = cv::sum(dxx)[0];
				float dysum = cv::sum(dyy)[0];

				cv::Mat dxdy = SobelXY(part);
				float dxdysum = cv::sum(dxdy)[0];

				float trace = dxsum + dysum;
				float det = dxsum * dysum;// - dxdysum * dxdysum;

				float R = (det - 0.06f * trace * trace) / 10;

				if (R > 8000000)
				{
					cv::Point2i cornerCoord;
					cornerCoord.x = column;
					cornerCoord.y = row;
					CornerPoint cp;
					cp.R = R;
					cp.coord = cornerCoord;
					cornerPoints.push_back(cp);
				}
			}
		}

		for (auto c : cornerPoints)
		{
			cv::Point corner;
			corner.x = c.coord.x;
			corner.y = c.coord.y;
			//cv::circle(keyFrames[frame], corner, 1, cv::Scalar(255, 0, 255), 1);
		}

		cv::Mat a;
		cornerHarris(keyFrames[frame], a, 2, 3, 0.06);
		a = a * 255;
		cv::imshow("video", a);
		std::cout << "a";
		cv::waitKey(0);

	}
}



