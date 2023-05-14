#include "ap_int.h"
#include "hls_stream.h"
// solution1 float type
#define solution1
#ifdef solution1
#endif
// solution2 half float type
#ifdef solution2
#endif
// solution3 22 and 13 integer and decimal values resp.
#ifdef solution3
#include <ap_fixed.h>
typedef ap_fixed<22,13> lets;
#endif
// solution4 32 and 8 integer and decimal values resp.
#ifdef solution4
#include <ap_fixed.h>
typedef ap_fixed<32,8> lets1;
#endif
// solution5 24 and 8 integer and decimal values resp.
#ifdef solution5
#include <ap_fixed.h>
typedef ap_fixed<24,8> lets2;
#endif
// axis data structure declaration 
struct axis_data
{
	float data;
	ap_int<1> last;
};
// function declaration 
void lsdnn1(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void lsdnn2(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void lsdnn3(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void lsdnn4(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
void lsdnn5(hls::stream<axis_data> &A,hls::stream<axis_data> &C);
