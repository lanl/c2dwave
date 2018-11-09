/* Travelling Salesman Problem (decision version)

The Travelling Salesman decision problem is: 
"Given a weighted graph (G) and an integereger (tspdsist), Is there a round trip hamiltonian path cost at least (tspdist) in (G)?"  
We solve this for the following, hard-wired graph (inside edge weights are for clockwise path, outside weights are for counter clockwise path)
The output is the order of cities to be visited, where each city i.e a=>3 means that city 'a' is the third city to be visited.
             
            A
         3_/ |
     1   /1  |
   S----C   1|1
         \_1 |
          2\ |
            B 

 Author: Mohamed Hassan <mwasfy@lanl.gov>
*/

bool TSP (int a, int b, int c, int tspdist)
{
	bool valid;
	int arr_s [4];
	int arr_a [4];
	int arr_b [4];
	int arr_c [4];


//starting city S costs
		arr_s[0] = 31;
		arr_s[1] = 31;
		arr_s[2] = 31;
		arr_s[3] = 1;
//City A costs
		arr_a[0] = 31;
		arr_a[1] = 31;
		arr_a[2] = 1;
		arr_a[3] = 1;
//City B costs
		arr_b[0] = 31;
		arr_b[1] = 1;
		arr_b[2] = 31;
		arr_b[3] = 2;
//City C costs
		arr_c[0] = 1;
		arr_c[1] = 3;
		arr_c[2] = 1;
		arr_c[3] = 31;

	int totcost;
	totcost = arr_s[3] + arr_a[a] + arr_b[b] + arr_c[c];
	

	if(totcost<tspdist && a > 0 && b > 0 && c > 0 && a < 4 && b < 4 && c < 4 && a != b && a != c && c != b)
	{
		valid = 1;
	}
	else
	{
		valid = 0;
	}
	return valid;
}
