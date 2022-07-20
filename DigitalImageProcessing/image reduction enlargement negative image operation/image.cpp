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

Image *AlternateReduction(Image *);
Image *fractionalReduction(Image *);
Image *Pixel_replication(Image *);
Image *Nearest_enlargement(Image *);
Image *Bilinear_interpolation(Image *);
Image *fractionalExpansion(Image*);
Image *negative_image(Image *);
int TestReadImage(char *, char *);
int n = 2;//reduction
int m = 2;//enlargement
float m2 = 2.2;//Non integer reduction and enlargement


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

	outimage = AlternateReduction(image);
	SavePNMImage(outimage, "..\\output\\alterLena.pgm");
	outimage = fractionalReduction(image);
	SavePNMImage(outimage, "..\\output\\fracReducLena.pgm");
	outimage = Pixel_replication(image);
	SavePNMImage(outimage, "..\\output\\repliLena.pgm");
	outimage = Nearest_enlargement(image);
	SavePNMImage(outimage, "..\\output\\NearLena.pgm");
	outimage = Bilinear_interpolation(image);
	SavePNMImage(outimage, "..\\output\\BiLena.pgm");
	outimage = fractionalExpansion(image);
	SavePNMImage(outimage, "..\\output\\fracExpanLena.pgm");
	outimage=negative_image(image);
	SavePNMImage(outimage, "..\\output\\NegaLena.pgm");

	return(0);
}



Image *AlternateReduction(Image * image)
{
	unsigned char *tempin, *temptout;

	int i, j, size;
	Image *outimage;
	int width = image->Width / n;
	int height = image->Height / n;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	temptout = outimage->data;

	if (image->Type == GRAY)

		size = width*height;

	else if (image->Type == COLOR)

		size = width*height * 3;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < height; j++)

		{
			temptout[i*width + j] = tempin[n *image->Width*i + n*j];
		}
	}return(outimage);

}

Image *fractionalReduction(Image *image){
	unsigned char *tempin, *tempout;
	int size, i, j;
	int height = floor((float)image->Height/m2);
	int width = floor((float)image->Width/m2);
	Image *outimage;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	if (image->Type == GRAY)

		size = width*height;

	else if (image->Type == COLOR)

		size = width*height * 3;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < height; j++)

		{
			tempout[i*width + j] = tempin[(int)(floor((float)(m2 *i))*image->Width + floor(m2*j))];
		}
	}return(outimage);

}


Image *Pixel_replication(Image *image)

{
	unsigned char *tempin, *tempout;
	int size, i, j, p, q;
	int height = image->Height*m;
	int width = image->Width*m;
	Image *outimage;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	for (i = 0; i < image->Height; i++)
	{
		for (j = 0; j < image->Width; j++)
		{
			for (p = 0; p < m; p++)

			{
				for (q = 0; q < m; q++)

				{
					tempout[(i*m + p)*width + j*m + q] = tempin[i *image->Width + j];
				}
			}
		}
	}

	return(outimage);
}

Image *Nearest_enlargement(Image *image)
{

	unsigned char *tempin, *tempout;
	int size, i, j, p, q;
	int height = image->Height*m;
	int width = image->Width*m;
	Image *outimage;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	float M = (float)m;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			int p = ceil(i / M);
			int q = ceil(j / M);
			tempout[i*width + j] = tempin[p*image->Width + q];

		}
	}return(outimage);
}

Image *Bilinear_interpolation(Image *image)
{
	unsigned char *tempin, *tempout;
	int i, j;
	float p, q;
	Image *outimage;
	int width = image->Width * m;
	int height = image->Height * m;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	float M = (float)m;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			//define 4 points
			int x21 = floor(i / M);
			int y21 = floor(j / M);

			int x12 = ceil(i / M);
			int y12 = ceil(j / M);

			int x11 = x12 - 1;
			int y11 = y12;

			int x22 = x21 + 1;
			int y22 = y21;

			p = i / M - x21;
			q = j / M - y21;

			tempout[i*width + j] = (1 - q)*(1 - p)*tempin[x21*image->Width + y21] + p*(1 - q)*tempin[x22*image->Width + y22] + (1 - p)*q*tempin[x12*image->Width + y12] + p*q*tempin[x11*image->Width + y11];

		}
	}

	return(outimage);

}

Image *fractionalExpansion(Image *image){
	unsigned char *tempin, *tempout;
	int size, i, j;
	int height = floor((float)image->Height*m2);
	int width = floor((float)image->Width*m2);
	Image *outimage;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	if (image->Type == GRAY)

		size = width*height;

	else if (image->Type == COLOR)

		size = width*height * 3;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < height; j++)

		{
			tempout[i*width + j] = tempin[(int)(floor((float)(i/m2))*image->Width + floor(j/m2))];
		}
	}return(outimage);

}

Image *negative_image(Image *image)
{
	unsigned char *tempin, *tempout;
	int i, j, size;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	if (image->Type == GRAY)
	{

		size = width*height;

		for (i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
			tempout[i*width + j] = 255 - tempin[i*width + j];

		}
		}
	}

	else if (image->Type == COLOR)
	{
		size = width*height * 3;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				for (int m = 0; m < 3; m++)
				{
					tempout[3 * i*width + 3 * j + m] = 255 - tempin[3 * i*image->Width + 3 * j + m];
				}
			}
		}
	}
	return(outimage);
}


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

