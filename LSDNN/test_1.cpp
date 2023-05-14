//#include<bits/stdc++.h>
//#include"what.h"
//#include"a1_1.hpp"
//#include<iostream>
//// data of DNN for software and hardware
//float DNN_outsw[104];
//float DNN_outhw[104];
//// lsdnn software
//void benchmark(float in[104])
//{
//		float DNN_in[104];//2D matrix to provide the input to the DNN
//		float DNN_out[104];//2D matrix to store DNN output values
//		float DNN_layer_1[52];//Hidden Layer_1 2D matrix
//		float DNN_layer_2[104];//Output Layer 2D matrix
//		float LS_out[104];
//		// variable to store the channel response
//		for(int i=0; i<104; i++)
//		{
//			// ls operation
//			LS_out[i]=in[i]/Xp[i];
//			DNN_in[i]=(LS_out[i]-mean_ip[i])/std_ip[i];
//			//Data normalization by performing (Y-Ymean)/Ystd_deviation
//		}
//		// Hidden Layer_1 neuron calculation
//		for (int j=0;j<52;j++) //Iterating through 52 neurons in Hidden Layer_1
//		{
//			float sum = 0;
//			for(int k=0;k<104;k++) //Iterating through 104 set of values in a particular channel
//			{
//				sum += DNN_in[k]*L1_weight[k][j];//Performing summation of products of each DNN input value with a particular weight
//			}
//			DNN_layer_1[j]= sum + L1_bias[j];  //Adding a single bias value for a particular neuron
//			if(DNN_layer_1[j] < 0)
//			{
//				DNN_layer_1[j] = 0;
//			}
//			//Performing ReLU operation on each neuron, f(x) = max(0, x), for Hidden Layer_1 only
//		}
//		for (int j=0;j<104;j++) //Iterating through 104 neurons in Output Layer
//		{
//			float sum = 0;
//			for(int k=0;k<52;k++) //Iterating through 52 set of neurons of Hidden Layer_1 which are input to the Output layer
//			{
//				sum += DNN_layer_1[k]*L2_weight[k][j]; //Performing summation of products of each DNN input value with a particular weight
//
//			}
//			DNN_layer_2[j]= sum + L2_bias[j]; //Adding a single bias value for a particular neuron
//			DNN_outsw[j]=DNN_layer_2[j]*std_op[j] + mean_op[j];
//			 // taking lsdnn software data
//		}
//}
//int main()
//{
//	float in[104];
//	// input declaration
//	for(int i=0; i<104; i++)
//	{
//		in[i]=(float)YinC[0][i][0];
//		// taking input data
//	}
//	// axis_data stream declaration
//	axis_data local_stream;
//	hls::stream<axis_data> inn,outt;
//	for(int i=0; i<104; i++)
//	{
//		// taking data for stream
//		local_stream.data = in[i];
//		// update last
//		if(i==103)
//			local_stream.last=1;
//		else
//			local_stream.last=0;
//		// updata stream values
//		inn.write(local_stream);
//	}
//	lsdnn1(inn,outt);
//	// stream function call
//	for(int i=0; i<104; i++)
//	{
//		local_stream = outt.read();
//		DNN_outhw[i]=local_stream.data;
//		//taking data back in form of array for the compare
//	}
//	benchmark(in);
//	// software function call
//	for(int i=0; i<104; i++)
//	{
//		// comaparing the results
//		if(fabs(DNN_outsw[i]-DNN_outhw[i]>0.01))
//		{
//			// return 1 if there is a error
//			std::cout<<"There is a problem while running the testbench-> "<<DNN_outsw[i]<<" "<<DNN_outhw[i]<<" "<<i<<"\n";
//			return 1;
//		}
//		// return 0 if all good
//	}
//	return 0;
//}
