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

Image *Laplacian_operator(Image *);
Image *Sobel_operator(Image *);
Image *Gamma_correction(Image *);
Image *Global_enhancement(Image *);
Image *Local_enhancement(Image *);



int TestReadImage(char *, char *);


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

	outimage = Laplacian_operator(image);
	SavePNMImage(outimage, "..\\output\\laplacianLena.pgm");
	outimage = Sobel_operator(image);
	SavePNMImage(outimage, "..\\output\\sobelLena.pgm");
	outimage = Gamma_correction(image);
	SavePNMImage(outimage, "..\\output\\gammaLena.pgm");
	outimage = Global_enhancement(image);
	SavePNMImage(outimage, "..\\output\\globalLena.pgm");
	outimage = Local_enhancement(image);
	SavePNMImage(outimage, "..\\output\\localLena.pgm");

	return(0);
}

Image *Laplacian_operator(Image *image)
{
	unsigned char *tempin, *tempout;
	int i, j;
	int ia1,ja1,im1,jm1,temp;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;

	for (i = 1; i < height - 1; i++)
	{
		for (j = 1; j < width - 1; j++)
		{
			temp = tempin[i * width + j] - (tempin[(i-1) * width + j] + tempin[(i+1) * width + j] - 4 * tempin[i * width + j] + tempin[i * width + j-1] + tempin[i * width + j + 1]) ;
			if (temp < 0) {
                    temp = 0;
                }
                else if (temp > 255) {
                    temp = 255;
                }
			tempout[i*width + j] = (unsigned char)temp;
		}
	}return (outimage);

}

Image *Sobel_operator(Image *image)
{

	unsigned char *tempin, *tempout;
	int i, j;
	int gx = 0, gy = 0;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	for (i = 1; i < height - 1; i++)
	{
		for (j = 1; j < width - 1; j++)
		{
			tempout[i*width + j] = abs(tempin[(i - 1)*width + j + 1] + 2 * tempin[i*width + j + 1] + tempin[(i + 1)*width + j + 1] - (tempin[(i - 1)*width + j - 1] + 2 * tempin[i*width + j - 1] + tempin[(i + 1)*width + j - 1]))
				+ abs(tempin[(i + 1)*width + j - 1] + 2 * tempin[(i + 1)*width + j] + tempin[(i + 1)*width + j + 1] - (tempin[(i - 1)*width + j - 1] + 2 * tempin[(i - 1)*width + j] + tempin[(i - 1)*width + j + 1]));
		}
	}
	return (outimage);
}

Image *Gamma_correction(Image *image)
{

	unsigned char *tempin, *tempout;
	int i, j;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			tempout[i*width + j] = pow((double)tempin[i*width + j] / 256,1.0) * 256;
		}
	}
	return (outimage);
}

Image *Global_enhancement(Image *image)
{
	unsigned char *tempin, *tempout;
	int m, n, p, i, j, k, r, temp = 0;
	float a;
	Image *outimage;
	int width = image->Width;
	int height = image->Height;
	outimage = CreateNewImage(image, "#testing Swap", width, height);
	tempin = image->data;
	tempout = outimage->data;
	int R[256];
	r = 0;
	
	for (m = 0; m < 256; m++)
	{

		for (n = 0; n < width; n++)
		{
			for (p = 0; p < height; p++)
			{
				if (tempin[n*width + p] == m)
				{
					r++;
				}
			}

		}
		R[m] = r;
	}

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			tempout[i*width + j] = floor(R[tempin[i*width + j]]*255 / (width * height) + 0.5);
		}

		
	}return (outimage);
}


Image *Local_enhancement(Image *image)
{
	int Width, Height, size,i,j,k,t;
	Width = image->Width;
	Height = image->Height;
	size = Width * Height;
	Image* outimage;
	unsigned char* tempin, * tempout,local[9];
	outimage = CreateNewImage(image, "#testing GlobalHE",Width,Height);
	tempin = image->data;
	tempout = outimage->data;
	double temp;
	int nums[9];
	int row, column;
	for(i = 1; i < image->Height - 1; ++i){
		for(j = 1; j < image->Width - 1; ++j){
			int pos = 0;
			for(k = -1; k < 2; ++k)
				for(t = -1; t < 2; ++t)
					local[pos++] = tempin[image->Width * (i + k) + (j + t)];
                temp = 0;
                for (int i = 0;i < 9;i++) {
                    if (local[i] <= local[4]) {
                        temp++;
                    }
                }
			    tempout[i*Width + j] = floor(255 / 9 * temp + 0.5);	
		}
	}return(outimage);
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