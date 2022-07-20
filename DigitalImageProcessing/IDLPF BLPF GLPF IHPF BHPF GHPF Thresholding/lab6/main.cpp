#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proto.h"
#include "fft.h"
#include "tool.h"
#define E 2.71828

int TestReadImage(char*, char*);
Image *IDLPF(Image *);
Image *BLPF(Image *);
Image *GLPF(Image *image);
Image *IHPF(Image *);
Image *BHPF(Image *);
Image *GHPF(Image *image);
Image *IHPFwithThresholding(Image *);
Image *BHPFwithThresholding(Image *);
Image *GHPFwithThresholding(Image *);
void thresholding(struct _complex* src, int Height, int Width);
Image *homomorphicFilter(Image *);

int main(int argc, char** argv) {
	char argv1[] = "C:\\Users\\steve\\Downloads\\FFT_Lily\\images\\fingerprint2.pgm";
	argv[1] = argv1;
	TestReadImage(argv[1], argv[2]);
	return(0);
}

int TestReadImage(char* filename, char* outfilename) {
	Image *image;
	Image *outimage;
	image = ReadPNMImage(filename);
	outimage = IDLPF(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\IDLPF.pgm");
	outimage = BLPF(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\BLPF.pgm");
	outimage = GLPF(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\GLPF.pgm");
	outimage = IHPF(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\IHPF.pgm");
	outimage = BHPF(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\BHPF.pgm");
	outimage = GHPF(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\GHPF.pgm");
	outimage = IHPFwithThresholding(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\IHPFwithThresholding.pgm");
	outimage = BHPFwithThresholding(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\BHPFwithThresholding.pgm");
	outimage = GHPFwithThresholding(image);
	SavePNMImage(outimage, "C:\\Users\\steve\\Downloads\\FFT_Lily\\output\\GHPFwithThresholding.pgm");

	return(0);
}

Image *IDLPF(Image *image){
	Image* temp, *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60;
	double D;
	for (int u = 0; u < 2*Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = sqrt(pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2));
			if (D > D0) {
				src[u * 2 * Width + v].x *= 0.0;
				src[u * 2 * Width + v].y *= 0.0;
			}
		}
	}
	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *BLPF(Image *image){
	Image *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60,n=2;
	double D, H;
	for (int u = 0; u < 2*Height; u++) {
		for (int v = 0; v < 2*Width; v++) {
			D = sqrt(pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2));
			H = 1 / (1.0 + pow(D / D0, 2 * n));
			src[u * 2 * Width + v].x *= H;
			src[u * 2 * Width + v].y *= H;
		}
	}

	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *GLPF(Image *image){
	Image *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60;
	double D, H;
	for (int u = 0; u < 2*Height; u++) {
		for (int v = 0; v < 2*Width; v++) {
			D = pow((double)u - 2*Height / 2, 2) + pow((double)v - 2 * Width / 2, 2);
			H = pow(E, (-1.0 * D) / (2 * D0 * D0));
			src[u * 2 * Width + v].x *= H;
			src[u * 2 * Width + v].y *= H;
		}
	}
	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *IHPF(Image *image){
	Image* temp, *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60;
	double D;
	for (int u = 0; u < 2 * Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = sqrt(pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2));
			if (D <= D0) {
				src[u * 2 * Width + v].x *= 0.0;
				src[u * 2 * Width + v].y *= 0.0;
			}
		}
	}
	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *BHPF(Image *image){
	Image *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60, n = 2;
	double D, H;
	for (int u = 0; u < 2 * Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = sqrt(pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2));
			H = 1 / (1.0 + pow(D0/D, 2 * n));
			src[u * 2 * Width + v].x *= H;
			src[u * 2 * Width + v].y *= H;
		}
	}

	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *GHPF(Image *image){
	Image *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60;
	double D, H;
	for (int u = 0; u < 2 * Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2);
			H = 1-pow(E, (-1.0 * D) / (2 * D0 * D0));
			src[u * 2 * Width + v].x *= H;
			src[u * 2 * Width + v].y *= H;
		}
	}
	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

void thresholding(struct _complex* src, int Height, int Width) {
	for (int u = 0; u < Height; u++) {
		for (int v = 0; v < Width; v++) {
			if (src[u * Width + v].x > 0) {
				src[u * Width + v].x = 255;
			}
			if (src[u * Width + v].x < 0) {
				src[u * Width + v].x = 0;
			}
			if (src[u * Width + v].y > 0) {
				src[u * Width + v].y = 255;
			}
			if (src[u * Width + v].y < 0) {
				src[u * Width + v].y = 0;
			}
		}
	}
	return;
}
Image *IHPFwithThresholding(Image *image){
	Image* temp, *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}

	fft(src, src, 1, 2 * Width);
	int D0 = 60;
	double D;
	for (int u = 0; u < 2 * Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = sqrt(pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2));
			if (D <= D0) {
				src[u * 2 * Width + v].x *= 0.0;
				src[u * 2 * Width + v].y *= 0.0;
			}
		}
	}
	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	thresholding(dst, Width, Height);
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *BHPFwithThresholding(Image *image){
	Image *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60, n = 2;
	double D, H;
	for (int u = 0; u < 2 * Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = sqrt(pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2));
			H = 1 / (1.0 + pow(D0 / D, 2 * n));
			src[u * 2 * Width + v].x *= H;
			src[u * 2 * Width + v].y *= H;
		}
	}
	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	thresholding(dst, Width, Height);
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}

Image *GHPFwithThresholding(Image *image){
	Image *outimage;

	int Height = image->Height;
	int Width = image->Width;

	int size = 2 * Width * 2 * Height;

	struct _complex* src = (struct _complex*)malloc(sizeof(struct _complex) * size);
	struct _complex* dst = (struct _complex*)malloc(sizeof(struct _complex) * Width * Height);

	int x, y;
	for (x = 0; x < 2 * Height; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x >= Height || y >= Width) {
				src[x * 2 * Width + y].x = 0.0;
				src[x * 2 * Width + y].y = 0.0;
			}
			else {
				src[x * 2 * Width + y].x = 1.0 * image->data[x * Width + y];
				src[x * 2 * Width + y].y = 0.0;
			}
		}
	}
	fft(src, src, 1, 2 * Width);
	int D0 = 60;
	double D, H;
	for (int u = 0; u < 2 * Height; u++) {
		for (int v = 0; v < 2 * Width; v++) {
			D = pow((double)u - 2 * Height / 2, 2) + pow((double)v - 2 * Width / 2, 2);
			H = 1 - pow(E, (-1.0 * D) / (2 * D0 * D0));
			src[u * 2 * Width + v].x *= H;
			src[u * 2 * Width + v].y *= H;
		}
	}

	fft(src, src, -1, 2 * Width);

	for (x = 0; x < 2 * Width; x++) {
		for (y = 0; y < 2 * Width; y++) {
			if (x < Height && y < Width) {
				dst[x * Width + y].x = src[x * 2 * Width + y].x;
				dst[x * Width + y].y = src[x * 2 * Width + y].y;
			}
		}
	}
	thresholding(dst, Width, Height);
	outimage = CreateNewSizeImage(image, Width, Height, "BLPF");
	outimage->data = Normal(getResult(dst, Width * Height, 0), Width * Height, 255);
	return(outimage);
}
