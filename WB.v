//-----------------------------------------
//	   Write Back Stage
//-----------------------------------------
module WB (	CLK,
		RESET,
		do_writeback1_PR,
		writeRegister1_PR,
		writeData1_PR,
		do_writeback1,
		aluResult1_OUT,
		writeRegister1,
		writeRegister1_OUT,
		writeData1_OUT,
		do_writeback1_OUT,
		aluResult1,
		Data_input1,
		MemtoReg1,
		);

   output reg      [31: 0] writeData1_OUT;
   output reg      [31: 0] aluResult1_OUT;
   output reg      [31: 0] writeData1_PR;
   output reg      [ 4: 0] writeRegister1_PR;
   output reg      [ 4: 0] writeRegister1_OUT;
   output reg	      do_writeback1_OUT;
   output reg	      do_writeback1_PR;
     
   input	   [31: 0] aluResult1;
   input	   [31: 0] Data_input1;
   input	   [ 4: 0] writeRegister1;
   input   		   MemtoReg1;
   input  		   CLK;
   input   		   RESET;
   input	    	   do_writeback1;     

   wire		    do_writeback1;     

	// TA: insert bypassing/forwarding logic if neccessary   

  //Pipe Register

   always @ (posedge CLK or negedge RESET) begin
       if(!RESET) begin
	     writeData1_PR <= 32'b0;
	     writeRegister1_PR <= 5'b0;
	     do_writeback1_PR <= 1'b0;
       end
    end

endmodule
