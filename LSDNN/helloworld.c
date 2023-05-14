#include <stdio.h>
#include <xtime_l.h>
#include <complex.h>
#include "platform.h"
#include "xil_printf.h"
#include "what.h"
#include<math.h>
#include "xaxidma.h"
#include "xparameters.h"
#include "platform.h"
#define SNR_in_dB SNR*5
#define SNR 6
float t=0;
float t1=0;
float t2=0;
float avg_err_LSDNN_ps;
float avg_err_LSDNN_hw1;
float avg_err_LSDNN_hw2;
float avg_err_LSDNN_hw3;
float NMSE_LSDNN_ps;
float NMSE_LSDNN_hw1;
float NMSE_LSDNN_hw2;
float NMSE_LSDNN_hw3;
void LSDNN()
{
	XTime time_LSDNN_start , time_LSDNN_end;
	    /////////////////////////////////////////////      LS      /////////////////////////////////////////////

		// variable to store the channel response
		float LS_out[104][100];

		// For execution time calculation
		XTime_SetTime(0);
		XTime_GetTime(&time_LSDNN_start);

		// nested for loop for element-wise division
		for (int i=0;i<104;i++) //Iterating for 104 YinC values with 52 real with its 52 imaginary values cascaded together
		{
			for(int j=0; j<100;j++) //Iterating for 100 such channels
			{
				LS_out[i][j] = YinC[SNR][i][j]/Xp[i];
			}
		}


		/////////////////////////////////////////////     LS-DNN      /////////////////////////////////////////////

		   float DNN_in[104][100] = {}; //2D matrix to provide the input to the DNN
		   float DNN_out[104][100] = {}; //2D matrix to store DNN output values
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



		   //Reversing Data Normalization process by performing (Y*Ystd_deviation + Ymean)
			for (int i=0;i<100;i++)
		   {
			   for (int j=0;j<104;j++)
			   {
				   DNN_out[j][i]=DNN_layer_2[j][i]*std_op[j] + mean_op[j];
			   }
		   }

			XTime_GetTime(&time_LSDNN_end); //End of time

		   // variable to store difference between calculated and actual channel response
		   float diff_LSDNN[104][100] = {};

		   // nested loop to calculate difference between calculated and actual channel response
		   for (int i=0;i<104;i++) {
					for(int j=0; j<100;j++)
					   {
						diff_LSDNN[i][j]=DNN_out[i][j]-actC[SNR][i][j];
					}
				 }

		   // variable to store the norm of the difference for 100 channels
		   float err_LSDNN[100] = {};
		   for(int i=0;i<100;i++)
		   {
			   err_LSDNN[i]=0;
		   }

		   // nested loop to calculate the norm of the difference
		   for (int i=0;i<100;i++) {
			 for(int j=0; j<52;j++) {
				   err_LSDNN[i] += (diff_LSDNN[j][i]*diff_LSDNN[j][i]) + (diff_LSDNN[52+j][i]*diff_LSDNN[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
			  }
		   }


		   float sum_err_LSDNN = 0;

		   // computing the sum of all the 100 normalized error
		   for (int i=0; i<100; i++) {
			sum_err_LSDNN += err_LSDNN[i];
		   }
		   // Averaging over 100 channels
		   avg_err_LSDNN_ps = sum_err_LSDNN / 100;


		   //Average channel power calculation
		   float Phf = 0;
		   float Phf_avg = 0;

		   // nested loop to calculate the norm of the actual channel
		   for (int i=0;i<100;i++) {
			 for(int j=0; j<52;j++) {
				   Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
			  }
		   }

		   // Averaging over 100 channels
		   Phf_avg = Phf / 100;


		   printf("\nAverage normalized error for LSDNN at %d dB is: %.10lf ", SNR*5, avg_err_LSDNN_ps);

		   NMSE_LSDNN_ps = avg_err_LSDNN_ps/Phf_avg; //Error normalized with channel power

		   printf("\nNMSE for LSDNN at %d dB : %.15f\n", SNR*5, NMSE_LSDNN_ps);


	    printf("\n-----------EXECUTION TIME FOR LSDNN--------------------\n");
	    float time_LSDNN = 0;
	    time_LSDNN = (float)1.0 * (time_LSDNN_end - time_LSDNN_start) / (COUNTS_PER_SECOND/1000000);
	    printf("Execution Time for LS-DNN in Micro-Seconds : %f\n" , time_LSDNN);
}
int LSDNN_FPGA()
{
	float DNN_out[104][100];
	for(int f=0; f<100; f++)
 {
	float DMA_input[104];
	float Find_output_DMA[104];
	int status;
	 XAxiDma_Config *DMA_confptracp; //DMA configuration pointer
	 XAxiDma AxiDMAacp; // DMA instance pointer
	 DMA_confptracp = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
	 status = XAxiDma_CfgInitialize(&AxiDMAacp, DMA_confptracp);
	 if(status != XST_SUCCESS)
	 {
		printf("ACP DMA Init Failed\t\n");
		return XST_FAILURE;
	 }
	 int index=0;
	 for(int i=0; i<104; i++)
	 {
	 		DMA_input[index]=YinC[SNR][i][f];
	 		index++;
	 }
	 XTime time_LSDNN_start , time_LSDNN_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LSDNN_start);
			   status = XAxiDma_SimpleTransfer(&AxiDMAacp, (UINTPTR)Find_output_DMA,(sizeof(float)*104),XAXIDMA_DEVICE_TO_DMA);
			   status = XAxiDma_SimpleTransfer(&AxiDMAacp, (UINTPTR)DMA_input, (sizeof(float)*104),XAXIDMA_DMA_TO_DEVICE);
			   status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
			   while(status!=0x00000002)
			   {
				 status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
			   }
			   status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
			  while(status!=0x00000002)
			  {
				status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
			  }
			  XTime_GetTime(&time_LSDNN_end);
			  printf("\n-----------LS-DNN %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LSDNN = 0;
			  time_LSDNN = (float)1.0 * (time_LSDNN_end - time_LSDNN_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for LS-DNN FPGA in Micro-Seconds : %f\n" , time_LSDNN);
			  t=t+time_LSDNN;
			  index=0;
			  while(index<104)
			  {
				  DNN_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
 }
	// variable to store difference between calculated and actual channel response
			   float diff_LSDNN[104][100] = {};

			   // nested loop to calculate difference between calculated and actual channel response
			   for (int i=0;i<104;i++) {
						for(int j=0; j<100;j++)
						   {
							diff_LSDNN[i][j]=DNN_out[i][j]-actC[SNR][i][j];
						}
					 }

			   // variable to store the norm of the difference for 100 channels
			   float err_LSDNN[100] = {};
			   for(int i=0;i<100;i++)
			   {
				   err_LSDNN[i]=0;
			   }

			   // nested loop to calculate the norm of the difference
			   for (int i=0;i<100;i++) {
				 for(int j=0; j<52;j++) {
					   err_LSDNN[i] += (diff_LSDNN[j][i]*diff_LSDNN[j][i]) + (diff_LSDNN[52+j][i]*diff_LSDNN[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
				  }
			   }


			   float sum_err_LSDNN = 0;

			   // computing the sum of all the 100 normalized error
			   for (int i=0; i<100; i++) {
				sum_err_LSDNN += err_LSDNN[i];
			   }
			   // Averaging over 100 channels
			   avg_err_LSDNN_hw1 = sum_err_LSDNN / 100;


			   //Average channel power calculation
			   float Phf = 0;
			   float Phf_avg = 0;

			   // nested loop to calculate the norm of the actual channel
			   for (int i=0;i<100;i++) {
				 for(int j=0; j<52;j++) {
					   Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
				  }
			   }

			   // Averaging over 100 channels
			   Phf_avg = Phf / 100;


			   printf("\nAverage normalized error for LSDNN at %d dB is: %.10lf ", SNR*5, avg_err_LSDNN_hw1);

			   NMSE_LSDNN_hw1 = avg_err_LSDNN_hw1/Phf_avg; //Error normalized with channel power

			   printf("\nNMSE for LSDNN at %d dB : %.15f\n", SNR*5, NMSE_LSDNN_hw1);
			   return 0;
}
int LSDNN_FPGA1()
{
	float DNN_out[104][100];
	for(int f=0; f<100; f++)
 {
	float DMA_input[104];
	float Find_output_DMA[104];
	int status;
	 XAxiDma_Config *DMA_confptracp; //DMA configuration pointer
	 XAxiDma AxiDMAacp; // DMA instance pointer
	 DMA_confptracp = XAxiDma_LookupConfig(XPAR_AXI_DMA_1_DEVICE_ID);
	 status = XAxiDma_CfgInitialize(&AxiDMAacp, DMA_confptracp);
	 if(status != XST_SUCCESS)
	 {
		printf("ACP DMA Init Failed\t\n");
		return XST_FAILURE;
	 }
	 int index=0;
	 for(int i=0; i<104; i++)
	 {
	 		DMA_input[index]=YinC[SNR][i][f];
	 		index++;
	 }
	 XTime time_LSDNN_start , time_LSDNN_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LSDNN_start);
			   status = XAxiDma_SimpleTransfer(&AxiDMAacp, (UINTPTR)Find_output_DMA,(sizeof(float)*104),XAXIDMA_DEVICE_TO_DMA);
			   status = XAxiDma_SimpleTransfer(&AxiDMAacp, (UINTPTR)DMA_input, (sizeof(float)*104),XAXIDMA_DMA_TO_DEVICE);
			   status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
			   while(status!=0x00000002)
			   {
				 status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
			   }
			   status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
			  while(status!=0x00000002)
			  {
				status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
			  }
			  XTime_GetTime(&time_LSDNN_end);
			  printf("\n-----------LS-DNN %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LSDNN = 0;
			  time_LSDNN = (float)1.0 * (time_LSDNN_end - time_LSDNN_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for LS-DNN FPGA in Micro-Seconds : %f\n" , time_LSDNN);
			  t1=t1+time_LSDNN;
			  index=0;
			  while(index<104)
			  {
				  DNN_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
	}
	// variable to store difference between calculated and actual channel response
			   float diff_LSDNN[104][100] = {};

			   // nested loop to calculate difference between calculated and actual channel response
			   for (int i=0;i<104;i++) {
						for(int j=0; j<100;j++)
						   {
							diff_LSDNN[i][j]=DNN_out[i][j]-actC[SNR][i][j];
						}
					 }

			   // variable to store the norm of the difference for 100 channels
			   float err_LSDNN[100] = {};
			   for(int i=0;i<100;i++)
			   {
				   err_LSDNN[i]=0;
			   }

			   // nested loop to calculate the norm of the difference
			   for (int i=0;i<100;i++) {
				 for(int j=0; j<52;j++) {
					   err_LSDNN[i] += (diff_LSDNN[j][i]*diff_LSDNN[j][i]) + (diff_LSDNN[52+j][i]*diff_LSDNN[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
				  }
			   }


			   float sum_err_LSDNN = 0;

			   // computing the sum of all the 100 normalized error
			   for (int i=0; i<100; i++) {
				sum_err_LSDNN += err_LSDNN[i];
			   }
			   // Averaging over 100 channels
			   avg_err_LSDNN_hw2 = sum_err_LSDNN / 100;


			   //Average channel power calculation
			   float Phf = 0;
			   float Phf_avg = 0;

			   // nested loop to calculate the norm of the actual channel
			   for (int i=0;i<100;i++) {
				 for(int j=0; j<52;j++) {
					   Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
				  }
			   }

			   // Averaging over 100 channels
			   Phf_avg = Phf / 100;


			   printf("\nAverage normalized error for LSDNN at %d dB is: %.10lf ", SNR*5, avg_err_LSDNN_hw2);

			   NMSE_LSDNN_hw2 = avg_err_LSDNN_hw2/Phf_avg; //Error normalized with channel power

			   printf("\nNMSE for LSDNN at %d dB : %.15f\n", SNR*5, NMSE_LSDNN_hw2);
			   return 0;
}
int LSDNN_FPGA2()
{
	float DNN_out[104][100];
	for(int f=0; f<100; f++)
 {
	float DMA_input[104];
	float Find_output_DMA[104];
	int status;
	 XAxiDma_Config *DMA_confptracp; //DMA configuration pointer
	 XAxiDma AxiDMAacp; // DMA instance pointer
	 DMA_confptracp = XAxiDma_LookupConfig(XPAR_AXI_DMA_3_DEVICE_ID);
	 status = XAxiDma_CfgInitialize(&AxiDMAacp, DMA_confptracp);
	 if(status != XST_SUCCESS)
	 {
		printf("ACP DMA Init Failed\t\n");
		return XST_FAILURE;
	 }
	 int index=0;
	 for(int i=0; i<104; i++)
	 {
	 		DMA_input[index]=YinC[SNR][i][f];
	 		index++;
	 }
	 XTime time_LSDNN_start , time_LSDNN_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LSDNN_start);
			   status = XAxiDma_SimpleTransfer(&AxiDMAacp, (UINTPTR)Find_output_DMA,(sizeof(float)*104),XAXIDMA_DEVICE_TO_DMA);
			   status = XAxiDma_SimpleTransfer(&AxiDMAacp, (UINTPTR)DMA_input, (sizeof(float)*104),XAXIDMA_DMA_TO_DEVICE);
			   status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
			   while(status!=0x00000002)
			   {
				 status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
			   }
			   status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
			  while(status!=0x00000002)
			  {
				status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
			  }
			  XTime_GetTime(&time_LSDNN_end);
			  printf("\n-----------LS-DNN %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LSDNN = 0;
			  time_LSDNN = (float)1.0 * (time_LSDNN_end - time_LSDNN_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for LS-DNN FPGA in Micro-Seconds : %f\n" , time_LSDNN);
			  t2=t2+time_LSDNN;
			  index=0;
			  while(index<104)
			  {
				  DNN_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
	}
	// variable to store difference between calculated and actual channel response
			   float diff_LSDNN[104][100] = {};

			   // nested loop to calculate difference between calculated and actual channel response
			   for (int i=0;i<104;i++) {
						for(int j=0; j<100;j++)
						   {
							diff_LSDNN[i][j]=DNN_out[i][j]-actC[SNR][i][j];
						}
					 }

			   // variable to store the norm of the difference for 100 channels
			   float err_LSDNN[100] = {};
			   for(int i=0;i<100;i++)
			   {
				   err_LSDNN[i]=0;
			   }

			   // nested loop to calculate the norm of the difference
			   for (int i=0;i<100;i++) {
				 for(int j=0; j<52;j++) {
					   err_LSDNN[i] += (diff_LSDNN[j][i]*diff_LSDNN[j][i]) + (diff_LSDNN[52+j][i]*diff_LSDNN[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
				  }
			   }


			   float sum_err_LSDNN = 0;

			   // computing the sum of all the 100 normalized error
			   for (int i=0; i<100; i++) {
				sum_err_LSDNN += err_LSDNN[i];
			   }
			   // Averaging over 100 channels
			   avg_err_LSDNN_hw3 = sum_err_LSDNN / 100;


			   //Average channel power calculation
			   float Phf = 0;
			   float Phf_avg = 0;

			   // nested loop to calculate the norm of the actual channel
			   for (int i=0;i<100;i++) {
				 for(int j=0; j<52;j++) {
					   Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
				  }
			   }

			   // Averaging over 100 channels
			   Phf_avg = Phf / 100;


			   printf("\nAverage normalized error for LSDNN at %d dB is: %.10lf ", SNR*5, avg_err_LSDNN_hw3);

			   NMSE_LSDNN_hw3 = avg_err_LSDNN_hw3/Phf_avg; //Error normalized with channel power

			   printf("\nNMSE for LSDNN at %d dB : %.15f\n", SNR*5, NMSE_LSDNN_hw3);
			   return 0;
}
int check(float g1,float g2)
{
	if(abs(g1-g2)>0.01)
	{
		return 1;
	}
	return 0;
}
int main()
{
	init_platform();
	LSDNN();
	LSDNN_FPGA();
	LSDNN_FPGA1();
	LSDNN_FPGA2();
	if(!check(NMSE_LSDNN_ps,NMSE_LSDNN_hw1) && !check(NMSE_LSDNN_ps,NMSE_LSDNN_hw2) && !check(NMSE_LSDNN_ps,NMSE_LSDNN_hw3))
	{
		printf("We are all good\n");
	}
	else
	{
		printf("We are not good at all\n");
	}
	printf("\n");
	printf("Total Time Taken for 100 Channels  for 1 %lf\n",t);
	printf("For Single Channel Average Time %lf\n",t/100);
	printf("Total Time Taken for 100 Channels  for 2 %lf\n",t1);
	printf("For Single Channel Average Time %lf\n",t1/100);
	printf("Total Time Taken for 100 Channels  for 3 %lf\n",t2);
	printf("For Single Channel Average Time %lf\n",t2/100);
	cleanup_platform();
	return 0;
}
