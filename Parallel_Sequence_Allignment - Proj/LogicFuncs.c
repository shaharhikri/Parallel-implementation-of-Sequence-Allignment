/*
 * LogicFuncs.c
 *  Created on: Jul 7, 2021
 *      Author: Shahar Hikri
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FinalProj.h"

/* Sort WintArr (array of indexes - before sorting: WintArr =  {0,1,2,3} )
   by the order of sorted W (but doesn't sort W!).
   
   if min_max_flag is MAX_FLAG 		  - descending
   else (if min_max_flag is MIN_FLAG) - ascending
   
   for example: W = {1,-3,-2,-4} and min_max_flag=MAX_FLAG
				-> sorted W will be {1,-2,-3,-4} and WindArr after sort will be {0,2,1,3}
*/
void sortWindArr(double W[W_LEN],int WindArr[W_LEN], int min_max_flag){
	int i,j, temp2;
	double temp1;
	double Wclone[W_LEN] = {W[0],W[1],W[2],W[3]};

	/*sort WindArr*/
	for (i = 0; i < W_LEN-1; i++){
		for ( j = 0; j < W_LEN-i-1; j++){
			if ((min_max_flag==MAX_FLAG && Wclone[j] < Wclone[j+1])
							|| (min_max_flag==MIN_FLAG && Wclone[j] > Wclone[j+1])){
				temp1 = Wclone[j+1];
				Wclone[j+1] = Wclone[j];
				Wclone[j] = temp1;

				temp2 = WindArr[j+1];
				WindArr[j+1] = WindArr[j];
				WindArr[j] = temp2;
			}
		}
	}
}

/* Divides the (non comm world master) processes into groups (rank's arrays) - evenly and according to the number of possible offsets.
	For example: for np=8, and max_n(largest offset) =2 (possible offsets 0 1 2 - 3 possible offsets)
				 it returns { {1,2,3}, {4,5,6}, {7,8} }
  */
int** getRanksArr(int max_n, int np, int* ranksArrays_len_p, int** rankArraysElementsLens_p){

	int np_without_master = np-1; //without master
	int offsets_num;
	if(max_n>0) 	//'commWorldMasterSenRsv' func using cuda
		offsets_num = max_n+1;
	else		//'commWorldMasterSenRsv' func doesn't using cuda (there's only offset 0 that calcultaed by MPI&OMP)
		offsets_num = 1;

	int group_base_size = np_without_master/offsets_num; //before handing out the rest to everyone
	int rest = np_without_master%offsets_num;
	int ranksArrays_len = np_without_master<offsets_num?np_without_master:offsets_num;

	int** ranksArrays = (int**)malloc( sizeof(int*)*ranksArrays_len );
	int* rankArraysElementsLens = (int*)malloc( sizeof(int)*ranksArrays_len );

	int cur_group_size;
	int i,j;
	int rank = 1;
	for(i=0;i<ranksArrays_len;i++){
		cur_group_size = group_base_size;
		if(rest>0){
			cur_group_size++;
			rest--;
		}
		rankArraysElementsLens[i] = cur_group_size;
		ranksArrays[i] = (int*)malloc( sizeof(int)*(cur_group_size) );
		for(j=0;j<cur_group_size;j++){
			ranksArrays[i][j] = rank;
			rank++;
		}
	}

	*rankArraysElementsLens_p = rankArraysElementsLens;
	*ranksArrays_len_p = ranksArrays_len;

	return ranksArrays;
}
