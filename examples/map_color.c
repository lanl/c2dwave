/*
Map of canada..............

NL=>0-----connected to ON(1),PE(5)
ON=>1-----connected to NL(0),MB(2),QC(6)
MB=>2-----connected to ON(1),SK(3),NU(7)
SK=>3-----connected to MB(2),AB(4),NT(8)
AB=>4-----connected to SK(3),NT(8),BC(11)
PE=>5-----connected to NL(0),QC(6)
QC=>6-----connected to ON(1),PE(5),NU(7),NB(9)
NU=>7-----connected to MB(2),QC(6),NT(8),NS(10)
NT=>8-----connected to SK(3),AB(4),NU(7),NS(10),BC(11),YT(12)
NB=>9-----connected to QC(6),NS(10)
NS=>10-----connected to NU(7),NT(8),NB(9)
BC=>11-----connected to AB(4),NT(8),YT(12)
YT=>12-----connected to NT(8),BC(11)

	arr[0][1] = 1;
	arr[0][5] = 1;

	arr[1][0] = 1;
	arr[1][2] = 1;
	arr[1][6] = 1;

	arr[2][1] = 1;
	arr[2][3] = 1;
	arr[2][7] = 1;

	arr[3][2] = 1;
	arr[3][4] = 1;
	arr[3][8] = 1;

	arr[4][3] = 1;
	arr[4][8] = 1;
	
	arr[5][0] = 1;
	arr[5][6] = 1;
	
	arr[6][1] = 1;
	arr[6][5] = 1;
	arr[6][7] = 1;
	arr[6][9] = 1;
	

	arr[7][2] = 1;
	arr[7][6] = 1;
	arr[7][8] = 1;

	arr[8][3] = 1;
	arr[8][4] = 1;
	arr[8][7] = 1;

	arr[9][6] = 1;

*/

bool map_color (int NL,int ON,int MB,int SK,int AB,int PE,int QC,int NU,int NT,int NB)
{
	bool reg_valid;
	int in_val[10];
	bool arr[10][10];
	bool check[10][10];

	int reg_a;
	int reg_b;
	int reg_i;
	int reg_j;
	int reg_x;
	int reg_y;
	
	int colors;

	colors = 4;
	

	
	//take input colors in an array of integers; this is step won't be necessary once we can support input arrays
	in_val[0] = NL;
	in_val[1] = ON;
	in_val[2] = MB;
	in_val[3] = SK;
	in_val[4] = AB;
	in_val[5] = PE;
	in_val[6] = QC;
	in_val[7] = NU;
	in_val[8] = NT;
	in_val[9] = NB;


	//initialize the 2d array with the connectivity between cities


	//zero out the rest of the 2d array
	for (int reg_i=0; reg_i < 10; reg_i = reg_i + 1)
	{
		for(int reg_j = 0; reg_j < 10; reg_j = reg_j + 1)
		{
			if((reg_i==0 && reg_j==1) || (reg_i==0 && reg_j==5) || (reg_i==1 && reg_j==0) || (reg_i==1 && reg_j==2) || (reg_i==1 && reg_j==6) || (reg_i==2 && reg_j==1) || (reg_i==2 && reg_j==3) || (reg_i==2 && reg_j==7) || (reg_i==3 && reg_j==2) || (reg_i==3 && reg_j==4) || (reg_i==3 && reg_j==8) || (reg_i==4 && reg_j==3) || (reg_i==4 && reg_j==8) || (reg_i==5 && reg_j==0) || (reg_i==5 && reg_j==6) || (reg_i==6 && reg_j==1) || (reg_i==6 && reg_j==5) || (reg_i==6 && reg_j==7) || (reg_i==6 && reg_j==9) || (reg_i==7 && reg_j==2) || (reg_i==7 && reg_j==6) || (reg_i==7 && reg_j==8) || (reg_i==8 && reg_j==3) || (reg_i==8 && reg_j==4) || (reg_i==8 && reg_j==7) || (reg_i==9 && reg_j==6))
			{
				arr[reg_i][reg_j] = 1;
			}
			else
			{
				arr[reg_i][reg_j] = 0;
			}
		}
	}

	//iterate on cities
	for (int reg_x = 0 ; reg_x < 10 ; reg_x = reg_x + 1)
	{
		for (int reg_y = 0 ; reg_y < 10 ; reg_y = reg_y + 1)
		{
			//if reg_a city is assigned reg_a color greater than the number of colors, then invalidate that input
			//if cities are connected and have same color, then invalidate the repective input
			if(((arr[reg_x][reg_y] == 1) && (in_val[reg_x] == in_val[reg_y])) || (in_val[reg_x] > colors) || (in_val[reg_y] > colors))
			{
				check[reg_x][reg_y] = 0;
			}
			else
			{
				check[reg_x][reg_y] = 1;
			}
		}
	}
	for (reg_a=0;reg_a<10;reg_a=reg_a+1)
	{
		for (reg_b=0;reg_b<10;reg_b=reg_b+1)
		{
			reg_valid = reg_valid && check[reg_a][reg_b];
		}
	}
	return reg_valid;
}

