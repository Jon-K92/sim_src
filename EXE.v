//-----------------------------------------
//	   Execute Stage
//-----------------------------------------
module EXE(	CLK, 
		RESET,
		ALUSrc1_PR,
		ALUSrc1,
		Instr1,
		Instr1_PR,
		Dest_Value1,
		Dst1_PR,
		readDataB1,
		readDataB1_PR,
		aluResult1_OUT,
		do_writeback1_WB,
		writeRegister1_WB,
		Data1_WB,
		ALU_control1_PR,
		ALU_control1,		
		Data1_MEM,
		writeRegister1_MEM,
		do_writeback1_MEM, 
		do_writeback1_PR,
		do_writeback1_ID,
		readRegisterA1,
		readRegisterB1,
		writeRegister1, 
		writeRegister1_PR, 
		Instr1_10_6, 
		Instr1_15_0,
		MemRead1, 
		MemWrite1, 
		MemRead1_PR, 
		MemWrite1_PR, 
		Operand_A1, 
		Operand_B1, 
		MemtoReg1, 
		MemtoReg1_PR, 
		aluResult1_PR,
		Regbase
		);

   output reg     [31: 0] aluResult1_PR;
   output reg     [31: 0] Dst1_PR;
   output reg     [31: 0] aluResult1_OUT;
   output reg     [31: 0] readDataB1_PR;

   output reg     [31: 0] Instr1_PR;
   output reg     [ 5: 0] ALU_control1_PR;
   output reg     [ 4: 0] writeRegister1_PR;
   output reg	     MemtoReg1_PR;
   output reg	     ALUSrc1_PR;
   output reg		  MemRead1_PR;
   output reg		  MemWrite1_PR;
   output reg		  do_writeback1_PR;

   input	  [31: 0] Operand_A1;
   input	  [31: 0] Operand_B1;
   input	  [31: 0] Dest_Value1;
   input	  [31: 0] Data1_MEM;
   input	  [31: 0] Data1_WB;
   input	  [31: 0] readDataB1;
   input	  [31: 0] Instr1;
   input	  [ 5: 0] ALU_control1;
   input	  [ 4: 0] writeRegister1_MEM;
   input	  [ 4: 0] readRegisterA1;
   input	  [ 4: 0] readRegisterB1;
   input	  [ 4: 0] writeRegister1;
   input	  [ 4: 0] Instr1_10_6;
   input   [ 15: 0] Instr1_15_0;
   input	  [ 4: 0] writeRegister1_WB;
   input		  ALUSrc1;
   input	 	  do_writeback1_MEM;
   input		  do_writeback1_WB;
   input	 	  do_writeback1_ID;
   input		  MemtoReg1;
   input		  MemRead1;
   input		  MemWrite1;
   input		  CLK;
   input		  RESET;
   input   [31:0] Regbase;
	
   wire	   [31: 0] aluResult1;
   wire	   [31: 0] OpA1;
   wire	   [31: 0] OpB1;
   wire	   [31: 0] Dst1;

   wire	    [31: 0] HI;
   wire	    [31: 0] LO;

   assign OpA1 = (MemWrite1)? Regbase : Operand_A1;
   assign OpB1 = (ALUSrc1)? Instr1_15_0: Operand_B1;

	// TA: Forwarding is missing here !

	ALU ALU1(HI, LO, aluResult1, OpA1, OpB1, ALU_control1, Instr1_10_6, CLK);

	//Pipeline Stage

	always @ (posedge CLK or negedge RESET) begin
		if(!RESET) begin
			MemtoReg1_PR <= 1'b0;
			MemRead1_PR <= 1'b0;
			MemWrite1_PR <= 1'b0;
			aluResult1_PR <= 32'b0;
			writeRegister1_PR <= 5'b0;
			do_writeback1_PR <= 1'b0;
			ALU_control1_PR <= 6'b0;
			readDataB1_PR <= 32'b0;
			Dst1_PR <= 32'b0;
			Instr1_PR <= 32'b0;
			ALUSrc1_PR <= 1'b0;
		end
	else 
	  begin
	    MemtoReg1_PR <= MemtoReg1;
			MemRead1_PR <= MemRead1;
			MemWrite1_PR <= MemWrite1;
			aluResult1_PR <= aluResult1;
			writeRegister1_PR <= writeRegister1;
			do_writeback1_PR <= 0;
			ALU_control1_PR <= ALU_control1;
			readDataB1_PR <= readDataB1;
			//Dst1_PR <= ;
			Instr1_PR <= Instr1;
			ALUSrc1_PR <= ALUSrc1;
		end
	end

endmodule
