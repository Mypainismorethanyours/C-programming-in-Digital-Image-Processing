#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proto.h"
#include "fft.h"
#include "tool.h"

int TestReadImage(char *, char *);

int main(int argc, char **argv)
{
	argv[1] = "D:\\2021Spring\\DIP\\workshop\\DFTimages\\testCircle.pgm";
	TestReadImage(argv[1],argv[2]);

	return(0);
}

int TestReadImage(char *filename, char *outfilename)
{
	Image *image;
	Image *outimage;

	image=ReadPNMImage(filename);

	int size = image->Height * image->Width;
	struct _complex *src = (struct _complex*)malloc(sizeof(struct _complex)*size);

	int i;
	for(i = 0; i < size; ++i){
		src[i].x = 1.0*image->data[i];
		src[i].y = 0.0;
	}

	fft(src, src, 1);
	
	outimage = CreateNewSizeImage(image, image->Width, image->Height, "FFT result");

	outimage->data = Normal(getResult(src, size), size, 255);

	SavePNMImage(outimage, "D:\\2021Spring\\DIP\\workshop\\fft_testCircle.pgm");


	fft(src, src, -1);
	outimage->data = Normal(getResult(src, size), size, 255);

	SavePNMImage(outimage, "D:\\2021Spring\\DIP\\workshop\\ifft_testCircle.pgm");

	return(0);
}
