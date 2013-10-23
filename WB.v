//-----------------------------------------
//	   Write Back Stage
//-----------------------------------------
module WB (	CLK,
		RESET,
		do_writeback1,
		aluResult1_OUT,
		writeRegister1,
		writeRegister1_OUT,
		writeData1_OUT,
		do_writeback1_OUT,
		aluResult1,
		Data_input1,
		MemtoReg1
		);

   output reg      [31: 0] writeData1_OUT;
   output reg      [31: 0] aluResult1_OUT;
   output reg      [ 4: 0] writeRegister1_OUT;
   output reg	      do_writeback1_OUT;
     
   input	   [31: 0] aluResult1;
   input	   [31: 0] Data_input1;
   input	   [ 4: 0] writeRegister1;
   input   		   MemtoReg1;
   input  		   CLK;
   input   		   RESET;
   input	    	   do_writeback1;     

   wire		    do_writeback1;     

always @ (posedge CLK)
	begin
	if (do_writeback1)
		begin	
			writeData1_OUT <= Data_input1;
			aluResult1_OUT <= aluResult1;
			writeRegister1_OUT <= writeRegister1;
			do_writeback1_OUT <= do_writeback1;
	else 
		
		end	


	end


endmodule
