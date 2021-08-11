/*
 * PharallelFuncs.c
 *  Created on: Jul 10, 2021
 *      Author: Shahar Hikri
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FinalProj.h"

/* Divides the processes into groups - evenly and according to the number of possible offsets,
   initializes all groups communicators
   and returns to each process the communicator of its group.
   (*size_p) value: if master of MPI_COMM_WORLD - equals to offsetComsMastersRanks len
 					else - equals to offsetComm_size*/
void getOffsetComms(int np,int max_n,int my_rank, int* CommSize_p,int* MyNewRank_p,MPI_Comm* MyOffsetComm_p, int** offsetComsMastersRanks_p, int* size_p){
	int ranksArrays_len;
	int* rankArraysElementsLens;
	int** ranksArrays;
	int* offsetComsMastersRanks;
	int i;

	if(my_rank==COMM_WORLD_MASTER_PROC_RANK){
		ranksArrays = getRanksArr(max_n, np, &ranksArrays_len, &rankArraysElementsLens);
		offsetComsMastersRanks = (int*)malloc(sizeof(int)*ranksArrays_len);
	}

	MPI_Bcast(&ranksArrays_len, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);

	if(my_rank!=COMM_WORLD_MASTER_PROC_RANK)
		rankArraysElementsLens = (int*)malloc(sizeof(int)*ranksArrays_len);
	MPI_Bcast(rankArraysElementsLens, ranksArrays_len, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);

	if(my_rank!=COMM_WORLD_MASTER_PROC_RANK)
		ranksArrays = (int**)malloc(sizeof(int*)*ranksArrays_len);
	for(i=0;i<ranksArrays_len;i++){
		if(my_rank!=COMM_WORLD_MASTER_PROC_RANK)
			ranksArrays[i] = (int*)malloc(sizeof(int)*rankArraysElementsLens[i]);
		MPI_Bcast(ranksArrays[i], rankArraysElementsLens[i], MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);
	}

	MPI_Group worldGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

	int* curr_ranksArr;
	int curr_ranksArr_len;
	for(i=0;i<ranksArrays_len;i++){
		MPI_Group curOffsetGroup;
		curr_ranksArr = ranksArrays[i];
		curr_ranksArr_len = rankArraysElementsLens[i];

		if(my_rank==COMM_WORLD_MASTER_PROC_RANK)
			offsetComsMastersRanks[i] = curr_ranksArr[0];

		MPI_Group_incl(worldGroup, curr_ranksArr_len, curr_ranksArr, &curOffsetGroup);

		if(my_rank>=curr_ranksArr[0] && my_rank<=curr_ranksArr[curr_ranksArr_len-1]){
			MPI_Comm_create(MPI_COMM_WORLD, curOffsetGroup, MyOffsetComm_p);
			MPI_Group_rank(curOffsetGroup, MyNewRank_p);
			*size_p = curr_ranksArr_len;
		}
		else{
			MPI_Comm CurOffsetCom;
			MPI_Comm_create(MPI_COMM_WORLD, curOffsetGroup, &CurOffsetCom);
		}
	}

	if(my_rank==COMM_WORLD_MASTER_PROC_RANK){
		*offsetComsMastersRanks_p = offsetComsMastersRanks;
		*size_p = ranksArrays_len;
		*MyOffsetComm_p = MPI_COMM_WORLD;
	}

	for(i=0;i<ranksArrays_len;i++)
		free(ranksArrays[i]);
	free(ranksArrays);
	free(rankArraysElementsLens);

}


Seq2_mutant_info commWorldMasterSenRsv(char* seq1,char* seq2, int min_max_flag, double W[W_LEN], int WindArr[W_LEN], int* offsetComsMastersRanks,int offsetCommsNum){
	int seq1_len,seq2_len;
	int n, max_n, i, message_from_master;
	int tag = 0;
	MPI_Status status;

	seq1_len = strlen(seq1);
	seq2_len = strlen(seq2);
	max_n = seq1_len - seq2_len;

	/*2.send seq1&seq2 to the offsetComms masters*/
	for (i = 0; i < offsetCommsNum; i++) {
		MPI_Send(&seq1_len, 1, MPI_INT, offsetComsMastersRanks[i], tag, MPI_COMM_WORLD);
		MPI_Send(seq1, seq1_len + 1, //include '/0'
		MPI_CHAR, offsetComsMastersRanks[i], tag, MPI_COMM_WORLD);
		MPI_Send(&seq2_len, 1, MPI_INT, offsetComsMastersRanks[i], tag, MPI_COMM_WORLD);
		MPI_Send(seq2, seq2_len + 1, //include '/0'
		MPI_CHAR, offsetComsMastersRanks[i], tag, MPI_COMM_WORLD);
	}
	/*end2*/

	/*3.send offsets to the masters! - dynamic task pool*/
	Seq2_mutant_info mpi_mut, cuda_mut;
	mpi_mut.seq2_mutant = (char*) malloc(sizeof(char) * seq2_len);
	cuda_mut.seq2_mutant = (char*) malloc(sizeof(char) * seq2_len);
	mpi_mut.n = -1;
	mpi_mut.weight = 0;
	double commWeight;
	int srcMasterRank;
	
	//copy seq2 by omp
	#pragma omp parallel private(i)
	{
		#pragma omp for
		for(i=0;i<seq2_len+1;i++){
			(cuda_mut.seq2_mutant)[i] = seq2[i];
		}
	}
	
	bool usingCuda = max_n>0? true: false;
	//'commWorldMasterSenRsv' func doesn't using cuda WHEN there's only offset 0 that calcultaed by MPI&OMP only.
	//and using cuda when ther's more then 1 offset to calculate (in that case cuda calculate optimal mutant for the last (max) offset).
	int mpi_max_n;
	if(usingCuda)
		mpi_max_n = max_n-1;
	else
		mpi_max_n = max_n;

	#pragma omp parallel
	{	
		if(usingCuda){
			#pragma omp single nowait
			{	
			cuda_mut.n = max_n;
			cuda_mut.weight = computeOnGPU(seq1+max_n,cuda_mut.seq2_mutant, seq2_len, W, min_max_flag, WindArr);
			printf("CUDA(Comm World Master): my_rank=%d, optimal mutant for offset(n) %d -  %s weight=%.2f\n", COMM_WORLD_MASTER_PROC_RANK, cuda_mut.n, cuda_mut.seq2_mutant,cuda_mut.weight);
			}
		}
		
		#pragma omp single nowait
		{		
			//sending 1 offset to each offsetComm master (offsets(n's): 0 to max offset minus 1 )
			for (n = 0; n < offsetCommsNum; n++)
				MPI_Send(&n, 1, MPI_INT, offsetComsMastersRanks[n], tag, MPI_COMM_WORLD);

			//dynamic taskpool - comm that finished his job get's new job
			for (i = offsetCommsNum; i <= mpi_max_n+offsetCommsNum; i++) {
				//after i=mpi_max_n recieve only and send FINISH_JOBS_OFFSET to the offsetsComm masters (for knowing to stop work)
				if(i>mpi_max_n)
					n=FINISH_JOBS_OFFSET;
				else
					n = i;

				MPI_Recv(&commWeight, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				srcMasterRank = status.MPI_SOURCE;

				if ( (commWeight > mpi_mut.weight && min_max_flag == MAX_FLAG) || (commWeight < mpi_mut.weight && min_max_flag == MIN_FLAG)) { //or n==-1

					message_from_master = SEND_REST_INFO;
					MPI_Send(&message_from_master, 1, MPI_INT, srcMasterRank, tag,MPI_COMM_WORLD);

					mpi_mut.weight = commWeight;
					MPI_Recv(mpi_mut.seq2_mutant, seq2_len + 1, MPI_CHAR, srcMasterRank,tag, MPI_COMM_WORLD, &status);
					MPI_Recv(&(mpi_mut.n), 1, MPI_INT, srcMasterRank, tag,
							MPI_COMM_WORLD, &status);
				} else {
					message_from_master = DONT_SEND_REST_INFO;
					MPI_Send(&message_from_master, 1, MPI_INT, srcMasterRank, tag, MPI_COMM_WORLD);
				}

				MPI_Send(&n, 1, MPI_INT, srcMasterRank, tag, MPI_COMM_WORLD);
			}
		}
	}
	/*end3*/
	
	if( (cuda_mut.weight > mpi_mut.weight && min_max_flag == MAX_FLAG) || (cuda_mut.weight < mpi_mut.weight && min_max_flag == MIN_FLAG) ){
		free(mpi_mut.seq2_mutant);
		return cuda_mut;
	}
	else{
		free(cuda_mut.seq2_mutant);
		return mpi_mut;
	}
}

void offsetCommMasterSenRsv(MPI_Comm* MyOffsetComm_p, int offsetComm_size, int MyNewRank, int my_rank, int min_max_flag, double W[W_LEN], int WindArr[W_LEN]){
	char* seq1;
	char* seq2;
	int seq1_len,seq2_len,n, message_from_master;
	int tag = 0;
	MPI_Status status;
	/*2.receive seq1&seq2,min_max_flag,WindArr from comm world master*/
	MPI_Recv(&seq1_len, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD, &status);
	seq1 = (char*) malloc(sizeof(char) * seq1_len);
	MPI_Recv(seq1, seq1_len + 1, MPI_CHAR, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD, &status);

	MPI_Recv(&seq2_len, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD, &status);
	seq2 = (char*) malloc(sizeof(char) * seq2_len);
	MPI_Recv(seq2, seq1_len + 1, MPI_CHAR, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD, &status);
	/*end2*/

	/*3.get 'n's' (offsets) from comm world master (part of dynamic task pool)*/
	Seq2_mutant_info comm_opt_mut;

	MPI_Recv(&n, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD, &status);

	while (n != FINISH_JOBS_OFFSET) {
		MPI_Bcast(&n, 1, MPI_INT, MASTER_PROC_RANK, *MyOffsetComm_p);
		comm_opt_mut = offsetCommJob(seq1, seq2, W, WindArr, n, MyNewRank, MyOffsetComm_p, offsetComm_size, min_max_flag);
		MPI_Barrier(*MyOffsetComm_p); //end of offset optimal mutant calculation
		printf("Group Master: my_rank=%d, optimal mutant for offset(n) %d -  %s weight=%.2f\n", my_rank, comm_opt_mut.n, comm_opt_mut.seq2_mutant, comm_opt_mut.weight);

		MPI_Send(&(comm_opt_mut.weight), 1, MPI_DOUBLE, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD);
		MPI_Recv(&message_from_master, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD, &status);

		if (message_from_master == SEND_REST_INFO) {
			MPI_Send(comm_opt_mut.seq2_mutant, seq2_len + 1, //include '/0'
			MPI_CHAR, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD);
			MPI_Send(&(comm_opt_mut.n), 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, tag, MPI_COMM_WORLD);
		}
		free(comm_opt_mut.seq2_mutant);
		MPI_Recv(&n, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, tag,
				MPI_COMM_WORLD, &status);
	}

	MPI_Bcast(&n, 1, MPI_INT, MASTER_PROC_RANK, *MyOffsetComm_p);
	/*end3*/

	free(seq1);
	free(seq2);
}


void offsetCommSlaveSenRsv(MPI_Comm* MyOffsetComm_p, int offsetComm_size, int MyNewRank, int min_max_flag, double W[W_LEN], int WindArr[W_LEN]){
	int n;
	MPI_Bcast(&n, 1, MPI_INT, MASTER_PROC_RANK, *MyOffsetComm_p);
	while (n != FINISH_JOBS_OFFSET) {
		offsetCommJob(NULL, NULL, W, WindArr, n, MyNewRank, MyOffsetComm_p,offsetComm_size, min_max_flag);
		MPI_Barrier(*MyOffsetComm_p); //end of offset optimal mutant calculation
		MPI_Bcast(&n, 1, MPI_INT, MASTER_PROC_RANK, *MyOffsetComm_p);
	}
}


/* gets communicator and give it a job to do.
 * (can get only for comm with size greater then 0)*/
Seq2_mutant_info offsetCommJob(char* seq1, char* seq2, double W[W_LEN], int WindArr[W_LEN], int n, int newRank, MPI_Comm* offsetComm_p, int offsetComm_size, int min_max_flag){
	if(offsetComm_size<=0){
		printf("offsetCommJob: group size (offsetComm_size arg) must be greater then 0!\n");
		MPI_Abort(MPI_COMM_WORLD, __LINE__);
		return (Seq2_mutant_info){-2,NULL,0};
	}

	int len; 				//seq2 length (seq2 length)
	double proc_result;
	int proc_len; 			//len of the proccess result
	int i;


	if(newRank == MASTER_PROC_RANK){
		/*1.master initializes:seq1,seq2 ,proc_len*/
		len = strlen(seq2);
		seq1 = seq1+n;
		char* seq2_mutant = (char*)malloc((len+1)*sizeof(char));
		strcpy(seq2_mutant,seq2);
		seq2= seq2_mutant;
		proc_len = len/offsetComm_size;
		/*end1*/
	}

	if(offsetComm_size==1){
		proc_result = procJob(seq1, seq2, len,W,WindArr, min_max_flag);
	}
	else{
		MPI_Request Igather_request;
		MPI_Status status;
		int tag = 0;
		char* proc_seq1;
		char* proc_seq2;

		/*2.master sends to everyone(include itself):
								  chunk of seq1/2 and their length (the length sent before the chunks - "proc_len")*/
		MPI_Bcast(&proc_len, 1, MPI_INT, MASTER_PROC_RANK, *offsetComm_p);


		proc_seq1 = (char*)malloc(proc_len*sizeof(char));
		MPI_Scatter(seq1, proc_len, MPI_CHAR,
												proc_seq1, proc_len, MPI_CHAR,
															MASTER_PROC_RANK, *offsetComm_p);

		proc_seq2 = (char*)malloc(proc_len*sizeof(char));
		MPI_Scatter(seq2, proc_len, MPI_CHAR,
												proc_seq2, proc_len, MPI_CHAR,
															MASTER_PROC_RANK, *offsetComm_p);
		/*end2*/


		/*3.every process calculate the result of its chuncks*/
		proc_result = procJob(proc_seq1, proc_seq2, proc_len,W,WindArr,min_max_flag);
		/*end3*/


		/*4.master proccess gets result chunks from all slaves(and from itself)
										  and "assemble" it to full result (- "result" var)
										  and in other thread/threads threats the rest if any*/
		if(newRank == MASTER_PROC_RANK){
			int rest_start = proc_len*offsetComm_size; 	//rest start in "result" (include)
			int rest_end = len;				//rest end in "result" (exclude)
			double threadWeight;
			omp_lock_t lock;
			omp_init_lock(&lock);
			#pragma omp parallel private(i,threadWeight)
			{
				#pragma omp single nowait
				{
					MPI_Igather(proc_seq2, proc_len, MPI_CHAR, seq2, proc_len, MPI_CHAR, MASTER_PROC_RANK, *offsetComm_p, &Igather_request);
				}
				#pragma omp for
				for(i=rest_start;i<rest_end;i++){
					threadWeight = replaceC2toOpt(seq1[i],(seq2+i),W,min_max_flag,WindArr);
					omp_set_lock(&lock);
					proc_result+=threadWeight;
					omp_unset_lock(&lock);
				}

			}
			MPI_Wait(&Igather_request, &status); //waiting foe finish iGather
		}
		else{
			//master process recieves chunks from all processes(include itself) into 'buff'
			MPI_Igather(proc_seq2, proc_len, MPI_CHAR, seq2, proc_len, MPI_CHAR, MASTER_PROC_RANK, *offsetComm_p,&Igather_request);
			MPI_Wait(&Igather_request, &status);
		}
		/*end4*/


		/*5.master get sum of all*/
		if(newRank == MASTER_PROC_RANK){
			double temp;
			for(i=1;i<offsetComm_size;i++){
				MPI_Recv(&temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, *offsetComm_p, &status);
				proc_result += temp;
			}
		}
		else{
			MPI_Send(&proc_result, 1, MPI_DOUBLE, MASTER_PROC_RANK, tag, *offsetComm_p);
		}
		/*end5*/


		/*6.free and return*/
		free(proc_seq1);
		free(proc_seq2);
	}

	if(newRank == MASTER_PROC_RANK){
		return (Seq2_mutant_info){ n, seq2,  proc_result};
	}
	else
		return (Seq2_mutant_info){-1,NULL,0};
	/*end6*/
}


/*slave(of some offsetComm) job - change proc_seq2 (his seq2 chunk) to the optimal mutant and update proc_sum (it's score)*/
double procJob(char* proc_seq1,char* proc_seq2,int proc_len, double W[W_LEN], int WindArr[W_LEN], int min_max_flag){

	double proc_sum=0;
	double theadWeight;

	int i;
	omp_lock_t lock;
	omp_init_lock(&lock);

	/*Pharallel*/
	#pragma omp parallel private(i,theadWeight)
	{
		#pragma omp for
		for(i=0;i<proc_len;i++){
			theadWeight = replaceC2toOpt(proc_seq1[i],(proc_seq2+i),W,min_max_flag,WindArr);
			omp_set_lock(&lock);
			proc_sum+=theadWeight;
			omp_unset_lock(&lock);
		}
	}
	return proc_sum;
}
