#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proto.h"
#include "fft.h"
#include "tool.h"
#define E 2.71828

Image* homomorphicFilter(Image*);
Image* sinusoidalNoise(Image*);
Image* IdealbandrejectFilter(Image*);
Image* adaptiveMedianFilter(Image*);
Image *medianFilter(Image *);
Image *arithmeticMeanFilter(Image *);
Image* geoometricMeanFilter(Image* image);
Image* Alpha_trimmedMeanFilter(Image*);
void BubbleSort(double a[], int length);
void adaptive(struct _complex* out, struct _complex* in, int Height, int Width, int MaxSize);
void ad_A(struct _complex* out, struct _complex* in, int Width, int X, int Y, int curSize, int MaxSize);
void ad_B(struct _complex* out, int Width, int X, int Y, int min, int max, int xy, int med);



int main(int argc, char** argv) {

	Image *image;
	Image *outimage;

	image = ReadPNMImage("bridge.pgm");

	outimage = homomorphicFilter(image);
	SavePNMImage(outimage, "homomorphic.pgm");
	image = ReadPNMImage("lenaD2.pgm");

	outimage = adaptiveMedianFilter(image);
	SavePNMImage(outimage, "adaptiveMedian.pgm");
	outimage = Alpha_trimmedMeanFilter(image);
	SavePNMImage(outimage, "Alpha_trimmedMean.pgm");
	outimage = arithmeticMeanFilter(image);
	SavePNMImage(outimage, "arithmeticMean.pgm");
	outimage = medianFilter(image);
	SavePNMImage(outimage, "median.pgm");
	outimage = geoometricMeanFilter(image);
	SavePNMImage(outimage, "geoometricMean.pgm");
	image = ReadPNMImage("lena.pgm");
	outimage = sinusoidalNoise(image);
	SavePNMImage(outimage, "sinusoidalNoise.pgm");

	image = ReadPNMImage("cameraWithNoise.pgm");

	outimage = IdealbandrejectFilter(image);
	SavePNMImage(outimage, "Idealbandreject.pgm");

	return(0);
}

Image* sinusoidalNoise(Image* image) {

	Image* temp, *outimage;

	int big, count = 1;
	int Height = image->Height;
	int Width = image->Width;
	if (Height > Width) {
		big = Height;
	}
	else {
		big = Width;
	}
	while (count <= big) {
		count *= 2;
	}
	int size = count * count;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* src2 = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x >= Height || y >= Width) {
				src[x * count + y].x = 0.0;
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 0.0;
				src2[x * count + y].y = 0.0;
			}
			else {
				src[x * count + y].x = 1.0 * image->data[x * Width + y] + 20 * sin((double)y) + 20 * sin((double)x);
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 1.0 * image->data[x * Width + y] + 20 * sin((double)y) + 20 * sin((double)x);
				src2[x * count + y].y = 0.0;
			}
		}
	}



	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * count + y].x;
				dst[x * Width + y].y = src[x * count + y].y;
			}
		}
	}
	temp = CreateNewSizeImage(image, Width, Height, "sinusoidalNoise");
	temp->data = Normal(getResult(dst, Height * Width, 0), Height * Width, 255, 0);


	return(temp);
}
Image* IdealbandrejectFilter(Image* image) {

	Image* temp, *outimage;

	int big, count = 1;
	int Height = image->Height;
	int Width = image->Width;
	if (Height > Width) {
		big = Height;
	}
	else {
		big = Width;
	}
	while (count <= big) {
		count *= 2;
	}
	int size = count * count;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* src2 = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x >= Height || y >= Width) {
				src[x * count + y].x = 0.0;
				src[x * count + y].y = 0.0;
			}
			else {
				src[x * count + y].x = 1.0 * image->data[x * Width + y];
				src[x * count + y].y = 0.0;
			}
		}
	}

	fft(src, src, 1, count);

	int D0 = 180;
	double W = 180;
	double D, H;
	for (int u = 0; u < count; u++) {
		for (int v = 0; v < count; v++) {
			D = sqrt(pow((double)u - count / 2, 2) + pow((double)v - count / 2, 2));
			if ((D >= (D0 - W / 2)) && (D <= (D0 + (W / 2)))) {
				src[u * count + v].x = 0.0;
				src[u * count + v].y = 0.0;
			}
		}
	}


	fft(src, src, -1, count);


	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * count + y].x;
				dst[x * Width + y].y = src[x * count + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "IdealbandrejectFilter");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255, 0);

	return(outimage);
}

Image* adaptiveMedianFilter(Image* image) {

	Image* temp, *outimage;

	int big, count = 1;
	int Height = image->Height;
	int Width = image->Width;
	if (Height > Width) {
		big = Height;
	}
	else {
		big = Width;
	}
	while (count <= big) {
		count *= 2;
	}
	int size = count * count;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* src2 = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x >= Height || y >= Width) {
				src[x * count + y].x = 0.0;
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 0.0;
				src2[x * count + y].y = 0.0;
			}
			else {
				src[x * count + y].x = 1.0 * image->data[x * Width + y];// +20 * sin(y) + 20 * sin(x);
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 1.0 * image->data[x * Width + y];// +20 * sin(y) + 20 * sin(x);
				src2[x * count + y].y = 0.0;
			}
		}
	}
	
	adaptive(src, src2, count, count, 49);

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * count + y].x;
				dst[x * Width + y].y = src[x * count + y].y;
			}
		}
	}
	temp = CreateNewSizeImage(image, Width, Height, "adaptiveMedianFilter");
	temp->data = Normal(getResult(dst, Height * Width, 0), Height * Width, 255, 0);


	return(temp);
}

Image* homomorphicFilter(Image* image) {

	Image* outimage;

	int big, count = 1;
	int Height = image->Height;
	int Width = image->Width;
	if (Height > Width) {
		big = Height;
	}
	else {
		big = Width;
	}
	while (count <= big) {
		count *= 2;
	}

	int size = count * count;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x >= Height || y >= Width) {
				src[x * count + y].x = 0.0;
				src[x * count + y].y = 0.0;
			}
			else {
				src[x * count + y].x = 1.0 * image->data[x * Width + y];
				src[x * count + y].y = 0.0;
			}
		}
	}

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			src[x * count + y].x += 1.0;
		}
	}

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			src[x * count + y].x = log(src[x * count + y].x);
		}
	}



	fft(src, src, 1, count);


	int D0 = 80;
	double c = 1.0;
	double L = 0.75;
	double H = 2.0;
	double D, HUV;
	for (int u = 0; u < count; u++) {
		for (int v = 0; v < count; v++) {
			D = pow((double)u - count / 2, 2) + pow((double)v - count / 2, 2);
			HUV = (H - L) * (1 - pow(E, (-1.0 * c) * (D / (D0 * D0)))) + L;
			src[u * count + v].x *= HUV;
			src[u * count + v].y *= HUV;
		}
	}

	fft(src, src, -1, count);

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			src[x * count + y].x = exp(src[x * count + y].x);
		}
	}

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			src[x * count + y].x -= 1.0;
		}
	}

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * count + y].x;
				dst[x * Width + y].y = src[x * count + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "homomorphicFilter");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255, 0);

	return(outimage);
}
Image *arithmeticMeanFilter(Image *image){
	unsigned char *tempin, *tempout;
	int size, i, j, k, t;
	Image *outimage;

	outimage = CreateNewImage(image, "#testing Swap");
	tempin = image->data;
	tempout = outimage->data;

	if (image->Type == GRAY)   size = image->Width * image->Height;
	else if (image->Type == COLOR) size = image->Width * image->Height * 3;

	for (i = 1; i < image->Height - 1; ++i){
		for (j = 1; j < image->Width - 1; ++j){
			int sum = 0;
			for (k = -1; k < 2; ++k)
			for (t = -1; t < 2; ++t)
				sum += tempin[image->Width * (i + k) + (j + t)];
			tempout[image->Width * i + j] = sum / 9;
		}
	}
	return(outimage);
}
Image* geoometricMeanFilter(Image* image) {

	Image* temp, *outimage;

	int big, count = 1;
	int Height = image->Height;
	int Width = image->Width;
	if (Height > Width) {
		big = Height;
	}
	else {
		big = Width;
	}
	while (count <= big) {
		count *= 2;
	}
	int size = count * count;//PQ 4mn

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* src2 = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x >= Height || y >= Width) {
				src[x * count + y].x = 0.0;
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 0.0;
				src2[x * count + y].y = 0.0;
			}
			else {
				src[x * count + y].x = 1.0 * image->data[x * Width + y];
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 1.0 * image->data[x * Width + y];
				src2[x * count + y].y = 0.0;
			}
		}
	}
	double arry[9];
	for (int x = 0; x < count; x++) {
		for (int y = 0; y < count; y++) {
			if (x != 0 && x != count - 1 && y != 0 && y != count - 1) {
				arry[0] = src2[(x - 1) * count + y - 1].x;
				arry[1] = src2[(x - 1) * count + y].x;
				arry[2] = src2[(x - 1) * count + y + 1].x;
				arry[3] = src2[x * count + y - 1].x;
				arry[4] = src2[x * count + y].x;
				arry[5] = src2[x * count + y + 1].x;
				arry[6] = src2[(x + 1) * count + y - 1].x;
				arry[7] = src2[(x + 1) * count + y].x;
				arry[8] = src2[(x + 1) * count + y + 1].x;
				src[x * count + y].x = pow((arry[0] * arry[1] * arry[2] * arry[3] * arry[4] * arry[5] * arry[6] * arry[7] * arry[8]), (1.0 / 9.0));
			}
		}
	}

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * count + y].x;
				dst[x * Width + y].y = src[x * count + y].y;
			}
		}
	}
	temp = CreateNewSizeImage(image, Width, Height, "geoometricMeanFilter");
	temp->data = Normal(getResult(dst, Height * Width, 0), Height * Width, 255, 0);
	return(temp);
}
Image *medianFilter(Image *image){
	unsigned char *tempin, *tempout, local[9];
	int size, i, j, k, t;
	Image *outimage;

	outimage = CreateNewImage(image, "#testing Swap");
	tempin = image->data;
	tempout = outimage->data;

	if (image->Type == GRAY)   size = image->Width * image->Height;
	else if (image->Type == COLOR) size = image->Width * image->Height * 3;

	for (i = 1; i < image->Height - 1; ++i){
		for (j = 1; j < image->Width - 1; ++j){
			int pos = 0;
			for (k = -1; k < 2; ++k)
			for (t = -1; t < 2; ++t)
				local[pos++] = tempin[image->Width * (i + k) + (j + t)];
			tempout[image->Width * i + j] = findMedian(local, 9);
		}
	}
	return(outimage);
}
Image* Alpha_trimmedMeanFilter(Image* image) {

	Image* temp, *outimage;

	int big, count = 1;
	int Height = image->Height;
	int Width = image->Width;
	if (Height > Width) {
		big = Height;
	}
	else {
		big = Width;
	}
	while (count <= big) {
		count *= 2;
	}
	int size = count * count;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* src2 = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x >= Height || y >= Width) {
				src[x * count + y].x = 0.0;
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 0.0;
				src2[x * count + y].y = 0.0;
			}
			else {
				src[x * count + y].x = 1.0 * image->data[x * Width + y];
				src[x * count + y].y = 0.0;
				src2[x * count + y].x = 1.0 * image->data[x * Width + y];
				src2[x * count + y].y = 0.0;
			}
		}
	}
	double arry[9], d = 4.0;
	for (int x = 0; x < count; x++) {
		for (int y = 0; y < count; y++) {
			if (x != 0 && x != count - 1 && y != 0 && y != count - 1) {
				arry[0] = src2[(x - 1) * count + y - 1].x;
				arry[1] = src2[(x - 1) * count + y].x;
				arry[2] = src2[(x - 1) * count + y + 1].x;
				arry[3] = src2[x * count + y - 1].x;
				arry[4] = src2[x * count + y].x;
				arry[5] = src2[x * count + y + 1].x;
				arry[6] = src2[(x + 1) * count + y - 1].x;
				arry[7] = src2[(x + 1) * count + y].x;
				arry[8] = src2[(x + 1) * count + y + 1].x;
				BubbleSort(arry, 9);
				src[x * count + y].x = (arry[0] + arry[1] + arry[2] + arry[3] + arry[4] + arry[5] + arry[6] + arry[7] + arry[8] - (arry[0] * d / 2) - (arry[8] * d / 2)) / (9 - d);
			}
		}
	}

	for (x = 0; x < count; x++) {
		for (y = 0; y < count; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * count + y].x;
				dst[x * Width + y].y = src[x * count + y].y;
			}
		}
	}
	temp = CreateNewSizeImage(image, Width, Height, "Alpha_trimmedMeanFilter");
	temp->data = Normal(getResult(dst, Height * Width, 0), Height * Width, 255, 0);


	return(temp);
}








void BubbleSort(double a[], int length) {
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length - i - 1; j++)
		{
			if (a[j] > a[j + 1])
			{
				int temp;
				temp = a[j + 1];
				a[j + 1] = a[j];
				a[j] = temp;
			}
		}
	}
}


void adaptive(struct _complex* out, struct _complex* in, int Height, int Width, int MaxSize) {
	int count = floor(sqrt((double)MaxSize)) + 1;
	int temp = (count - 1) / 2;
	for (int x = 0; x < Height; x++) {
		for (int y = 0; y < Width; y++) {
			if (x > temp - 1 && x < Height - temp && y >temp - 1 && y < Width - temp) {
				ad_A(out, in, Width, x, y, 1, MaxSize);
			}
		}
	}
}

void ad_A(struct _complex* out, struct _complex* in, int Width, int X, int Y, int curSize, int MaxSize) {
	int min, max, med, xy;
	int A1, A2;
	int temp = (curSize - 1) / 2;
	int count = 0;
	double* arry = (double*)malloc(sizeof(double)* curSize * curSize);
	for (int i = X - temp; i <= X + temp; i++) {
		for (int n = Y - temp; n <= Y + temp; n++) {
			arry[count] = in[i * Width + n].x;
			count++;
		}
	}
	BubbleSort(arry, curSize * curSize);
	min = arry[0];
	max = arry[curSize * curSize - 1];
	med = arry[(curSize * curSize - 1) / 2];
	xy = in[X * Width + Y].x;
	A1 = med - min;
	A2 = med - max;
	if (A1 > 0 && A2 < 0) {
		ad_B(out, Width, X, Y, min, max, xy, med);
	}
	else {
		curSize += 2;
		if (curSize * curSize > MaxSize) {
			out[X * Width + Y].x = med;
			return;
		}
		else {
			ad_A(out, in, Width, X, Y, curSize, MaxSize);
			return;
		}
	}
}

void ad_B(struct _complex* out, int Width, int X, int Y, int min, int max, int xy, int med) {
	int B1, B2;
	B1 = xy - min;
	B2 = xy - max;
	if (B1 > 0 && B2 < 0) {
		return;
	}
	else {
		out[X * Width + Y].x = med;
		return;
	}
}






