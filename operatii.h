#pragma once
#include <iostream>
#include <fcntl.h> 
#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/cvstd.hpp>

using namespace std;

//detect plate with yolo
//cv::Mat getPlate(cv::Mat original)
void getPlate(cv::Mat original, int height, int width)
{
	cv::dnn::Net net = cv::dnn::readNet("yolov3_training_last.weights", "yolov3_testing.cfg");
	std::vector<std::string> layerNames = net.getLayerNames();
	std::vector<int> unconnectedOutLayers = net.getUnconnectedOutLayers();
	std::vector<std::string> outputLayers;

	for (int i : unconnectedOutLayers)
		outputLayers.push_back(layerNames[i - 1]);
	
	int channels = original.channels();

	cv::Mat blob = cv::dnn::blobFromImage(original, 0.00392, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false, CV_32F);

	net.setInput(blob);
	const std::vector<cv::String> outs;

	//net.forward(outputLayers, outs);

	for (cv::String out : outs)
	{
		cout << typeid(out).name();
	}
}


unsigned char* histogramEqualisation(unsigned char* image, int w, int h)
{
	int hist[256] = { 0 };
	int new_gray_level[256] = { 0 };

	unsigned char* result = new unsigned char[w * h];

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			hist[(int)image[y * w + x]]++;

	int total = w * h;

	int curr = 0;
 
	for (int i = 0; i < 256; i++) {
		// calcul frecventa cumulativa
		curr += hist[i];
		new_gray_level[i] = round((((float)curr) * 255) / total);
	}


	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			result[y * w + x] = (unsigned char)new_gray_level[image[y * w + x]];

	return result;
}


unsigned char* binarizeImage(unsigned char* img, int w, int h, int threshold)
{
	unsigned char* result = new unsigned char[w * h];

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			if (img[y * w + x] > threshold)
				result[y * w + x] = 255;
			else
				result[y * w + x] = 0;
		}

	return result;
}

unsigned char* removeBorder(unsigned char* img, int w, int h)
{
	unsigned char* result = new unsigned char[w * h];

	bool detectBlack = false;
	int minValX = 0;
	int minValY = 0;

	int maxValX = h - 1;
	int maxValY = w - 1;
	
	detectBlack = true;

	//while(detectBlack == true)
	for(int i=0;i<20;i++)
	{ 
		detectBlack = false;

		//Linie sus
		for (int x = 0; x < w; x++)
		{
			if ((int)img[minValY * w + x] == 0)
			{
				detectBlack = true;
				img[minValY * w + x] = 255;
			}
		}

		//Linie dreapta
		for (int y = 0; y < h; y++)
		{
			if ((int)img[y * w + maxValX] == 0)
			{
				detectBlack = true;
				img[y * w + maxValX] = 255;
			}
		}


		//Linie jos
		for (int x = 0; x < w; x++)
		{
			if ((int)img[maxValY * w + x] == 0)
			{
				detectBlack = true;
				img[maxValY * w + x] = 255;
			}
		}

		//Linie stanga
		for (int y = 0; y < h; y++)
		{
			if ((int)img[y * w + minValX] == 0)
			{
				detectBlack = true;
				img[y * w + maxValX] = 255;
			}
		}

		minValX++;
		minValY++;

		maxValX--;
		maxValY--;

	}

	return img;

}