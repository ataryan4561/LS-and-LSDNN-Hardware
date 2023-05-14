#include<bits/stdc++.h>
#include"what.h"
#include"a1.hpp"
#include<iostream>
// header file
float LSoutsw[104];
float LSouthw[104];
// output array from software and hardware
int main()
{
	float in[104];
	// input for the data in of 104 elements 
	for(int i=0; i<104; i++)
	{
		in[i]=(float)YinC[0][i][0];
		// taking snr zero and channel 0 data as input 
	}
	for(int i=0; i<104; i++)
	{
		LSoutsw[i]=in[i]/Xp[i];
		// ls operation 
	}
	axis_data local_stream;
	hls::stream<axis_data> inn,outt;
	// stream defination 
	for(int i=0; i<104; i++)
	{
		// taking data in into stream 
		local_stream.data = in[i];
		// update the last value
		if(i==103)
			local_stream.last=1;
		else
			local_stream.last=0;
		// writing the data 
		inn.write(local_stream);
	}
	ls4(inn,outt);
	// ls function calling 
	for(int i=0; i<104; i++)
	{
		// taking stream output data into array of 104  
		local_stream = outt.read();
		LSouthw[i]=local_stream.data;
	}
	for(int i=0; i<104; i++)
	{
		// comapring the software and the hardware data 
		if(fabs(LSoutsw[i]-LSouthw[i]>0.01))
		{
			// call if the fault 
			std::cout<<"There is a problem while running the testbench-> "<<i<<"\n";
			return 1;
		}
	}
	// else return 0
	return 0;
}
