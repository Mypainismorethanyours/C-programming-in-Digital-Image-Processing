#include <stdlib.h>
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

unsigned char findMxMnNeighbor(unsigned char* arr, int length){
	unsigned char temp, center = arr[length/2], max = 0, min = 255;
	int i;
	for(i = 0; i < length; ++i){
		temp = arr[i];
		if(temp < min) min = temp;
		if(temp > max) max = temp;
	}
	unsigned char diff1 = center - min;
	unsigned char diff2 = max - center;
	return diff1<diff2?min:max;
}

unsigned char calVariance(unsigned char*, int length);

unsigned char* Normal(int* arr_in, int size, int range){
	int max = 0, min = range, i, temp;
	unsigned char *arr_out = (unsigned char*)malloc(sizeof(char)*size);

//	printf("\n");
	for(i = 0; i < size; ++i){
		temp = arr_in[i];
//		printf("%d\t", temp);
		if(temp > max) max = temp;
		if(temp < min) min = temp;
	}
//	printf("\nmax=%d, min=%d\n", max, min);
	for(i = 0; i < size; ++i)
		arr_out[i] = 1.0 * (arr_in[i] - min) / (max - min) * range;
	return arr_out;
}

unsigned char* Normal(double* arr_in, int size, int range){
	double max = 0, min = range, temp;
	int i;
	unsigned char *arr_out = (unsigned char*)malloc(sizeof(char)*size);

//	printf("\n");
	for(i = 0; i < size; ++i){
		temp = arr_in[i];
//		printf("%d\t", temp);
		if(temp > max) max = temp;
		if(temp < min) min = temp;
	}
//	printf("\nmax=%d, min=%d\n", max, min);
	for(i = 0; i < size; ++i)
		arr_out[i] = (int)((arr_in[i] - min) / (max - min) * range);
	return arr_out;
}