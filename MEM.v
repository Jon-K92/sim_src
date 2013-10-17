//-----------------------------------------
//           Data Memory Stage
//-----------------------------------------
module MEM (	CLK, 
		RESET,
		ALUSrc1,
		Instr1,
		Instr_OUT,
		writeData1_WB,
		writeRegister1_WB, 
		do_writeback1_WB,
		Dest_Value1,
		readDataB1,
		Data1_2ID,
		do_writeback1_PR, 
		do_writeback1,
		writeRegister1, 
		writeRegister1_PR, 
		data_write_2DM,
		data_address_2DM,
		MemRead_2DM,
		MemWrite_2DM,
		data_read_fDM,
		MemtoReg1, 
		MemtoReg1_PR, 
		MemRead1, 
		MemWrite1, 
		ALU_control1, 
		aluResult1, 
		aluResult1_PR, 
		data_read1_PR,
		);

   output reg    [31: 0] aluResult1_PR;
   output reg    [31: 0] data_read1_PR;
   output reg    [31: 0] data_write_2DM;
   output reg    [31: 0] data_address_2DM;
   output reg    [31: 0] Instr_OUT;
   output reg    [31: 0] Data1_2ID;
   output reg    [ 4: 0] writeRegister1_PR;
   output reg            MemtoReg1_PR;
   output reg            do_writeback1_PR;
   output reg	         MemRead_2DM;
   output reg            MemWrite_2DM;   

   input         [31: 0] aluResult1;
   input         [31: 0] data_read_fDM;
   input         [31: 0] Dest_Value1;
   input         [31: 0] readDataB1;
   input         [31: 0] writeData1_WB;
   input         [31: 0] Instr1;
   input         [ 5: 0] ALU_control1;
   input         [ 4: 0] writeRegister1_WB;
   input         [ 4: 0] writeRegister1;
   input                 ALUSrc1;
   input                 do_writeback1_WB;
   input                 do_writeback1;
   input                 CLK;
   input                 RESET;
   input                 MemtoReg1;
   input                 MemRead1;
   input                 MemWrite1;

   wire         [31: 0] data_read_aligned;
   wire         [31: 0] Dest_Value;
   wire         [31: 0] aluResult;
   wire         [31: 0] readDataB;
   wire         [31: 0] writeData_WB;
   wire         [31: 0] Instr;
   wire         [ 5: 0] ALU_control;
   wire                 MemRead;
   wire                 MemWrite;
   wire                 select1_WB;


   // TA: bypassing is missing here!

   assign Instr_OUT = (MemWrite2)?Instr2:Instr1;
   assign MemRead_2DM = (MemRead1|MemRead2);
   assign MemWrite_2DM = (MemWrite1|MemWrite2);
   assign select1_WB = (do_writeback1_WB&&(writeRegister1_WB==((MemWrite2)?writeRegister2:writeRegister1)))/*&&(!ALUSrc1)/**/;
   assign data_write_2DM = (select2_WB)?writeData2_WB:((select1_WB)?writeData1_WB:((MemWrite2)?readDataB2:readDataB1));
   assign data_address_2DM = (MemWrite2|MemRead2)?aluResult2:aluResult1;
   
   always
     begin
        data_read_aligned = writeData2_WB;
	ALU_control = (MemRead2)? ALU_control2: ALU_control1;//~DS//ALU_control = (MemRead2)? ALU_control1: ALU_control2;
	aluResult = (MemRead2)? aluResult2: aluResult1;//~DS//aluResult = (MemRead2)? aluResult1: aluResult2;
	
	       case(ALU_control)
        		6'b101101: begin //LWX
                        	case( aluResult[1:0] )
                       	   		0:  data_read_aligned = data_read_fDM;
                      	        	1:  data_read_aligned[31:8] = data_read_fDM[23:0];
                       	        	2:  data_read_aligned[31:16] = data_read_fDM[15:0];
                       	        	3:  data_read_aligned[31:24] = data_read_fDM[7:0];
               		        endcase
                        end
                	6'b101110: begin //LWX
                    		case( aluResult[1:0] )
                     		   	0:  data_read_aligned[7:0] = data_read_fDM[31:24];
                        		1:  data_read_aligned[15:0] = data_read_fDM[31:16];
                    		endcase
                	end
                	6'b100001: begin //LB
					1: data_read_aligned={{24{data_read_fDM[23]}},data_read_fDM[23:16]};
					0: data_read_aligned={{24{data_read_fDM[31]}},data_read_fDM[31:24]};
					default: begin end
		    		endcase
      	         	end
         	        6'b101011: begin //LH
		    		case( aluResult[1:0] )
					0:data_read_aligned={{16{data_read_fDM[15]}},data_read_fDM[15:0]};
					2:data_read_aligned={{16{data_read_fDM[31]}},data_read_fDM[31:16]};
					default: begin end
		    		endcase
                	end
                	6'b101010: begin //LBU
                    		case( aluResult[1:0] )
					0: data_read_aligned={24'b0,data_read_fDM[31:24]};
					1: data_read_aligned={24'b0,data_read_fDM[23:16]};
					default: begin end 
		    		endcase
                	end
                	6'b101100: begin //LHU
		    		case(aluResult%4)
					0:data_read_aligned={16'b0,data_read_fDM[15:0]};
	            			2:data_read_aligned={16'b0,data_read_fDM[31:16]};
					default: begin end
		    		endcase
                	end
                        default: data_read_aligned = data_read_fDM;  
		endcase
     end


    // Pipeline Register 1 (MEM/WB)
   always @ (posedge CLK or posedge RESET)
     begin
       if(RESET == 1'b0)
         begin
           MemtoReg1_PR <= 1'b0;
           writeRegister1_PR <= 5'b0;
           aluResult1_PR <= 32'b0;
           data_read1_PR <= 32'b0;
           do_writeback1_PR <= 1'b0;
         end
     end

endmodule

