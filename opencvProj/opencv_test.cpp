#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

uchar vectModule(double alpha, double beta, double gamma, Vec3b vect) {
	return	alpha*vect.val[0] 
			+ beta*vect.val[1] 
			+ gamma*vect.val[2];
}

uchar virtualVectModule(double alpha, double beta, double gamma, Vec3b vect1, Vec3b vect2) {
	return	alpha*abs(vect1.val[0] - vect2.val[0])
			+ beta*abs(vect1.val[1] - vect2.val[1])
			+ gamma*abs(vect1.val[2] - vect2.val[2]);
}

int vectRelation(double alpha, double beta, double gamma, Vec3b vect1, Vec3b vect2) {
	if (vectModule(alpha, beta, gamma, vect1) < vectModule(alpha, beta, gamma, vect2)) {
		return 1;
	}
	
	if (vectModule(alpha, beta, gamma, vect1) == vectModule(alpha, beta, gamma, vect2)) {
		if (vect1.val[0] < vect2.val[0])
			return 1;

		if (vect1.val[0] == vect2.val[0]) {
			if (vect1.val[1] < vect2.val[1])
				return 1;
		}

		if (vect1.val[0] == vect2.val[0]) {
			if (vect1.val[1] == vect2.val[1]) {
				if (vect1.val[2] <= vect2.val[2])
					return 1;
			}
		}
	}

	return 0;
}

uchar optimumModuloDifference(double alpha, double beta, double gamma, Vec3b vect1, Vec3b vect2) {
	if (vectRelation(alpha, beta, gamma, vect1, vect2) == 1)
		return	(-virtualVectModule(alpha, beta, gamma, vect1, vect2) 
				+ vectModule(alpha, beta, gamma, vect1) 
				- vectModule(alpha, beta, gamma, vect2)) / 2;
	else
		return	(virtualVectModule(alpha, beta, gamma, vect1, vect2)
				- vectModule(alpha, beta, gamma, vect1)
				+ vectModule(alpha, beta, gamma, vect2)) / 2;
}

Mat computeInput(double alpha, double beta, double gamma, Mat input) {
	
	Mat output = Mat(input.rows, input.cols, CV_8UC1, double(0));
	int i, j;
	
	for (i = 0; i<input.rows; i++)
		for (j = 0; j<input.cols; j++)
			output.at<uchar>(i, j) = vectModule(alpha, beta, gamma, input.at<Vec3b>(i, j));

	return output;
}

Mat computeBV(double alpha, double beta, double gamma, Mat input) {

	Mat output = Mat(input.rows, input.cols, CV_8UC1, double(0));
	int i, j;

	for (i = 1; i < input.rows; i++) {
		for (j = 0; j < input.cols; j++)
			output.at<uchar>(i, j) = optimumModuloDifference(alpha, beta, gamma, input.at<Vec3b>(i - 1, j), input.at<Vec3b>(i, j));
	}
		
	for (j = 0; j < input.cols; j++) {
		output.at<uchar>(0, j) = 0;
		output.at<uchar>(output.rows - 1, j) = 0;
	}

	return output;
}

Mat computeBH(double alpha, double beta, double gamma, Mat input) {

	Mat output = Mat(input.rows, input.cols, CV_8UC1, double(0));
	int i, j;

	for (i = 0; i < input.rows; i++) {
		for (j = 1; j < input.cols; j++)
			output.at<uchar>(i, j) = optimumModuloDifference(alpha, beta, gamma, input.at<Vec3b>(i, j - 1), input.at<Vec3b>(i, j));
	}

	for (i = 0; j < input.rows; j++) {
		output.at<uchar>(i, 0) = 0;
		output.at<uchar>(i, input.cols - 1) = 0;
	}

	return output;
}

void dispResults(Mat input, char* inputName, Mat output, char* outputName) {

	imwrite(outputName, output);

	namedWindow(inputName, CV_WINDOW_AUTOSIZE);
	namedWindow(outputName, CV_WINDOW_AUTOSIZE);

	cvMoveWindow(inputName, 300, 200);
	cvMoveWindow(outputName, input.rows + 302, 200);

	imshow(inputName, input);
	imshow(outputName, output);

	waitKey(0);
}

int main(int argc, char** argv)
{
	double alpha = 0.33;
	double beta = 0.33;
	double gamma = 0.33;
	char* inputName = argv[1];
	Mat input = imread(inputName, 1);

	if (argc != 2 || !input.data)
	{
		printf(" No image data \n ");
		return -1;
	}

	Mat G = computeInput(alpha, beta, gamma, input);
	Mat A = G;
	Mat BV = computeBV(alpha, beta, gamma, input);
	Mat BH = computeBH(alpha, beta, gamma, input);
	Mat BM = A;

	//dispResults(input, inputName, BV, "Output.jpg");

	return 0;
}