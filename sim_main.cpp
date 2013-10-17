/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
|        WRAPPER PROGRAM: Emulating OS for MIPS I Processor (MIPS32 ABI)                         |
|           This program automates the process of generating a binary                            |
|           and loading the hex dump of that program into a verilog processor                    |
|           memory map.  This wrapper also acts as the clock generator for                       |
|           the processor.                                                                       |
|        Written by Dan Snyder                                                                   |
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
  
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h> 
#include <sys/socket.h>   
#include <sys/time.h>
#include <sys/times.h>
#include <sys/mman.h>
#include <sys/resource.h> 
#include <sys/ioctl.h> 
#include <sys/uio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <ulimit.h>
#include <dirent.h>
#include <ulimit.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <cstdio> 
#include <cmath>
#include <iostream>
#include <fstream>
#include <istream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <map>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <time.h>
#include "VMIPS.h"		//for access to verilog parent module
#include "VMIPS_MIPS.h"		//for access to verilog submodules
#include <verilated.h>		//verilator lib

using namespace std;

typedef map<int,int> INT_INT_MAP;
typedef map<int,string> INT_STRING_MAP;
INT_INT_MAP MAIN_MEMORY;
INT_INT_MAP HEAP_STATUS;
INT_STRING_MAP HEX_MAIN_MEMORY;
INT_STRING_MAP offset;
vector<int> ARGUMENT_VECTOR;
static int GSP = 0xf7021fc0;//for noio
//static int GSP = 0xf603dcf8+712;//forfact12
static int GRA = 0x1006a244;//for noio
static int GPC_START = 268435636-8;
static int HEAPSTART = 3993198592;
unsigned int MAINTIME = 0;
float IPC;
int INSTR_COUNT;
int BLOCKBASE;
int BLOCKNUM;
int LOAD_MEMORY;
int START_REG;
int NUMBER_OF_REGS;
int RF_FPRF_BOTH;
int ENABLE_TIMING;
int FILE_COUNTER;
int PC_START;
int CLOCK_COUNTER;
int FUNCTION_FLAG;
string FILE_ARG;
string LIBC_OPEN_ADDRESS;
string LIBC_READ_ADDRESS;
string EXIT_ADDRESS;
string MUNMAP_ADDRESS;
string GETEUID_ADDRESS;
string GETUID_ADDRESS;
string UNAME_ADDRESS;
string GETPID_ADDRESS;
string GETGID_ADDRESS;
string GETEGID_ADDRESS;
string LIBC_MALLOC_ADDRESS;
string CFREE_ADDRESS;
string FXSTAT64_ADDRESS;
string MMAP_ADDRESS;
string LIBC_WRITE_ADDRESS;

//DISPLAYS HEAP CONTENTS
void heapDump(){
	int temp = HEAPSTART+HEAP_STATUS.size();					//shift pointer to end of the allocated heap
	while ((unsigned)temp%4!=0) temp++;					//align pointer
	for(int i=temp; i>=HEAPSTART; i-=4) {					//start from top down, print heap contents
		if (((MAIN_MEMORY[i]+MAIN_MEMORY[i+1]+MAIN_MEMORY[i+2]+MAIN_MEMORY[i+3])!=0)) {
			printf("Heap:  0x%x",i);
			printf("(+%*u): ",4,i-HEAPSTART);
			printf("0x%s ",((HEX_MAIN_MEMORY[i+3])+(HEX_MAIN_MEMORY[i+2])+(HEX_MAIN_MEMORY[i+1])+(HEX_MAIN_MEMORY[i+0])).c_str());
			cout << HEAP_STATUS[i];
			cout << HEAP_STATUS[i+1];
			cout << HEAP_STATUS[i+2];
			cout << HEAP_STATUS[i+3] << endl;
		}
	}
}
//ALLOCATES HEAP BLOCKS WHEN MALLOC IS CALLED
void allocateHeapBlock(int addr, int size){
	while ((unsigned)addr%4!=0) addr++;					//align block
	BLOCKBASE=addr;								//return aligned block start address
	for(int i=addr; i<=addr+size-1; i++){
		HEAP_STATUS[i] = BLOCKNUM;		//set heap word state variable	
	}
}
//CLEAR HEAP BLOCKS WHEN MALLOC IS CALLED
void clearHeapBlock(int addr){
	int num = HEAP_STATUS[addr];						//store the block # to be cleared
	for(int i=addr; i<=HEAPSTART+HEAP_STATUS.size(); i++) {			//iterate through memory resetting any states that match the block number
		if (HEAP_STATUS[i]==num)HEAP_STATUS[i] = 0;				//reset state
		else break;								//if the end of block is found break
	}
}
//DETERMINES ELF FILE SEGMENT OFFSETS BY READING ELF HEADER FILE
void getSegmentOffsets(string str, string str2){
  ifstream readFile1( str.c_str() );
  string word;
  int flag = 0;
  vector<string> words;
  if ( !readFile1 ) cerr << "File couldn't be opened" << endl;
  if (readFile1.is_open()) 
  {
	  while (!readFile1.eof() ) 
	  {
		  getline (readFile1,word);
		  if( word.find("[19]")!=string::npos){offset[19] = word.substr(52,4);}
		  else if( word.find("[20]")!=string::npos){offset[20] = word.substr(52,4);}
		  else if( word.find("[21]")!=string::npos){offset[21] = word.substr(52,4);}
		  else if( word.find("[22]")!=string::npos){offset[22] = word.substr(52,4);}
		  else if( word.find("[23]")!=string::npos){offset[23] = word.substr(52,4);}
		  else if( word.find("[24]")!=string::npos){offset[24] = word.substr(52,4);}
		  else if( word.find("[25]")!=string::npos){offset[25] = word.substr(52,4);}
		  else if( word.find("[26]")!=string::npos){offset[26] = word.substr(52,4);}
		  else if( word.find("[27]")!=string::npos){offset[27] = word.substr(52,4);}
		  else if( word.find("[29]")!=string::npos){offset[29] = word.substr(52,4);}
		  else if( word.find("[30]")!=string::npos){offset[30] = word.substr(52,4);}
		  else if( word.find("[31]")!=string::npos){offset[31] = word.substr(52,4);}
		  else if( word.find("[32]")!=string::npos){offset[32] = word.substr(52,4);}
		  else if( word.find("[33]")!=string::npos){offset[33] = word.substr(52,4);}
	  }
  }	
  readFile1.close();
  int counter=0;
  for(int i=0; i<=33; i++)
    {
      stringstream s;
      s<<i;
      string a = str2.substr(0,6)+ "_" + s.str() + ".txt";
      string newline = "\n";
      ifstream readFile2(a.c_str() );
      if (readFile2.is_open()) 
	{
	  while (!readFile2.eof() ) 
	    {
	      getline (readFile2,word);
	      if ( word.find("has no data to dump.")!=string::npos ) remove(a.c_str());
	      else if(word.find("Hex dump of section")==string::npos)
		{
		  if (counter == 1)
		    {
		      if( i>=19 && i!=28 && i!=29 )
			{
			  word.erase(0,12);
			  word.insert(0,"  0x0000"+offset[i]);
			}
		    }
		  words.push_back(word);
		  counter++;
		}
	    }
	  counter=0;
	  readFile2.close();
	  remove(a.c_str());
	  ofstream writeFile1(a.c_str());
	  for(int j=0; j<words.size(); j++)
	    {
	      writeFile1.write(words[j].c_str(),words[j].size());
	      writeFile1.write(newline.c_str(),newline.size());
	    }
	  writeFile1.close();
	  words.clear();
	}
    }
}
//BYPASSES NATIVE FUNCTIONS BY DETERMINING MEMORY LOCATION IN OBJDUMP OF EACH FUNCTION CALL
void functionBypass(string str){
	ifstream readFile1( str.c_str() );					//open the elf header file
	string word;								//variable for streaming file
	int flag = 0;								//prevents multiple tag detection
	vector<string> words;							
	if ( !readFile1 ) cerr << "File couldn't be opened" << endl;		//error if the file doesn't exist
	if (readFile1.is_open()) { 						//traverse file
		while (!readFile1.eof() ) { 						//until the end of the file is reached...
			getline (readFile1,word);						//look for tag names and store them if found
			if ( flag == 1 ) flag = 0;
			if( word.find("getgid")!=string::npos){ GETGID_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("getegid")!=string::npos){ GETEGID_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("getpid")!=string::npos){ GETPID_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("geteuid")!=string::npos){ GETEUID_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("uname")!=string::npos){ UNAME_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("getuid")!=string::npos){ GETUID_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<__libc_malloc>")!=string::npos){ LIBC_MALLOC_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<__cfree>")!=string::npos){ CFREE_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<___fxstat64>")!=string::npos){ FXSTAT64_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<__mmap>")!=string::npos){ MMAP_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<__libc_write>")!=string::npos){ LIBC_WRITE_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<__munmap>")!=string::npos){ MUNMAP_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<_exit>")!=string::npos){ EXIT_ADDRESS=word.substr(0,8); flag = 1;}
			else if( word.find("<__libc_read>")!=string::npos){ LIBC_READ_ADDRESS=word.substr(0,8); flag = 1;}	
			else if( word.find("<__libc_open>")!=string::npos){ LIBC_OPEN_ADDRESS=word.substr(0,8); flag = 1;}	
		}
    	}
	readFile1.close();
}
//CONVERTS INTEGER TO BINARY CHARACTERS
char *itob(int x){
  static char buff[sizeof(int) * CHAR_BIT + 1];
  int i;
  int j = sizeof(int) * CHAR_BIT - 1;
  buff[j] = 0;
  for(i=0;i<sizeof(int) * CHAR_BIT; i++)
    {
      if(x & (1 << i)) buff[j] = '1';
      else buff[j] = '0';
      j--;
    }
  return buff;
}
//CONVERTS HEX CHARACTERS TO INTEGERS
static inline int hexCharValue(char ch){
  if (ch>='0' && ch<='9')return ch-'0';
  if (ch>='a' && ch<='f')return ch-'a'+10;
  return 0;
}
//STORES VALUE IN MEMORY (STRING ARGUMENT)
void loadSingleHEX(string newValue, int location, int comment, int bh_word){
	switch (bh_word) {
		case 0:{												//store word	
			HEX_MAIN_MEMORY[location+0] = newValue.substr(0,2);							//msb
			HEX_MAIN_MEMORY[location+1] = newValue.substr(2,2);
			HEX_MAIN_MEMORY[location+2] = newValue.substr(4,2);
			HEX_MAIN_MEMORY[location+3] = newValue.substr(6,2);							//lsb
			MAIN_MEMORY[location+0] = ((hexCharValue(newValue[1])) + (hexCharValue(newValue[0])<<4));		//msb
			MAIN_MEMORY[location+1] = ((hexCharValue(newValue[3])) + (hexCharValue(newValue[2])<<4));
			MAIN_MEMORY[location+2] = ((hexCharValue(newValue[5])) + (hexCharValue(newValue[4])<<4));
			MAIN_MEMORY[location+3] = ((hexCharValue(newValue[7])) + (hexCharValue(newValue[6])<<4));		//lsb
			break;}
		case 1:{												//store byte
			HEX_MAIN_MEMORY[location] = newValue.substr(0,2);
			MAIN_MEMORY[location] = ((hexCharValue(newValue[1])) + (hexCharValue(newValue[0])<<4));
			break;}
		case 2:{												//store halfword
			HEX_MAIN_MEMORY[location]   = newValue.substr(0,2);							//msB
			HEX_MAIN_MEMORY[location+1] = newValue.substr(2,2);							//lsB
			MAIN_MEMORY[location]   = (hexCharValue(newValue[0]) + hexCharValue(newValue[1])<<4);			//msB
			MAIN_MEMORY[location+1] = (hexCharValue(newValue[3]) + hexCharValue(newValue[2])<<4);			//lsB
			break;}
		default:{break;}
	}
} 
//STORES VALUE IN MEMORY (INTEGER ARGUMENT)
void loadSingleHEX(int newValue, int location, int comment, int bh_word){
	string newBinValue = itob(newValue);										//convert integer value to its (string) binary equivalent
	stringstream s;													//for binary conversion
	switch (bh_word) {
		case 0:	{												//store word	
			stringstream temp;
			string binary_str(newBinValue.substr(0,32));								//convert binary string to bitset
			bitset<32> set(binary_str);										
			temp << hex << set.to_ulong();
			while((temp.str()).size() < 8) {									//every byte, dump contents into stream
				s << "0";
				temp << "0";
			}
			s << hex << set.to_ulong();										//convert set 
			HEX_MAIN_MEMORY[ location + 3 ] = s.str().substr(6,2);							//lsb
			HEX_MAIN_MEMORY[ location + 2 ] = s.str().substr(4,2);
			HEX_MAIN_MEMORY[ location + 1 ] = s.str().substr(2,2);
			HEX_MAIN_MEMORY[ location + 0 ] = s.str().substr(0,2);							//msb
			MAIN_MEMORY[    location + 3 ] = (hexCharValue((s.str().substr(6,2))[0])<<4) + (hexCharValue((s.str().substr(6,2))[1]));//lsb
			MAIN_MEMORY[    location + 2 ] = (hexCharValue((s.str().substr(4,2))[0])<<4) + (hexCharValue((s.str().substr(4,2))[1]));
			MAIN_MEMORY[    location + 1 ] = (hexCharValue((s.str().substr(2,2))[0])<<4) + (hexCharValue((s.str().substr(2,2))[1]));
			MAIN_MEMORY[    location + 0 ] = (hexCharValue((s.str().substr(0,2))[0])<<4) + (hexCharValue((s.str().substr(0,2))[1]));//msb
			break;}
		case 1: {												//store byte
			stringstream temp;
			string binary_str(newBinValue.substr(24,8));
			bitset<8> set(binary_str);
			temp << hex << set.to_ulong();
			while((temp.str()).size() < 2) {
				s << "0";
				temp << "0";
			}
			s << hex << set.to_ulong();
			HEX_MAIN_MEMORY[location] = s.str();
			MAIN_MEMORY[location] = (hexCharValue((s.str())[0])<<4) + (hexCharValue((s.str())[1]));
			break;
		}
		case 2: {												//store halfword
			stringstream temp;
			string binary_str(newBinValue.substr(16,16));
			bitset<16> set(binary_str);
			temp << hex << set.to_ulong();
			while((temp.str()).size() < 4) {
				s << "0";
				temp << "0";
			}
			s << hex << set.to_ulong();
			HEX_MAIN_MEMORY[location+1] = s.str().substr(0,2);							//msB
			HEX_MAIN_MEMORY[location] = s.str().substr(2,2);								//lsB
			MAIN_MEMORY[location+1] = (hexCharValue((s.str())[0])<<4) + (hexCharValue((s.str())[1]));		//msb
			MAIN_MEMORY[location] = (hexCharValue((s.str())[2])<<4) + (hexCharValue((s.str())[3]));			//lsb
			break;
		}
		default:{break;}
	}
	s.str("");
}
//ELF LOADER
void LoadMemory(string str){
	FILE_COUNTER++;
	vector<int> V;										//temperary vector
	vector<string> tempV;									//temperary vector
	vector<string> tempVect;
	vector<string> words;
	string word;
	int offset=0;
	ifstream getFile( str.c_str(),ios::in ); 						//open the file and cut out anything unwanted if neccessary
		if (getFile.is_open()) 
		{
			while (!getFile.eof() ) 
			{
				getline (getFile,word);
				if(word.find("Hex")==string::npos) tempVect.push_back(word.substr(0,48));
			}
		}
		getFile.close();
		ofstream putFile( str.c_str(),ios::trunc );						//reopen the file to be written to (truncating old contents)
		for(int f=0; f<tempVect.size(); f++) putFile << tempVect[f] << endl;
		putFile.close();
	
	//open the file to be read into memory
	ifstream inClientFile( str.c_str(),ios::in );
		if ( !inClientFile ) cerr << "File couldn't be opened" << endl;			//test if instruction file can be opened
		while (inClientFile >> word){words.push_back(word);}				//capture raw code from file
		const int wordCount=words.size();						//determine most efficient sizing for vectors
		tempV.reserve(wordCount);							//size vector
		for(int i=0; i<wordCount; i++) {	
			if (i==0 && words[i].length()==10){ tempV.push_back(words[i]);}		//include first word to obtain data offset (memory insertion point)
			if (words[i].length()==8 && words[i].find(".")==string::npos && words[i].find(".")==string::npos ){ tempV.push_back(words[i]);}//cut out undesired strings from vector
		}
		for( int y=2; y<10; y++) offset+=hexCharValue(tempV[0][y])<<(4*(9-y));		//convert offset from hex to decimal
		tempV.erase(tempV.begin());							//delete offset from vector
		V.resize(tempV.size());								//resize vector
		for( int j=0; j<tempV.size(); j++ ) {						//convert string hex to numerical decimal
			for( int y=0; y<8; y++) V[j]+=hexCharValue(tempV[j][y])<<(4*(7-y)); 	//convert hex into int
			if (LOAD_MEMORY) loadSingleHEX(tempV[j],4*j+offset,0,0); 		//insert element into memory
		}
		if( FILE_COUNTER == 1 ) PC_START = offset-4;
}
void uname(int sp){
	/*insert into stack...
		"SescLinux"
		"sesc"
		"2.4.18"
		"#1 SMP Tue Jun 4 16:05:29 CDT 2002"
		"mips"*/
	
	loadSingleHEX("6d697073",sp +348,0,0);
	loadSingleHEX("32000000",sp +316,0,0);
	loadSingleHEX("20323030",sp +312,0,0);
	loadSingleHEX("20434454",sp +308,0,0);
	loadSingleHEX("353a3239",sp +304,0,0);
	loadSingleHEX("31363a30",sp +300,0,0);
	loadSingleHEX("6e203420",sp +296,0,0);
	loadSingleHEX("65204a75",sp +292,0,0);
	loadSingleHEX("50205475",sp +288,0,0);
	loadSingleHEX("3120534d",sp +284,0,0);
	loadSingleHEX("00000023",sp +280,0,0);
	loadSingleHEX("342e3138",sp +220,0,0);
	loadSingleHEX("0000322e",sp +216,0,0);
	loadSingleHEX("63000000",sp +156,0,0);
	loadSingleHEX("00736573",sp +152,0,0);
	loadSingleHEX("78000000",sp +96,0,0);
	loadSingleHEX("4c696e75",sp +92,0,0);
	loadSingleHEX("53657363",sp +88,0,0);
}
void getArguments(string str){

	//for noio
	ARGUMENT_VECTOR.push_back(1);//+0
	ARGUMENT_VECTOR.push_back(0);//+4
	ARGUMENT_VECTOR.push_back(0xf7021fd4);
	ARGUMENT_VECTOR.push_back(0);//+12
	ARGUMENT_VECTOR.push_back(0);//+16
	ARGUMENT_VECTOR.push_back(0x6e6f696f);/**/

	//for hello world
	/*ARGUMENT_VECTOR.push_back(1);//+0
	ARGUMENT_VECTOR.push_back(0);//+4
	ARGUMENT_VECTOR.push_back(0xf603DFD4);//+8
	ARGUMENT_VECTOR.push_back(0);//+12
	ARGUMENT_VECTOR.push_back(0);//+16
	ARGUMENT_VECTOR.push_back(0x66616374);//+20
	ARGUMENT_VECTOR.push_back(0x31320000);//+24/**/

	string temp = str.substr(0,str.find(".txt"));
	vector<string> local_argument_vector;
	int flag=0;
	for(int i=0; i<local_argument_vector.size(); i++) {
		//convert first 4 characters = 4 bytes or one word
		int tempInt=0;
		//convert word to integer
		for(int j=0; j<local_argument_vector[i].size(); j++) {
			string c = local_argument_vector[i].substr(j,1);
			char *cs = new char[c.size() + 1];
			std::strcpy ( cs, c.c_str() );
			char a = *cs; 
			int as = a;
			tempInt = (tempInt + (as<<(24-j*8)));
		}
		ARGUMENT_VECTOR.push_back(tempInt);
	}
	for(int i=0; i<=ARGUMENT_VECTOR.size()-1; i++) loadSingleHEX(ARGUMENT_VECTOR[i],GSP+(i*4),0,0);
}
void fxstat64(int sp)
{
	loadSingleHEX("00000009",sp +32,0,0);
	loadSingleHEX("00000000",sp +48,0,0);
	loadSingleHEX("00000002",sp +52,0,0);
	loadSingleHEX("00002190",sp +56,0,0);
	loadSingleHEX("00000001",sp +60,0,0);
	loadSingleHEX("00001fb3",sp +64,0,0);
	loadSingleHEX("00000005",sp +68,0,0);
	loadSingleHEX("00008800",sp +72,0,0);
	loadSingleHEX("00000000",sp +88,0,0);
	loadSingleHEX("00000000",sp +92,0,0);
	loadSingleHEX("00000400",sp +120,0,0);
	loadSingleHEX("00000000",sp +128,0,0);
	loadSingleHEX("00000000",sp +132,0,0);	
}
/************************************/
/*********** MAIN PROGRAM ***********/
/************************************/
int main(int argc, char **argv)
{	
	Verilated::commandArgs(argc, argv);
	ofstream memWrite ("memoryWrites.txt");
	VMIPS *top = new VMIPS;
	vector<string> FDT_filename;
	vector<int> FDT_state;//1 = open, 0 = closed 
	time_t seconds;
	stringstream temps;
	stringstream s;
	int FileDescriptorIndex=3;//start of non-reserved indexes
	int duration = 1000000000;
	int sysFunc=0;
	int sysFunc_Ex=0;
	int fakeSyscal=0;
	int fakeSyscal_Ex=0;
	int temp_address=0;
	int syscallIndex=0;
	int instruction=0;
	int source=0;
	int immediate=0;
	int base=0;
	int rt=0;
	int SWC_tmp=0;

	top->CLK = 0;
	//first 3 positions reserved for stdin, stdout, and stderr    
	FDT_filename.push_back("stdin");FDT_state.push_back(0);		//reserve fildes 0 for stdin
	FDT_filename.push_back("stdout");FDT_state.push_back(0);	//reserve fildes 1 for stdout
	FDT_filename.push_back("stderr");FDT_state.push_back(0);	//reserve fildes 2 for stderr
	if(argc>3 || argc <2) { 
		cout << "USAGE: VMIPS [APP_NAME] <DURATION>" << endl; 
		exit(1);
	}
	FILE_ARG = argv[1];	
	temps<<argv[2];
	if(argc==3) {
		temps>>duration;
		temps.str("");
	}
	cout << "		*** ELF LOADING, PLEASE WAIT ***\n";	
//////////////////////////////////////////////////////////////////////////////////////////////
	top->Instr1_fIM = 0;
	top->Instr2_fIM = 0;
	LOAD_MEMORY = 1;
	RF_FPRF_BOTH = 2;
	START_REG = 0;
	NUMBER_OF_REGS = 32;
	ENABLE_TIMING = 1;
//////////////////////////////////////////////////////////////////////////////////////////////
	if ( LOAD_MEMORY ) {
		getArguments("app_obj/"+FILE_ARG+".txt");								//captures arguments for initializing stack
		getSegmentOffsets("app_obj/readelf"+FILE_ARG+".txt",FILE_ARG+".txt");				//determines offset of elf segments
		functionBypass("app_obj/"+FILE_ARG+".txt");								//captures function addressing
		LoadMemory("app_obj/"+FILE_ARG+"_1.txt");								//reginfo
		LoadMemory("app_obj/"+FILE_ARG+"_2.txt");								//init
		LoadMemory("app_obj/"+FILE_ARG+"_3.txt");								//text
		LoadMemory("app_obj/"+FILE_ARG+"_4.txt");								//__libc_freeres_fn
		LoadMemory("app_obj/"+FILE_ARG+"_5.txt");								//fini
		LoadMemory("app_obj/"+FILE_ARG+"_6.txt");								//rodata
		LoadMemory("app_obj/"+FILE_ARG+"_7.txt");								//data
		LoadMemory("app_obj/"+FILE_ARG+"_8.txt");								//__libc_subfreeres
		LoadMemory("app_obj/"+FILE_ARG+"_9.txt");								//__libc_atexit
		LoadMemory("app_obj/"+FILE_ARG+"_10.txt");								//eh_frame
		LoadMemory("app_obj/"+FILE_ARG+"_11.txt");								//gcc_except_table
		LoadMemory("app_obj/"+FILE_ARG+"_12.txt");								//ctors
		LoadMemory("app_obj/"+FILE_ARG+"_13.txt");								//dtors
		LoadMemory("app_obj/"+FILE_ARG+"_14.txt");								//jcr
		if (FILE_ARG.find("noio")==string::npos)LoadMemory("app_obj/"+FILE_ARG+"_15.txt");		//got
		LoadMemory("app_obj/"+FILE_ARG+"_19.txt");								//comment
		LoadMemory("app_obj/"+FILE_ARG+"_20.txt");								//debug_aranges
		LoadMemory("app_obj/"+FILE_ARG+"_21.txt");								//debug_pubnames
		LoadMemory("app_obj/"+FILE_ARG+"_22.txt");								//debug_info
		LoadMemory("app_obj/"+FILE_ARG+"_23.txt");								//debug_abbrev
		LoadMemory("app_obj/"+FILE_ARG+"_24.txt");								//debug_line
		if (FILE_ARG.find("noio")==string::npos)LoadMemory("app_obj/"+FILE_ARG+"_25.txt");		//debug_frame
		LoadMemory("app_obj/"+FILE_ARG+"_26.txt");								//debug_str
		LoadMemory("app_obj/"+FILE_ARG+"_27.txt");								//pdr
		LoadMemory("app_obj/"+FILE_ARG+"_28.txt");								//note.ABI-tag
		if (FILE_ARG.find("noio")==string::npos)LoadMemory("app_obj/"+FILE_ARG+"_30.txt");		//debug_ranges
		if (FILE_ARG.find("noio")==string::npos)LoadMemory("app_obj/"+FILE_ARG+"_31.txt");		//shstrtab
		if (FILE_ARG.find("noio")==string::npos)LoadMemory("app_obj/"+FILE_ARG+"_32.txt");		//symtab
		top->v->Reg_ID[29] = GSP;					//stack pointer
		top->v->Reg_ID[31] = GRA;					//return address
		top->PC_init = GPC_START;
	}
	/*------------------------------------------------------------------------------------------------  
	|        This section contains									 |
	|	    -MIPS object and all of the run-time displays			   		 |
	|           -Syscall interface									 |
	|           -Low level test interface (load instructions manually)				 |
	------------------------------------------------------------------------------------------------*/ 
	cout << "		      *** PROGRAM EXECUTING ***\n";
	seconds  = time (NULL);
	top->RESET = 1;
	top->eval();
	top->RESET = 0;
	top->eval();
	top->RESET = 1;
	top->eval();
	while (!Verilated::gotFinish()){
		top->CLK=!(top->CLK);									//generate a clock that pulses on eval()
		MAINTIME++;										//increment time	
		s.str("");									//for instruction processing
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////fakeSyscal_EX///////
		if(top->iBlkRead) {
			temp_address = top->v->Instr_address_2IM & 0xFFFFFFE0;
			top->block_read_fIM[7] = ((MAIN_MEMORY[temp_address+ 0]<<24)+(MAIN_MEMORY[temp_address+ 1]<<16)+(MAIN_MEMORY[temp_address+ 2]<<8)+(MAIN_MEMORY[temp_address+ 3]<<0));
			top->block_read_fIM[6] = ((MAIN_MEMORY[temp_address+ 4]<<24)+(MAIN_MEMORY[temp_address+ 5]<<16)+(MAIN_MEMORY[temp_address+ 6]<<8)+(MAIN_MEMORY[temp_address+ 7]<<0));
			top->block_read_fIM[5] = ((MAIN_MEMORY[temp_address+ 8]<<24)+(MAIN_MEMORY[temp_address+ 9]<<16)+(MAIN_MEMORY[temp_address+10]<<8)+(MAIN_MEMORY[temp_address+11]<<0));
			top->block_read_fIM[4] = ((MAIN_MEMORY[temp_address+12]<<24)+(MAIN_MEMORY[temp_address+13]<<16)+(MAIN_MEMORY[temp_address+14]<<8)+(MAIN_MEMORY[temp_address+15]<<0));
			top->block_read_fIM[3] = ((MAIN_MEMORY[temp_address+16]<<24)+(MAIN_MEMORY[temp_address+17]<<16)+(MAIN_MEMORY[temp_address+18]<<8)+(MAIN_MEMORY[temp_address+19]<<0));
			top->block_read_fIM[2] = ((MAIN_MEMORY[temp_address+20]<<24)+(MAIN_MEMORY[temp_address+21]<<16)+(MAIN_MEMORY[temp_address+22]<<8)+(MAIN_MEMORY[temp_address+23]<<0));
			top->block_read_fIM[1] = ((MAIN_MEMORY[temp_address+24]<<24)+(MAIN_MEMORY[temp_address+25]<<16)+(MAIN_MEMORY[temp_address+26]<<8)+(MAIN_MEMORY[temp_address+27]<<0));
			top->block_read_fIM[0] = ((MAIN_MEMORY[temp_address+28]<<24)+(MAIN_MEMORY[temp_address+29]<<16)+(MAIN_MEMORY[temp_address+30]<<8)+(MAIN_MEMORY[temp_address+31]<<0));
			if(MAINTIME%2==0){
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////fakeSyscal_EX///////
		if(top->dBlkRead) {
			temp_address = top->data_address_2DM & 0xFFFFFFE0;
			top->block_read_fDM[7] = ((MAIN_MEMORY[temp_address+ 0]<<24)+(MAIN_MEMORY[temp_address+ 1]<<16)+(MAIN_MEMORY[temp_address+ 2]<<8)+(MAIN_MEMORY[temp_address+ 3]<<0));
			top->block_read_fDM[6] = ((MAIN_MEMORY[temp_address+ 4]<<24)+(MAIN_MEMORY[temp_address+ 5]<<16)+(MAIN_MEMORY[temp_address+ 6]<<8)+(MAIN_MEMORY[temp_address+ 7]<<0));
			top->block_read_fDM[5] = ((MAIN_MEMORY[temp_address+ 8]<<24)+(MAIN_MEMORY[temp_address+ 9]<<16)+(MAIN_MEMORY[temp_address+10]<<8)+(MAIN_MEMORY[temp_address+11]<<0));
			top->block_read_fDM[4] = ((MAIN_MEMORY[temp_address+12]<<24)+(MAIN_MEMORY[temp_address+13]<<16)+(MAIN_MEMORY[temp_address+14]<<8)+(MAIN_MEMORY[temp_address+15]<<0));
			top->block_read_fDM[3] = ((MAIN_MEMORY[temp_address+16]<<24)+(MAIN_MEMORY[temp_address+17]<<16)+(MAIN_MEMORY[temp_address+18]<<8)+(MAIN_MEMORY[temp_address+19]<<0));
			top->block_read_fDM[2] = ((MAIN_MEMORY[temp_address+20]<<24)+(MAIN_MEMORY[temp_address+21]<<16)+(MAIN_MEMORY[temp_address+22]<<8)+(MAIN_MEMORY[temp_address+23]<<0));
			top->block_read_fDM[1] = ((MAIN_MEMORY[temp_address+24]<<24)+(MAIN_MEMORY[temp_address+25]<<16)+(MAIN_MEMORY[temp_address+26]<<8)+(MAIN_MEMORY[temp_address+27]<<0));
			top->block_read_fDM[0] = ((MAIN_MEMORY[temp_address+28]<<24)+(MAIN_MEMORY[temp_address+29]<<16)+(MAIN_MEMORY[temp_address+30]<<8)+(MAIN_MEMORY[temp_address+31]<<0));

		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(top->MemWrite) {												//write to memory
			if (top->v->Instr_fMEM>>26 == 40) loadSingleHEX(top->data_write_2DM,top->data_address_2DM,0,1);		//sb
			else if (top->v->Instr_fMEM>>26 == 41) loadSingleHEX(top->data_write_2DM,top->data_address_2DM,0,2);	//sh
			else loadSingleHEX(top->data_write_2DM,top->data_address_2DM,0,0);					//sw
			memWrite << CLOCK_COUNTER << endl;									//store memory access in file
			memWrite << top->data_write_2DM << " " << top->data_address_2DM << endl;				//store memory access in file
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		s << hex << top->v->Instr_address_2IM << endl;							//gets the next instruction ready for processing
		
		if(MAINTIME%2==0) {									//when the clock is positive do the following
			CLOCK_COUNTER ++;			
			/*------------------------------------------------------------------------------------------------  
			|				       DISPLAYS		  				          |
			------------------------------------------------------------------------------------------------*/ 
			if(CLOCK_COUNTER>=duration) {
				printf("#####################################################\nsp = 0x%x\n",top->v->Reg_ID[29]);
				cout << "-------------------------------------" << endl;
				cout << "*-------------------------------------" << endl;
				heapDump();
				cout << "*-------------------------------------";
				cout << HEX_MAIN_MEMORY[4127448456]<<HEX_MAIN_MEMORY[4127448456+1]<<HEX_MAIN_MEMORY[4127448456+2]<<HEX_MAIN_MEMORY[4127448456+3] << endl;
				printf("MemoryAddress:%x MemoryElement:%x",top->v->Instr_address_2IM,(MAIN_MEMORY[top->v->Instr_address_2IM+0]<<24) + (MAIN_MEMORY[top->v->Instr_address_2IM+1]<<16) + (MAIN_MEMORY[top->v->Instr_address_2IM+2]<<8) + (MAIN_MEMORY[top->v->Instr_address_2IM+3]));
				for (int j=START_REG; j < NUMBER_OF_REGS; j++) {
					cout<<endl;
					if ( ( top->v->Reg_ID[j] ) == 3735928559 ) printf("REG[%*d]:%*x   |   ",2,j,8,top->v->Reg_ID[j]);
					else if ( ( RF_FPRF_BOTH == 0 ) | ( RF_FPRF_BOTH == 2 ) ) printf("REG[%*d]:%*u %*x   |   ",2,j,10,top->v->Reg_ID[j],8,top->v->Reg_ID[j]);
				}
				printf("	Time:%d\n",CLOCK_COUNTER);				//displays current "time" (current cycle #)
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
			//function substitutions
			if (s.str().find(UNAME_ADDRESS)!=string::npos) {				//uname
				sysFunc = 4122;	
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(LIBC_MALLOC_ADDRESS)!=string::npos) {			//libc_malloc
				sysFunc = 4555;				
				FUNCTION_FLAG=4;
				fakeSyscal = 1;
			}
			else if (s.str().find(MMAP_ADDRESS)!=string::npos) {				//mmap
				sysFunc = 4090;				
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(CFREE_ADDRESS)!=string::npos) {				//cfree
				sysFunc = 4091;				
				FUNCTION_FLAG=4;
				fakeSyscal = 1;
			}
			else if (s.str().find(FXSTAT64_ADDRESS)!=string::npos) {			//fxstat64
				sysFunc = 4028;	
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(LIBC_WRITE_ADDRESS)!=string::npos) {			//libc_write
				sysFunc = 4004;	
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(MUNMAP_ADDRESS)!=string::npos) {				//munmap
				sysFunc = 4091;				
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(EXIT_ADDRESS)!=string::npos) {				//exit
				sysFunc=4001;
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(LIBC_READ_ADDRESS)!=string::npos) {			//libc_read
				sysFunc=4003;
				FUNCTION_FLAG=4;
			}
			else if (s.str().find(LIBC_OPEN_ADDRESS)!=string::npos) {			//libc_open
				sysFunc=4005;
				FUNCTION_FLAG=4;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			//function sub FSM
			if(top->v->FREEZE){
				;// FSM needs to freeze as well
			}
			else if (FUNCTION_FLAG == 2) {//nop
				top->Instr1_fIM = 0;
				top->v->Instr_read_fIC = 0;
				INSTR_COUNT++;
				FUNCTION_FLAG = 0;
			}
			else if (FUNCTION_FLAG == 5) {//nop
				top->Instr1_fIM = 0;
				top->v->Instr_read_fIC = 0;
				INSTR_COUNT++;
				if (top->v->SYS==1){
					top->Instr1_fIM = 65011720;//jr
					top->v->Instr_read_fIC = 65011720;
					FUNCTION_FLAG = 2;
				}
			}
			else if (FUNCTION_FLAG == 3) {//syscall
				top->Instr1_fIM = 12;
				top->v->Instr_read_fIC = 12;
				if(MAINTIME%2==0) CLOCK_COUNTER--;
				FUNCTION_FLAG=5;
			}
			else if (FUNCTION_FLAG == 4) {//addi (load arg register)
				top->v->Instr2_IFID = 0;
				top->Instr1_fIM = (fakeSyscal==1)? 0: 537001984 + sysFunc;
				top->v->Instr_read_fIC = (fakeSyscal==1)? 0: 537001984 + sysFunc;
				if(MAINTIME%2==0) CLOCK_COUNTER--;
				FUNCTION_FLAG = 3;
			}
			else {//normal instruction supply (no function call or special instruction call)	
				top->Instr1_fIM = (MAIN_MEMORY[top->v->Instr_address_2IM+3]) + (MAIN_MEMORY[top->v->Instr_address_2IM+2]<<8) + (MAIN_MEMORY[top->v->Instr_address_2IM+1]<<16) + (MAIN_MEMORY[top->v->Instr_address_2IM+0]<<24);
				top->Instr2_fIM = (MAIN_MEMORY[top->v->Instr_address_2IM+7]) + (MAIN_MEMORY[top->v->Instr_address_2IM+6]<<8) + (MAIN_MEMORY[top->v->Instr_address_2IM+5]<<16) + (MAIN_MEMORY[top->v->Instr_address_2IM+4]<<24);

				if(top->Instr2_fIM == 0xc0820000) {
					if(fakeSyscal){
						sysFunc_Ex    = 4556;
						fakeSyscal_Ex = 1;
					}
					else{
						sysFunc    = 4556;
						fakeSyscal = 1;
					}	
					top->Instr2_fIM = 0xc;
				}
				else if(top->Instr2_fIM == 0xe0820000) {
					if(fakeSyscal){
						sysFunc_Ex    = 4557;
						fakeSyscal_Ex = 1;
					}
					else{
						sysFunc    = 4557;
						fakeSyscal = 1;
					}	
					top->Instr2_fIM = 0xc;
					SWC_tmp=0x82;
				}
				else if (top->Instr2_fIM == 0xe0830000) {
					if(fakeSyscal){
						sysFunc_Ex    = 4557;
						fakeSyscal_Ex = 1;
					}
					else{
						sysFunc    = 4557;
						fakeSyscal = 1;
					}	
					top->Instr2_fIM = 0xc;
					SWC_tmp=0x83;
				}

				INSTR_COUNT+=2;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			

			/*------------------------------------------------------------------------------------------------  
			|						SYSCALLS													 |
			------------------------------------------------------------------------------------------------*/
			syscallIndex = (fakeSyscal==1)? sysFunc: ((fakeSyscal_Ex==1)? sysFunc_Ex: top->v->Reg_ID[2]);	//get syscall number from register 2
			if (top->v->Instr1_IFID==12 && top->v->SYS==1) {											//if a syscall is detected
				if(fakeSyscal==1)
					fakeSyscal = 0;
				else
					fakeSyscal_Ex = 0;

				switch (syscallIndex) {
					case 4001:{	cout << "Exit at time:" << CLOCK_COUNTER << endl;										//exit
						if(ENABLE_TIMING){
							seconds  = time(NULL) - seconds;
							cout << "*********************************" << endl;
							cout << "Simulation time : " << seconds << " sec" << endl;
							cout << "Total cycles: " << CLOCK_COUNTER << endl;
							cout << "Total instructions: " <<INSTR_COUNT << endl;
							IPC = (float)INSTR_COUNT/((float)CLOCK_COUNTER);
							cout << "IPC: " << IPC << endl;
						}
						 
						syscall(SYS_exit);
					break;}								
					case 4003:{cout << "ReadFile at time:" << CLOCK_COUNTER << endl;										//read
						string input1;
						string input;
						int addr,i;
						addr = top->v->Reg_ID[5];						//memory entry pointed to by argument
						if(top->v->Reg_ID[4]==0) cin >> input;					//if STDIN use stdio					
						else {												//otherwise must be a file
							ifstream indata(FDT_filename[top->v->Reg_ID[4]].c_str());	//stream in contents of file
							while(!indata.eof()){									//until eof
								getline (indata,input1);						
								input = input + input1;								//accumulate string
							}
						}
						if (input.size()>70)input.insert(70,"\n");							//syscall reads 70 chars at a time
						for (i=addr;i<=addr+input.size();i++) loadSingleHEX(input[i-addr],i,0,1);			//load content to memory
						loadSingleHEX("0a",i-1,0,1);									//end block with "0a"
						if (top->v->Reg_ID[4]==0) {						//if STDIN && open
							if (FDT_state[top->v->Reg_ID[4]]!=0){					//close file when done
								top->v->Reg_ID[2] = i-addr;									//return number of chars read
								FDT_state[top->v->Reg_ID[4]]=0;					//set state bit
							}else top->v->Reg_ID[2] = i-addr;									//if STDIN && closed
						}
						else {												//if fildes > 2 ( !(STD(IN,OUT,ERR) )
							if (FDT_state[top->v->Reg_ID[4]]!=0){					//close file when done
								top->v->Reg_ID[2] = i-addr;									//return number of chars read
								FDT_state[top->v->Reg_ID[4]]=0;					//set state bit
							}else top->v->Reg_ID[2] = 0;									//if fildes > 2 && closed
						}						
					break;}
					case 4004:{										//write
						cout << "WriteToFile at time:" << CLOCK_COUNTER << endl;
						int convert;									//accumulator for filename char convert
						int flag = 0;									//loop break flag
						int byte_offset;
						unsigned int k=top->v->Reg_ID[5];						//start at specified element
						unsigned int length=top->v->Reg_ID[6];
						int i = k;
						if (top->v->Reg_ID[4]!=1) {
							ofstream _file(FDT_filename[top->v->Reg_ID[4]].c_str());
							while (MAIN_MEMORY[i]!=00) {
								length--; _file << (char)MAIN_MEMORY[i];
								i++; if(length == 0)break;
							}
							_file.close();
						}
						else {
							while (MAIN_MEMORY[i]!=00) {
								length--; cout<<(char)MAIN_MEMORY[i];
								i++; if(length == 0)break;
							}
						}							
						i++;
						top->v->Reg_ID[2] = i-k-1;
					break;}
					case 4005:{		 									//open file
						cout << "OpenFile at time:" << CLOCK_COUNTER << endl;
						string filename;
						int k=(top->v->Reg_ID[4]);
						while ( MAIN_MEMORY[k]!=0 ) { filename = filename + (char)MAIN_MEMORY[k]; k++; }
					 	FDT_filename.push_back(filename);			        			//add new filename to newest location
						FDT_state.push_back(1);									//add new open indicator to newest location
						top->v->Reg_ID[2] = FileDescriptorIndex;						//place file descriptor into register
						FileDescriptorIndex++;									//ready the next file descriptor
					break;}
					case 4006:{cout << "CloseFile at time:" << CLOCK_COUNTER << endl;FDT_state[top->v->Reg_ID[4]]=0;break;}			//close file
					case 4018:{cout << "Stat at time:" << CLOCK_COUNTER << endl;									//stat
						top->v->Reg_ID[4] = top->v->Reg_ID[5];
						top->v->Reg_ID[5] = top->v->Reg_ID[6];
						struct stat buf;
						top->v->Reg_ID[2]=stat(FDT_filename[top->v->Reg_ID[4]].c_str(),&buf);
						fxstat64(top->v->Reg_ID[29]);
					break;}
					case 4020:{cout << "Getpid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getpid);break;}		//getpid
					case 4024:{cout << "Getuid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getuid);break;}		//getuid
					case 4028:{	cout << "FStat at time:" << CLOCK_COUNTER << endl;										//fstat
						top->v->Reg_ID[4] = top->v->Reg_ID[5];
						top->v->Reg_ID[5] = top->v->Reg_ID[6];
						struct stat buf;
						top->v->Reg_ID[2]=fstat(top->v->Reg_ID[4],&buf);
						fxstat64(top->v->Reg_ID[29]);
					break;}
					case 4037:{cout << "Kill at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_kill);break;}			//kill
					case 4047:{cout << "Getgid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getgid);break;}		//getgid
					case 4049:{cout << "Geteuid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_geteuid);CLOCK_COUNTER-=3;break;}
					case 4050:{cout << "Getegid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getegid);break;}		//getegid
					case 4064:{cout << "Getppid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getppid);break;}		//getppid
					case 4065:{cout << "Getpgrp at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getpgrp);break;}		//getpgrp
					case 4076:{cout << "Getrlimit at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getrlimit);break;}		//getrlimit
					case 4077:{cout << "Getrusage at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getrusage);break;}		//getrusage
					case 4078:{cout << "GetTimeofDay at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_gettimeofday);break;}		//gettimeofday
					case 4090:{cout << "MMap at time:" << CLOCK_COUNTER << endl;
						BLOCKNUM++;
						int blockCounter=0;
						int blockStart=0;
						int size = top->v->Reg_ID[5]*(1+top->v->Reg_ID[4]);
						if(size < 32)size = 32;
						for(int i=HEAPSTART; i<=HEAPSTART+HEAP_STATUS.size()+size; i++) {
							if (HEAP_STATUS[i]==0) blockCounter++;
							else blockCounter=0;
							if (blockCounter==size) {
								allocateHeapBlock(i-size+1,size);
								break;
							}
						}
						top->v->Reg_ID[2] = BLOCKBASE;
					break;}
					case 4091:{cout << "Munmap at time:" << CLOCK_COUNTER << endl;
						clearHeapBlock(top->v->Reg_ID[4]);
					break;}
					case 4122:{cout << "Uname at time:" << CLOCK_COUNTER << endl;
						uname(top->v->Reg_ID[29]);		
						top->v->Reg_ID[2] = 0;
					break;}
					case 4132:{cout << "Getpid at time:" << CLOCK_COUNTER << endl;top->v->Reg_ID[2] = syscall(SYS_getpgid);break;}		//getpgid
					case 4246:{cout << "Exit at time:" << CLOCK_COUNTER << endl;syscall(SYS_exit);break;}								//exit
					case 4555:{cout << "Malloc at time:" << CLOCK_COUNTER << endl;
						BLOCKNUM++;
						int blockCounter=0;
						int blockStart=0;
						int size = top->v->Reg_ID[4];
						if(size < 32)size = 32;
							for(int i=HEAPSTART; i<=HEAPSTART+HEAP_STATUS.size()+size; i++) {
								if (HEAP_STATUS[i]==0) blockCounter++;
								else blockCounter=0;
								if (blockCounter==size) {
									allocateHeapBlock(i-size+1,size);
									break;
								}
							}
						top->v->Reg_ID[2] = BLOCKBASE;
					break;}
					case 4556:{//cout << "LWC0 at time:" << CLOCK_COUNTER << endl;
							instruction = ((MAIN_MEMORY[top->v->Instr_address_2IM+3]) + 
								   (MAIN_MEMORY[top->v->Instr_address_2IM+2]<<8) + 
								   (MAIN_MEMORY[top->v->Instr_address_2IM+1]<<16) + 
								   (MAIN_MEMORY[top->v->Instr_address_2IM+0]<<24));
							source = (instruction << 12)>>28;
							immediate = (instruction << 16)>>16;
							base = (instruction << 6)>>26;
							top->v->Reg_ID[2] = ((MAIN_MEMORY[top->v->Reg_ID[4]+immediate+3])+
								(MAIN_MEMORY[top->v->Reg_ID[4]+immediate+2]<<8)+
								(MAIN_MEMORY[top->v->Reg_ID[4]+immediate+1]<<16)+
								(MAIN_MEMORY[top->v->Reg_ID[4]+immediate+0]<<24));
					break;}
					case 4557:{//cout << "SWC0 at time:" << CLOCK_COUNTER << endl;
							instruction = ((MAIN_MEMORY[top->v->Instr_address_2IM+3]) + 
								   (MAIN_MEMORY[top->v->Instr_address_2IM+2]<<8) + 
								   (MAIN_MEMORY[top->v->Instr_address_2IM+1]<<16) + 
								   (MAIN_MEMORY[top->v->Instr_address_2IM+0]<<24));
							base = (instruction << 6)>>26;
							immediate = (instruction << 16)>>16;
							if(SWC_tmp==0x83)rt = 3;
							else  if(SWC_tmp==0x82)rt = 2;
							loadSingleHEX(top->v->Reg_ID[rt],top->v->Reg_ID[4]+immediate,0,0);
							memWrite << CLOCK_COUNTER << endl;
							memWrite << top->v->Reg_ID[rt] << " " << top->v->Reg_ID[4]+immediate << endl;
							if(HEX_MAIN_MEMORY[top->v->Instr_address_2IM+1] == "82")top->v->Reg_ID[2]=1;
							else if(HEX_MAIN_MEMORY[top->v->Instr_address_2IM+1] == "83")top->v->Reg_ID[3]=1;
					break;}
					
					default: { cout << "Sorry, syscall " << syscallIndex << " has not been implemented. Process terminated at cycle " << MAINTIME/2 << "..." << endl; return 0; }
				}
			}			
			if(CLOCK_COUNTER >= duration) cin.get(); 		//prevents next instruction traversal until user input (any key pressed)
		}
		top->eval();							//assert c++ to verilog modules
	}
	
	
	memWrite.close();							//closes memory tracking file
}
