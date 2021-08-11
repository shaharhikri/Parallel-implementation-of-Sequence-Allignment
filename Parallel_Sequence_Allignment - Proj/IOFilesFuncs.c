/*
 * IOFilesFuncs.c
 *  Created on: Jul 13, 2021
 *      Author: Shahar Hikri
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FinalProj.h"

/* gets input from input file (input.txt by definition)*/
bool readFromFIle(char* filename, double W[W_LEN], char* seq1, char* seq2,int* min_max_flag_p) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		perror("input file error: Error in file open\n");
		return false;
	}

	char min_max_str[10];
	int i;
	float input;
	for (i = 0; i < W_LEN; i++){
		if (!fscanf(f, "%f", &input)) {
			perror("input file error: not enough weights in input file! (need 4 weights) \n");
			fclose(f);
			return false;
		}
		W[i] = (double)input;
		if(i>0)
			W[i] = -W[i];
	}
	if (!fscanf(f, "%s", seq1) || !strcmp(seq1,"")) {
		perror("input file error: there's no sequences nor minimum/maximum string in input file! \n");
		fclose(f);
		return false;
	}
	if (!fscanf(f, "%s", seq2) || !strcmp(seq2,"")) {
		perror("input file error: there's no seq2 nor minimum/maximum string in input file! \n");
		fclose(f);
		return false;
	}
	if (!fscanf(f, "%s", min_max_str) || !strcmp(min_max_str,"")) {
		perror("input file error: there's no minimum/maximum string in input file! \n");
		fclose(f);
		return false;
	}

	if(strcmp(min_max_str,"maximum")==0)
		*min_max_flag_p = MAX_FLAG;
	else if(strcmp(min_max_str,"minimum")==0)
		*min_max_flag_p = MIN_FLAG;
	else{
		*min_max_flag_p = WRONG_MIN_MAX_FLAG;
		perror("input file error: wrong minimum/maximum string in input file!! \n");
		fclose(f);
		return false;
	}

	fclose(f);
	return true;
}


/* print output to output file (output.txt by definition)*/
bool writeToFIle(char* filename, char* seq2_mutant, int n,double score){
	FILE *f = fopen(filename, "w");
	if (!f) {
		perror("output file error: Error in file open\n");
		return false;
	}
	fprintf(f,"%s\n%d\n%.3f",seq2_mutant,n,(float)score);

	fclose(f);
	return true;
}
