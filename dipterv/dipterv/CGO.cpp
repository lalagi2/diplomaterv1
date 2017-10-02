#include "stdafx.h"
#include "CGO.h"
#include <limits>

std::vector<std::string> split(const std::string &text, char sep) 
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

std::string CGO::checkMatchInDB(std::vector<std::vector<double>> fingerPrintToSearch)
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
			for (int j = 0; j < 8; j++)
			{
				errorSum += fabs(fingerPrintToSearch[i][j] - video.second[i][j]);
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

void CGO::loadDB()
{
	std::vector<std::vector<double>> videoFingerPrints;
	std::vector<double> fingerPrintNumbers;

	std::string line;
	std::ifstream myfile("CGO_database.txt");
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
				std::vector<std::string> values = split(line, ' ');
				int frame = std::stoi(values[0]);
				for (int i = 1; i < 9; i++)
				{
					fingerPrintNumbers.push_back(std::stod(values[i]));
				}

				videoFingerPrints.push_back(fingerPrintNumbers);
				fingerPrintNumbers.clear();
			}

			lineNum++;
		}

		myfile.close();
	}

	else std::cout << "Unable to open file";

}

void CGO::appendToDatabase(int n, std::vector<int> keyFrameNumbers)
{
	std::ofstream dbFile;
	dbFile.open("CGO_database.txt", std::ios::out | std::ios::app);
	dbFile << std::to_string(n) + ".mp4" << "\n";

	for (int i = 0; i < 5; i++)
	{
		dbFile << keyFrameNumbers[i] << " ";
		for (auto cgo : fingerPrint[i])
		{
			dbFile << cgo << " ";
		}

		dbFile << "\n";
	}

	dbFile.close();
}

void CGO::printFingerPrint()
{
	for (auto kf : fingerPrint)
	{
		for (auto cgo : kf)
		{
			std::cout << cgo << " ";
		}

		std::cout << std::endl;
	}
}

std::vector<double> CGO::run(cv::Mat keyFrame)
{
	std::vector<double> subMatricesCGO;

	//#pragma omp parallel for 
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

		if (sumM < 1e-8)
		{
			subMatricesCGO.push_back(0);
		}
		else
		{
			subMatricesCGO.push_back(szamlaloosszeg / sumM);
		}
	}

	fingerPrint.push_back(subMatricesCGO);

	return subMatricesCGO;
}



//if (!cap.isOpened())
//{
//	std::cout << "Cannot open the video file. \n";
//	exit(1);
//}

//std::vector<int> framesToCheck;
//for (auto o : dbFingerPrint[videoName])
//{
//	framesToCheck.push_back(o.first);
//}

//std::vector<cv::Mat> keyFrameInNewVideo;
//for (auto frame : framesToCheck)
//{
//	cap.set(CV_CAP_PROP_POS_FRAMES, frame); // Index beállítása arra a framere ami az eredeti videóban keyframe volt
//	cv::Mat newKeyFrame;
//	cap.read(newKeyFrame);
//	keyFrameInNewVideo.push_back(newKeyFrame);
//}