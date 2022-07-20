#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "proto.h"
#include <iostream> 

void SavePNMImage(Image *, char *);
Image *SwapImage(Image *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment, int, int);

Image *Image_translation(Image *);
Image *Image_rotation(Image *);
Image *Shear_operations1(Image *);
Image *Shear_operations2(Image *);
Image *Bilinear_interpolation(Image *);
Image *Average_fliter3times3(Image *);
Image *Average_fliter5times5(Image *);
Image *Median_fliter3times3(Image *);
Image *Median_fliter5times5(Image *);
unsigned char findMedian(unsigned char*, int lenth);



int TestReadImage(char *, char *);

int tx = 50;//Horizontal transformation
int ty = 50;//Vertical transformation

int main()
{
	char* input = "..\\images\\lena.pgm";
	char* output = "";
	TestReadImage(input,output);
	return(0);
}
int TestReadImage(char *filename, char *outfilename)
{
	Image *image;
	Image *outimage;

	image = ReadPNMImage(filename);

	outimage = Image_translation(image);
	SavePNMImage(outimage, "..\\output\\transLena.pgm");
	outimage = Image_rotation(image);
	SavePNMImage(outimage, "..\\output\\rotateLena.pgm");
	outimage = Shear_operations1(image);
	SavePNMImage(outimage, "..\\output\\shearVerticalLena.pgm");
	outimage = Shear_operations2(image);
	SavePNMImage(outimage, "..\\output\\shearHorizontalLena.pgm");
	outimage = Average_fliter3times3(image);
	SavePNMImage(outimage, "..\\output\\average3Lena.pgm");
	outimage = Average_fliter5times5(image);
	SavePNMImage(outimage, "..\\output\\average5Lena.pgm");
	outimage = Median_fliter3times3(image);
	SavePNMImage(outimage, "..\\output\\median3Lena.pgm");
	outimage = Median_fliter5times5(image);
	SavePNMImage(outimage, "..\\output\\median5Lena.pgm");

	return(0);
}

Image *Image_translation(Image * image)
{
	unsigned char *tempin, *tempout;
	int i, j, size;
	Image *outimage;
	int width = image->Width + tx;
	int height = image->Height + ty;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;

	for (i = 0; i < image->Width; i++)
	{
		for (j = 0; j < image->Height; j++)
		{
			tempout[(i+tx)*width + j+ty] = tempin[ i*image->Width + j];
		}
	}

	return(outimage);
}

Image *Image_rotation(Image *image)
{
	unsigned char *tempin, *tempout,*tempout1;
	int i, j;
	float  M = 2;
	Image *outimage;

	float RotaryAngle = 45 * 3.1415926535898 / 180;
	float sinA = sin(RotaryAngle);
	float cosA = cos(RotaryAngle);
	
	int width = pow(2,0.5)*image->Width;
	int height = pow(2, 0.5)* image->Height;

	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	int p, q,a,b;
	for (i = 0; i<height; i++)
	{
		for (j = 0; j< width; j++)
		{
			int a = 0, b = 0;
			a = floor((i*cosA - (j - image->Height*sinA)*sinA));
			b = floor((i*sinA + (j - image->Height*sinA) *cosA));

			 if (a> 0 && a <=255 && b >0 && b <=255)
			 {
				 tempout[i*width + j] = tempin[a*image->Width + b];
			 }
			 else
			 {
				 tempout[i*width + j] = 200;
			 }		
		}
	}
	return(outimage);
}

Image *Shear_operations1(Image *image)
{
	unsigned char *tempin, *tempout;
	int i, j, size;
	Image *outimage;
	int width = image->Width*2;
	int height = image->Height*2;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;

	if (image->Type == GRAY)
		size = width * height;
	else if (image->Type == COLOR)
		size = width * height * 3;

	//Define center of rotation
	int x0 = image->Width*0.5;
	int y0 = image->Height*0.5;

	for (i = 0; i< image->Width; i++)
	{
		for (j = 0; j< image->Height; j++)
		{
			int a = ((i - x0) +(j - y0) + width*0.5);
			int b = ((j - y0) + height*0.5);
			tempout[a*width + b] = tempin[i*image->Width + j];
		}
	}

	return(outimage);


}

Image *Shear_operations2(Image *image)
{
	unsigned char *tempin, *tempout;
	int i, j, size;
	Image *outimage;
	int width = image->Width*1.5;
	int height = image->Height*1.5;
	float RotaryAngle = 45 * 3.14 / 180;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;

	if (image->Type == GRAY)
		size = width * height;
	else if (image->Type == COLOR)
		size = width * height * 3;

	//Define center of rotation
	int x0 = image->Width*0.5;
	int y0 = image->Height*0.5;

	for (i = 0; i< image->Width; i++)
	{
		for (j = 0; j< image->Height; j++)
		{
			int a = ((i - x0) + width*0.5);
			int b = ((j - y0) + (i - x0)*0.5+ height*0.5);
			tempout[a*width + b] = tempin[i*image->Width + j];
		}
	}

	return(outimage);

}

Image *Average_fliter3times3(Image * image)
{
	unsigned char *tempin, *tempout;
	int size, i, j, k, t;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin=image->data;
	tempout=outimage->data;

	if(image->Type==GRAY)   size  = image->Width * image->Height;
	else if(image->Type==COLOR) size  = image->Width * image->Height * 3;
	
	for(i = 1; i < height - 1; ++i){
		for(j = 1; j < width - 1; ++j){
			int sum = 0;
			for(k = -1; k < 2; ++k)
				for(t = -1; t < 2; ++t)
					sum += tempin[width * (i + k) + (j + t)];
			tempout[width * i + j] = sum/9;
		}
	}
	return(outimage);
}

Image *Average_fliter5times5(Image * image)
{
	unsigned char *tempin, *tempout;
	int size, i, j, k, t;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin=image->data;
	tempout=outimage->data;

	if(image->Type==GRAY)   size  = image->Width * image->Height;
	else if(image->Type==COLOR) size  = image->Width * image->Height * 3;
	
	for(i = 2; i < height - 1; ++i){
		for(j = 2; j < width - 1; ++j){
			int sum = 0;
			for(k = -2; k < 3; ++k)
				for(t = -2; t < 3; ++t)
					sum += tempin[width * (i + k) + (j + t)];
			tempout[width * i + j] = sum/25;
		}
	}
	return(outimage);
}

Image *Median_fliter3times3(Image *image)
{
	unsigned char *tempin, *tempout, local[9];
	int size, i, j, k, t;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin=image->data;
	tempout=outimage->data;

	if(image->Type==GRAY)   size  = image->Width * image->Height;
	else if(image->Type==COLOR) size  = image->Width * image->Height * 3;
	
	for(i = 1; i < height - 1; ++i){
		for(j = 1; j < width - 1; ++j){
			int pos = 0;
			for(k = -1; k < 2; ++k)
				for(t = -1; t < 2; ++t)
					local[pos++] = tempin[width * (i + k) + (j + t)];
			tempout[width * i + j] = findMedian(local, 9);
		}
	}
	return(outimage);

}

Image *Median_fliter5times5(Image *image)
{
	unsigned char *tempin, *tempout, local[25];
	int size, i, j, k, t;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin=image->data;
	tempout=outimage->data;

	if(image->Type==GRAY)   size  = image->Width * image->Height;
	else if(image->Type==COLOR) size  = image->Width * image->Height * 3;
	
	for(i = 2; i < height - 1; ++i){
		for(j = 2; j < width - 1; ++j){
			int pos = 0;
			for(k = -2; k < 3; ++k)
				for(t = -2; t < 3; ++t)
					local[pos++] = tempin[width * (i + k) + (j + t)];
			tempout[width * i + j] = findMedian(local, 25);
		}
	}
	return(outimage);

}

unsigned char findMedian(unsigned char* arr, int length){
	unsigned char temp;
	int i, j;
	for(i = 0; i < length/2+1; ++i)
		for(j = i+1; j < length; ++j)
			if(arr[j] < arr[i]){
				temp = arr[j];
				arr[j] = arr[i];
				arr[i] = temp;
			}
	return arr[length/2];
}

/*******************************************************************************/
//Read PPM image and return an image pointer                                   
/**************************************************************************/
Image *ReadPNMImage(char *filename)
{
	char ch;
	int  maxval, Width, Height;
	int size, num, j;
	FILE *fp;
	Image *image;
	int num_comment_lines = 0;

	image = (Image *)malloc(sizeof(Image));

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("Cannot open %s\n", filename);
		exit(0);
	}

	printf("Loading %s ...", filename);

	if (fscanf(fp, "P%c\n", &ch) != 1)
	{
		printf("File is not in ppm/pgm raw format; cannot read\n");
		exit(0);
	}
	if (ch != '6' && ch != '5') {
		printf("File is not in ppm/pgm raw format; cannot read\n");
		exit(0);
	}

	if (ch == '5')
		image->Type = GRAY;  // Gray (pgm)
	else if (ch == '6')
		image->Type = COLOR;  //Color (ppm)
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

	if (!isdigit((int)ch))
	{
		printf("Cannot read header information from ppm file");
		exit(0);
	}

	ungetc(ch, fp);               /* put that digit back */

	/* read the width, height, and maximum value for a pixel */
	fscanf(fp, "%d%d%d\n", &Width, &Height, &maxval);

	/*
	if (maxval != 255){
	printf("image is not true-color (24 bit); read failed");
	exit(0);
	}
	*/

	if (image->Type == GRAY)
		size = Width * Height;
	else  if (image->Type == COLOR)
		size = Width * Height * 3;
	image->data = (unsigned char *)malloc(size*sizeof(unsigned char));
	image->Width = Width;
	image->Height = Height;
	image->num_comment_lines = num_comment_lines;

	if (!image->data){
		printf("cannot allocate memory for new image");
		exit(0);
	}

	num = fread((void *)image->data, 1, (size_t)size, fp);
	//printf("Complete reading of %d bytes \n", num);
	if (num != size){
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

void SavePNMImage(Image *temp_image, char *filename)
{
	int num, j;
	int size;
	FILE *fp;
	//char comment[100];


	printf("Saving Image %s\n", filename);
	fp = fopen(filename, "wb");
	if (!fp){
		printf("cannot open file for writing");
		exit(0);
	}

	//strcpy(comment,"#Created by Dr Mohamed N. Ahmed");

	if (temp_image->Type == GRAY){  // Gray (pgm)
		fprintf(fp, "P5\n");
		size = temp_image->Width * temp_image->Height;
	}
	else  if (temp_image->Type == COLOR){  // Color (ppm)
		fprintf(fp, "P6\n");
		size = temp_image->Width * temp_image->Height * 3;
	}

	for (j = 0; j<temp_image->num_comment_lines; j++)
		fprintf(fp, "%s\n", temp_image->comments[j]);

	fprintf(fp, "%d %d\n%d\n", temp_image->Width, temp_image->Height, 255);

	num = fwrite((void *)temp_image->data, 1, (size_t)size, fp);

	if (num != size){
		printf("cannot write image data to file");
		exit(0);
	}

	fclose(fp);
}

/*************************************************************************/
/*Create a New Image with same dimensions as input image                 */
/*************************************************************************/

Image *CreateNewImage(Image * image, char *comment, int width, int height)
{
	Image *outimage;
	int size, j;
	outimage = (Image *)malloc(sizeof(Image));

	outimage->Type = image->Type;
	if (outimage->Type == GRAY)
		size = width * height;
	else if (outimage->Type == COLOR)
		size = width * height * 3;

	outimage->Width = width;
	outimage->Height = height;
	outimage->num_comment_lines = image->num_comment_lines;

	/*--------------------------------------------------------*/
	/* Copy Comments for Original Image      */
	for (j = 0; j<outimage->num_comment_lines; j++)
		strcpy(outimage->comments[j], image->comments[j]);

	/*----------- Add New Comment  ---------------------------*/
	strcpy(outimage->comments[outimage->num_comment_lines], comment);
	outimage->num_comment_lines++;


	outimage->data = (unsigned char *)malloc(size*sizeof(unsigned char));
	if (!outimage->data)
	{
		printf("cannot allocate memory for new image");
		exit(0);
	}
	return(outimage);
}