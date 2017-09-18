#include "stdafx.h"
#include "CGO.h"

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

void CGO::run(cv::Mat keyFrame)
{
	double subMatricesCGO[8];
	auto start = std::chrono::system_clock::now();

	#pragma omp parallel for 
	for (int n = 0; n < 8; n++)
	{
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

		double** szamlaloOsszegArray = new double*[partMat.rows];
		for (int i = 0; i < partMat.rows; i++)
			szamlaloOsszegArray[i] = new double[partMat.cols];

		double** sumMArray = new double*[partMat.rows];
		for (int i = 0; i < partMat.rows; i++)
			sumMArray[i] = new double[partMat.cols];

		for (int row = 1; row < partMat.rows - 1; row++)
		{
			#pragma omp parallel for
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

				szamlaloOsszegArray[row][col] += M * theta;
				sumMArray[row][col] += M;
			}
		}

		double sumM = 0.0;
		double szamlaloOsszeg = 0.0;
		#pragma omp parallel for reduction(+:sumM, szamlaloOsszeg)
		for (int row = 1; row < partMat.rows - 1; row++)
		{
			for (int col = 1; col < partMat.cols - 1; col++)
			{
				szamlaloOsszeg += szamlaloOsszegArray[row][col];
				sumM += sumMArray[row][col];
			}
		}

		subMatricesCGO[n] = (szamlaloOsszeg / sumM);


		// Felszabadításkor már eredményt ad a centroidra
		/*for (int i = 0; i < partMat.rows; i++)
			delete[] szamlaloOsszegArray[i];
		delete[] szamlaloOsszegArray;

		for (int i = 0; i < partMat.rows; i++)
			delete[] sumMArray[i];
		delete[] sumMArray;*/
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	std::cout << " CGO runtime: " << elapsed_seconds << " sec" << std::endl;

	std::vector<double> subMatricesCGOVector(std::begin(subMatricesCGO), std::end(subMatricesCGO));
	fingerPrint.push_back(subMatricesCGOVector);
}