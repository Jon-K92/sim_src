//-----------------------------------------
//	   Instruction Fetch Stage
//-----------------------------------------
module IF(	CLK, 
		RESET, 
		PCA_PR,
		CIA_PR,
		single_fetch,
		taken_branch1,
		taken_branch2, 
		nextInstruction_address, 
		PC_init, 
		Instr1_fIM, 
		Instr1_PR, 
		Instr_address_2IM,
		Instr2_PR,
		FREEZE,
		fetchNull1,
		no_new_fetch
		);

  output wire     [31: 0] Instr_address_2IM;
  output reg     [31: 0] Instr1_PR;
  output reg     [31: 0] Instr2_PR;
  output reg     [31: 0] PCA_PR;
  output reg     [31: 0] CIA_PR;

  input	  [31: 0] nextInstruction_address;
  input	  [31: 0] PC_init;
  input	  [31: 0] Instr1_fIM;
  input		  single_fetch;
  input		  CLK;
  input		  RESET;
  input		  FREEZE;
  input		  taken_branch1;
  input		  taken_branch2; 
  input		  no_new_fetch;
  input			 fetchNull1;

  wire	   [31: 0] Instr1;
  wire	   [31: 0] Instr2;
  wire	   [31: 0] PCA;
  wire	   [31: 0] CIA;

  reg	    [31: 0] PC;
  reg	    [31: 0] FPC;

  assign Instr_address_2IM   = (taken_branch1|taken_branch2)? nextInstruction_address: PC;
  assign PCA		 = (single_fetch)?PC:Instr_address_2IM+32'h00000008;
  assign CIA		 = (single_fetch)?FPC:Instr_address_2IM;
  assign Instr1	      = (fetchNull1)? 32'h00000000: ((single_fetch)? Instr2_PR : Instr1_fIM);
  

  // Pipeline Register (IF/ID)
  always @ (posedge CLK or negedge RESET)
  begin
    if(!RESET)
      begin
	Instr1_PR	    <= 32'b0;
	Instr2_PR	    <= 32'b0;
	PCA_PR	       <= 32'b0;
	CIA_PR	       <= 32'b0;
	FPC		  <= 32'b0;
	PC		   <= PC_init;
      end
    else if(!no_new_fetch && !FREEZE)
      begin
	Instr1_PR	    <= Instr1;
	PCA_PR	       <= PCA;
	CIA_PR	       <= CIA;
	FPC		  <= Instr_address_2IM;
	PC		   <= Instr_address_2IM + ((single_fetch)?32'h00000004:32'h00000008);	
      end
  end

endmodule
