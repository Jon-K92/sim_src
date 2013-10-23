//-----------------------------------------
//            Pipelined MIPS
//-----------------------------------------
module MIPS (	R2_output,
             	MemRead,
             	MemWrite,
             	iBlkRead,
             	iBlkWrite,
             	dBlkRead,
             	dBlkWrite,
             	data_write_2DM,
             	//data_read_fDM,
             	block_write_2DM,
             	block_write_2IM,
             	block_read_fDM,
             	block_read_fIM,
             	Instr1_fIM,
		Instr2_fIM,
             	data_address_2DM,
             	CLK,
	     	RESET,
             	R2_input,
             	PC_init
            	);

   output [31: 0] R2_output;
   output [31: 0] data_address_2DM;
   output [31: 0] data_write_2DM;
   output [255:0] block_write_2DM;
   output [255:0] block_write_2IM;
   output         MemRead;
   output         MemWrite;
   output         iBlkRead;
   output         iBlkWrite;
   output         dBlkRead;
   output         dBlkWrite;

   input  [31: 0]  PC_init;
   input  [31: 0]  R2_input;
   //input  [31: 0]  data_read_fDM;
   input  [255:0]  block_read_fDM;
   input  [255:0]  block_read_fIM;
   input  [31: 0]  Instr1_fIM;
   input  [31: 0]  Instr2_fIM;
   input 	  CLK;
   input          RESET;

   //connecting wires (signals passing through more than 1 stage)
   wire [31: 0]   R2_output_ID/*verilator public*/;
   wire [31: 0]   Instr_fMEM/*verilator public*/;
   wire [31: 0]   Reg_ID [0:31]/*verilator public*/;  
   wire [31: 0]   Instr_address_2IM/*verilator public*/;
   wire [31: 0]   CIA_IFID/*verilator public*/;
   wire [31: 0]   PCA_IFID/*verilator public*/;
   wire [31: 0]   nextInstruction_address_IDIF/*verilator public*/;
   wire           no_fetch/*verilator public*/;
   wire           SYS/*verilator public*/;
   wire           single_fetch_IDIF/*verilator public*/;
   
   //NEW WIRES! (Added 10/22)
   wire 	taken_branch1_IDIF/*verilator public*/;
   wire	[31:0]	Instr1_IFID;
   wire		no_new_fetch;
   wire 	fetchNull1;
   wire		ALUSrc1_IDEXE;
   wire [31:0]	Instr1_IDEXE;
   wire [31:0]	Dest_Value1_IDEXE;
   wire [4:0] 	Instr1_10_6_IDEXE;
   wire		do_writeback1_WB;
   wire [4:0]	writeRegister1_WB;
   wire		do_writeback1_ID;
   wire [4:0	readRegisterA1_IDEXE;
   wire [4:0]	readRegisterB1_IDEXE;
   wire [4:0]	writeRegister1_IDEXE;
   wire [31:0] 	Reg;
    wire [31:0] nextInstruction_address;
    wire [31:0] Operand_A1_IDEXE;
   wire [31:0]	Operand_B1_IDEXE;
   wire [5:0]	ALU_control1_IDEXE;
   wire		MemRead1_IDEXE;
   wire		MemWrite1_IDEXE;
   wire		MemtoReg1_IDEXE;
   wire [31:0]	writeData1_WB;
   wire		insertBubble_OUT;
   wire 	ALUSrc1_EXEMEM;
   wire [31:0]	Instr1_EXEMEM;
   wire [31:0] 	Dst1_EXEMEM;
   wire [31:0] 	readDataB1;
   wire [31:0] 	readDataB1_EXEMEM;
   wire [31:0]	 aluResult1_OUT;
   wire [31:0] 	Data1_WB;
   wire 	ALU_control1_EXEMEM;
   wire [31:0] 	Data1_MEM;
   wire [4:0] 	writeRegister1_MEMEXE;
   wire 	do_writeback1_MEM;
   wire 	do_writeback1_EXE;
   wire [4:0]	writeRegister1_EXEMEM;
   wire 	MemRead1_EXEMEM;
   wire 	MemWrite1_EXEMEM;
   wire 	MemtoReg1_EXEMEM;
   wire 	aluResult1_EXEMEM;
   
   
   
   
   
   
   
   
   
   
   
   
   
   

   // Pipeline Stages Instantiation
   IF IF1(CLK, RESET, PCA_IFID, FREEZE,CIA_IFID, taken_branch1_IDIF,
   nextInstruction_address_IDIF,PC_init,Instr1_fIM,Instr1_IDIF,Instr_address_2IM,FREEZE,
   fetchNull1,no_new_fetch/* TA: arguments are missing */);
   //IF(CLK,RESET, 
   // PCA_PR ----Use 'PCA_IFID' (wires) 
   //CIA_PR -----Value doesn't matter-Use 'CIA_IFID' (wires)
   //taken_branch1 --USE taken_branch1_IFID (wires)
   //nextInstruction_address -- nextInstruction_address_IDIF/
   //PC_init,Instr1_fIM,
   //Instr1_PR --Instr1_PR_IFID
   //Instr_address_2IM,FREEZE, fetchNull1,no_new_fetch );

   ID ID1(CLK, RESET,ALUSrc1_IDEXE, FREEZE,Instr1_IDEXE,Dest_Value1_IDEXE,SYS,
   readDataB1_IDEXE,Instr1_10_6_IDEXE,do_writeback1_WB,writeRegister1_WB,do_writeback1_ID,
   readRegisterA1_IDEXE,readRegisterB1_IDEXE,taken_branch1_IFID,writeRegister1_IDEXE,
   nextInstruction_address_IDIF, nextInstrcution_address,R2_output,Operand_A1_IDEXE,Operand_B1_IDEXE,
   ALU_control1_IDEXE,MemRead1_IDEXE,MemWrite1_IDEXE,MemtoReg1_IDEXE, Instr1_IFID,PCA_IFID,
   writeData1_WB, R2_input,FREEZE,insertBubble_OUT);
  //ID (CLK,RESET,ALUSrc1_PR, Instr1_PR,Dest_Value1_PR, SYS_OUT, readDataB1_PR,Instr1_10_6_PR,
   //do_writeback1_WB, writeRegister1_WB, do_writeback1_PR, readRegisterA1_PR,readRegisterB1_PR,
//  taken_branch1_PR, writeRegister1_PR, nextInstruction_address_PR,
  // Reg, nextInstruction_address, R2_output_PR, Operand_A1_PR, Operand_B1_PR,
   //ALU_control1_PR, MemRead1_PR,MemWrite1_PR, MemtoReg1_PR,Instr1,
   //PCA ----Use 'PCA_IFID' (wires)
   //writeData1_WB,R2_input, FREEZE,insertBubble_OUT);

   EXE EXE1 (CLK, RESET,ALUSrc1_EXEMEM,	ALUSrc1_IDEXE,Instr1_IDEXE,Instr1_EXEMEM,Dest_Value1_IDEXE,
   Dst1_EXEMEM,readDataB1,readDataB1_EXEMEM,aluResult1_OUT,do_writeback1_WB, writeRegister1_WB,Data1_WB,
   ALU_control1_EXEMEM,ALU_control1_IDEXE, Data1_MEM,writeRegister1_MEMEXE,do_writeback1_MEM,
   do_writeback1_EXE,do_writeback1_ID,readRegisterA1_IDEXE,readRegisterB1_IDEXE,writeRegister1_IDEXE,
   writeRegister1_EXEMEM,Instr1_10__6_IDEXE,MemRead1_IDEXE, MemWrite1_IDEXE,MemRead1_EXEMEM,MemWrite1_EXEMEM,
   Operand_Al_IDEXE, Operand_B1_IDEXE,MemtoReg1_IDEXE, MemtoReg1_EXEMEM,aluResult_EXEMEM);
 //  EXE(CLK,RESET,ALUSrc1_PR, ALUSrc1, Instr1, Instr1_PR,Dest_Value1, Dst1_PR,readDataB1,readDataB1_PR,
 // aluResult1_OUT,do_writeback1_WB, writeRegister1_WB,Data1_WB,ALU_control1_PR, ALU_control1,                
 //Data1_MEM, writeRegister1_MEM, do_writeback1_MEM, do_writeback1_PR,do_writeback1_ID, readRegisterA1,
 //readRegisterB1, writeRegister1, writeRegister1_PR, Instr1_10_6, MemRead1,MemWrite1,MemRead1_PR,
 //MemWrite1_PR, Operand_A1,Operand_B1,  MemtoReg1,MemtoReg1_PR,aluResult1_PR,);

   MEM MEM1(CLK, RESET, /* TA: arguments are missing */);

   WB WriteBack (CLK, RESET, /* TA: arguments are missing */);

endmodule
