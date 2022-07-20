void fft_shift(struct _complex *src);
void split_array(struct _complex *src, struct _complex *dst , int x_n, int y_n, int flag);
void printf_array(struct _complex *dst);
void fft(struct _complex *src, struct _complex *dst, int flag);
double* getResult(struct _complex *src, int size);
