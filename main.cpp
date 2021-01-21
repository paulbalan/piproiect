#include <QApplication>
#include<fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <windows.h>
#include <string>
#include<windows.h>
#include "ImageGrid.h"
#include "operatii.h"

using namespace std;

int main(int argc, char *argv[])
{
	int threshold;
	string zoom;
	QApplication a(argc, argv);

	ImageGrid* grid = new ImageGrid("Prelucrarea imaginilor");

	int option = 1;
	string numeImagine = "";
	int w, h;

	//Step 1
	//Select the original image
	cout << "Nume imagine: ";
	cin >> numeImagine;
	cv::Mat inputImgMat = cv::imread("Images/" + numeImagine, 0);
	unsigned char* inputImg = inputImgMat.data;

	w = inputImgMat.cols;
	h = inputImgMat.rows; 

	grid->addImage(inputImg, w, h, 0, 0, "Imagine originala");

	//Step 2
	//Run YOLO script
	cout << "Valoare zoom imagine (ex: 1,5,10, etc): ";
	cin >> zoom;
	string pyScriptStr = "python yolo_object_detection.py --file Images/" + numeImagine + " --zoom " + zoom;
	const char* pyScriptChr = pyScriptStr.c_str();
	WinExec(pyScriptChr, 1);
	Sleep(5000);

	//Step 3
	//Processing of the plate
	cout << "Introduceti threshold-ul: ";
	cin >> threshold;

	cv::Mat plateInput = cv::imread("plate.jpg", 0);
	Sleep(1000);
	unsigned char* plate = plateInput.data;

	w = plateInput.cols;
	h = plateInput.rows;

	unsigned char* histogramEq = histogramEqualisation(plate, w, h);
	unsigned char* binaryImg = binarizeImage(histogramEq, w, h, threshold);

	grid->addImage(histogramEq, w, h, 0, 1, "Histo eq");
	grid->addImage(binaryImg, w, h, 0, 2, "binary image");

	grid->show();
	Sleep(1000);

	cv::Mat plateMat(h, w, CV_8UC1, binaryImg);
	cv::imwrite("plateProcessed.jpg", plateMat);

	WinExec("tesseract plateProcessed.jpg textResult", 1);
	Sleep(5000);

	cout << "Numarul este: ";

	ifstream myReadFile;
	myReadFile.open("textResult.txt");
	char output[100];
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> output;
			cout << output;
		}
	}
	myReadFile.close();

	return a.exec();
}

