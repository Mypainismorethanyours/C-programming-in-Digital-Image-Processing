#pragma warning(disable: 4996)
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "proto.h"

Image* DFT2(Image*);
Image* phaseAngle(Image* );
Image* magnitude(Image* );

int TestReadImage(char*, char*);
Image* ReadPNMImage(char*);
Image* SwapImage(Image*);
Image* CreateNewImage(Image*, const char* comment);
void SavePNMImage(Image*, char*);

int main(int argc, char** argv) {
	char* input = "..\\images\\lena.pgm";
	char* output = "";
	TestReadImage(input, output);
	return(0);
}

int TestReadImage(char *filename, char *outfilename)
{
	Image *image;
	Image *outimage;

	image = ReadPNMImage(filename);

	outimage = DFT2(image);
	SavePNMImage(outimage, "..\\output\\DFT_2D.pgm");
	outimage = phaseAngle(image);
	SavePNMImage(outimage, "..\\output\\phaseAngle.pgm");
	outimage = magnitude(image);
	SavePNMImage(outimage, "..\\output\\magnitude.pgm");

	return(0);
}

Image *DFT2(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "#testing DFT2";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	double* re1, *im1, pi = acos(-1.0);
	re1 = (double*)malloc(width*height * sizeof(double));
	im1 = (double*)malloc(width*height * sizeof(double));
	int i, j, x, y,move;
	double temp, re, im;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			re = 0;
			im = 0;
			for (x = 0; x < height; x++) {
				for (y = 0; y < width; y++) {
					temp = -2 * pi * ((double)i * (double)x / (double)height + (double)j * (double)y / (double)width);
					move = (x + y) % 2 == 0 ? 1 : -1;
					re += cos(temp) * (double)(*(tempin + width * x + y))*move;
					im += sin(temp) * (double)(*(tempin + width * x + y))*move;
				}
			}//DFT
			if (re1 && im1) {//Deal with C6011
				*(re1 + i * width + j) = re;
				*(im1 + i * width + j) = im;
			}
			*(tempout + i * width + j) = (double)(1 + log((sqrt(re*re + im*im))));
		}
	}
	double max = tempout[0];
	double min = tempout[0];
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (tempout[i*width + j] >= max){
				max = tempout[i*width + j];
			}
			if (tempout[i*width + j] <= min){
				min = tempout[i*width + j];
			}
		}
	}
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			tempout[i*width + j] = (double)(tempout[i*width + j] - min) * 255 / (max - min);
		}
	}
	free(re1);
	free(im1);
	return(outimage);
}


Image *phaseAngle(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "#testing phase angle";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	//Store IDFT data
	double re, im, temp,pi = acos(-1.0);
	double* re1, *im1;
	re1 = (double*)malloc(width*height * sizeof(double));
	im1 = (double*)malloc(width*height * sizeof(double));

	int i, j, x, y;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			re = 0;
			im = 0;
			for (x = 0; x < height; x++) {
				for (y = 0; y < width; y++) {
					temp = -2 * pi * ((double)i * (double)x / (double)height + (double)j * (double)y / (double)width);
					re += cos(temp) * (double)(*(tempin + width * x + y));
					im += sin(temp) * (double)(*(tempin + width * x + y));
				}
			}//DFT
			if (re1 && im1) {//Deal with C6011
				*(re1 + i * width + j) = re;
				*(im1 + i * width + j) = im;
			}
			*(tempout + i * width + j) = atan2(im, re);//phase angle
		}
	}
	for (i = 0; i < (int)height; i++) {
		for (j = 0; j < (int)width; j++) {
			re = cos((double)(*(tempout + width * i + j)));
			im = sin((double)(*(tempout + width * i + j)));
			if (re1 && im1) {//Deal with C6011
				*(re1 + i * width + j) = re;
				*(im1 + i * width + j) = im;
			}
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			re = 0;
			im = 0;
			for (x = 0; x < height; x++) {
				for (y = 0; y < width; y++) {
					temp = 2 * pi * ((double)i * (double)x / (double)height + (double)j * (double)y / (double)width);
					if (re1 && im1) {//Deal with C6011
						re += cos(temp) * (double)(*(re1 + width * x + y)) - sin(temp) * (double)(*(im1 + width * x + y));
					}
				}
			}//IDFT
			*(tempout + i * width + j) = 1 + 20 * log(re / (width*height));
		}
	}return(outimage);
}

Image *magnitude(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "#testing magnitude";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	double* re1, *im1;
	re1 = (double*)malloc(width*height * sizeof(double));
	im1 = (double*)malloc(width*height * sizeof(double));

	int i, j, x, y,move;
	double temp, re, im, pi = acos(-1.0);;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			re = 0;
			im = 0;
			for (x = 0; x < height; x++) {
				for (y = 0; y < width; y++) {
					temp = -2 * pi * ((double)i * (double)x / (double)height + (double)j * (double)y / (double)width);
					move = (x + y) % 2 == 0 ? 1 : -1;
					re += cos(temp) * (double)(*(tempin + width * x + y))*move;
					im += sin(temp) * (double)(*(tempin + width * x + y))*move;
				}
			}//DFT
			if (re1 && im1) {//Deal with C6011
				*(re1 + i * width + j) = re;
				*(im1 + i * width + j) = im;
			}
			*(tempout + i * width + j) = temp = sqrt(re * re + im * im) / sqrt((double)(width*height));;//magnitude
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			re = (double)(*(tempout + width * i + j));
			im = 0;
			if (re1 && im1) {//Deal with C6011
				*(re1 + i * width + j) = re;
				*(im1 + i * width + j) = im;
			}
		}
	}

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			re = 0;
			im = 0;
			for (x = 0; x < height; x++) {
				for (y = 0; y < width; y++) {
					temp = 2 * pi * ((double)i * (double)x / (double)height + (double)j * (double)y / (double)width);
					if (re1 && im1) {//Deal with C6011
						move = (x + y) % 2 == 0 ? 1 : -1;
						re += cos(temp) * (double)(*(re1 + width * x + y))*move;
						im += sin(temp) * (double)(*(im1 + width * x + y))*move;
					}
				}
			}//IDFT
			temp = sqrt(re * re + im * im) / sqrt((double)(width*height));
			if (temp > 255) {
				temp = 255;

			}
			if (temp < 0) {
				temp = 0;
			}
			*(tempout + i * width + j) = temp;
		}

	}
	free(re1);
	free(im1);
	return(outimage);
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