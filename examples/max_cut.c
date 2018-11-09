bool max_cut (bool a,bool b,bool c,bool d,bool e)
{
	register bool valid;
	int cut;
	register int reg_temp;
	register int reg_i;
	register int reg_j;
	register int reg_x;
	register int reg_y;	
	register int reg_z;
	register bool shit;
	register bool shittt;
	bool shit1;
	bool shit2;
	register bool shit3;


	valid=0;
	cut = 4;
	reg_temp = 0;
	reg_temp /= cut;
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
	for (reg_i=0; reg_i < 5; reg_i++)
	{
		for(reg_j = 0; reg_j < 5; reg_j = reg_j + 1)
		{
			if((reg_i==0 && reg_j==1) || (reg_i==0 && reg_j==2) || (reg_i==0 && reg_j==3) || (reg_i==1 && reg_j==0) || (reg_i==1 && reg_j==4) || (reg_i==2 && reg_j==0) || (reg_i==2 && reg_j==3) || (reg_i==3 && reg_j==0) || (reg_i==3 && reg_j==2) || (reg_i==3 && reg_j==4) || (reg_i==4 && reg_j==1) || (reg_i==4 && reg_j==3))
			{
				arr[reg_i][reg_j] = 1;
			}
			else
			{
				arr[reg_i][reg_j] = 0;
			}
		}
	}

	//iterate on nodes
	for (int reg_x = 0 ; reg_x < 5 ; reg_x = reg_x + 1)
	{
		for (int reg_y = 0 ; reg_y < 5 ; reg_y = reg_y + 1)
		{
			//if a city is assigned a color greater than the number of colors, then invalidate that input
			//if cities are connected and have same color, then invalidate the repective input
			if( (arr[reg_x][reg_y] == 1) && (in_val[reg_x] != in_val[reg_y]) )
			{
				reg_temp=reg_temp+1;
			}
		}
	}
	if(reg_temp>=2*cut)
	{
		valid = 1;
	}
	else
	{
		valid = 0;
	}
	return valid;
}

