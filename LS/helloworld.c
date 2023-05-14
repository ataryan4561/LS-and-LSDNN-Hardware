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
float t3=0;
float avg_err_LS_ps;
float avg_err_LS_hw1;
float avg_err_LS_hw2;
float avg_err_LS_hw3;
float avg_err_LS_hw4;
float NMSE_LS_ps;
float NMSE_LS_hw1;
float NMSE_LS_hw2;
float NMSE_LS_hw3;
float NMSE_LS_hw4;
void LS()
{
    XTime time_LS_start , time_LS_end;
    /////////////////////////////////////////////      LS      /////////////////////////////////////////////

        // variable to store the channel response
           float LS_out[104][100];
           XTime_SetTime(0);
           XTime_GetTime(&time_LS_start);
        // nested for loop for element-wise division
            for (int i=0;i<104;i++) //Iterating for 104 YinC values with 52 real with its 52 imaginary values cascaded together
            {
                for(int j=0; j<100;j++) //Iterating for 100 such channels
                {
                    LS_out[i][j]=YinC[SNR][i][j]/Xp[i];
                }
            }

            XTime_GetTime(&time_LS_end); // end of time

        // variable to store difference between calculated and actual channel response
        float diff_LS[104][100];

        // nested loop to calculate element wise difference between calculated and actual channel response
        for (int i=0;i<104;i++) {
                    for(int j=0; j<100;j++)
                    {
                        diff_LS[i][j]=LS_out[i][j]-actC[SNR][i][j];
                    }
                 }

        // variable to store the norm of the difference for 100 channels
        float err_LS[100];
        for (int i=0;i<100;i++)
        {
            err_LS[i]=0;
        }

        // nested loop to calculate the norm of the difference
        for (int i=0;i<100;i++) {
             for(int j=0; j<52;j++) {
                   err_LS[i] += (diff_LS[j][i]*diff_LS[j][i])+(diff_LS[52+j][i]*diff_LS[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
              }
        }

        float sum_err_LS = 0;

        // computing the sum of all the 100 normalized error
        for (int i=0; i<100; i++) {
            sum_err_LS += err_LS[i];
        }
        // Averaging over 100 channels
        avg_err_LS_ps = sum_err_LS / 100;

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

       printf("\nAverage normalized error for LS at %d dB is: %.10lf ", SNR*5, avg_err_LS_ps);

       NMSE_LS_ps = avg_err_LS_ps/Phf_avg; //Error normalized with channel power

       printf("\nNMSE for LS at %d dB : %.10f", SNR*5, NMSE_LS_ps);

       printf("\n-----------EXECUTION TIME FOR LS--------------------\n");
       float time_LS = 0;
       time_LS = (float)1.0 * (time_LS_end - time_LS_start) / (COUNTS_PER_SECOND/1000000);
       printf("Execution Time for LS in Micro-Seconds : %f\n" , time_LS);
}
int LS_FPGA()
{
	float LS_out[104][100];
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
	 XTime time_LS_start , time_LS_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LS_start);
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
			  XTime_GetTime(&time_LS_end);
			  printf("\n-----------LS %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LS = 0;
			  time_LS = (float)1.0 * (time_LS_end - time_LS_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for ACP FPGA in Micro-Seconds : %f\n" , time_LS);
			  t=t+time_LS;
			  index=0;
			  while(index<104)
			  {
				  LS_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
	}
   // variable to store difference between calculated and actual channel response
	float diff_LS[104][100];
	for(int i=0; i<104; i++)
	{
		for(int j=0; j<100; j++)
	    {
			 diff_LS[i][j]=0;
		 }
	}
	// nested loop to calculate element wise difference between calculated and actual channel response
    for (int i=0;i<104;i++)
	{
		for(int j=0; j<100;j++)
		{
			diff_LS[i][j]=LS_out[i][j]-actC[SNR][i][j];
		}
	}
	// variable to store the norm of the difference for 100 channels
	float err_LS[100];
	for (int i=0;i<100;i++)
	    {
		 err_LS[i]=0;
		}
	// nested loop to calculate the norm of the difference
	for (int i=0;i<100;i++)
   {
	for(int j=0; j<52;j++)
      {
		err_LS[i] += (diff_LS[j][i]*diff_LS[j][i])+(diff_LS[52+j][i]*diff_LS[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
	  }
	}
	float sum_err_LS = 0;
	// computing the sum of all the 100 normalized error
	for (int i=0; i<100; i++)
	{
	    sum_err_LS += err_LS[i];
	}
    // Averaging over 100 channels
	avg_err_LS_hw1 = sum_err_LS / 100;
    //Average channel power calculation
	float Phf = 0;
	float Phf_avg = 0;
	// nested loop to calculate the norm of the actual channel
	for (int i=0;i<100;i++)
	{
		for(int j=0; j<52;j++)
		{
			Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
	    }
	}
	// Averaging over 100 channels
	Phf_avg = Phf / 100;
    printf("\nAverage normalized error for LS at %d dB is: %.10lf ", SNR*5, avg_err_LS_hw1);
	NMSE_LS_hw1 = avg_err_LS_hw1/Phf_avg; //Error normalized with channel power
	printf("\nNMSE for LS at %d dB : %.10f", SNR*5, NMSE_LS_hw1);
	 return 0;
}
int LS_FPGA1()
{
	float LS_out[104][100];
	for(int f=0; f<100; f++)
 {
	float DMA_input[104];
	float Find_output_DMA[104];
	int status;
	 XAxiDma_Config *DMA_confptracp; //DMA configuration pointer
	 XAxiDma AxiDMAacp; // DMA instance pointer
	 DMA_confptracp = XAxiDma_LookupConfig(XPAR_AXI_DMA_2_DEVICE_ID);
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
	 XTime time_LS_start , time_LS_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LS_start);
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
			  XTime_GetTime(&time_LS_end);
			  printf("\n-----------LS %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LS = 0;
			  time_LS = (float)1.0 * (time_LS_end - time_LS_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for LS FPGA in Micro-Seconds : %f\n" , time_LS);
			  t1=t1+time_LS;
			  index=0;
			  while(index<104)
			  {
				  LS_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
	}
   // variable to store difference between calculated and actual channel response
	float diff_LS[104][100];
	for(int i=0; i<104; i++)
	{
		for(int j=0; j<100; j++)
	    {
			 diff_LS[i][j]=0;
		 }
	}
	// nested loop to calculate element wise difference between calculated and actual channel response
    for (int i=0;i<104;i++)
	{
		for(int j=0; j<100;j++)
		{
			diff_LS[i][j]=LS_out[i][j]-actC[SNR][i][j];
		}
	}
	// variable to store the norm of the difference for 100 channels
	float err_LS[100];
	for (int i=0;i<100;i++)
	    {
		 err_LS[i]=0;
		}
	// nested loop to calculate the norm of the difference
	for (int i=0;i<100;i++)
   {
	for(int j=0; j<52;j++)
      {
		err_LS[i] += (diff_LS[j][i]*diff_LS[j][i])+(diff_LS[52+j][i]*diff_LS[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
	  }
	}
	float sum_err_LS = 0;
	// computing the sum of all the 100 normalized error
	for (int i=0; i<100; i++)
	{
	    sum_err_LS += err_LS[i];
	}
    // Averaging over 100 channels
	avg_err_LS_hw2 = sum_err_LS / 100;
    //Average channel power calculation
	float Phf = 0;
	float Phf_avg = 0;
	// nested loop to calculate the norm of the actual channel
	for (int i=0;i<100;i++)
	{
		for(int j=0; j<52;j++)
		{
			Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
	    }
	}
	// Averaging over 100 channels
	Phf_avg = Phf / 100;
    printf("\nAverage normalized error for LS at %d dB is: %.10lf ", SNR*5, avg_err_LS_hw2);
	NMSE_LS_hw2 = avg_err_LS_hw2/Phf_avg; //Error normalized with channel power
	printf("\nNMSE for LS at %d dB : %.10f", SNR*5, NMSE_LS_hw2);
	 return 0;
}
int LS_FPGA2()
{
	float LS_out[104][100];
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
	 XTime time_LS_start , time_LS_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LS_start);
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
			  XTime_GetTime(&time_LS_end);
			  printf("\n-----------LS %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LS = 0;
			  time_LS = (float)1.0 * (time_LS_end - time_LS_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for LS FPGA in Micro-Seconds : %f\n" , time_LS);
			  t2=t2+time_LS;
			  index=0;
			  while(index<104)
			  {
				  LS_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
	}
   // variable to store difference between calculated and actual channel response
	float diff_LS[104][100];
	for(int i=0; i<104; i++)
	{
		for(int j=0; j<100; j++)
	    {
			 diff_LS[i][j]=0;
		 }
	}
	// nested loop to calculate element wise difference between calculated and actual channel response
    for (int i=0;i<104;i++)
	{
		for(int j=0; j<100;j++)
		{
			diff_LS[i][j]=LS_out[i][j]-actC[SNR][i][j];
		}
	}
	// variable to store the norm of the difference for 100 channels
	float err_LS[100];
	for (int i=0;i<100;i++)
	    {
		 err_LS[i]=0;
		}
	// nested loop to calculate the norm of the difference
	for (int i=0;i<100;i++)
   {
	for(int j=0; j<52;j++)
      {
		err_LS[i] += (diff_LS[j][i]*diff_LS[j][i])+(diff_LS[52+j][i]*diff_LS[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
	  }
	}
	float sum_err_LS = 0;
	// computing the sum of all the 100 normalized error
	for (int i=0; i<100; i++)
	{
	    sum_err_LS += err_LS[i];
	}
    // Averaging over 100 channels
	avg_err_LS_hw3 = sum_err_LS / 100;
    //Average channel power calculation
	float Phf = 0;
	float Phf_avg = 0;
	// nested loop to calculate the norm of the actual channel
	for (int i=0;i<100;i++)
	{
		for(int j=0; j<52;j++)
		{
			Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
	    }
	}
	// Averaging over 100 channels
	Phf_avg = Phf / 100;
    printf("\nAverage normalized error for LS at %d dB is: %.10lf ", SNR*5, avg_err_LS_hw3);
	NMSE_LS_hw3 = avg_err_LS_hw3/Phf_avg; //Error normalized with channel power
	printf("\nNMSE for LS at %d dB : %.10f", SNR*5, NMSE_LS_hw3);
	 return 0;
}
int LS_FPGA3()
{
	float LS_out[104][100];
	for(int f=0; f<100; f++)
 {
	float DMA_input[104];
	float Find_output_DMA[104];
	int status;
	 XAxiDma_Config *DMA_confptracp; //DMA configuration pointer
	 XAxiDma AxiDMAacp; // DMA instance pointer
	 DMA_confptracp = XAxiDma_LookupConfig(XPAR_AXI_DMA_4_DEVICE_ID);
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
	 XTime time_LS_start , time_LS_end;
	             XTime_SetTime(0);
	             XTime_GetTime(&time_LS_start);
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
			  XTime_GetTime(&time_LS_end);
			  printf("\n-----------LS %d FPGA EXECUTION TIME--------------------\n",f);
			  float time_LS = 0;
			  time_LS = (float)1.0 * (time_LS_end - time_LS_start) / (COUNTS_PER_SECOND/1000000);
			  printf("Execution Time for LS FPGA in Micro-Seconds : %f\n" , time_LS);
			  t3=t3+time_LS;
			  index=0;
			  while(index<104)
			  {
				  LS_out[index][f]=Find_output_DMA[index];
				  index++;
			  }
	}
   // variable to store difference between calculated and actual channel response
	float diff_LS[104][100];
	for(int i=0; i<104; i++)
	{
		for(int j=0; j<100; j++)
	    {
			 diff_LS[i][j]=0;
		 }
	}
	// nested loop to calculate element wise difference between calculated and actual channel response
    for (int i=0;i<104;i++)
	{
		for(int j=0; j<100;j++)
		{
			diff_LS[i][j]=LS_out[i][j]-actC[SNR][i][j];
		}
	}
	// variable to store the norm of the difference for 100 channels
	float err_LS[100];
	for (int i=0;i<100;i++)
	    {
		 err_LS[i]=0;
		}
	// nested loop to calculate the norm of the difference
	for (int i=0;i<100;i++)
   {
	for(int j=0; j<52;j++)
      {
		err_LS[i] += (diff_LS[j][i]*diff_LS[j][i])+(diff_LS[52+j][i]*diff_LS[52+j][i]);  //calculating the euclediean norm of the set of 52 complex values in a channel
	  }
	}
	float sum_err_LS = 0;
	// computing the sum of all the 100 normalized error
	for (int i=0; i<100; i++)
	{
	    sum_err_LS += err_LS[i];
	}
    // Averaging over 100 channels
	avg_err_LS_hw4 = sum_err_LS / 100;
    //Average channel power calculation
	float Phf = 0;
	float Phf_avg = 0;
	// nested loop to calculate the norm of the actual channel
	for (int i=0;i<100;i++)
	{
		for(int j=0; j<52;j++)
		{
			Phf += (actC[SNR][j][i]*actC[SNR][j][i]) + (actC[SNR][52+j][i]*actC[SNR][52+j][i]);  //calculating the euclediean norm of the set of 52 complex values of the actual channel response
	    }
	}
	// Averaging over 100 channels
	Phf_avg = Phf / 100;
    printf("\nAverage normalized error for LS at %d dB is: %.10lf ", SNR*5, avg_err_LS_hw4);
	NMSE_LS_hw4 = avg_err_LS_hw4/Phf_avg; //Error normalized with channel power
	printf("\nNMSE for LS at %d dB : %.10f", SNR*5, NMSE_LS_hw4);
	 return 0;
}
int check(int g1, int g2)
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
	LS();
	LS_FPGA();
	LS_FPGA1();
	LS_FPGA2();
	LS_FPGA3();
	if(!check(avg_err_LS_ps,avg_err_LS_hw1) && !check(avg_err_LS_ps,avg_err_LS_hw2) && !check(avg_err_LS_ps,avg_err_LS_hw3) && !check(avg_err_LS_ps,avg_err_LS_hw4))
	{
		printf("We are all good\n");
	}
	else
	{
		printf("We are not good at all\n");
	}
	printf("\n");
	printf("Total Time Taken for 100 Channels for 1 %lf\n",t);
	printf("For Single Channel Average Time %lf\n",t/100);
	printf("Total Time Taken for 100 Channels for 2 %lf\n",t1);
	printf("For Single Channel Average Time %lf\n",t1/100);
	printf("Total Time Taken for 100 Channels for 3  %lf\n",t2);
	printf("For Single Channel Average Time %lf\n",t2/100);
	printf("Total Time Taken for 100 Channels for 4 %lf\n",t3);
	printf("For Single Channel Average Time %lf\n",t3/100);
	cleanup_platform();
	return 0;
}
