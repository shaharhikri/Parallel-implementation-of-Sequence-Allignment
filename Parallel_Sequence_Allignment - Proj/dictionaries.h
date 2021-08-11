/*
 * dictionaries.h
 *  Created on: Jul 7, 2021
 *      Author: Shahar Hikri
 */

#ifndef DICT_H_
#define DICT_H_

/*dictionaries - for example: Con_dict['Q'-'A'] will give all the chars from 'A' to 'Z'
                            that share conservative group with 'Q' (excluding 'A')*/
#define LETTERS_NUM 26

/*device(gpu) dictionaries*/
__device__ 
const char* Con_dict_d[LETTERS_NUM] = 
{"ST"  ,	//A
""  ,		//B
""  ,		//C
"ENQ"  ,	//D
"DKNQ"  ,	//E
"ILMWY"  ,	//F
""  ,		//G
"KNQRY"  ,	//H
"FLMV"  ,	//I
""  ,		//J
"EHNQR"  ,	//K
"FIMV"  ,	//L
"FILV"  ,	//M
"DEHKQ"  ,	//N
""  ,		//O
""  ,		//P
"DEHKNR"  ,	//Q
"HKQ"  ,	//R
"AT"  ,		//S
"AS"  ,		//T
""  ,
"ILM"  ,
"FY"  ,
""  ,
"FHW"  ,
""  		}; //Z

__device__ 
const char* notConAndNotSemi_dict_d[LETTERS_NUM] = 
{"BDEFHIJKLMNOQRUWXYZ"  ,			//A
"ACDEFGHIJKLMNOPQRSTUVWXYZ"  ,		//B
"BDEFGHIJKLMNOPQRTUVWXYZ"  ,		//C
"ABCFIJLMOPRTUVWXYZ"  ,				//D
"ABCFGIJLMOPTUVWXYZ"  ,				//E
"ABCDEGJKNOPQRSTUXZ"  ,				//F
"BCEFHIJKLMOPQRTUVWXYZ"  ,			//G
"ABCGIJLMOPSTUVWXZ"  ,				//H
"ABCDEGHJKNOPQRSTUWXYZ"  ,			//I
"ABCDEFGHIKLMNOPQRSTUVWXYZ"  ,		//J
"ABCFGIJLMOPUVWXYZ"  ,				//K
"ABCDEGHJKNOPQRSTUWXYZ"  ,			//L
"ABCDEGHJKNOPQRSTUWXYZ"  ,			//M
"ABCFIJLMOPUVWXYZ"  ,				//N
"ABCDEFGHIJKLMNPQRSTUVWXYZ"  ,		//O
"BCDEFGHIJKLMNOQRUVWXYZ"  ,			//P
"ABCFGIJLMOPTUVWXYZ"  ,				//Q
"ABCDFGIJLMOPSTUVWXYZ"  ,			//R
"BFHIJLMORUVWXYZ"  ,				//S
"BCDEFGHIJLMOQRUWXYZ"  ,			//T
"ABCDEFGHIJKLMNOPQRSTVWXYZ"  ,
"BCDEGHJKNOPQRSUWXYZ"  ,
"ABCDEGHIJKLMNOPQRSTUVXZ"  ,
"ABCDEFGHIJKLMNOPQRSTUVWYZ"  ,
"ABCDEGIJKLMNOPQRSTUVXZ"  ,
"ABCDEFGHIJKLMNOPQRSTUVWXY"  		//Z
};

__device__ 
const char* SemiAndNotCon_dict_d[LETTERS_NUM] =  
{"CGPV"  ,		//A
""  ,			//B
"AS"  ,			//C
"GHKS"  ,		//D
"HRS"  ,		//E
"HV"  ,			//F
"ADNS"  ,		//G
"DEF"  ,		//H
""  ,			//I
""  ,			//J
"DST"  ,		//K
""  ,			//L
""  ,			//M
"GRST"  ,		//N
""  ,			//O
"AST"  ,		//P
"S"  ,			//Q
"EN"  ,			//R
"CDEGKNPQ"  ,	//S
"KNPV"  ,		//T
""  ,
"AFT"  ,
""  ,
""  ,
""  ,
""  };			//Z

__device__ 
const char* ConAndNotSemi_dict_d[LETTERS_NUM] = {		""  ,	//A
""  ,	//B
""  ,	//C
""  ,	//D
""  ,	//E
"W"  ,	//F
""  ,	//G
""  ,	//H
""  ,	//I
""  ,	//J
""  ,	//K
""  ,	//L
""  ,	//M
""  ,	//N
""  ,	//O
""  ,	//P
""  ,	//Q
""  ,	//R
""  ,	//S
""  ,	//T
""  ,
""  ,
"FY"  ,	//W
""  ,	//X
"W"  ,	//Y
"" 	//Z
};

__device__ 
const char* ConAndSemi_dict_d[LETTERS_NUM] =  
{"ST"  ,
""  ,
""  ,
"ENQ"  ,
"DKNQ"  ,
"ILMY"  ,
""  ,
"KNQRY"  ,
"FLMV"  ,
""  ,
"EHNQR"  ,
"FIMV"  ,
"FILV"  ,
"DEHKQ"  ,
""  ,
""  ,
"DEHKNR"  ,
"HKQ"  ,
"AT"  ,
"AS"  ,
""  ,
"ILM"  ,
""  ,
""  ,
"FH"  ,
""  };

/*host(cpu) dictionaries*/
const char* Con_dict[LETTERS_NUM] = 
{"ST"  ,	//A
""  ,		//B
""  ,		//C
"ENQ"  ,	//D
"DKNQ"  ,	//E
"ILMWY"  ,	//F
""  ,		//G
"KNQRY"  ,	//H
"FLMV"  ,	//I
""  ,		//J
"EHNQR"  ,	//K
"FIMV"  ,	//L
"FILV"  ,	//M
"DEHKQ"  ,	//N
""  ,		//O
""  ,		//P
"DEHKNR"  ,	//Q
"HKQ"  ,	//R
"AT"  ,		//S
"AS"  ,		//T
""  ,
"ILM"  ,
"FY"  ,
""  ,
"FHW"  ,
""  		}; //Z

const char* notConAndNotSemi_dict[LETTERS_NUM] = 
{"BDEFHIJKLMNOQRUWXYZ"  ,			//A
"ACDEFGHIJKLMNOPQRSTUVWXYZ"  ,		//B
"BDEFGHIJKLMNOPQRTUVWXYZ"  ,		//C
"ABCFIJLMOPRTUVWXYZ"  ,				//D
"ABCFGIJLMOPTUVWXYZ"  ,				//E
"ABCDEGJKNOPQRSTUXZ"  ,				//F
"BCEFHIJKLMOPQRTUVWXYZ"  ,			//G
"ABCGIJLMOPSTUVWXZ"  ,				//H
"ABCDEGHJKNOPQRSTUWXYZ"  ,			//I
"ABCDEFGHIKLMNOPQRSTUVWXYZ"  ,		//J
"ABCFGIJLMOPUVWXYZ"  ,				//K
"ABCDEGHJKNOPQRSTUWXYZ"  ,			//L
"ABCDEGHJKNOPQRSTUWXYZ"  ,			//M
"ABCFIJLMOPUVWXYZ"  ,				//N
"ABCDEFGHIJKLMNPQRSTUVWXYZ"  ,		//O
"BCDEFGHIJKLMNOQRUVWXYZ"  ,			//P
"ABCFGIJLMOPTUVWXYZ"  ,				//Q
"ABCDFGIJLMOPSTUVWXYZ"  ,			//R
"BFHIJLMORUVWXYZ"  ,				//S
"BCDEFGHIJLMOQRUWXYZ"  ,			//T
"ABCDEFGHIJKLMNOPQRSTVWXYZ"  ,
"BCDEGHJKNOPQRSUWXYZ"  ,
"ABCDEGHIJKLMNOPQRSTUVXZ"  ,
"ABCDEFGHIJKLMNOPQRSTUVWYZ"  ,
"ABCDEGIJKLMNOPQRSTUVXZ"  ,
"ABCDEFGHIJKLMNOPQRSTUVWXY"  		//Z
};

const char* SemiAndNotCon_dict[LETTERS_NUM] =  
{"CGPV"  ,		//A
""  ,			//B
"AS"  ,			//C
"GHKS"  ,		//D
"HRS"  ,		//E
"HV"  ,			//F
"ADNS"  ,		//G
"DEF"  ,		//H
""  ,			//I
""  ,			//J
"DST"  ,		//K
""  ,			//L
""  ,			//M
"GRST"  ,		//N
""  ,			//O
"AST"  ,		//P
"S"  ,			//Q
"EN"  ,			//R
"CDEGKNPQ"  ,	//S
"KNPV"  ,		//T
""  ,
"AFT"  ,
""  ,
""  ,
""  ,
""  };			//Z

const char* ConAndNotSemi_dict[LETTERS_NUM] = {		""  ,	//A
""  ,	//B
""  ,	//C
""  ,	//D
""  ,	//E
"W"  ,	//F
""  ,	//G
""  ,	//H
""  ,	//I
""  ,	//J
""  ,	//K
""  ,	//L
""  ,	//M
""  ,	//N
""  ,	//O
""  ,	//P
""  ,	//Q
""  ,	//R
""  ,	//S
""  ,	//T
""  ,
""  ,
"FY"  ,	//W
""  ,	//X
"W"  ,	//Y
"" 	//Z
};

const char* ConAndSemi_dict[LETTERS_NUM] =  
{"ST"  ,
""  ,
""  ,
"ENQ"  ,
"DKNQ"  ,
"ILMY"  ,
""  ,
"KNQRY"  ,
"FLMV"  ,
""  ,
"EHNQR"  ,
"FIMV"  ,
"FILV"  ,
"DEHKQ"  ,
""  ,
""  ,
"DEHKNR"  ,
"HKQ"  ,
"AT"  ,
"AS"  ,
""  ,
"ILM"  ,
""  ,
""  ,
"FH"  ,
""  };
							
#endif
