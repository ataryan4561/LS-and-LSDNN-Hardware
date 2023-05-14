#include"a1.hpp"
#ifdef solution1
// interface to define the LS IP using 104 input values float values
void ls1(hls::stream<axis_data> &A,hls::stream<axis_data> &C)
{
	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis register both port=C
	#pragma HLS INTERFACE axis register both port=A
	// defining the HLS interface axis port 
	float LS_out[104];
	float YinC[104];
	// describing the input and output parameters 
	int Xp[104]={1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1};
	axis_data lo_do,co;
	for(int i=0; i<104; i++)
	{
		// taking input of 104 using loop
			#pragma HLS PIPELINE
			// HLS PIPELINE to decrease the time
			lo_do = A.read();
			YinC[i]=lo_do.data;
			// data taking from the stream
			LS_out[i]=YinC[i]/Xp[i];
			// lS operation 
			co.data = LS_out[i];
			// update the stream last when last is incounter
			if(i==103)
				co.last=1;
			else
				co.last=0;
			C.write(co);
	}
}
#endif
#ifdef solution2
// interface to define the LS IP using 104 input values half floating
void ls2(hls::stream<axis_data> &A,hls::stream<axis_data> &C)
{
	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis register both port=C
	#pragma HLS INTERFACE axis register both port=A
	// defining the HLS interface axis port 
	half LS_out[104];
	half YinC[104];
	// describing the input and output parameters 
	half Xp[104]={1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1};
	axis_data lo_do,co;
	for(int i=0; i<104; i++)
	{
		// taking input of 104 using loop
			#pragma HLS PIPELINE
			// HLS PIPELINE to decrease the time
			lo_do = A.read();
			YinC[i]=lo_do.data;
			// data taking from the stream
			LS_out[i]=YinC[i]/Xp[i];
			// LS operation 
			co.data = (float)LS_out[i];
			// update the stream last when last is incounter
			if(i==103)
				co.last=1;
			else
				co.last=0;
			C.write(co);
	}
}
#endif
#ifdef solution3
// interface to define the LS IP using 104 input values apstream type
void ls3(hls::stream<axis_data> &A,hls::stream<axis_data> &C)
{
	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis register both port=C
	#pragma HLS INTERFACE axis register both port=A
	// defining the HLS interface axis port 
	lets LS_out[104];
	lets YinC[104];
	// describing the input and output parameters 
	lets Xp[104]={1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1};
	axis_data lo_do,co;
	for(int i=0; i<104; i++)
	{
		// taking input of 104 using loop
			#pragma HLS PIPELINE
			// HLS PIPELINE to decrease the time
			lo_do = A.read();
			YinC[i]=lo_do.data;
			// data taking from the stream
			LS_out[i]=YinC[i]/Xp[i];
			// LS operation 
			co.data = (float)LS_out[i];
			// update the stream last when last is incounter
			if(i==103)
				co.last=1;
			else
				co.last=0;
			C.write(co);
	}
}
#endif
#ifdef solution4
// interface to define the LS IP using 104 input values apstream type
void ls4(hls::stream<axis_data> &A,hls::stream<axis_data> &C)
{
	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS INTERFACE axis register both port=C
	#pragma HLS INTERFACE axis register both port=A
	// defining the HLS interface axis port 
	lets1 LS_out[104];
	lets1 YinC[104];
	// describing the input and output parameters 
	lets1 Xp[104]={1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1};
	axis_data lo_do,co;
	for(int i=0; i<104; i++)
	{
		// taking input of 104 using loop
			#pragma HLS PIPELINE
			// HLS PIPELINE to decrease the time
			lo_do = A.read();
			YinC[i]=lo_do.data;
			// data taking from the stream
			LS_out[i]=YinC[i]/Xp[i];
			// LS operation 
			co.data = (float)LS_out[i];
			// update the stream last when last is incounter
			if(i==103)
				co.last=1;
			else
				co.last=0;
			C.write(co);
	}
}
#endif
