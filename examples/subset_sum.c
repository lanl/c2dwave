bool partial_sum(int a , int b , int c , int psum)
{
	bool valid[2];
	int arr[4];
	int sum;
	int reg_i;
	bool ch1; 
	bool ch2; 
	bool ch3; 
	//initialize the array 
	for (reg_i=0;reg_i<4;reg_i++)
	{
		arr[reg_i]=reg_i;
	}
	sum = arr[a] + arr[b] + arr[c];
	if((sum == psum) && a<4 && b<4 && c<4)
		valid[0] = 1;
	else
		valid[0] = 0;

	ch1 = a !=b ? 1 : (a !=0 ? 1 : 0);
	ch2 = a !=c ? 1 : (a !=0 ? 1 : 0);
	ch3 = c !=b ? 1 : (a !=0 ? 1 : 0);

	if(valid[0] && ch1 && ch2 && ch3)
	{
		valid[1] = 1;
	}
	else
	{
		valid[1] = 0;
	}
	return valid[1];
}
