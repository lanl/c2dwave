/* DWave-c2v: source to source translation of C-code to verilog-code compliant with edif2qmasm using clang tools */

     // Begin conversion of 'dummy' to Verilog 
      module dummy (
 input [4:0] a,
      output [4:0] result )
     ;
wire [4:0] i;
reg [4:0] val;
always@*
begin
               val = 0;
       for (i = 0; i < 4; i++)
	begin
	val += a;
	end
       result =  val;
     end
endmodule
     // End of 'dummy'
     
