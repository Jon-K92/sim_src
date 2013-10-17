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

   // Pipeline Stages Instantiation
   IF IF1(CLK, RESET, FREEZE, /* TA: arguments are missing */);

   ID ID1(CLK, RESET, FREEZE, /* TA: arguments are missing */);

   EXE EXE1 (CLK, RESET, /* TA: arguments are missing */);

   MEM MEM1(CLK, RESET, /* TA: arguments are missing */);

   WB WriteBack (CLK, RESET, /* TA: arguments are missing */);

endmodule
