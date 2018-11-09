/* DWave-c2v: source to source translation of C-code to verilog-code compliant with edif2qmasm using clang tools */

// Begin conversion of 'partial_sum' to Verilog 
module partial_sum(
 input [4:0] a , 
 input [4:0] b , 
 input [4:0] c , 
 input [4:0] psum,
 output result )
;
wire ch3;
wire ch2;
wire ch1;
wire [4:0] reg_i;
wire [4:0] sum;
wire [4:0] arr [3:0];
wire valid [1:0];
always@*
begin
	
	
	
	
	 
	 
	 
	//initialize the array 
	for (reg_i=0;reg_i<4;reg_i= reg_i + 1)
	begin
		arr[reg_i]=reg_i;
	end
	sum = arr[a] + arr[b] + arr[c];
	if((sum == psum) && a<4 && b<4 && c<4)
		begin
		valid[0] = 1;
		end
	else
		begin
		valid[0] = 0;
		end

	ch1 = a !=b ? 1 : (a !=0 ? 1 : 0);
	ch2 = a !=c ? 1 : (a !=0 ? 1 : 0);
	ch3 = c !=b ? 1 : (a !=0 ? 1 : 0);

	if(valid[0] && ch1 && ch2 && ch3)
	begin
		valid[1] = 1;
	end
	else
	begin
		valid[1] = 0;
	end
	result =  valid[1];
end
endmodule
// End of 'partial_sum'

