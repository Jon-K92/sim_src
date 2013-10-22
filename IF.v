//-----------------------------------------
//	   Instruction Fetch Stage
//-----------------------------------------
module IF(	CLK, 
		RESET, 
		PCA_PR,
		CIA_PR,
		taken_branch1,
		//taken_branch2, 
		nextInstruction_address, 
		PC_init, 
		Instr1_fIM, //From instruction Memory
		Instr1_PR, 
		Instr_address_2IM,
	//	Instr2_PR,
		FREEZE,
		fetchNull1, 
		no_new_fetch
		);

  output wire     [31: 0] Instr_address_2IM;
  output reg     [31: 0] Instr1_PR;
//  output reg     [31: 0] Instr2_PR;
  output reg     [31: 0] PCA_PR;
  output reg     [31: 0] CIA_PR;

  input	  [31: 0] nextInstruction_address;
  input	  [31: 0] PC_init;
  input	  [31: 0] Instr1_fIM;
  input		  CLK;
  input		  RESET;
  input		  FREEZE;
  input		  taken_branch1;
  //input		  taken_branch2; 
  input		  no_new_fetch;
  input		fetchNull1;

  wire	   [31: 0] Instr1;
//  wire	   [31: 0] Instr2;
  wire	   [31: 0] PCA; //Program Counter with 4 added
  wire	   [31: 0] CIA; //Current Instruction Address

  reg	    [31: 0] PC;
  reg	    [31: 0] FPC; //Fetch PC

  assign Instr_address_2IM = (taken_branch1)? nextInstruction_address: PCA; //Chooses a next instuction based on if a branch is taken or not. 
  assign PCA	= PC + 4;
  //assign CIA	 = FPC; 
  assign Instr1 = (fetchNull1)? 32'h00000000: (Instr1_fIM);//if fetchcNUll, fetch a null instrcution, else: on single_fetch true get
  //instruction from primary register and on single_fetch false, get from instruction memory. 
  

  // Pipeline Register (IF/ID)
  always @ (posedge CLK or negedge RESET)
  begin
    if(!RESET) //Resets on RESET = 0.
      begin
	Instr1_PR	    <= 32'b0;
//	Instr2_PR	    <= 32'b0;
	PCA_PR	       <= 32'b0;
	CIA_PR	       <= 32'b0;
	FPC		  <= 32'b0;
	PC		   <= PC_init; //Sets an initial program counter if program is reset
      end
    else if(!no_new_fetch && !FREEZE) //if we aren't fetching something new and are not frozen then...
      begin
	Instr1_PR	    <= Instr1;
	PCA_PR	       <= PCA;
	CIA_PR	       <= CIA;
	FPC		  <= Instr_address_2IM;
	PC		   <= Instr_address_2IM + (32'h00000004);	
      end
  end

endmodule
