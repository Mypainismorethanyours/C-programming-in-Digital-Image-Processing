#pragma warning(disable: 4996)
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "proto.h"

typedef struct QNode{
	int data;
	struct QNode *next;
}QNode;

typedef struct Queue{
	struct QNode* first;
	struct QNode* last;
}Queue;

Image* erosion(Image*);
Image* dialation(Image*);
Image* open(Image*);
Image* close(Image*);
Image *Extractboundaries(Image*);
void PushQueue(Queue *queue, int data);
int PopQueue(Queue *queue);
void SearchNeighbor(unsigned char *bitmap, int width, int height, unsigned char *labelmap,
	int labelIndex, int pixelIndex, Queue *queue);
void countNumberOfPixelsInWhiteConnectedComponetAndPutIntoTXT(Image*);
Image *seprdateParticlesMergedWithBoundries(Image*);
Image *seprdateOverlappingParticles(Image*);
Image *seprdateNonOverlappingParticles(Image*);


int TestReadImage(char*, char*);
Image* ReadPNMImage(char*);
Image* SwapImage(Image*);
Image* CreateNewImage(Image*, const char* comment);
void SavePNMImage(Image*, char*);


void PushQueue(Queue *queue, int data){
	QNode *p = NULL;
	p = (QNode*)malloc(sizeof(QNode));
	p->data = data;
	if (queue->first == NULL){
		queue->first = p;
		queue->last = p;
		p->next = NULL;
	}
	else{
		p->next = NULL;
		queue->last->next = p;
		queue->last = p;
	}
}

int PopQueue(Queue *queue){
	QNode *p = NULL;
	int data;
	if (queue->first == NULL){
		return -1;
	}
	p = queue->first;
	data = p->data;
	if (queue->first->next == NULL){
		queue->first = NULL;
		queue->last = NULL;
	}
	else{
		queue->first = p->next;
	}
	free(p);
	return data;
}

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

	image = ReadPNMImage("..\\images\\noisy_fingerprint.pgm");
	outimage = erosion(image);
	SavePNMImage(outimage, "..\\output\\erosion_noisy_fingerprint.pgm");
	outimage = dialation(image);
	SavePNMImage(outimage, "..\\output\\dialation_noisy_fingerprint.pgm");
	outimage = open(image);
	SavePNMImage(outimage, "..\\output\\open_noisy_fingerprint.pgm");
	outimage = close(image);
	SavePNMImage(outimage, "..\\output\\close_noisy_fingerprint.pgm");

	image = ReadPNMImage("..\\images\\noisy_rectangle.pgm");
	outimage = erosion(image);
	SavePNMImage(outimage, "..\\output\\erosion_noisy_rectangle.pgm");
	outimage = dialation(image);
	SavePNMImage(outimage, "..\\output\\dialation_noisy_rectangle.pgm");
	outimage = open(image);
	SavePNMImage(outimage, "..\\output\\open_noisy_rectangle.pgm");
	outimage = close(image);
	SavePNMImage(outimage, "..\\output\\close_noisy_rectangle.pgm");
	
	image = ReadPNMImage("..\\images\\licoln.pgm");
	outimage = Extractboundaries(image);
	SavePNMImage(outimage, "..\\output\\licoln_boundary.pgm");

	image = ReadPNMImage("..\\images\\U.pgm");
	outimage = Extractboundaries(image);
	SavePNMImage(outimage, "..\\output\\U_boundary.pgm");

	image = ReadPNMImage("..\\images\\connected.pgm");
	countNumberOfPixelsInWhiteConnectedComponetAndPutIntoTXT(image);

	image = ReadPNMImage("..\\images\\bubbles_on_black_background.pgm");
	outimage = seprdateParticlesMergedWithBoundries(image);
	SavePNMImage(outimage, "..\\output\\particlesMergedWithBoundries.pgm");
	outimage = seprdateOverlappingParticles(image);
	SavePNMImage(outimage, "..\\output\\overlappingParticles.pgm");
	outimage = seprdateNonOverlappingParticles(image);
	SavePNMImage(outimage, "..\\output\\nonOverlappingParticles.pgm");


	return(0);
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

	int i, j, x, y,flag;
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


Image *dialation(Image* image) {
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "dialation";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;

	int i, j, x, y, flag;
	for (i = 1; i < height - 1; i++){
		for (j = 1; j < width - 1; j++){
			flag = 1;
			for (x = i - 1; x < i + 2; x++){
				for (y = j - 1; y < j + 2; y++){
					if (tempin[x*width + y] == 255){
						flag = 0;
						break;
					}
				}
				if (flag == 0){
					break;
				}
			}
			if (flag == 0){
				tempout[i*width + j] = 255;
			}
			else{
				tempout[i*width + j] = 0;
			}
		}
	}return(outimage);
}

Image *open(Image* image) {
	return(dialation(erosion(image)));
}
Image *close(Image* image) {
	return(erosion(dialation(image)));
}
Image *Extractboundaries(Image* image){
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "Extractboundaries";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	int i, j;	
	outimage = erosion(image);
	tempout = outimage->data;

	for (i = 0; i < height; i++){
		for (j = 0; j < width; j++){
			tempout[i*width + j] = tempin[i*width + j] - tempout[i*width + j];
		}
	}return(outimage);
}

void SearchNeighbor(unsigned char *tempin, int width, int height, unsigned char *tempout, int labelIndex, int pixelIndex, Queue *queue){
	static int NeighborDirection[8][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
	int searchIndex, i, length;
	tempout[pixelIndex] = labelIndex;
	length = width * height;
	for (i = 0; i < 8; i++){
		searchIndex = pixelIndex + NeighborDirection[i][0] * width + NeighborDirection[i][1];
		if (searchIndex > 0 && searchIndex < length &&
			tempin[searchIndex] == 255 && tempout[searchIndex] == 0){
			tempout[searchIndex] = labelIndex;
			PushQueue(queue, searchIndex);
		}
	}
}

void countNumberOfPixelsInWhiteConnectedComponetAndPutIntoTXT(Image* image){
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "countNumberOfPixelsInWhiteConnectedComponetAndPutIntoTXT";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	memset(tempout, 0, width * height);

	int i, j, popIndex, labelIndex = 0;
	Queue *queue = NULL;
	queue = (Queue*)malloc(sizeof(Queue));
	queue->first = NULL;
	queue->last = NULL;
	
	for (i = 1; i < height - 1; i++){
		for (j = 1; j < width - 1; j++){
			if (tempin[i * width + j] == 255 && tempout[i * width + j] == 0){
				labelIndex++;
				SearchNeighbor(tempin, width, height, tempout, labelIndex, i * width + j, queue);

				popIndex = PopQueue(queue);
				while (popIndex > -1){
					SearchNeighbor(tempin, width, height, tempout, labelIndex, popIndex, queue);
					popIndex = PopQueue(queue);
				}
			}
		}
	}
	free(queue);
	SavePNMImage(outimage, "..\\output\\ConnectedComponentLabeling.pgm");
	int k, count;
	FILE* fp = NULL;
	fp = fopen("..\\output\\numberOfPixelsInWhiteConnectedComponet.txt", "w+");
	for (k = 1; k <= labelIndex; k++){
		count = 0;
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){
				if (outimage->data[i * width + j] == k){
					count++;
				}
			}
		}
		fprintf(fp, "The number of pixels in %dth white connected componet is %d.\n", k, count);
	}
	fclose(fp);
}

Image *seprdateParticlesMergedWithBoundries(Image* image){
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "splitParticlesMergedWithBoundries";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	memset(tempout, 0, width * height);

	int i, j, popIndex, labelIndex = 0;
	for (j = 0; j < width; j++){
		tempin[j] = 255;
	}
	for (j = 0; j < width; j++){
		tempin[(height - 1) * width + j] = 255;
	}
	for (i = 0; i < height; i++){
		tempin[i * width] = 255;
	}
	for (i = 0; i < height; i++){
		tempin[i * width + width - 1] = 255;
	}

	Queue *queue = NULL;
	queue = (Queue*)malloc(sizeof(Queue));
	queue->first = NULL;
	queue->last = NULL;

	for (i = 1; i < height - 1; i++){
		for (j = 1; j < width - 1; j++){
			if (tempin[i * width + j] == 255 && tempout[i * width + j] == 0){
				labelIndex++;
				SearchNeighbor(tempin, width, height, tempout, labelIndex, i * width + j, queue);

				popIndex = PopQueue(queue);
				while (popIndex > -1){
					SearchNeighbor(tempin, width, height, tempout, labelIndex, popIndex, queue);
					popIndex = PopQueue(queue);
				}
			}
		}
	}
	free(queue);

	for (i = 0; i < height; i++){
		for (j = 0; j < width; j++){
			if (tempout[i * width + j] == 1){
				tempout[i * width + j] = 255;
			}
			else{
				tempout[i * width + j] = 0;
			}
		}
	}
	return(outimage);
}

Image *seprdateOverlappingParticles(Image* image){
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "seprdateOverpallingParticles";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	memset(tempout, 0, width * height);

	int i, j, popIndex, labelIndex = 0;
	for (i = 0; i < 7; i++){
		for (j = 0; j < width; j++){
			tempin[i * width + j] = 0;
		}
	}
	for (j = 0; j < width; j++){
		tempin[(height - 1) * width + j] = 0;
	}
	for (i = 0; i < height; i++){
		for (j = 0; j < 7; j++){
			tempin[i * width + j] = 0;
		}
	}
	for (i = 0; i < height; i++){
		for (j = 452; j < width; j++){
			tempin[i * width + j] = 0;
		}
	}

	Queue *queue = NULL;
	queue = (Queue*)malloc(sizeof(Queue));
	queue->first = NULL;
	queue->last = NULL;

	for (i = 1; i < height - 1; i++){
		for (j = 1; j < width - 1; j++){
			if (tempin[i * width + j] == 255 && tempout[i * width + j] == 0){
				labelIndex++;
				SearchNeighbor(tempin, width, height, tempout, labelIndex, i * width + j, queue);

				popIndex = PopQueue(queue);
				while (popIndex > -1){
					SearchNeighbor(tempin, width, height, tempout, labelIndex, popIndex, queue);
					popIndex = PopQueue(queue);
				}
			}
		}
	}
	free(queue);

	int k, count;
	for (k = 1; k <= labelIndex; k++){
		count = 0;
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){
				if (outimage->data[i * width + j] == k){
					count++;
				}
			}
		}
		if (count < 22 * 22){
			for (i = 0; i < height; i++){
				for (j = 0; j < width; j++){
					if (outimage->data[i * width + j] == k){
						outimage->data[i * width + j] = 0;
					}
				}
			}
		}
		else{
			for (i = 0; i < height; i++){
				for (j = 0; j < width; j++){
					if (outimage->data[i * width + j] == k){
						outimage->data[i * width + j] = 255;
					}
				}
			}
		}
	}
	return(outimage);
}

Image *seprdateNonOverlappingParticles(Image* image){
	int width, height;
	width = image->Width;
	height = image->Height;
	Image* outimage;
	unsigned char* tempin, *tempout;
	const char* comment = "seprdateNonOverlappingParticles";
	outimage = CreateNewImage(image, comment);
	tempin = image->data;
	tempout = outimage->data;
	memset(tempout, 0, width * height);

	int i, j, popIndex, labelIndex = 0;
	for (i = 0; i < 7; i++){
		for (j = 0; j < width; j++){
			tempin[i * width + j] = 0;
		}
	}
	for (j = 0; j < width; j++){
		tempin[(height - 1) * width + j] = 0;
	}
	for (i = 0; i < height; i++){
		for (j = 0; j < 7; j++){
			tempin[i * width + j] = 0;
		}
	}
	for (i = 0; i < height; i++){
		for (j = 452; j < width; j++){
			tempin[i * width + j] = 0;
		}
	}

	Queue *queue = NULL;
	queue = (Queue*)malloc(sizeof(Queue));
	queue->first = NULL;
	queue->last = NULL;

	for (i = 1; i < height - 1; i++){
		for (j = 1; j < width - 1; j++){
			if (tempin[i * width + j] == 255 && tempout[i * width + j] == 0){
				labelIndex++;
				SearchNeighbor(tempin, width, height, tempout, labelIndex, i * width + j, queue);

				popIndex = PopQueue(queue);
				while (popIndex > -1){
					SearchNeighbor(tempin, width, height, tempout, labelIndex, popIndex, queue);
					popIndex = PopQueue(queue);
				}
			}
		}
	}
	free(queue);

	int k, count;
	for (k = 1; k <= labelIndex; k++){
		count = 0;
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){
				if (outimage->data[i * width + j] == k){
					count++;
				}
			}
		}
		if (count < 22 * 22){
			for (i = 0; i < height; i++){
				for (j = 0; j < width; j++){
					if (outimage->data[i * width + j] == k){
						outimage->data[i * width + j] = 255;
					}
				}
			}
		}
		else{
			for (i = 0; i < height; i++){
				for (j = 0; j < width; j++){
					if (outimage->data[i * width + j] == k){
						outimage->data[i * width + j] = 0;
					}
				}
			}
		}
	}
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