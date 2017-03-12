#include "MyMath.h"

int16_t FastTanTable[90] = 
{
  0,5,10,16,21,26,32,37,42,48,
  53,58,64,69,75,80,86,92,97,103,
  109,115,121,127,134,140,146,153,160,166,
  173,180,187,195,202,210,218,226,234,243,
  252,261,270,280,290,300,311,322,333,345,
  358,370,384,398,413,428,445,462,480,499,
  520,541,564,589,615,643,674,707,743,782,
  824,871,923,981,1046,1120,1203,1299,1411,1543,
  1701,1894,2135,2443,2854,3429,4290,5724,8591,17187,
};

int32_t Max(int32_t a, int32_t b)
{
  if (a > b) 
  {
	  return a;
  }
  else	
  {
	  return b;
  }
}

int32_t Min(int32_t a, int32_t b)
{
  if(a < b) 
  {
	  return a;
  }
  else
  {
    return b;
  }
}

double MAXf(double a, double b)
{
  if (a > b)
  {	  
	  return a;
  }
  else
  {
    return b;
  }
}

double MINf(double a, double b)
{
  if (a < b)
  {
	  return a;
  }
  else
  {
    return b;
  }
}

uint32_t MyAbs(long date)
{
  if(date>0)
  {
	  return date;
  }
  else 
  {
	  return -date;
  }
}

int32_t FastAtan(double y)
{//arctanx(x)=x-1/3*x^3+1/5*x^5-1/7*x^7+1/9*x^9
  int16_t lab = 1;
  int16_t yAbs;
  uint8_t i = 0;
  int16_t start = 0, end = 89;
  int16_t m;
  if (y < 0)
  {
    y = -y;
    lab = (int16_t)(-lab);
  }
  yAbs = (int16_t)(y * 300);
  if (yAbs < FastTanTable[1])
    return 0;
  else
    if (yAbs > FastTanTable[89])
      return 90 * lab;
  while ((i++) < 8)
  {
    m = (int16_t)((end + start) / 2);
    if (m == end || m == start)
    {
      return m * lab;  //1 ??
    }
    if (yAbs > FastTanTable[m])
    {
      start = m;
    }
    else
      if (yAbs < FastTanTable[m])
      {
        end = m;
      }
      else
      {
        return m * lab;
      }
  }
  return 0;
}
