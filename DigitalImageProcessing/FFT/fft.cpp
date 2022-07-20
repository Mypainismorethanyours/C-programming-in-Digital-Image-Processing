#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>

#define M_PI 3.14159

// 快速傅里叶变换的数据必须是 2^n  
int NUM_x = 256, NUM_y = 256;

// 将傅里叶变换后的四角移到中心 
void fft_shift(struct _complex *src)
{
	int x, y, a, b;
	struct _complex tmp;

	for(y = 0; y<NUM_y/2; y++){
		for(x = 0; x<NUM_x; x++){	
			a = y * NUM_x + x;
			b = ( (y + NUM_y/2)%NUM_y )*NUM_x + (NUM_x/2 + x)%NUM_x;
			
			tmp 	= src[a];
			src[a]  = src[b];
			src[b]  = tmp;
		}
	}
}

// 以 {0，2，4，6， 1，3，5，7} 排序，
// 数组传参使用 &f[y][x] 的形式传入，取数据时使用 f[y*NUM_x + x].x 形式获取
// split_array(&f[u][0], &F[u][0], NUM_x, 0, 0);	// 对 x分组 
// split_array(&f[u][0], &F[u][0], 0, NUM_y, 1);	// 对 y分组 
// flag: 0 时，对x 分组 
// flag: 1 时，对y 分组 
void split_array(struct _complex *src, struct _complex *dst , int x_n, int y_n, int flag)
{	
	int i;

//	struct _complex t[flag == 0 ? x_n/2 : y_n/2]; 
	struct _complex *s = src, *d = dst;
	struct _complex *t = (struct _complex*)malloc(sizeof(struct _complex)*(flag == 0 ? x_n/2 : y_n/2));
	
	if(flag == 0){ 
		if(x_n <= 1) return;
		
		for(i = 0; i<x_n/2 ; i++){
			t[i].x = s[i*2 + 1].x;		// 暂存奇数项 
			t[i].y = s[i*2 + 1].y;
			
			d[i].x = s[i*2].x;			// 拷贝偶数项到低位 
			d[i].y = s[i*2].y;
		}
		for(i = 0; i<x_n/2 ; i++){
			d[i + x_n/2].x = t[i].x;	// 拷贝奇数项到高位 
			d[i + x_n/2].y = t[i].y;
		}
		split_array(dst, 		dst, 		x_n/2, y_n, flag);
		split_array(dst+x_n/2, 	dst+x_n/2, 	x_n/2, y_n, flag);	
	} 
	else
	{
		if(y_n <= 1) return;
			
		for(i = 0; i<y_n/2 ; i++){
			t[i].x = s[(i*2 + 1)*NUM_x ].x;		// 暂存奇数项 
			t[i].y = s[(i*2 + 1)*NUM_x ].y;
			
			d[i*NUM_x ].x = s[(i*2)*NUM_x ].x;			// 拷贝偶数项到低位 
			d[i*NUM_x ].y = s[(i*2)*NUM_x ].y;
		}
		for(i = 0; i<y_n/2 ; i++){
			d[(i+y_n/2)*NUM_x ].x = t[i].x;	// 拷贝奇数项到高位 
			d[(i+y_n/2)*NUM_x ].y = t[i].y;
		}
		split_array(dst, 				dst, 				x_n, y_n/2, flag);
		split_array(dst+NUM_x*y_n/2, 	dst+NUM_x*y_n/2, 	x_n, y_n/2, flag);	
	} 
}

void printf_array(struct _complex *dst)
{
	int x,y;
	for(y = 0; y<NUM_y; y++)
	{
		for(x = 0; x<NUM_x; x++){
			printf("[%7.2lf+%7.2lfj] ", dst[y*NUM_x +x].x, dst[y*NUM_x +x].y);
		}
		printf("\n");
	}	
}
// src:源数组 &f[y][x]  dst:结果数组&F[y][x]   flag: 1:正变换   -1 逆变换
void fft(struct _complex *src, struct _complex *dst, int flag)
{
	int y, x, i, u, k , n;
	double wu;
	struct _complex w, a0, a1, t; 
	clock_t start, end;
	start = clock();
	
	// 傅里叶正变换时，将中心平移到四边 
	if(flag == -1)
		fft_shift(src);
	
	/// 
	// 对x每一行做傅里叶变换 
	for(y = 0; y<NUM_y; y++){ 	
		// 先分割数组 
		split_array(&src[y*NUM_x+0], &dst[y*NUM_x+0], NUM_x, 0, 0);	// 对 x分组 	
		
		// 对 f[y][] 这一组数进行傅里叶变换 
		for(i = 0; i<log(1.0*NUM_x)/log(2.0); i++){	//计算次数为 2^n = num，即n = log2^num 
			// 每次计算的间隔是 2^n,分别为 1，2，4，8 
			n = 2 * pow(2.0, i);						// 本轮一组个数为 2 * 2^n，分别为 2,4,8,则好3轮 
			for(k = 0; k<NUM_x/n; k++){				// num/n 为当前的组数，分别为 4，2，1 
				for(u=0; u<n/2; u++){ 				// 对每组进行计算, a0 和 b0 的个数分别为 n/2 
					wu = -1 * 2 * M_PI * u / n;		// 计算旋转因子
					w.x = cos(wu);
					w.y = flag * sin(wu);			//  如果是傅里叶逆变换，此处 flag = -1 
					
					a0 = dst[y*NUM_x + k*n + u];			// 奇数项 	[y][k*n+u]
					a1 = dst[y*NUM_x + k*n + u + n/2];		// 偶数项 	[y][k*n+u+n/2]
					
					t.x =  w.x*a1.x - w.y*a1.y;
					t.y =  w.x*a1.y + w.y*a1.x;
					
					dst[y*NUM_x + k*n + u].x =  a0.x + t.x;				// F[u] = A0 + wA1
					dst[y*NUM_x + k*n + u].y =  a0.y + t.y;
					dst[y*NUM_x + k*n + u + n/2].x =  a0.x - t.x;		// F[u+n/2] = A0 - wA1
					dst[y*NUM_x + k*n + u + n/2].y =  a0.y - t.y;
				}
			}
		}
		if(flag == 1)			// 正向时，除一个X长度，避免数据过大，本来不应该除的 
			for(u = 0; u<NUM_x; u++){
				dst[y*NUM_x + u].x /= NUM_x;
				dst[y*NUM_x + u].y /= NUM_x;
		} 
	}
	/*
	// 打印当变换结果 
	end = clock();
	printf("\n\n每行傅里叶%s变换结果为： 耗时%fs, NUM=%d x %d\n",flag==1?"":"逆", (double)(end-start)/CLOCKS_PER_SEC, NUM_x, NUM_y);
	
	printf_array(dst);
	*/
	
	
	/// 
	// 对y每一列做傅里叶变换 
	for(x = 0; x<NUM_x; x++){
		// 先分割数组 
		split_array(&dst[0*NUM_x+x], &dst[0*NUM_x+x], 0, NUM_y, 1);	// 对 y分组 	
		
		// 对 f[][x] 这一组数进行傅里叶变换 
		for(i = 0; i<log(1.0*NUM_y)/log(2.0); i++){	//计算次数为 2^n = num，即n = log2^num 
			// 每次计算的间隔是 2^n,分别为 1，2，4，8 
			n = 2 * pow(2.0, i);				// 本轮一组个数为 2 * 2^n，分别为 2,4,8,则好3轮 
			for(k = 0; k<NUM_y/n; k++){		// num/n 为当前的组数，分别为 4，2，1 
				for(u=0; u<n/2; u++){		// 对每组进行计算, a0 和 b0 的个数分别为 n/2 
					wu = -1 * 2 * M_PI * u / n;	// 计算旋转因子
					w.x = cos(wu);
					w.y = flag * sin(wu);		//  如果是傅里叶逆变换，此处 flag = -1 
					
					a0 = dst[(k*n + u)*NUM_x 		+ x ];			// 奇数项 	[k*n+u][x]
					a1 = dst[(k*n + u + n/2)*NUM_x 	+ x ];		// 偶数项 		[(k*n + u + n/2)*NUM_y 	+ x ][x]
					
					t.x =  w.x*a1.x - w.y*a1.y;
					t.y =  w.x*a1.y + w.y*a1.x;
					
					dst[(k*n + u)*NUM_x 		+ x ].x =  a0.x + t.x;				// F[u] = A0 + wA1
					dst[(k*n + u)*NUM_x 		+ x ].y =  a0.y + t.y;
					dst[(k*n + u + n/2)*NUM_x 	+ x ].x =  a0.x - t.x;				// F[u+n/2] = A0 - wA1
					dst[(k*n + u + n/2)*NUM_x 	+ x ].y =  a0.y - t.y;
					
				}
			}
		}
		if(flag == -1)
			for(u = 0; u<NUM_y; u++){
				dst[u*NUM_x + x].x /= NUM_y;
				dst[u*NUM_x + x].y /= NUM_y;
			}
	}

	// 傅里叶正变换时，将四边平移到中心 
	if(flag == 1)
		fft_shift(dst);
	
	// 打印当变换结果 
	end = clock(); 
	printf("\n傅里叶%s变换结束，耗时%fs, NUM=%d x %d\n",flag==1?"":"逆", (double)(end-start)/CLOCKS_PER_SEC, NUM_x, NUM_y);
 
//	printf_array(dst);
}

double* getResult(struct _complex *src, int size){
	int i;
	double* res = (double*)malloc(sizeof(double)*size);

	for(i = 0; i < size; ++i)
		res[i] = sqrt(src[i].x * src[i].x + src[i].y * src[i].y);

	return res;
}


/*
————————————————
版权声明：本文为CSDN博主「CielleeX」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/Ciellee/article/details/108520298
*/