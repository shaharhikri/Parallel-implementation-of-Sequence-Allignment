/*
 * FinalProj.C
 *  Created on: Jul 7, 2021
 *      Author: Shahar Hikri
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FinalProj.h"

int main(int argc, char* argv[]){
	int my_rank; 	// rank of process
	int np; 		// number of processes

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	char seq1[1000];
	char seq2[5000];
	double W[4];
	int WindArr[W_LEN] = {AST,COL,DOT,SPC}; //sorted by W array (will be sorted later).des
	int min_max_flag;
	MPI_Comm MyOffsetComm;

	if(my_rank==COMM_WORLD_MASTER_PROC_RANK){

		/*1.INPUT - NEEDED TO BE TAKEN FROM INPUT.TXT FILE*/
		if(argc<3){
			perror("need 2 arguments to this program (input file path and output file path)\n");
			MPI_Abort(MPI_COMM_WORLD, __LINE__);
			return -1;
		}
		if(!readFromFIle(argv[1], W, seq1, seq2, &min_max_flag)){
			MPI_Abort(MPI_COMM_WORLD, __LINE__);
			return -1;
		}
		printf("Input: W1=%.2f W2=%.2f W3=%.2f W4=%.2f seq1=%s seq2=%s %s\n\n",W[0],-W[1],-W[2],-W[3],seq1,seq2,
				min_max_flag==MAX_FLAG?"MAX":"MIN");

		int max_n = strlen(seq1) - strlen(seq2);
		if(max_n<0){
			perror("seq2 longer then seq1!\n");
			MPI_Abort(MPI_COMM_WORLD, __LINE__);
			return -1;
		}
		/*INPUT END*/


		/*Calculating and Send\Recv*/
		sortWindArr(W, WindArr, min_max_flag); //sort WindArr
		Seq2_mutant_info opt_mut;
		if(np==1){
			opt_mut = offsetCommJob(seq1, seq2, W, WindArr, 0, 0, NULL, 1, min_max_flag);
			Seq2_mutant_info cur_mut;
			int i;
			for (i = 1; i <= max_n; i++) {
				cur_mut = offsetCommJob(seq1, seq2, W, WindArr, i, 0, NULL, 1, min_max_flag);
				printf("Comm World Master: my_ramk=%d, optimal mutant for offset(n) %d -  %s weight=%.2f\n", my_rank, cur_mut.n, cur_mut.seq2_mutant, cur_mut.weight);
				if((min_max_flag==MAX_FLAG && cur_mut.weight>opt_mut.weight) || (min_max_flag==MIN_FLAG && cur_mut.weight<opt_mut.weight)){
					free(opt_mut.seq2_mutant);
					opt_mut = cur_mut;
				}
				else{
					free(cur_mut.seq2_mutant);
				}
			}
		}
		else{
			int offsetCommsNum;
			int *offsetComsMastersRanks;

			getOffsetComms(np, max_n-1, my_rank, NULL, NULL, &MyOffsetComm, &offsetComsMastersRanks, &offsetCommsNum);

			MPI_Bcast(W, W_LEN, MPI_DOUBLE, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);
			MPI_Bcast(WindArr, W_LEN, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);
			MPI_Bcast(&min_max_flag, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);

			opt_mut = commWorldMasterSenRsv(seq1, seq2, min_max_flag, W, WindArr, offsetComsMastersRanks, offsetCommsNum);
			free(offsetComsMastersRanks);
		}
		/*Calculating and Send\Recv END*/


		/*OUTPUT - SHOULD BE WRITEN TO OUTPUT.TXT*/
		if(!writeToFIle(argv[2], opt_mut.seq2_mutant, opt_mut.n, opt_mut.weight)){
			MPI_Abort(MPI_COMM_WORLD, __LINE__);
			return -1;
		}
		printf("\nOutput: seq2_opt_mutant=%s weight=%.2f n=%d\n", opt_mut.seq2_mutant,opt_mut.weight, opt_mut.n);
		/*OUTPUT END*/
		free(opt_mut.seq2_mutant);
	}
	else{
		int MyNewRank, CommSize, offsetComm_size;
		getOffsetComms(np, DONT_CARE, my_rank, &CommSize, &MyNewRank, &MyOffsetComm, NULL,&offsetComm_size);

		MPI_Bcast(W, W_LEN, MPI_DOUBLE, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);
		MPI_Bcast(WindArr, W_LEN, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);
		MPI_Bcast(&min_max_flag, 1, MPI_INT, COMM_WORLD_MASTER_PROC_RANK, MPI_COMM_WORLD);

		if(MyNewRank==MASTER_PROC_RANK){
			offsetCommMasterSenRsv(&MyOffsetComm, offsetComm_size, MyNewRank, my_rank, min_max_flag, W, WindArr);
		}
		else{
			offsetCommSlaveSenRsv(&MyOffsetComm, offsetComm_size, MyNewRank, min_max_flag, W, WindArr);
		}
	}

    MPI_Finalize();
    return 0;
}
