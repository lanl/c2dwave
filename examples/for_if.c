int for_if(int in1 , int in2)
{
	int arr[5];
	int total;
	int reg_i;
	for(reg_i = 0; reg_i<5; reg_i=reg_i+1)
	{
		arr[reg_i]=reg_i;	
	}
	bool check; 
	check = (in1>in2);
	if(check)
	{
		total = arr[0] + arr[1] + arr[2] + arr[3] + arr[4]; 
	}
	else
	{
		total = in1 + in2;
	}
	return total;
}
