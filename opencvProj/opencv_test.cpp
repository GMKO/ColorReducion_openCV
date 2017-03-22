#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

Mat computeInput(Mat input) {
	
	Mat output = Mat(input.rows, input.cols, CV_8UC1, double(0));
	int i, j;
	for (i = 0; i<input.rows; i++)
		for (j = 0; j<input.cols; j++)
		{
			Vec3b color = input.at<Vec3b>(i, j);
			uchar blue = color.val[0];
			uchar green = color.val[1];
			uchar red = color.val[2];

			output.at<uchar>(i, j) = (red + green + blue) / 3;
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
	char* inputName = argv[1];
	Mat input = imread(inputName, 1);

	if (argc != 2 || !input.data)
	{
		printf(" No image data \n ");
		return -1;
	}

	Mat G = computeInput(input);
	Mat A = computeInput(input);

	dispResults(input, inputName, A, "Output.jpg");

	return 0;
}