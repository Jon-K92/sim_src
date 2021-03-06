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
             	data_read_fDM,
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
   input  [31: 0]  data_read_fDM;
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
	 wire [15:0] Instr1_15_0_IDEXE;
   wire		do_writeback1_WB;
   wire [4:0]	writeRegister1_WB;
   wire		do_writeback1_ID;
   wire [4:0]	readRegisterA1_IDEXE;
   wire [4:0]	readRegisterB1_IDEXE;
   wire [4:0]	writeRegister1_IDEXE;
   wire [31:0] 	Reg;
   
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
   wire 	[5:0]ALU_control1_EXEMEM;
   wire [31:0] 	Data1_MEM;
   wire [4:0] 	writeRegister1_MEMEXE;
   wire 	do_writeback1_MEM;
   wire 	do_writeback1_EXE;
   wire [4:0]	writeRegister1_MEM;
   wire 	MemRead1_EXEMEM;
   wire 	MemWrite1_EXEMEM;
   wire 	MemtoReg1_EXEMEM;
   wire 	[31:0] aluResult1_EXEMEM;
   wire 	[31:0] Instr_OUT;
   wire		MemtoReg1_MEMWB;
   wire [31:0]	aluResult1_MEMWB;
   wire [31:0]	data_read1_MEM;
   wire [31:0]	Data_input1;
   wire [31:0] readDataA1_IDEXE;
   
   wire do_writeback_ID;
   wire [31:0] readDataB1_IDEXE;
   wire [31:0] regBase;
   
   //TEMPORARY 
wire FREEZE;
always begin
FREEZE = 1'b0;
end
   
   
   

   // Pipeline Stages Instantiation
   IF IF1(CLK, RESET, PCA_IFID, CIA_IFID, taken_branch1_IDIF,
   nextInstruction_address_IDIF,PC_init,Instr1_fIM,Instr1_IFID,Instr_address_2IM,FREEZE,
   fetchNull1,no_new_fetch/* TA: arguments are missing */);
   //IF(CLK,RESET, 
   // PCA_PR ----Use 'PCA_IFID' (wires) 
   //CIA_PR -----Value doesn't matter-Use 'CIA_IFID' (wires)
   //taken_branch1 --USE taken_branch1_IFID (wires)
   //nextInstruction_address -- nextInstruction_address_IDIF/
   //PC_init,Instr1_fIM,
   //Instr1_PR --Instr1_PR_IFID
   //Instr_address_2IM,FREEZE, fetchNull1,no_new_fetch );

   ID ID1(CLK,  RESET, ALUSrc1_IDEXE, Instr1_IDEXE,  Dest_Value1_IDEXE,
   SYS,readDataA1_IDEXE, readDataB1_IDEXE,Instr1_10_6_IDEXE,Instr1_15_0_IDEXE,
   do_writeback1_WB,do_writeback1_ID, readRegisterA1_IDEXE,readRegisterB1_IDEXE,
   taken_branch1_IFID,writeRegister1_WB,writeRegister1_IDEXE, nextInstruction_address_IDIF,Reg,
    R2_output,Operand_A1_IDEXE,Operand_B1_IDEXE,ALU_control1_IDEXE,MemRead1_IDEXE,
    MemWrite1_IDEXE,MemtoReg1_IDEXE, Instr1_IFID,PCA_IFID, writeData1_WB,
     R2_input,FREEZE,insertBubble_OUT,regBase);
  //ID (CLK,RESET,ALUSrc1_PR, Instr1_PR,Dest_Value1_PR, SYS_OUT, readDataB1_PR,Instr1_10_6_PR,
   //do_writeback1_WB, writeRegister1_WB, do_writeback1_PR, readRegisterA1_PR,readRegisterB1_PR,
//  taken_branch1_PR, writeRegister1_PR, nextInstruction_address_PR,
  // Reg, R2_output_PR, Operand_A1_PR, Operand_B1_PR,
   //ALU_control1_PR, MemRead1_PR,MemWrite1_PR, MemtoReg1_PR,Instr1,
   //PCA ----Use 'PCA_IFID' (wires)
   //writeData1_WB,R2_input, FREEZE,insertBubble_OUT);

   EXE EXE1 (CLK,  RESET, ALUSrc1_EXEMEM,	  ALUSrc1_IDEXE,  Instr1_IDEXE, 
    Instr1_EXEMEM,  Dest_Value1_IDEXE,  Dst1_EXEMEM,  readDataB1,   readDataB1_EXEMEM,
     aluResult1_OUT, do_writeback1_WB,   writeRegister1_WB,  Data1_WB,  ALU_control1_EXEMEM,
      ALU_control1_IDEXE, Data1_MEM,  writeRegister1_MEMEXE,  do_writeback1_MEM, do_writeback1_EXE,
      do_writeback1_ID, readRegisterA1_IDEXE, readRegisterB1_IDEXE,  writeRegister1_IDEXE,  writeRegister1_MEM, 
      Instr1_10_6_IDEXE,Instr1_15_0_IDEXE, MemRead1_IDEXE,  MemWrite1_IDEXE, Read1_EXEMEM, MemWrite1_EXEMEM, 
      Operand_A1_IDEXE, Operand_B1_IDEXE,MemtoReg1_IDEXE,  MemtoReg1_EXEMEM,  aluResult1_EXEMEM,regBase);
 //  EXE(CLK,RESET,ALUSrc1_PR, ALUSrc1, Instr1, Instr1_PR,Dest_Value1, Dst1_PR,readDataB1,readDataB1_PR,
 // aluResult1_OUT,do_writeback1_WB, writeRegister1_WB,Data1_WB,ALU_control1_PR, ALU_control1,                
 //Data1_MEM, writeRegister1_MEM, do_writeback1_MEM, do_writeback1_PR,do_writeback1_ID, readRegisterA1,
 //readRegisterB1, writeRegister1, writeRegister1_PR, Instr1_10_6, MemRead1,MemWrite1,MemRead1_PR,
 //MemWrite1_PR, Operand_A1,Operand_B1,  MemtoReg1,MemtoReg1_PR,aluResult1_PR,);

   MEM MEM1(CLK, RESET, ALUSrc1_EXEMEM,Instr1_EXEMEM, Instr_OUT,
    writeRegister1_WB, do_writeback1_WB,readDataB1_EXEMEM,do_writeback1_MEM,
   writeRegister1_MEMEXE,writeRegister1_MEM,data_write_2DM,data_address_2DM,
   MemRead_2DM, MemWrite_2DM,data_read_fDM,MemtoReg1_EXEMEM,MemtoReg1_MEMWB,MemRead1_EXEMEM,
    MemWrite_EXEMEM,ALU_control1_EXEMEM,aluResult1_EXEMEM,aluResult1_MEMWB,data_read1_MEM);
  // module MEM (CLK, RESET,ALUSrc1,Instr1,Instr_OUT, writeData1_WB,writeRegister1_WB, do_writeback1_WB,
   //readDataB1, do_writeback1_PR,writeRegister1, writeRegister1_PR,data_write_2DM, data_address_2DM,
   //MemRead_2DM,MemWrite_2DM,data_read_fDM,MemtoReg1_PR,MemRead1, MemRead1, MemWrite1,ALU_control1,
    // aluResult1, aluResult1_PR, data_read1_PR, );

   WB WriteBack (CLK, RESET,do_Writeback1_MEM,aluResult1_OUT, writeRegister1_MEM, writeRegister1_WB,
   writeData1_WB,do_Writeback_WB,aluResult1_MEMWB,Data_input1,MemtoReg1_MEMWB);
     //  CLK, RESET, do_writeback1,aluResult1_OUT,writeRegister1,writeRegister1_OUT, writeData1_OUT,
      //do_writeback1_OUT,aluResult1, Data_input1,MemtoReg1, );

endmodule
