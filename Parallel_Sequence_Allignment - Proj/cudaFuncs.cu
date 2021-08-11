/*
 * cudaFuncs.cu
 *  Created on: Jul 18, 2021
 *      Author: Shahar Hikri
 */

#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <stdlib.h>
#define CUDA_LOGIC
#include "FinalProj.h"

/*same as strlen func (from 'string.h').
*/
__host__ __device__ int strlen_dh(const char* str){
	int i=0;
	while(str[i]!='\0')
		i++;
	return i;
}

/*if str contains c return true
  else return false.
*/
__host__ __device__ bool isContains(const char* str, char c){
	if(str==NULL)
		return false;
	
	int str_len = strlen_dh(str);
	int i=0;
	
	for(i=0;i<str_len;i++){
		if(str[i]==c)
			return true;
	}
	return false;
}

/*if c2 in c2In_str: leave it's val and return.
 * else: replace C2 with char in c2In_str but not in c2NotIn_str.*/
__host__ __device__  bool replaceC2(char c1,char* c2_p,const char* c2In_str,const char* c2NotIn_str){
	char c2 = *c2_p;

	if(c1<'A' || c1>'Z' || c2<'A' || c2>'Z'){
		return false;
	}

	if(isContains(c2In_str, c2)) //leave c2 as is
		return true;

	int c2In_str_len = strlen_dh(c2In_str);
	int i;
	for(i=0;i<c2In_str_len;i++){
		if((c2NotIn_str==NULL || !isContains(c2NotIn_str, c2In_str[i])) ){
			*c2_p = c2In_str[i];
			return true;
		}
	}
	return false;
}

/*replacing c2 to optimal value and return the score that c1 and the new c2 gives
  - won't change c2 value if it cant replace its value to somthing that gives better score with c1.
  - semi&con priority:  for example if semi weight better then CON wieght, the function will try to change c2(if not already is) 
						to semi that not con with c1, if it didn't find semi and not con, it will try to give c2 semi that is also con with c1.
						In either case it will bring back semi's weight.
*/
__host__ __device__ double replaceC2toOpt(char c1,char* c2_p, double* W , int min_max_flag, int* WindArr){
	#ifdef  __CUDA_ARCH__ //gpu(device) using this function
	
	#define CONANDSEMI_DICT ConAndSemi_dict_d
	#define SEMIANDNOTCON_DICT SemiAndNotCon_dict_d
	#define CONANDNOTSEMI_DICT ConAndSemi_dict_d
	#define CON_DICT Con_dict_d
	#define NOTCONANDNOTSEMI_DICT notConAndNotSemi_dict_d

	#else  //cpu(host) using this function

	#define CONANDSEMI_DICT ConAndSemi_dict
	#define SEMIANDNOTCON_DICT SemiAndNotCon_dict
	#define CONANDNOTSEMI_DICT ConAndSemi_dict
	#define CON_DICT Con_dict
	#define NOTCONANDNOTSEMI_DICT notConAndNotSemi_dict

	#endif

	char c2 = *c2_p;

	int i;

	bool searched_in_ConAndSemi_flag = false;

	for(i=0;i<W_LEN;i++){

		if(WindArr[i]==DOT){
			if(replaceC2(c1, c2_p, SEMIANDNOTCON_DICT[c1-'A'], CON_DICT[c2-'A']) )
				return W[DOT];
			else if(!searched_in_ConAndSemi_flag && replaceC2(c1, c2_p, CONANDSEMI_DICT[c1-'A'], CON_DICT[c2-'A']))
				return W[DOT];
			else
				searched_in_ConAndSemi_flag=true;
		}

		else if(WindArr[i]==COL){

			if(replaceC2(c1, c2_p, CONANDNOTSEMI_DICT[c1-'A'], CON_DICT[c2-'A']) )
				return W[COL];
			else if(!searched_in_ConAndSemi_flag && replaceC2(c1, c2_p, CONANDSEMI_DICT[c1-'A'], CON_DICT[c2-'A']))
				return W[COL];
			else
				searched_in_ConAndSemi_flag=true;
		}

		else if(WindArr[i]==AST){
			char c2In_str[2] = {c1 , '\0' };
			if(replaceC2(c1, c2_p, c2In_str, CON_DICT[c2-'A']))
				return W[AST];
		}

		else{
			if(replaceC2(c1, c2_p, NOTCONANDNOTSEMI_DICT[c1-'A'], CON_DICT[c2-'A']))
				return W[SPC];
		}
	}
	return 0;
} 


__global__ void fillDict(char* proc_seq1,char* proc_seq2, double* W, int* WindArr, int min_max_flag, double* proc_sum_p){
	int i = threadIdx.x;
	if(i==0)
		*proc_sum_p = 0;
	__syncthreads();
	
	double theadWeight = replaceC2toOpt(proc_seq1[i],(proc_seq2+i),W,min_max_flag,WindArr);

	int j;
	for(j=0;j<26;j++){
		if(i==j)
			(*proc_sum_p)+=theadWeight;
			
		__syncthreads();
	}
}

double computeOnGPU(char *proc_seq1,char *proc_seq2, int seq_len, double W[4], int min_max_flag, int WindArr[4]) {

	char *proc_seq1_d, *proc_seq2_d;
	double* cudaWeight_p_d;
	double cudaWeight = 0;
	double *W_d;
	int* WindArr_d;
	
	cudaMalloc((void **)&proc_seq1_d, seq_len*sizeof(char));
	cudaMalloc((void **)&proc_seq2_d, seq_len*sizeof(char));
	cudaMalloc((void **)&cudaWeight_p_d, sizeof(double));
	cudaMalloc((void **)&W_d, 4*sizeof(double));
	cudaMalloc((void **)&WindArr_d, 4*sizeof(int));

	cudaMemcpy(proc_seq1_d, proc_seq1, seq_len*sizeof(char), cudaMemcpyHostToDevice);
	cudaMemcpy(proc_seq2_d, proc_seq2, seq_len*sizeof(char), cudaMemcpyHostToDevice);
	cudaMemcpy(W_d, W, 4*sizeof(double), cudaMemcpyHostToDevice);
	cudaMemcpy(WindArr_d, WindArr, 4*sizeof(int), cudaMemcpyHostToDevice);

	fillDict<<<1,seq_len>>>(proc_seq1_d,proc_seq2_d, W_d, WindArr_d, min_max_flag, cudaWeight_p_d);
	
	cudaMemcpy(proc_seq2, proc_seq2_d, seq_len*sizeof(char), cudaMemcpyDeviceToHost);
	cudaMemcpy(&cudaWeight, cudaWeight_p_d, sizeof(double), cudaMemcpyDeviceToHost);

	cudaFree(proc_seq1_d);
	cudaFree(proc_seq2_d);
	cudaFree(cudaWeight_p_d);
	cudaFree(W_d);
	cudaFree(WindArr_d);
	
   	return cudaWeight;
}
