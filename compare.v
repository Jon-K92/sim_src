//-----------------------------------------
//           Quick Compare
//-----------------------------------------
module compare(	Jump,
		OpA,
		OpB,
		Instr_input,
		taken
		);

	output wire taken;

	input      [31: 0] OpA;
	input      [31: 0] OpB;
	input      [31: 0] Instr_input;	
	input	           Jump;

	reg              br_taken;
	
	assign taken=br_taken|Jump;

	always begin
		case(Instr_input[31:26])
			6'b000001:begin
				case(Instr_input[20:16])
					5'b00000,5'b10000:br_taken=(OpA[31]==1)?1'b1:1'b0;		  	//BLTZ,BLTZAL
					5'b00001,5'b10001:br_taken=(OpA[31]==0)?1'b1:1'b0;	//BGEZ,BGEZAL
					default: br_taken=1'b0;
				endcase
			end			
			6'b000100:br_taken=(OpA==OpB)?1'b1:1'b0;						//BEQ
			6'b000101:br_taken=(OpA!=OpB)?1'b1:1'b0;						//BNE
			6'b000110:br_taken=((OpA[31]==1)||(OpA==0))?1'b1:1'b0;				//BLEZ
			6'b000111: begin
				br_taken=((OpA[31]==0)&&(OpA!=0))?1'b1:1'b0;			//BGTZ
			end
			default:br_taken=1'b0;
		endcase
	end

endmodule
