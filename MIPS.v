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
   wire 	  taken_branch1_IDIF/*verilator public*/;
   wire	[31:0]	  Instr1_PR_IFID;
   wire		  no_new_fetch;
   wire 	  fetchNull1;

   // Pipeline Stages Instantiation
   IF IF1(CLK, RESET, PCA_IFID, FREEZE,CIA_IFID, taken_branch1_IDIF,
   nextInstruction_address_IDIF,PC_init,Instr1_fIM,Instr1_PR_IDIF,Instr_address_2IM,FREEZE,
   fetchNull1,no_new_fetch/* TA: arguments are missing */);
   //IF(CLK,RESET, 
   // PCA_PR ----Use 'PCA_IFID' (wires) 
   //CIA_PR -----Value doesn't matter-Use 'CIA_IFID' (wires)
   //taken_branch1 --USE taken_branch1_IFID (wires)
   //nextInstruction_address -- nextInstruction_address_IDIF/
   //PC_init,Instr1_fIM,
   //Instr1_PR --Instr1_PR_IFID
   //Instr_address_2IM,FREEZE, fetchNull1,no_new_fetch );

   ID ID1(CLK, RESET, FREEZE, /* TA: arguments are missing */);
  //ID (CLK,RESET,ALUSrc1_PR,Instr1_PR,Dest_Value1_PR, SYS_OUT, readDataB1_PR,Instr1_10_6_PR,
   // do_writeback1_WB, writeRegister1_WB, do_writeback1_PR, readRegisterA1_PR,readRegisterB1_PR,
   //taken_branch1_PR, writeRegister1_WB,writeRegister1_PR, nextInstruction_address_PR,
   //Reg, nextInstruction_address, R2_output_PR, Operand_A1_PR, Operand_B1_PR,
   //ALU_control1_PR, MemRead1_PR,MemWrite1_PR, MemtoReg1_PR,Instr1,
   //PCA ----Use 'PCA_IFID' (wires)
   //writeData1_WB,R2_input, FREEZE,insertBubble_OUT);

   EXE EXE1 (CLK, RESET, /* TA: arguments are missing */);

   MEM MEM1(CLK, RESET, /* TA: arguments are missing */);

   WB WriteBack (CLK, RESET, /* TA: arguments are missing */);

endmodule
