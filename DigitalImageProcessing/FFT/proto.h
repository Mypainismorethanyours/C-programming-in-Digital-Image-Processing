#pragma once
#define GRAY    1
#define COLOR   2
#define PI      3.14159


typedef struct Image
{
	int Width;
	int Height;
	int Type;  // Gray=1, Color=2;
	unsigned char *data;
	char comments[15][100];
	int num_comment_lines;
}Image;



Image *CreateNewImage(Image *, char *comment);
void SavePNMImage(Image *, char *);
Image *SwapImage(Image *);
Image *ReadPNMImage(char *);

Image *CreateNewSizeImage(Image *, double factor, char *comment);

Image *CreateNewSizeImage(Image *, int width, int height, char *comment);