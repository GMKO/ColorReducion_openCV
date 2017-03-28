#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <stdio.h>
#include <stdint.h>

using namespace cv;
using namespace std;

uchar vectModule(double alpha, double beta, double gamma, Vec3b vect) {
	return	alpha*vect.val[0] 
			+ beta*vect.val[1] 
			+ gamma*vect.val[2];
//	return (vect.val[0] + vect.val[1] + vect.val[2]) / 3;
}

uchar virtualVectModule(double alpha, double beta, double gamma, Vec3b vect1, Vec3b vect2) {
	return	alpha*abs(vect1.val[0] - vect2.val[0])
			+ beta*abs(vect1.val[1] - vect2.val[1])
			+ gamma*abs(vect1.val[2] - vect2.val[2]);
//	return (abs(vect1.val[0] - vect2.val[0]) + abs(vect1.val[1] - vect2.val[1]) + abs(vect1.val[2] - vect2.val[2])) / 3;
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
	
	for (i = 0; i < input.rows; i++){
		for (j = 0; j < input.cols; j++){
			output.at<uchar>(i, j) = vectModule(alpha, beta, gamma, input.at<Vec3b>(i, j));
			//Scalar intensity = output.at<uchar>(j,i);
			//printf("%.0f ", intensity.val[0]);
		}
		//printf("\n");
	}

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
		output.at<uchar>(input.rows - 1, j) = 0;
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

void printValues(Mat input) {

	FILE *f = fopen("log.txt", "w");
	int i, j;
	fprintf(f, "Rows: %d Cols: %d\n", input.rows, input.cols);
	for (i = 0; i < input.rows; i++){
		for (j = 0; j < input.cols; j++){
			Scalar intensity = input.at<uchar>(j, i);
			int value = intensity.val[0];
			fprintf(f, "%d ", value);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

uchar getGap(int i, int j, Mat A, Mat BH, Mat BV) {

	int diff1, diff2, diff3, diff4;

	if (i == A.rows - 1) {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valB1 = BV.at<uchar>(i, j);
		
		diff1 = valA1.val[0];
		diff1 -= valB1.val[0];
	}
	else {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valA2 = A.at<uchar>(i + 1, j);
		Scalar valB1 = BV.at<uchar>(i + 1, j);

		diff1 = valA1.val[0] - valA2.val[0];
		diff1 -= valB1.val[0];
	}

	if (i == 0) {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valB1 = BV.at<uchar>(i, j);

		diff2 = valA1.val[0];
		diff2 -= valB1.val[0];
	}
	else {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valA2 = A.at<uchar>(i - 1, j);
		Scalar valB1 = BV.at<uchar>(i - 1, j);

		diff2 = valA1.val[0] - valA2.val[0];
		diff2 -= valB1.val[0];
	}

	if (j == A.cols - 1) {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valB1 = BH.at<uchar>(i, j);

		diff3 = valA1.val[0];
		diff3 -= valB1.val[0];
	}
	else {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valA2 = A.at<uchar>(i, j + 1);
		Scalar valB1 = BH.at<uchar>(i, j + 1);

		diff3 = valA1.val[0] - valA2.val[0];
		diff3 -= valB1.val[0];
	}

	if (j == 0) {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valB1 = BH.at<uchar>(i, j);

		diff4 = valA1.val[0];
		diff4 -= valB1.val[0];
	}
	else {
		Scalar valA1 = A.at<uchar>(i, j);
		Scalar valA2 = A.at<uchar>(i, j - 1);
		Scalar valB1 = BH.at<uchar>(i, j - 1);

		diff4 = valA1.val[0] - valA2.val[0];
		diff4 -= valB1.val[0];
	}

	int result = diff1 + diff2 + diff3 + diff4;
	uchar diff = (uchar)result;

	return diff;
}

int getErrorIndex(Mat A, Mat BH, Mat BV) {

	int i, j, errorIndex = 0;;
	
	for (i = 0; i < A.rows; i++){
		for (j = 0; j < A.cols; j++){
			Scalar value = getGap(i, j, A, BH, BV);
			int result = value.val[0];
			
			errorIndex += result;
		}
	}

	return errorIndex;
}

int main(int argc, char** argv)
{
	double alpha = 0.34;
	double beta = 0.33;
	double gamma = 0.33;
	double q = 0.1;
	int MOD_RANGE = 255, MAX_ITERATIONS = 5;
	int iteration, bestErrorIndex;
	char* inputName = argv[1];
	Mat input = imread(inputName);

	if (argc != 2 || !input.data)
	{
		printf(" No image data \n ");
		return -1;
	}

	Mat G = computeInput(alpha, beta, gamma, input);
	Mat A = G.clone();
	Mat BV = computeBV(alpha, beta, gamma, input);
	Mat BH = computeBH(alpha, beta, gamma, input);
	Mat BM = A.clone();
	
	bestErrorIndex = getErrorIndex(A, BH, BV);
	iteration = MAX_ITERATIONS;

	while (iteration > 0) {
		
		uchar ACC[256];
		int i, j;
		int errorIndex;
		
		for (i = 0; i <= MOD_RANGE; i++) {
			ACC[i] = 0;
		}

		for (i = 0; i < A.rows; i++) {
			for (j = 0; j < A.cols; j++) {
				Scalar value = G.at<uchar>(i, j);
				int index = value.val[0];

				ACC[index] = getGap(i, j, A, BH, BV);
			}
		}

		for (i = 0; i < A.rows; i++) {
			for (j = 0; j < A.cols; j++) {
				Scalar a, b;
				a = A.at<uchar>(i, j);
				b = ACC[G.at<uchar>(i, j)];
				double check = a.val[0] + b.val[0] *q;

				if (check <= 255)
					A.at<uchar>(i, j) = (uchar) check;
				else
					A.at<uchar>(i, j) = (uchar) 255;
			}
		}
		printValues(A);
		errorIndex = getErrorIndex(A, BH, BV);
		if (errorIndex < bestErrorIndex)
		{
			BM = A.clone();
			bestErrorIndex = errorIndex;
		}

		iteration--;
	}

	dispResults(input, inputName, BM, "Output.jpg");

	return 0;
}