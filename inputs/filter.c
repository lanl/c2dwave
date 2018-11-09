     int dummy (int a)
     {
       register int val;
       int i;
       val = 0;
       for (i = 0; i < 4; i++)
	{
	val += a;
	}
       return val;
     }
