#include "ap_int.h"
#include "hls_stream.h"
// interface for the LS header file 
#define solution4
#ifdef solution1
#endif
// solution1 float type 
#ifdef solution2
#endif
// solution2 half type
#ifdef solution3
#include <ap_fixed.h>
typedef ap_fixed<22,13> lets;
#endif
// solution3 22 bit integer and 13 bit decimal values support  
#ifdef solution4
#include <ap_fixed.h>
typedef ap_fixed<18,9> lets1;
#endif
// solution4 18 bit integer and 9 bit decimal values support 
struct axis_data
{
	float data;
	ap_int<1> last;
};
// DS defination for the axis_data which of the structure type
// function decalaration  
void ls1(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void ls2(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void ls3(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void ls4(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
