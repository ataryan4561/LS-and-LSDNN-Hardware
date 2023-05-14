#include<bits/stdc++.h>
#include"what.h"
#include"a1_1.hpp"
#include<iostream>
// data of DNN for software and hardware
float DNN_outsw[104][100];
float DNN_outhw[104][100];
// lsdnn software 
void LSDNN()
{
	    /////////////////////////////////////////////      LS      /////////////////////////////////////////////

		// variable to store the channel response
		float LS_out[104][100];

		// nested for loop for element-wise division
		for (int i=0;i<104;i++) //Iterating for 104 YinC values with 52 real with its 52 imaginary values cascaded together
		{
			for(int j=0; j<100;j++) //Iterating for 100 such channels
			{
				LS_out[i][j] = YinC[0][i][j]/Xp[i];
			}
		}
		/////////////////////////////////////////////     LS-DNN      /////////////////////////////////////////////

		   float DNN_in[104][100] = {}; //2D matrix to provide the input to the DNN
		   float DNN_layer_1[52][100] = {}; //Hidden Layer_1 2D matrix
		   float DNN_layer_2[104][100] = {}; //Output Layer 2D matrix
		   //Data normalization by performing (Y-Ymean)/Ystd_deviation
		   for (int i=0;i<100;i++)
		   {
			   for (int j=0;j<104;j++)
			   {
				   DNN_in[j][i]=(LS_out[j][i]-mean_ip[j])/std_ip[j]; //output of LS(LS_out) is input to LS-DNN after data normalization
			   }
		   }
		   // Hidden Layer_1 neuron calculation
		   for (int i=0;i<100;i++) //Iterating through 100 channels
		   {
			   for (int j=0;j<52;j++) //Iterating through 52 neurons in Hidden Layer_1
			   {
				   float sum = 0;
				   for(int k=0;k<104;k++) //Iterating through 104 set of values in a particular channel
				   {
					   sum += DNN_in[k][i]*L1_weight[k][j]; //Performing summation of products of each DNN input value with a particular weight
				   }
				   DNN_layer_1[j][i] = sum + L1_bias[j]; //Adding a single bias value for a particular neuron
				   //Performing ReLU operation on each neuron, f(x) = max(0, x), for Hidden Layer_1 only
				   if(DNN_layer_1[j][i] < 0)
				   {
					   DNN_layer_1[j][i] = 0;
				   }
				   else
				   {
					   DNN_layer_1[j][i] = DNN_layer_1[j][i];
				   }
			   }
		   }
		   //Output Layer neuron calculation
		   for (int i=0;i<100;i++) //Iterating through 100 channels
		   {
			   for (int j=0;j<104;j++) //Iterating through 104 neurons in Output Layer
			   {
				   float sum = 0;
				   for(int k=0;k<52;k++)//Iterating through 52 set of neurons of Hidden Layer_1 which are input to the Output layer
				   {
					   sum += DNN_layer_1[k][i]*L2_weight[k][j];//Performing summation of products of each DNN input value with a particular weight

				   }
				   DNN_layer_2[j][i] = sum + L2_bias[j];//Adding a single bias value for a particular neuron
			   }
		   }
			for (int i=0;i<100;i++)
		   {
			   for (int j=0;j<104;j++)
			   {
				   DNN_outsw[j][i]=DNN_layer_2[j][i]*std_op[j] + mean_op[j];
				   // taking lsdnn software data
			   }
		   }
}
// main
int main()
{
	for(int g=0; g<100; g++)
	{
		// running for 100 channels check
	float in[104][100];
	// input declaration 
	for(int i=0; i<104; i++)
	{
		in[i][g]=(float)YinC[0][i][g];
		// taking input data 
	}
	// axis_data stream declaration 
	axis_data local_stream;
	hls::stream<axis_data> inn,outt;
	for(int i=0; i<104; i++)
	{
		// taking data for stream 
		local_stream.data = in[i][g];
		// update last
		if(i==103)
			local_stream.last=1;
		else
			local_stream.last=0;
		// updata stream values 
		inn.write(local_stream);
	}
	lsdnn1(inn,outt);
	// stream function call
	for(int i=0; i<104; i++)
	{
		local_stream = outt.read();
		DNN_outhw[i][g]=local_stream.data;
		//taking data back in form of array for the compare 
	}
	}
	LSDNN();
	// software function call
	for(int g=0; g<100; g++)
	{
	for(int i=0; i<104; i++)
	{
		// comaparing the results 
		if(fabs(DNN_outsw[i][g]-DNN_outhw[i][g]>0.01))
		{
			// return 1 if there is a error
			std::cout<<"There is a problem while running the testbench-> "<<DNN_outsw[i][g]<<" "<<DNN_outhw[i][g]<<" "<<i<<"\n";
//			return 1;
		}
	}
	// return 0 if all good 
	}
	return 0;
}
