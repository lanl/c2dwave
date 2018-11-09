/* DWave-c2v: source to source translation of C-code to verilog-code compliant with edif2qmasm using clang tools */

// Begin conversion of 'max_cut' to Verilog 
module max_cut (
 input a,
 input b,
 input c,
 input d,
 input e,
 output result )
;
reg shit3;
wire shit2;
wire shit1;
reg shittt;
reg shit;
reg [4:0] reg_z;
reg [4:0] reg_y;
reg [4:0] reg_x;
reg [4:0] reg_j;
reg [4:0] reg_i;
reg [4:0] reg_temp;
wire [4:0] cut;
reg valid;
always@*
begin
	
	
	
	
	
	
		
	
	
	
	
	
	


	valid=0;
	cut = 4;
	reg_temp = 0;
	reg_temp = reg_temp / cut;
	//bool in_val[5];
	//bool arr[5][5];
	//take input colors in an array of integers; this is step won't be necessary once we can support input arrays
	in_val[0] = a;
	in_val[1] = b;
	in_val[2] = c;
	in_val[3] = d;
	in_val[4] = e;
	
	

	//initialize the 2d array with the connectivity between cities
	//zero out the rest of the 2d array
	for (reg_i=0; reg_i < 5; reg_i= reg_i + 1)
	begin
		for(reg_j = 0; reg_j < 5; reg_j = reg_j + 1)
		begin
			if((reg_i==0 && reg_j==1) || (reg_i==0 && reg_j==2) || (reg_i==0 && reg_j==3) || (reg_i==1 && reg_j==0) || (reg_i==1 && reg_j==4) || (reg_i==2 && reg_j==0) || (reg_i==2 && reg_j==3) || (reg_i==3 && reg_j==0) || (reg_i==3 && reg_j==2) || (reg_i==3 && reg_j==4) || (reg_i==4 && reg_j==1) || (reg_i==4 && reg_j==3))
			begin
				arr[reg_i][reg_j] = 1;
			end
			else
			begin
				arr[reg_i][reg_j] = 0;
			end
		end
	end

	//iterate on nodes
	for ( reg_x = 0 ; reg_x < 5 ; reg_x = reg_x + 1)
	begin
		for ( reg_y = 0 ; reg_y < 5 ; reg_y = reg_y + 1)
		begin
			//if a city is assigned a color greater than the number of colors, then invalidate that input
			//if cities are connected and have same color, then invalidate the repective input
			if( (arr[reg_x][reg_y] == 1) && (in_val[reg_x] != in_val[reg_y]) )
			begin
				reg_temp=reg_temp+1;
			end
		end
	end
	if(reg_temp>=2*cut)
	begin
		valid = 1;
	end
	else
	begin
		valid = 0;
	end
	result =  valid;
end
endmodule
// End of 'max_cut'


