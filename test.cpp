#include <stdio.h>
#include <ap_int.h>
#include "2dconv.hpp"
#include "define.hpp"


int main(int argc, char** argv){
	int i, j;

	FILE *fpin, *fpin2, *fpout,*fpout2;
	fpin = fopen("in.txt", "wb");
	fpin2 = fopen("in2.txt", "wb");
	fpout = fopen("out.txt", "wb");
	fpout2 = fopen("out2.txt", "wb");
	uint8_t stride = STRIDE;
	uint32_t nrows_o = ROWS_O;
	uint32_t ncols_o = COLS_O;
	uint32_t nrows_i = nrows_o*stride;
	uint32_t ncols_i = ncols_o*stride;
	uint32_t k = FILTER_SIZE;

	int8_t *in_data, *out_data,*in_data2, *out_data2;

	in_data = (int8_t*)malloc((nrows_i+k-1)*(ncols_i+k-1)*sizeof(int8_t)); //padding image鈥檚 edge
	out_data = (int8_t*)malloc(nrows_o*ncols_o*sizeof(int8_t));
	in_data2 = (int8_t*)malloc((nrows_i+k-1)*(ncols_i+k-1)*sizeof(int8_t)); //padding image鈥檚 edge
	out_data2 = (int8_t*)malloc(nrows_o*ncols_o*sizeof(int8_t));

	int8_t filter[FILTER_SIZE][FILTER_SIZE] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
	int8_t filter2[FILTER_SIZE][FILTER_SIZE] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};

	printf("Initialize input data......\n"); fflush(stdout);
	for(i=0; i<nrows_i+k-1; i++){
		for(j=0; j<ncols_i+k-1; j++){
			if(i<nrows_i && j<ncols_i){
				*(in_data+i*(ncols_i+k-1)+j) = j;
				*(in_data2+i*(ncols_i+k-1)+j) = j;
			}

			else{
				*(in_data+i*(ncols_i+k-1)+j) = 0;       //pixels out of boundary are filled with 0
				*(in_data2+i*(ncols_i+k-1)+j) = 0;
			}

			fprintf(fpin, "%2d ", *(in_data+i*(ncols_i+k-1)+j)); fflush(stdout);
			fprintf(fpin2, "%2d ", *(in_data2+i*(ncols_i+k-1)+j)); fflush(stdout);
		}
		fprintf(fpin, "\n");
		fprintf(fpin2, "\n");
	}
	printf("Completed!\n"); fflush(stdout);
	fclose(fpin);
	fclose(fpin2);
	printf("Filter:\n");
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			printf("%2d ", filter[i][j]);
		}
		printf("\n");
	}
	printf("Filter2:\n");
	for(i=0; i<3; i++){
			for(j=0; j<3; j++){
				printf("%2d ", filter2[i][j]);
			}
			printf("\n");
		}

	memset(out_data, 0, nrows_o*ncols_o*sizeof(int8_t));
	memset(out_data2, 0, nrows_o*ncols_o*sizeof(int8_t));

	int8_t buffer[ROWS_I*COLS_I*2+FILTER_SIZE*FILTER_SIZE*2+ROWS_O*COLS_O*2];

	int8_t* in_data_ptr = buffer;
	int8_t* filter_ptr = in_data_ptr + ROWS_I*COLS_I;
	int8_t* in_data_ptr2 = filter_ptr + FILTER_SIZE*FILTER_SIZE;
	int8_t* filter_ptr2 = in_data_ptr2 + ROWS_I*COLS_I;

	int8_t* out_data_ptr = filter_ptr2 + FILTER_SIZE*FILTER_SIZE;
	int8_t* out_data_ptr2 = out_data_ptr + ROWS_O*COLS_O;
	memcpy(in_data_ptr, in_data, ROWS_I*COLS_I*sizeof(int8_t));
	memcpy(in_data_ptr2, in_data2, ROWS_I*COLS_I*sizeof(int8_t));
	memcpy(filter_ptr, &filter[0][0], FILTER_SIZE*FILTER_SIZE*sizeof(int8_t));
	memcpy(filter_ptr2, &filter2[0][0], FILTER_SIZE*FILTER_SIZE*sizeof(int8_t));


#ifndef FPGA   //Run CPU codes
	printf("Run 2D convolution......"); fflush(stdout);
	_2DConv(in_data, out_data, &filter[0][0], nrows_o, ncols_o, k, stride);
	printf("Completed!\n"); fflush(stdout);
#else
	printf("[HLS] Run 2D convolution......"); fflush(stdout);
	HW_2DConv_Mmap_3(in_data_ptr,in_data_ptr2, out_data_ptr, out_data_ptr2, 0);
	printf("Completed!\n"); fflush(stdout);
	memcpy(out_data, out_data_ptr, ROWS_O*COLS_O*sizeof(int8_t));
	memcpy(out_data2, out_data_ptr2, ROWS_O*COLS_O*sizeof(int8_t));
#endif
	printf("Print results of 2D convolution:\n"); fflush(stdout);
	for(i=0; i<nrows_o; i++){
		for(j=0; j<ncols_o; j++){
			fprintf(fpout, "%3d ", *(out_data+i*ncols_o+j));
			fprintf(fpout2, "%3d ", *(out_data2+i*ncols_o+j));
		}
		fprintf(fpout, "\n");
		fprintf(fpout2, "\n");
	}
	fclose(fpout);
	fclose(fpout2);
	return 0;
}

