#ifndef _MyMath_H_
#define _MyMath_H_

#include "stdint.h"

#define _PI	3.1415926535897932f
#define TAN_MAP_RES  0.003921569  //for fast_atan 
#define TAN_MAP_SIZE  256         //for fast_atan 

//float acosf(float x);
//float asinf(float x);
//float atan2f(float y, float x);
//float atanf(float x);


//float tanf(float x);
//float sinf(float x);
//float cosf(float x);

//float exp10f(float x);
//float expf(float x);
//float fabsf(float x);
//float fmodf(float y, float z);
//float froundf(float d);     
//float log10f(float x);
//float sqrtf(float x);
////long  round(float x);


//#define acos	 acosf
//#define asin	 asinf
//#define atan2	 atan2f
//#define atan	 atanf
//#define cos	     cosf
//#define exp10	 exp10f
//#define exp	     expf
//#define fabs	 fabsf
//#define floor	 floorf
//#define fmod	 fmodf
////#define fround	 froundf
//#define log10	 log10f
//#define sin	     sinf
//#define sqrt	 sqrtf
//#define tan	     tanf
//#define tanh	 tanhf

int32_t Max(int32_t a, int32_t b);
int32_t Min(int32_t a, int32_t b);

double MAXf(double a, double b);

double MINf(double a, double b);
uint32_t MyAbs(long date);
int32_t FastAtan(double y);

#endif
