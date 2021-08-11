/*
 * FinalProj.h
 *  Created on: Jul 7, 2021
 *      Author: Shahar Hikri
 */

#ifndef FINALPROJ_H_
#define FINALPROJ_H_

#define MASTER_PROC_RANK 0
#define COMM_WORLD_MASTER_PROC_RANK 0

#define MIN_FLAG -1
#define MAX_FLAG 1
#define WRONG_MIN_MAX_FLAG 0

#define DONT_CARE 0

#define SEND_REST_INFO 1
#define DONT_SEND_REST_INFO 0
#define FINISH_JOBS_OFFSET -1

//W array indexes
#define AST 0 //asterisk ('*')
#define COL 1 //colon (':')
#define DOT 2 //DOT ('.')
#define SPC 3 //space(' ')

#define W_LEN 4

//typedef enum{false,true} bool;

#ifdef CUDA_LOGIC
#include "dictionaries.h"

#else

typedef struct{
	int n; //offset
	char* seq2_mutant;
	double weight; //sum of weights
} Seq2_mutant_info;


#include <mpi.h>
#include <omp.h>

/*PharallelFuncs prototypes*/
void getOffsetComms(int np,int max_n,int my_rank, int* CommSize_p,int* MyNewRank_p,MPI_Comm* MyOffsetComm_p, int** offsetComsMastersRanks_p, int* size_p);

Seq2_mutant_info commWorldMasterSenRsv(char* seq1,char* seq2, int min_max_flag, double W[W_LEN], int WindArr[W_LEN], int* offsetComsMastersRanks,int offsetCommsNum);
void offsetCommMasterSenRsv(MPI_Comm* MyOffsetComm_p, int offsetComm_size, int MyNewRank, int my_rank, int min_max_flag, double W[W_LEN], int WindArr[W_LEN]);
void offsetCommSlaveSenRsv(MPI_Comm* MyOffsetComm_p, int offsetComm_size, int MyNewRank, int min_max_flag, double W[W_LEN], int WindArr[W_LEN]);

Seq2_mutant_info offsetCommJob(char* seq1, char* seq2, double W[W_LEN], int WindArr[W_LEN], int n, int newRank, MPI_Comm* offsetComm_p, int offsetComm_size, int min_max_flag);
double procJob(char* proc_seq1,char* proc_seq2,int proc_len, double W[W_LEN], int WindArr[W_LEN], int min_max_flag);

/*MututalLogicFuncs prototypes*/
extern double replaceC2toOpt(char c1,char* c2_p, double W[W_LEN] , int min_max_flag, int WindArr[W_LEN]);
bool replaceC2(char c1,char* c2_p,const char* c2In_str,const char* c2NotIn_str);
bool isContains(const char* str, char c);

/*LogicFuncs prototypes*/
void sortWindArr(double W[W_LEN],int WindArr[W_LEN], int min_max_flag);
int** getRanksArr(int max_n, int np, int* ranksArrays_len_p, int** rankArraysElementsLens_p);


/*IOFilesFuncs prototypes*/
bool readFromFIle(char* filename, double W[W_LEN], char* seq1, char* seq2,int* min_max_flag_p);
bool writeToFIle(char* filename, char* seq2_mutant, int n,double score);

/*Cuda Function*/
double computeOnGPU(char *proc_seq1,char *proc_seq2, int seq_len, double W[4], int min_max_flag, int WindArr[4]);

#endif /* not CUDA_LOGIC*/

#endif /* FINALPROJ_H_ */
