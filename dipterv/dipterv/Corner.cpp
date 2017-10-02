#include "stdafx.h"
#include "Corner.h"
#include <fstream>

std::vector<std::string> split1(const std::string &text, char sep)
{
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}



std::string Corner::checkMatchInDB(std::vector<std::vector<CornerPoint>> fingerPrintToSearch)
{
	loadDB();

	std::string bestVideo = "Nincs benne az adatbazisban";

	float bestError = std::numeric_limits<float>::max();

	for (auto video : dbFingerPrint)
	{
		std::string videoName = video.first;
		float errorSum = 0.0f;
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < video.second[i].size(); j++)
			{
				if (fingerPrintToSearch[i].size() > 0)
				{
					float dist = distance(fingerPrintToSearch[i][j], video.second[i][j]);
					errorSum += dist;
				}
			}
		}

		if (errorSum < bestError && errorSum < 1) // 1 = threshold
		{
			bestError = errorSum;
			bestVideo = videoName;
		}
	}

	return bestVideo;
}

float Corner::distance(CornerPoint p1, CornerPoint p2)
{
	float difX = fabs(p1.coord.x - p2.coord.x);
	float difY = fabs(p1.coord.y - p2.coord.y);

	return sqrt(difX * difX + difY * difY);
}

void Corner::appendToDatabase(int n, std::vector<int> keyFrameNumbers)
{
	std::ofstream dbFile;
	dbFile.open("Corner_database.txt", std::ios::out | std::ios::app);
	dbFile << std::to_string(n) + ".mp4" << "\n";

	for (int i = 0; i < 5; i++)
	{
		dbFile << keyFrameNumbers[i] << " ";
		for (auto cornerPosition : videoFingerPrints[i])
		{
			dbFile << cornerPosition.coord.x << " " << cornerPosition.coord.y << " " ;
		}

		dbFile << "\n";
	}

	dbFile.close();
}

void Corner::loadDB()
{
	std::vector<std::vector<CornerPoint>> videoFingerPrints;
	std::vector<CornerPoint> fingerPrintNumbers;

	std::string line;
	std::ifstream myfile("Corner_database.txt");
	int lineNum = 0;
	std::string elozoVideoNev;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (lineNum % 6 == 0)
			{
				if (videoFingerPrints.size() > 0)
				{
					dbFingerPrint[elozoVideoNev] = videoFingerPrints;
					videoFingerPrints.clear();
				}
				elozoVideoNev = line;
			}
			else
			{
				std::vector<std::string> values = split1(line, ' ');
				int frame = std::stoi(values[0]);

				for (int i = 1; i < values.size() - 1; i += 2)
				{
					CornerPoint cp;
					cp.R = -1.0f;
					
					cp.coord.x = std::stoi(values[i]);
					cp.coord.y = std::stoi(values[i + 1]);

					fingerPrintNumbers.push_back(cp);
				}

				videoFingerPrints.push_back(fingerPrintNumbers);
				fingerPrintNumbers.clear();
			}

			lineNum++;
		}

		myfile.close();
	}
}

void Corner::printFingerPrint()
{

}

/*void Corner::appendToDatabase(int n, std::vector<int> keyFrameNumbers)
{
	std::ofstream dbFile;
	dbFile.open("CGO_database.txt", std::ios::out | std::ios::app);
	dbFile << std::to_string(n) + ".mp4" << "\n";

	for (int i = 0; i < 5; i++)
	{
		dbFile << keyFrameNumbers[i] << " ";
		for (auto cgo : cornerPoints[i])
		{
			dbFile << cgo << " ";
		}

		dbFile << "\n";
	}

	dbFile.close();
}*/

bool Corner::nincsBenneEddig(int maxX, int maxY)
{
	//for (auto o : cornerPoints)
	//{
	//	if (o.coord.x == maxX && o.coord.y == maxY)
	//	{
	//		return false;
	//	}
	//}

	//return true;
	return true;
}

std::vector<CornerPoint> Corner::run(cv::Mat keyFrame)
{
	std::vector<CornerPoint> fingerPrint;
	auto start = std::chrono::system_clock::now();

	cv::Mat SobelX;
	cv::Mat SobelY;
	cv::Mat SobelXY;

	cv::Sobel(keyFrame, SobelX, CV_8UC1, 1, 0);
	cv::Sobel(keyFrame, SobelY, CV_8UC1, 0, 1);
	cv::Sobel(SobelX, SobelXY, CV_8UC1, 0, 1);
	//#pragma omp parallel for
	for (int row = 5; row < keyFrame.rows - 5; row += 5) // jó ha 10et ugrunk?
	{
		for (int column = 5; column < keyFrame.cols - 5; column += 5)
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
			cv::Mat dxdy = dx.mul(dy);

			float dxsum = cv::sum(dxx)[0];
			float dysum = cv::sum(dyy)[0];
			float dxdysum = cv::sum(dxdy)[0];

			float trace = dxsum + dysum;
			float det = dxsum * dysum - dxdysum * dxdysum;

			float R = (det - 0.06f * trace * trace) / 10.0f;

			if (R > 800000)
			{
				cv::Point2i cornerCoord;
				cornerCoord.x = column;
				cornerCoord.y = row;
				CornerPoint cp;
				cp.R = R;
				cp.coord = cornerCoord;

				if (fingerPrint.size() <= 5) // Csak a 10 legnagyobb sarokértékű pontot tároljuk
				{
					fingerPrint.push_back(cp);
				}
				else // Egyébként lecseréljük a legkisebb R értékűt
				{
					int minIndex = 0;
					for (int index = 0; index < fingerPrint.size(); index++)
					{
						if (fingerPrint[index].R < fingerPrint[minIndex].R) minIndex = index;
					}

					if (cp.R > fingerPrint[minIndex].R)
					{
						fingerPrint[minIndex] = cp;
					}
				}
			}
		}

		/*for (auto c : fingerPrint)
		{
			cv::Point corner;
			corner.x = c.coord.x;
			corner.y = c.coord.y;
			cv::circle(keyFrames[frame], corner, 1, cv::Scalar(255, 0, 255), 1);
		}*/

		//cv::imshow("video", keyFrames[frame]);
	//	cv::waitKey();
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	std::cout << " Corner runtime: " << elapsed_seconds << " sec" << std::endl;

	videoFingerPrints.push_back(fingerPrint);

	return fingerPrint;
}