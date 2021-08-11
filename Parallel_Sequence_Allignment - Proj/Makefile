# Makefile
# Created on: Jul 7, 2021
#     Author: Shahar Hikri

RUN_PROC_NUM = 18
RUNON2_PROC_NUM = 2

build: 
	mpicxx -fopenmp -c FinalProj.c -o FinalProj.o -O0 -g3 -Wall -c -fmessage-length=0
	
	mpicxx -fopenmp -c PharallelFuncs.c -o PharallelFuncs.o -O0 -g3 -Wall -c -fmessage-length=0
	
	mpicxx -fopenmp -c LogicFuncs.c -o LogicFuncs.o -O0 -g3 -Wall -c -fmessage-length=0
	
	mpicxx -fopenmp -c IOFilesFuncs.c -o IOFilesFuncs.o -O0 -g3 -Wall -c -fmessage-length=0
	
	nvcc -I./inc -c cudaFuncs.cu -o cudaFuncs.o
	
	mpicxx -fopenmp -o FinalProjProg FinalProj.o PharallelFuncs.o LogicFuncs.o IOFilesFuncs.o cudaFuncs.o  /usr/local/cuda-9.1/lib64/libcudart_static.a -ldl -lrt

clean:
	rm -f *.o *.gch ./FinalProjProg

run:
	mpiexec -np $(RUN_PROC_NUM) ./FinalProjProg input.txt output.txt

runOn2:
	mpiexec -np $(RUNON2_PROC_NUM) -machinefile mf -map-by node ./FinalProjProg input.txt output.txt
