#pragma warning(disable: 4996)
#include <math.h>
#include <cmath>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "proto.h"

Image* Roberts(Image*);
Image* Prewitt(Image*);
Image *Sobel(Image*);
Image *LoG(Image*);
Image* Canny(Image*);
Image *GloabalThresholding(Image*);

int TestReadImage(char*, char*);
Image* ReadPNMImage(char*);
Image* SwapImage(Image*);
Image* CreateNewImage(Image*, const char* comment);
void SavePNMImage(Image*, char*);

int main(int argc, char** argv) {
	char* input = "";
	char* output = "";
	TestReadImage(input, output);
	return(0);
}

int TestReadImage(char *filename, char *outfilename)
{
	Image *image;
	Image *outimage;

	image = ReadPNMImage("..\\images\\headCT-Vandy.pgm");
	outimage = Roberts(image);
	SavePNMImage(outimage, "..\\output\\Roberts(headCT_Vandy).pgm");
	outimage = Prewitt(image);
	SavePNMImage(outimage, "..\\output\\Prewitt(headCT_Vandy).pgm");
	outimage = Sobel(image);
	SavePNMImage(outimage, "..\\output\\Sobel(headCT_Vandy).pgm");

	image = ReadPNMImage("..\\images\\building_original.pgm");
	outimage = Roberts(image);
	SavePNMImage(outimage, "..\\output\\Roberts(building_original).pgm");
	outimage = Prewitt(image);
	SavePNMImage(outimage, "..\\output\\Prewitt(building_original).pgm");
	outimage = Sobel(image);
	SavePNMImage(outimage, "..\\output\\Sobel(building_original).pgm");

	image = ReadPNMImage("..\\images\\noisy_fingerprint.pgm");
	outimage = Roberts(image);
	SavePNMImage(outimage, "..\\output\\Roberts(noisy_fingerprint).pgm");
	outimage = Prewitt(image);
	SavePNMImage(outimage, "..\\output\\Prewitt(noisy_fingerprint).pgm");
	outimage = Sobel(image);
	SavePNMImage(outimage, "..\\output\\Sobel(noisy_fingerprint).pgm");

	image = ReadPNMImage("..\\images\\building_original.pgm");
	outimage = Canny(image);
	SavePNMImage(outimage, "..\\output\\Canny(building_original).pgm");

	image = ReadPNMImage("..\\images\\building_original.pgm");
	outimage = LoG(image);
	SavePNMImage(outimage, "..\\output\\LoG(building_original).pgm");

	image = ReadPNMImage("..\\images\\headCT-Vandy.pgm");
	outimage = LoG(image);
	SavePNMImage(outimage, "..\\output\\LoG(headCT_Vandy).pgm");

	image = ReadPNMImage("..\\images\\noisy_fingerprint.pgm");
	outimage = GloabalThresholding(image);
	SavePNMImage(outimage, "..\\output\\GloabalThresholding(noisy_fingerprint).pgm");

	image = ReadPNMImage("..\\images\\polymersomes.pgm");
	outimage = GloabalThresholding(image);
	SavePNMImage(outimage, "..\\output\\GloabalThresholding(polymersomes).pgm");

	return(0);
}

Image *Roberts(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Roberts";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	int x, y;
	for (int i = 1; i < height-1; i++){
		for (int j = 1; j < width - 1; j++){
			tempout[i*width + j] = abs(tempin[i*width + j] - tempin[(i - 1)*width + j - 1]) + abs(tempin[i*width + j - 1] - tempin[(i - 1)*width + j]);
		}
	}return(outimage);
}

Image *Prewitt(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Prewitt";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	int x, y, flag;
	for (int i = 1; i < height - 1; i++){
		for (int j = 1; j < width - 1; j++){
			tempout[i*width + j] = abs(tempin[(i - 1)*width + j - 1] + tempin[(i - 1)*width + j] + tempin[(i - 1)*width + j + 1] - (tempin[(i + 1)*width + j - 1] + tempin[(i + 1)*width + j] + tempin[(i + 1)*width + j + 1])) + abs(tempin[(i - 1)*width + j + 1] + tempin[i * width + j+1] + tempin[(i + 1)*width + j + 1] - (tempin[(i - 1)*width + j - 1] + tempin[i * width + j - 1] + tempin[(i + 1)*width + j - 1]));
		}
	}return(outimage);
}

Image *Sobel(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Sobel";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	for (int i = 1; i < height - 1; i++){
		for (int j = 1; j < width - 1; j++){
			tempout[i*width + j] = abs(tempin[(i - 1)*width + j + 1] + 2 * tempin[i*width + j + 1] + tempin[(i + 1)*width + j + 1] - (tempin[(i - 1)*width + j - 1] + 2 * tempin[i*width + j - 1] + tempin[(i + 1)*width + j - 1])) + abs(tempin[(i + 1)*width + j - 1] + 2 * tempin[(i + 1)*width + j] + tempin[(i + 1)*width + j + 1] - (tempin[(i - 1)*width + j - 1] + 2 * tempin[(i - 1)*width + j] + tempin[(i - 1)*width + j + 1]));
		}
	}return(outimage);
}

Image *LoG(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Sobel";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	for (int i = 2; i < height - 2; i++){
		for (int j = 2; j < width - 2; j++){
			tempout[i*width + j] = tempin[(i - 2)*width + j] + tempin[(i - 1)*width + j - 1] + 2 * tempin[(i - 1)*width + j] + tempin[(i - 1)*width + j + 1] + tempin[i*width + j - 2] + 2 * tempin[i*width + j - 1] - 16 * tempin[i*width + j] + 2 * tempin[i*width + j + 1] + tempin[i*width + j + 1] + tempin[(i + 1)*width + j - 1] + 2 * tempin[(i + 1)*width + j] + 2 * tempin[(i + 1)*width + j + 1] + tempin[(i + 2)*width + j];
		}
	}return(outimage);
}

Image *Canny(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Canny";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	double pi = acos((double)-1);

	Image* GaussianImage;
	unsigned char* Gaussian;
	GaussianImage = CreateNewImage(image, comment);
	Gaussian = GaussianImage->data;
	for (int i = 2; i < height - 2; i++){
		for (int j = 2; j < width - 2; j++){
			Gaussian[i*width + j] = (2 * tempin[(i - 2)*width + j - 2] + 4 * tempin[(i - 2)*width + j - 1] + 5 * tempin[(i - 2)*width + j] + 4 * tempin[(i - 2)*width + j + 1] + tempin[(i - 2)*width + j + 2]
								  + 4 * tempin[(i - 1)*width + j - 2] + 9 * tempin[(i - 1)*width + j - 1] + 12 * tempin[(i - 1)*width + j] + 9 * tempin[(i - 1)*width + j + 1] + 4 * tempin[(i - 1)*width + j + 2]
								  + 5 * tempin[i*width + j - 2] + 12 * tempin[i*width + j - 1] + 15 * tempin[i*width + j] + 12 * tempin[i*width + j + 1] + 5 * tempin[i*width + j + 2]
								  + 4 * tempin[(i + 1)*width + j - 2] + 9 * tempin[(i + 1)*width + j - 1] + 12 * tempin[(i + 1)*width + j] + 9 * tempin[(i + 1)*width + j + 1] + 4 * tempin[(i + 1)*width + j + 2]
								  + 2 * tempin[(i + 2)*width + j - 2] + 4 * tempin[(i + 2)*width + j - 1] + 5 * tempin[(i + 2)*width + j] + 4 * tempin[(i + 2)*width + j + 1] + tempin[(i + 2)*width + j + 2]) / 159;
		}
	}
	SavePNMImage(GaussianImage, "..\\output\\Gaussian.pgm");

	Image* SobelImage;
	unsigned char* gradient;
	SobelImage = CreateNewImage(image, comment);
	gradient = SobelImage->data;
	for (int i = 1; i < height - 1; i++){
		for (int j = 1; j < width - 1; j++){
			gradient[i*width + j] = sqrt(pow((double)Gaussian[(i - 1)*width + j + 1] + 2 * Gaussian[i*width + j + 1] + Gaussian[(i + 1)*width + j + 1] - (Gaussian[(i - 1)*width + j - 1] + 2 * Gaussian[i*width + j - 1] + Gaussian[(i + 1)*width + j - 1]), 2)
									  + pow((double)Gaussian[(i + 1)*width + j - 1] + 2 * Gaussian[(i + 1)*width + j] + Gaussian[(i + 1)*width + j + 1] - (Gaussian[(i - 1)*width + j - 1] + 2 * Gaussian[(i - 1)*width + j] + Gaussian[(i - 1)*width + j + 1]), 2));
		}
	}
	SavePNMImage(SobelImage, "..\\output\\cannySobel.pgm");
	
	unsigned char* angle;
	angle = (unsigned char*)malloc(width*height);
	for (int i = 1; i < height - 1; i++){
		for (int j = 1; j < width - 1; j++){
			angle[i*width + j] = atan2((float)Gaussian[(i - 1)*width + j + 1] + 2 * Gaussian[i*width + j + 1] + Gaussian[(i + 1)*width + j + 1] - (Gaussian[(i - 1)*width + j - 1] + 2 * Gaussian[i*width + j - 1] + Gaussian[(i + 1)*width + j - 1]),
						Gaussian[(i + 1)*width + j - 1] + 2 * Gaussian[(i + 1)*width + j] + Gaussian[(i + 1)*width + j + 1] - (Gaussian[(i - 1)*width + j - 1] + 2 * Gaussian[(i - 1)*width + j] + Gaussian[(i - 1)*width + j + 1]))*180/pi;
		}
	}
	
	Image* suppressionImage;
	unsigned char* suppression;
	suppressionImage = CreateNewImage(image, comment);
	suppressionImage->data = SobelImage->data;
	suppression = suppressionImage->data;

	for (int i = 1; i < height - 1; i++){
		for (int j = 1; j < width - 1; j++){
			double g0, g1;
			if (angle[i*width + j] >= -3 * pi / 8 && angle[i*width + j] < -pi / 8 && angle[i*width + j] < 7 * pi / 8 && angle[i*width + j] >= 5 * pi / 8){
				g0 = gradient[(i - 1)*width + j - 1];
				g1 = gradient[(i + 1)*width + j + 1];
			}
			else if (angle[i*width + j] >= -pi / 8 && angle[i*width + j] < pi / 8 && angle[i*width + j] > -7*pi / 8 && angle[i*width + j] <= 7*pi / 8){
				g0 = gradient[i*width + j - 1];
				g1 = gradient[i*width + j + 1];
			}
			else if (angle[i*width + j] >= pi / 8 && angle[i*width + j] < 3 * pi / 8 && angle[i*width + j] >= -7 * pi / 8 && angle[i*width + j] < 5 * pi / 8){
				g0 = gradient[(i - 1)*width + j + 1];
				g1 = gradient[(i + 1)*width + j - 1];
			}
			else{
				g0 = gradient[(i - 1)*width + j];
				g1 = gradient[(i + 1)*width + j];
			}
			if (gradient[i*width + j] <= g0 || gradient[i*width + j] <= g1) {
				suppression[i*width + j] = 0;
			}	
		}
	}

	SavePNMImage(suppressionImage, "..\\output\\suppression.pgm");
	
	Image* doubleThresholdingLinkImage;
	unsigned char* doubleThresholdingLink;
	doubleThresholdingLinkImage = CreateNewImage(image, comment);
	doubleThresholdingLinkImage->data = suppressionImage->data;
	doubleThresholdingLink = doubleThresholdingLinkImage->data;
	for (int i = 1; i < height ; i++) {
		for (int j = 1; j < width - 1; j++) {

			if (suppression[i*width + j] > 100) {
				doubleThresholdingLink[i*width + j] = 255;
			}
			else if (suppression[i*width + j] < 50) {
				doubleThresholdingLink[i*width + j] = 0;
			}
		}
	}
	SavePNMImage(doubleThresholdingLinkImage, "..\\output\\doubleThresholding.pgm");

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (doubleThresholdingLink[i*width + j] == 255) {
				for (int m = -1; m < 2; m++) {
					for (int n = -1; n < 2; n++) {
						if (doubleThresholdingLink[(i + m)*width + j + n] != 0 && doubleThresholdingLink[(i + m)*width + j + n] != 255) {
							doubleThresholdingLink[(i + m)*width + j + n] = 255;
						}
					}
				}
			}
		}
	}

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (doubleThresholdingLink[i*width + j] != 255 && doubleThresholdingLink[i*width + j] != 0) {
				doubleThresholdingLink[i*width + j] = 0;
			}
		}
	}

	outimage->data = doubleThresholdingLink;
	return(outimage);
}

Image *GloabalThresholding(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "GloabalThresholding";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	
	int max = 0;
	int min = 0;
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			if (tempin[i*width + j] >= max){
				max = tempin[i*width + j];
			}
			if (tempin[i*width + j] <= min){
				min = tempin[i*width + j];
			}
		}
	}
	float T = (max + min) / 2;
	float ¦Ì1 = 0, ¦Ì2 = 0;
	int count1 = 0, count2 = 0, delt_t = 0;
	bool judge = false;
	while (!judge){
		¦Ì1 = 0;
		¦Ì2 = 0;
		count1 = 0;
		count2 = 0;
		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				if (tempin[i*width + j] > T){
					¦Ì1 += tempin[i*width + j];
					count1++;
				}
				else{
					¦Ì2 += tempin[i*width + j];
					count2++;
				}
			}
		}
		if (count1 != 0){
			¦Ì1 /= count1;
		}
		if (count2 != 0){
			¦Ì2 /= count2;
		}
		judge = abs(T - (¦Ì1 + ¦Ì2) / 2) < 0.5;
		T = (¦Ì1 + ¦Ì2) / 2;
	}

	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			if (tempin[i*width + j] <= T){
				tempout[i*width + j] = 255;
			}
			else{
				tempout[i*width + j] = 0;
			}
		}
	}return(outimage);
}

/*******************************************************************************/
//Read PPM image and return an image pointer                                   
/**************************************************************************/
Image* ReadPNMImage(char* filename) {
	char ch;
	int  maxval, Width, Height;
	int size, num, j;
	FILE* fp;
	Image* image;
	int num_comment_lines = 0;


	image = (Image*)malloc(sizeof(Image));

	if ((fp = fopen(filename, "rb")) == NULL) {
		printf("Cannot open %s\n", filename);
		exit(0);
	}

	printf("Loading %s ...", filename);

	if (fscanf(fp, "P%c\n", &ch) != 1) {
		printf("File is not in ppm/pgm raw format; cannot read\n");
		exit(0);
	}
	if (ch != '6' && ch != '5') {
		printf("File is not in ppm/pgm raw format; cannot read\n");
		exit(0);
	}

	if (ch == '5') {
		image->Type = GRAY;  // Gray (pgm)
	}
	else if (ch == '6') {
		image->Type = COLOR;  //Color (ppm)
	}
	/* skip comments */
	ch = getc(fp);
	j = 0;
	while (ch == '#')
	{
		image->comments[num_comment_lines][j] = ch;
		j++;
		do {
			ch = getc(fp);
			image->comments[num_comment_lines][j] = ch;
			j++;
		} while (ch != '\n');     /* read to the end of the line */
		image->comments[num_comment_lines][j - 1] = '\0';
		j = 0;
		num_comment_lines++;
		ch = getc(fp);            /* thanks, Elliot */
	}

	if (!isdigit((int)ch)) {
		printf("Cannot read header information from ppm file");
		exit(0);
	}

	ungetc(ch, fp);               /* put that digit back */

	/* read the width, height, and maximum value for a pixel */
	num = fscanf(fp, "%d %d\n%d\n", &Width, &Height, &maxval);

	/*
	if (maxval != 255){
	printf("image is not true-color (24 bit); read failed");
	exit(0);
	}
	*/

	if (image->Type == GRAY) {
		size = Width * Height;
	}
	else  if (image->Type == COLOR) {
		size = Width * Height * 3;
	}
	image->data = (unsigned char*)malloc(size);
	image->Width = Width;
	image->Height = Height;
	image->num_comment_lines = num_comment_lines;

	if (!image->data) {
		printf("cannot allocate memory for new image");
		exit(0);
	}

	num = fread((void*)image->data, 1, (size_t)size, fp);
	//printf("Complete reading of %d bytes \n", num);
	if (num != size) {
		printf("cannot read image data from file");
		exit(0);
	}

	//for(j=0;j<image->num_comment_lines;j++){
	//      printf("%s\n",image->comments[j]);
	//      }

	fclose(fp);

	/*-----  Debug  ------*/

	if (image->Type == GRAY)printf("..Image Type PGM\n");
	else printf("..Image Type PPM Color\n");
	/*
	printf("Width %d\n", Width);
	printf("Height %d\n",Height);
	printf("Size of image %d bytes\n",size);
	printf("maxvalue %d\n", maxval);
	*/
	return(image);
}
/*************************************************************************/
/*Create a New Image with same dimensions as input image                 */
/*************************************************************************/void SavePNMImage(Image* temp_image, char* filename) {
	int num, j;
	int size;
	FILE* fp;
	//char comment[100];


	printf("Saving Image %s\n", filename);
	fp = fopen(filename, "wb");
	if (!fp) {
		printf("cannot open file for writing");
		exit(0);
	}

	//strcpy(comment,"#Created by Dr Mohamed N. Ahmed");

	if (temp_image->Type == GRAY) {  // Gray (pgm)
		fprintf(fp, "P5\n");
		size = temp_image->Width * temp_image->Height;
	}
	else  if (temp_image->Type == COLOR) {  // Color (ppm)
		fprintf(fp, "P6\n");
		size = temp_image->Width * temp_image->Height * 3;
	}

	for (j = 0; j < temp_image->num_comment_lines; j++)
		fprintf(fp, "%s\n", temp_image->comments[j]);

	fprintf(fp, "%d %d\n%d\n", temp_image->Width, temp_image->Height, 255);

	num = fwrite((void*)temp_image->data, 1, (size_t)size, fp);

	if (num != size) {
		printf("cannot write image data to file");
		exit(0);
	}

	fclose(fp);
}/*Create a New Image with same dimensions as input image*/

Image* CreateNewImage(Image* image, const char* comment) {
	Image* outimage;
	int size, j;

	outimage = (Image*)malloc(sizeof(Image));

	outimage->Type = image->Type;
	if (outimage->Type == GRAY) {
		size = image->Width * image->Height;
	}
	else if (outimage->Type == COLOR) {
		size = image->Width * image->Height * 3;
	}

	outimage->Width = image->Width;
	outimage->Height = image->Height;
	outimage->num_comment_lines = image->num_comment_lines;

	/*--------------------------------------------------------*/
	/* Copy Comments for Original Image      */
	for (j = 0; j < outimage->num_comment_lines; j++)
		strcpy(outimage->comments[j], image->comments[j]);

	/*----------- Add New Comment  ---------------------------*/
	strcpy(outimage->comments[outimage->num_comment_lines], comment);
	outimage->num_comment_lines++;


	outimage->data = (unsigned char*)malloc(size);
	if (!outimage->data) {
		printf("cannot allocate memory for new image");
		exit(0);
	}
	return(outimage);
}