#pragma warning(disable: 4996)
#include <math.h>
#include <cmath>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "proto.h"

typedef struct Seeds{
	int x;
	int y;
}Seeds;

typedef struct QNode{
	Seeds data;
	struct QNode *next;
}QNode;

typedef struct Queue{
	struct QNode* first;
	struct QNode* last;
}Queue;

#define GRAY_LEVELS 256

Image* Otsu(Image*);
Image *AverageFilt(Image *);
double dabs(double a, double b);
void calculate_histogram(long height, long width, unsigned char *image, unsigned long histogram[]);
void calculate_pi(long height, long width, unsigned long histogram[], double pi[]);
double p1(int k, double pi[]);
double m(int k, double pi[]);
double calculate_sigma(int k, double mg, double pi[]);
Image *partitionOtsu6(Image*);
Image *movingAverageThresholding(Image*);
Image *regionGrowingSegmentation(Image*);
void Grow(struct _complex* in, int Height, int Width, int oldx, int oldy);

int TestReadImage(char*, char*);
Image* ReadPNMImage(char*);
Image* SwapImage(Image*);
Image* CreateNewImage(Image*, const char* comment);
Image* CreateNewSizeImage(Image* image, const char* comment, int width, int height);
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

	image = ReadPNMImage("..\\images\\large_septagon_gaussian_noise_mean_0_std_50_added.pgm");
	outimage = Otsu(AverageFilt(image));
	SavePNMImage(outimage, "..\\output\\Otsu.pgm");

	image = ReadPNMImage("..\\images\\septagon_noisy_shaded.pgm");
	outimage = partitionOtsu6(image);
	SavePNMImage(outimage, "..\\output\\partitionOtsu6.pgm");

	image = ReadPNMImage("..\\images\\spot_shaded_text_image.pgm");
	outimage = movingAverageThresholding(image);
	SavePNMImage(outimage, "..\\output\\movingAverageThresholding.pgm");

	image = ReadPNMImage("..\\images\\defective_weld.pgm");
	outimage = regionGrowingSegmentation(image);
	SavePNMImage(outimage, "..\\output\\regionGrowingSegmentation(defective_weld).pgm");
	image = ReadPNMImage("..\\images\\noisy_region.pgm");
	outimage = regionGrowingSegmentation(image);
	SavePNMImage(outimage, "..\\output\\regionGrowingSegmentation(noisy_region).pgm");
	return(0);
}

Image *AverageFilt(Image *image){
	unsigned char *tempin, *tempout;
	int size, i, j, k, t;
	Image *outimage;

	outimage = CreateNewImage(image, "#testing AverageFilt");
	tempin = image->data;
	tempout = outimage->data;

	if (image->Type == GRAY)   size = image->Width * image->Height;
	else if (image->Type == COLOR) size = image->Width * image->Height * 3;

	for (i = 2; i < image->Height - 2; ++i){
		for (j = 2; j < image->Width - 2; ++j){
			int sum = 0;
			for (k = -2; k < 3; ++k)
			for (t = -2; t < 3; ++t)
				sum += tempin[image->Width * (i + k) + (j + t)];
			tempout[image->Width * i + j] = sum / 25;
		}
	}
	return(outimage);
}

Image *Otsu(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Otsu";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	unsigned long histogram[GRAY_LEVELS] = {};
	double pi[GRAY_LEVELS] = {};
	double sigma[GRAY_LEVELS] = {};
	double mg;
	short max_count = 0;
	short k = 0;
	double max_value = 0.0;
	double k_star;

	calculate_histogram(height, width, tempin, histogram);
	calculate_pi(height, width, histogram, pi);
	mg = m(GRAY_LEVELS - 1, pi);

	for (int i = 0; i < GRAY_LEVELS; i++)
		sigma[i] = calculate_sigma(i, mg, pi);

	max_value = sigma[0];
	max_count = 1;
	k = 0;
	for (int i = 1; i < GRAY_LEVELS; i++){
		if (dabs(sigma[i], max_value) < 1e-10){
			k += i;
			max_count++;
		}
		else if (sigma[i] > max_value){
			max_value = sigma[i];
			max_count = 1;
			k = i;
		}
	}
	k_star = k / max_count;

	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			if (tempin[i * width + j] <= k_star)
				tempout[i * width + j] = 0x00;
			else
				tempout[i * width + j] = 0xff;
		}
	}
	return (outimage); 
}

Image *movingAverageThresholding(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "movingAverageThresholding";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	unsigned char *temp = (unsigned char*)malloc(width*height); 
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			if (y % 2 == 0){
				temp[y*width + x] = tempin[y*width + x];
			}
			else{
				temp[y*width + x] = tempin[y*width + width - 1 - x];
			}
		}
	}
	float m_now = 0, m_pre = 0, dif = 0;
	int n = 20;
	float b = 0.75;
	for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++){
		if (y*width + x + 1<n)
			dif = temp[y*width + x];
		else
			dif = temp[y*width + x] - temp[y*width + x + 1 - n];

		dif *= 1 / n;
		m_now = m_pre + dif;
		m_pre = m_now;

		if (tempin[y*width + x]>b*m_now)
			tempout[y*width + x] = 255;
		else
			tempout[y*width + x] = 0;
	}return(outimage);
}

double dabs(double a, double b)
{
	if (a < b)
		return b - a;
	else
		return a - b;
}

void calculate_histogram(long height, long width, unsigned char *image, unsigned long histogram[])
{
	short k;
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			k = image[i * width + j];
			histogram[k] = histogram[k] + 1;
		}
	}
}

void calculate_pi(long height, long width, unsigned long histogram[], double pi[])
{
	for (int i = 0; i < GRAY_LEVELS; ++i){
		pi[i] = (double)histogram[i] / (double)(height * width);
	}
}

double p1(int k, double pi[])
{
	double sum = 0.0;

	for (int i = 0; i <= k; i++){
		sum += pi[i];
	}

	return sum;
}

double m(int k, double pi[])
{
	double sum = 0.0;

	for (int i = 0; i <= k; i++){
		sum += i * pi[i];
	}

	return sum;
}

double calculate_sigma(int k, double mg, double pi[])
{
	double p1k = p1(k, pi);
	double mk = m(k, pi);

	if (p1k < 1e-10 || (1 - p1k) < 1e-10)
		return 0.0;
	else
		return pow(mg * p1k - mk, 2) / (p1k * (1 - p1k));
}

Image *partitionOtsu6(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage, *tempimage1, *tempimage2, *tempimage3, *tempimage4, *tempimage5, *tempimage6;
	unsigned char* tempin, *tempout, *temp1, *temp2, *temp3, *temp4, *temp5, *temp6;
	const char* comment = "partitionOtsu";
	outimage = CreateNewImage(image, comment);	
	tempimage1 = CreateNewSizeImage(image, comment, 271, 325);
	tempimage2 = CreateNewSizeImage(image, comment, 271, 325);
	tempimage3 = CreateNewSizeImage(image, comment, 272, 325);
	tempimage4 = CreateNewSizeImage(image, comment, 271, 326);
	tempimage5 = CreateNewSizeImage(image, comment, 271, 326);
	tempimage6 = CreateNewSizeImage(image, comment, 272, 326);
	tempin = image->data;
	tempout = outimage->data;
	temp1 = tempimage1->data;
	temp2 = tempimage2->data;
	temp3 = tempimage3->data;
	temp4 = tempimage4->data;
	temp5 = tempimage5->data;
	temp6 = tempimage6->data;
	
	for (int y = 0; y < 325; y++){
		for (int x = 0; x < 271; x++){
			temp1[y*271 + x] = tempin[y*width + x];
		}
	}
	for (int y = 0; y < 325; y++){
		for (int x = 271; x < 542; x++){
			temp2[y*271 + x-271] = tempin[y * width + x];
		}
	}
	for (int y = 0; y < 325; y++){
		for (int x = 542; x < 814; x++){
			temp3[y*272 + x-542] = tempin[y * width + x];
		}
	}
	for (int y = 325; y < 651; y++){
		for (int x = 0; x < 271; x++){
			temp4[(y - 325) * 271 + x] = tempin[y * width + x];
		}
	}
	for (int y = 325; y < 651; y++){
		for (int x = 271; x < 542; x++){
			temp5[(y - 325) * 271 + x-271] = tempin[y * width + x];
		}
	}
	for (int y = 325; y < 651; y++){
		for (int x = 542; x < 814; x++){
			temp6[(y - 325) * 272 + x-542] = tempin[y * width + x];
		}
	}
	SavePNMImage(tempimage1, "..\\output\\partitionExample.pgm");

	tempimage1 = Otsu(tempimage1);
	tempimage2 = Otsu(tempimage2);
	tempimage3 = Otsu(tempimage3);
	tempimage4 = Otsu(tempimage4);
	tempimage5 = Otsu(tempimage5);
	tempimage6 = Otsu(tempimage6);
	SavePNMImage(tempimage1, "..\\output\\partitionOtsuExample.pgm");
	temp1 = tempimage1->data;
	temp2 = tempimage2->data;
	temp3 = tempimage3->data;
	temp4 = tempimage4->data;
	temp5 = tempimage5->data;
	temp6 = tempimage6->data;

	for (int y = 0; y < 325; y++){
		for (int x = 0; x < 271; x++){
			tempout[y*width + x] = temp1[y * 271 + x];
		}
	}
	for (int y = 0; y < 325; y++){
		for (int x = 271; x < 542; x++){
			tempout[y*width + x] = temp2[y * 271 + x - 271];
		}
	}
	
	for (int y = 0; y < 325; y++){
		for (int x = 542; x < 814; x++){
			tempout[y*width + x] = temp3[y * 272 + x - 542];
		}
	}
	for (int y = 325; y < 651; y++){
		for (int x = 0; x < 271; x++){
			tempout[y*width + x] = temp4[(y - 325) * 271 + x];
		}
	}
	for (int y = 325; y < 651; y++){
		for (int x = 271; x < 542; x++){
			tempout[y*width + x] = temp5[(y - 325) * 271 + x - 271];
		}
	}
	for (int y = 325; y < 651; y++){
		for (int x = 542; x < 814; x++){
			tempout[y*width + x] = temp6[(y - 325) * 272 + x - 542];
			
		}
	}
	return(outimage);
}

Image *erosion(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "white connected component";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	int i, j, x, y, flag;
	for (i = 1; i < height - 1; i++){
		for (j = 1; j < width - 1; j++){
			flag = 1;
			for (x = i - 1; x < i + 2; x++){
				for (y = j - 1; y < j + 2; y++){
					if (tempin[x*width + y] == 0){
						flag = 0;
						break;
					}
				}
				if (flag == 0){
					break;
				}
			}
			if (flag == 0){
				tempout[i*width + j] = 0;
			}
			else{
				tempout[i*width + j] = 255;
			}
		}
	}return(outimage);
}

Image *regionGrowingSegmentation(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "regionGrowingSegmentation";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * (width * height));
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * (width * height));

	int x, y;
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			src[x * width + y].x = image->data[x * width + y];
			src[x * width + y].y = 0.0;
		}
	}
	struct _complex* S = (struct _complex*)malloc(sizeof(struct _complex) * (width * height));
	struct _complex* Q = (struct _complex*)malloc(sizeof(struct _complex) * (width * height));
	struct _complex* Q1 = (struct _complex*)malloc(sizeof(struct _complex) * (width * height));
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (src[x * width + y].x > 254) {
				S[x * width + y].x = 255;
				S[x * width + y].y = 1;
			}
			else {
				S[x * width + y].x = 0;
				S[x * width + y].y = 0;
			}
		}
	}

	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			Q[x * width + y].x = fabs(src[x * width + y].x - S[x * width + y].x);
			Q1[x * width + y].x = 255 - Q[x * width + y].x;

		}
	}//×ª»¯
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (Q1[x * width + y].x <= 68) {
				Q1[x * width + y].x = 255;
			}
			else {
				Q1[x * width + y].x = 0;
			}
			Q1[x * width + y].y = 0;
		}
	}
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (S[x * width + y].y == 1) {
				Q1[x * width + y].y = 1;
				Q1[x * width + y].x = 255;
			}
		}
	}
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (Q1[x * width + y].y == 1) {
				Grow(Q1, height, width, x, y);
			}
		}
	}
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (Q1[x * width + y].y == 1) {
				dst[x * width + y].x = 255;
			}
			else {
				dst[x * width + y].x = 0;
			}
		}
	}
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			tempout[x*width + y] = dst[x * width + y].x;
		}
	}return(outimage);
}

void Grow(struct _complex* in, int Height, int Width, int oldx, int oldy) {
	if (oldx >= 1) {
		if (in[(oldx - 1) * Width + oldy].x == 255) {
			if (in[(oldx - 1) * Width + oldy].y != 1) {
				in[(oldx - 1) * Width + oldy].y = 1;
				Grow(in, Height, Width, oldx - 1, oldy);
			}
		}
	}
	if (oldx < Height - 1) {
		if (in[(oldx + 1) * Width + oldy].x == 255) {
			if (in[(oldx + 1) * Width + oldy].y != 1) {
				in[(oldx + 1) * Width + oldy].y = 1;
				Grow(in, Height, Width, oldx + 1, oldy);
			}

		}
	}
	if (oldy >= 1) {
		if (in[oldx * Width + oldy - 1].x == 255) {
			if (in[oldx * Width + oldy - 1].y != 1) {
				in[oldx * Width + oldy - 1].y = 1;
				Grow(in, Height, Width, oldx, oldy - 1);
			}

		}
	}
	if (oldy < Width - 1) {
		if (in[oldx * Width + oldy + 1].x == 255) {
			if (in[oldx * Width + oldy + 1].y != 1) {
				in[oldx * Width + oldy + 1].y = 1;
				Grow(in, Height, Width, oldx, oldy + 1);
			}

		}
	}
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
/*************************************************************************/
void SavePNMImage(Image* temp_image, char* filename) {
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

Image* CreateNewSizeImage(Image* image, const char* comment, int width, int height) {
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

	outimage->Width = width;
	outimage->Height = height;
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
