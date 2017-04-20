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

		cv::Sobel(keyFrames[frame], SobelX, CV_8UC1, 1, 0, 5);
		cv::Sobel(keyFrames[frame], SobelY, CV_8UC1, 0, 1, 5);

		for (int row = 5; row < keyFrames[frame].rows - 5; row ++)
		{
			for (int column = 5; column < keyFrames[frame].cols - 5; column++)
			{
				cv::Mat C(2, 2, CV_8UC1);
				cv::Rect part;

				part.x = column - 5;
				part.width = 11;
				part.y = row - 5;
				part.height = 11;

				cv::Mat dx = SobelX(part);
				cv::Mat dy = SobelY(part);

				//cv::waitKey(1);

				float dxsum = cv::sum(dx)[0];
				float dysum = cv::sum(dy)[0];

				cv::Mat dxdy = dy.mul(dx);
				float dxdysum = cv::sum(dxdy)[0];

				C.at<char>(cv::Point(0, 0)) = dxsum;
				C.at<char>(cv::Point(0, 1)) = dxdysum;
				C.at<char>(cv::Point(1, 0)) = dxdysum;
				C.at<char>(cv::Point(1, 1)) = dysum;

				float trace = dxsum + dysum;
				float det = dxsum * dysum - dxdysum * dxdysum;

				float R = det - 0.06f * trace * trace;

				if (R > 10000000)
				{
					cv::Point2i cornerCoord;
					cornerCoord.x = column;
					cornerCoord.y = row;
					cornerPoints.push_back(cornerCoord);
				}
			}
		}

		for (auto c : cornerPoints)
		{
			cv::Point corner;
			corner.x = c.x;
			corner.y = c.y;
			cv::circle(keyFrames[frame], corner, 5, cv::Scalar(0, 0, 255), 1);
		}

		cv::imshow("video", keyFrames[frame]);
		
		cv::waitKey(0);

	}
}


