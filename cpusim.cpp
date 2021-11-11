/*
Required standard and developed libraries
*/
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <vector>

using namespace std; 

class Instruction{

  public:
   vector<string> bytes;
   int type; //r-type = 0, i-type = 1, lw = 2, sw = 3
   string rs1;
   string rs2;
   string rd;
   string imms;
   string opcode;
   string funct7;
   string funct3;
   string op;
   int rdval;
   int rs1val;
   int rs2val; //where imm val is also stored
   int memval;
};


class CPU{
  public:
  int PC;
  int myRegs[32];
  int myDataMem [1024]; // 4096 bytes
  string * myInstMem; 
  vector <string> Fetch(int PC, string * myInstMem, vector <string> &bytes);
  void Decode(int * myRegs, Instruction *myInstr);
  void ALU(Instruction * myInstr);
};




//helper function/definitions

string decToBinary(string decimal);
int binaryToDecimal(string binary); 
string decToBinary2(int decimal); 



int main (int argc, char* argv[]) 
// executed: ./cpusim filename.txt, prints (a0,a1) 
{
	
  //Read the instructions that are stored in a file and load them into an instruction memory. 
	
  // Define instruction memory dynamically. Each sell stores 1 byte. Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode). 
	
  //	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). Read the file line by line and store it into the memory. 
        

	int lines = 0;
	ifstream txtFile(argv[1]);
	string onebyte;
	
	//count number of lines in input text file
	while(txtFile){
	  getline(txtFile, onebyte);
	  lines++;
	  }

	//Instantiate CPU object
	CPU myCPU;
	 
	myCPU.myInstMem = new string [lines];

	//return to top of txtFile
	txtFile.clear();
	txtFile.seekg(0, ios::beg);
	
	int i = 0;
	while(txtFile){
	  getline(txtFile, onebyte);
	  myCPU.myInstMem[i] = decToBinary(onebyte);
	  
	  i++;
	}
       
	txtFile.close();
	
	
	/* Instantiate Instruction object pointers */
	Instruction * Inst1 = NULL; 
	Instruction * Inst2 = NULL;
	Instruction * Inst3 = NULL;
	Instruction * Inst4 = NULL;
	Instruction * Inst5 = NULL;
	

	//Instantiate Memory Pointers
	int *Reg = myCPU.myRegs;  
	int *Mem = myCPU.myDataMem; 
	
	for (int i = 0; i < 32; i++)
	  Reg[i] = 0;

	// Clock and Program Counter
	int myClock = 0; 
	int myPC = 0; 
	
	int zeros = 0;
	string zero = "000000"; 

	//Instantiate Control Signals
	bool dvalc = false;
	bool evalc = false;
	bool mvalc = false;
	bool wvalc = false;
	bool dvaln = false;
	bool evaln = false;
	bool mvaln = false;
	bool wvaln = false; 
	
	

	while (1) //processor main loop, each iteration = 1 clock cycle
	    {
		//fetch
	        if (myClock < (lines/4)){ //if there are instructions left to fetch
		  vector <string>  temp;
		  Inst1 = new Instruction;

		  Inst1->bytes = myCPU.Fetch(myPC, myCPU.myInstMem, temp); // fetching the instruction
		
	       
		  if (Inst1->bytes[0].substr(1) == "0000000"){ //opcode is 0	  
		    dvaln = false; 
		    zeros++;
		   
		  }else{
		    zeros = 0;
		    dvaln = true; 
		  }
		
	        }else
		  dvaln = false; 

		// decode
		if (dvalc){

		  myCPU.Decode(Reg, Inst2); // decoding

		  evaln = true;

		}else
		  evaln = false;
		
		// execute
		if (evalc){
		  mvaln = true;
		  switch(Inst3->type){
		    case 0: //r-type
		    case 1: //i-type
		    case 2:
		    case 3:
		      myCPU.ALU(Inst3);
		      break;
		    
		      
		  }

		}else{
		  mvaln = false;
		}
		
		// memory
		if (mvalc && (Inst4->type == 2)){ //lw 
		  Inst4->memval = myCPU.myDataMem[Inst4->rdval]; //value in memory at rdval
		  
		  wvaln = true;
		}else if (mvalc && Inst4->type == 3){ //sw
		  myCPU.myDataMem[Inst4->rdval] = Inst4->rs2val;
		  
		  wvaln = true; 
		  
	        }else if (mvalc){
		  
		  wvaln = true; 
		}else{
		  wvaln = false; 
		}

		// writeback valid
		if(wvalc){
		  
		  int loc = binaryToDecimal(Inst5->rd);
		  
		  if (Inst5->type == 0 || Inst5->type == 1) //rtype or itype
		    {
		      myCPU.myRegs[loc] = Inst5->rdval;
	
		  }
		  else if (Inst5->type == 2) //lw
		    myCPU.myRegs[loc] = Inst5->memval;
		  delete Inst5; //delete dynamically allocated instruction after completion
		}else{
		  //do nothing
		}

		

		//increment clock and PC
		myClock += 1;
		myPC += 4;

		// we should break the loop if ALL instructions in the pipeline has opcode==0 	
		if (zeros == 5 ) //five consecutive instructions with opcode 0
		   break;
		
				
		
		//pass Intructions to next stage
		Inst5 = Inst4;
		Inst4 = Inst3;
		Inst3 = Inst2;
		Inst2= Inst1;
		
		//update control signals
		dvalc = dvaln;
		evalc = evaln;
		mvalc = mvaln;
		wvalc = wvaln;
	       		
	    }
	
	
      
	// print the stats
	cout << "(" << myCPU.myRegs[0] << "," << myCPU.myRegs[1] << ")" << endl;
	
	//clean up Instruction Memory
	delete [] myCPU.myInstMem; 

	return 0; 

}

//converting string to binary
string decToBinary(string decimal){
  int decval = 0;
  int p = 1;
  int len = decimal.length();

  for (int i = len-1; i >= 0; i--){ //convert decimal value from string formal to decimal
    int num = decimal[i] - '0';
    decval += num*p;
    p *= 10;
  }
 
  string retval =  decToBinary2(decval); 
  return retval; 
}

//converting int to binary
string decToBinary2(int decimal){
  
  int leftover = decimal; 
  string retval = "00000000";
  
  for (int i = 7; i >= 0; i--){
    if (leftover /pow(2,i) >=1){
      retval[7-i] = '1';
      leftover -= pow(2, i); 
    }
  }

  return retval; 

}

//converting binary to decimal
int binaryToDecimal(string binary){
  int len = binary.length();
  int p = len - 1; 
  int sum = 0;
  for (int i = 0; i < len; i++){
   if(binary[i] == '1'){
     	sum += pow(2,p);
    }
    p--; 
  }

  return sum; 

}

//fetch instruction in the form of four bytes
vector<string> CPU::Fetch(int PC, string * myInstMem, vector <string> &bytes){
  
  for (int i = 0; i < 4; i++)
    {bytes.push_back(myInstMem[PC + i]);
    }
  return bytes;
  
  

  

}

//decode instruction and update member variables of MyInstr
void CPU::Decode(int * myRegs, Instruction * myInstr){
  
  //set myInstr values

  myInstr->opcode = myInstr->bytes[0].substr(1);
  myInstr->rd = myInstr->bytes[1].substr(4) + myInstr->bytes[0][0]; 

  myInstr->funct3 = myInstr->bytes[1].substr(1, 3);
  
  //grab rs1 value from register file 
  myInstr->rs1 = myInstr->bytes[2].substr(4) + myInstr->bytes[1][0];

  myInstr->rs1val = myRegs[binaryToDecimal(myInstr->rs1)];
  
  
  if (myInstr->opcode == "0110011" || myInstr->opcode == "0100011")//if r-type or sw
  {
    if (myInstr->opcode == "0110011")
      myInstr->type = 0; //r-type
    else {myInstr->type = 3;} //sw
     
    //grab rs2 value from register file 
    myInstr->rs2 = myInstr->bytes[3][7] + myInstr->bytes[2].substr(0, 4);
    myInstr->rs2val = myRegs[binaryToDecimal(myInstr->rs2)]; 
    
    myInstr->funct7 = myInstr->bytes[3].substr(0, 7);
  }else{
    myInstr->imms = myInstr->bytes[3] + myInstr->bytes[2].substr(0,4); 
    myInstr->rs2val = binaryToDecimal(myInstr->imms);
  
    if (myInstr->opcode == "0000011")
      myInstr->type = 2;//lw
    else
      myInstr->type = 1; //i-type
    
  }
  

  //decode operation
  
  if (myInstr->type == 0 || myInstr->type == 3) //r-type or sw
    {
      if (myInstr->funct3 == "010")
	myInstr->op = "sw"; 
      else if (myInstr->funct3 == "111")
        myInstr->op = "and";
      else if (myInstr->funct3 == "110")
        myInstr->op = "or"; 
      else if (myInstr->funct7 == "0100000")
        myInstr->op = "sub"; 
      else 
        myInstr->op = "add"; 
      
  } else{
    if (myInstr->funct3 == "000")
      myInstr->op = "addi";
    else if (myInstr->funct3 == "110")
      myInstr->op = "ori";
    else if (myInstr->funct3 == "111")
      myInstr->op = "andi";
    else 
      myInstr->op = "lw"; 
  }

}


//execute instruction in ALU
void CPU::ALU(Instruction * myInstr){
  int rdv; 
  int val1 = myInstr->rs1val;
  int val2 = myInstr->rs2val;
  
  string op = myInstr->op;

  if (op == "sw")
    {
      string rd = myInstr->funct7 + myInstr->rd;
      
      myInstr->rdval = val1 + binaryToDecimal(rd); 
      
  }else {
  if (op == "add" || op == "addi" || op == "lw"){
    rdv = val1+val2;
  }else if (op == "sub" || op == "subi"){
    rdv = val1-val2;
  }else{
    
    string binval1 = decToBinary2(val1);
    string binval2 = decToBinary2(val2);
    
    string rds = "";

    //and operation
    if (op == "and" || op == "andi"){
     
      for (int j = 0; j <= 7; j++){
	if (binval1[j] == '1' && binval2[j] == '1')
	  rds = rds + '1';
	else
	  rds = rds + '0'; 
      }
      
    }else if (op == "or" || op == "ori"){  // or operation
      for (int j = 0; j <= 7; j++){
	if (binval1[j] == '1' || binval2[j] == '1')
	  rds = rds + '1';
	else
	  rds = rds + '0';
      }
    }

    //convert rds to decimal form
    
    rdv = binaryToDecimal(rds);
    //cout << "ANDI/ORI RDV" << rdv << endl; 
  } 
  
  myInstr->rdval = rdv;
  }
}
