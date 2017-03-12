#include "fuzzySpeed.h"

const int16_t EMAX = 100;
const int16_t EMIN = 20 ;
const int16_t PMAX = 250;//250;  //����250��zigma��������250    300
const int16_t PMIN = 100;//50;   //С��50��zigma��������50    ����Խ�򵥣��ٶ�Խ�죬����Խ�Ǹ��Ӿ�Խ��   100
const int16_t DMAX = 50;   //���ֵ��ʾͼ���DMAX�пյĶ�С���յ�Խ�࣬�ٶ�Խ��
const int16_t DMIN = 0;
const int16_t SMAX = 100;   //����������ͳһ��0~100��ֵ(��һ��)
const int16_t SMIN = 0;
const int16_t FMAX = 100;   //����ֵ������ֵ
                 //0  1   2   3   4   5   6 
int16_t PFF[7] = { 0, 16, 32, 48, 64, 80, 96 };    //ͳһ�����ֵ100����Сֵ0
int16_t DFF[7] = { 0, 16, 32, 48, 64, 80, 96 };

/*�����U����ֵ������ 0     1   2     3   4    5    6  */
int16_t UFFA[7] = {     250, 260, 270, 275, 295, 350 , 380};
int16_t UFFB[7] = {     260, 270, 275, 290, 300, 360 , 400};
int16_t UFFC[7] = {     265, 275, 285, 300, 310, 380 , 430};
int16_t UFFD[7] = {     270, 275, 285, 305, 320, 390 , 440};
//int16_t UFF2[7] = {     260, 270, 275, 285, 295, 350 , 380 };
//int16_t UFF3[7] = {     265, 270, 275, 290, 300, 360 , 420 };
//int16_t UFF4[7] = {     270, 275, 280, 295, 310, 380 , 430 };  //preSee 115-120,1.12,1.50
//int16_t UFF5[7] = {     270, 275, 285, 305, 320, 390 , 440 };    //preSee 115-120,1.12,1.50

uint16_t rule[7][7] =
{ 
  /*zigma */  //  0 , 1 , 2 , 3 , 4 , 5 , 6     preSee      
  /* 0 */       { 6 , 5 , 4 , 3 , 2 , 1 , 1 ,},   
  /* 1 */       { 5 , 5 , 4 , 3 , 2 , 1 , 1 ,},   
  /* 2 */       { 4 , 4 , 3 , 2 , 1 , 1 , 0 ,},    
  /* 3 */       { 3 , 3 , 3 , 2 , 1 , 1 , 0 ,},  
  /* 4 */       { 2 , 2 , 2 , 1 , 1 , 0 , 0 ,},   
  /* 5 */       { 2 , 1 , 1 , 1 , 0 , 0 , 0 ,},   
  /* 6 */       { 1 , 1 , 0 , 0 , 0 , 0 , 0  }    
};

int16_t *UFF = UFFA;
int16_t Fuzzy(int16_t P, int16_t D)   /*ģ����������*/
{
  int16_t U,E;                       /*ƫ�ƫ��΢���Լ����ֵ�ľ�ȷ��*/
  uint16_t PF[2];
  uint16_t DF[2];
  uint16_t UF[4];                    /*ƫ�ƫ��΢���Լ����ֵ��������PF[1]��P�������ȣ�PF[0]�������ȵĲ���*/
  int16_t Pn = 0, Dn = 0;
  int16_t Un[4];
  int32_t temp1, temp2;
  
  //С��50��zigma��������50������250��zigma��������250
  if (P < PMIN)
    P = PMIN;
  else
    if (P > PMAX)
      P = PMAX;
  
  //��P��һ����0~100
  P = (int16_t)(((double)(P - PMIN)) / (PMAX - PMIN) * (SMAX - SMIN) + SMIN); //��һ����SMIN~SMAX
  
  //D��ʾͼ���ն��٣��յ�Խ�࣬�ٶ�Խ��
  D = (int16_t)(144 - D);
  if (D < DMIN)
    D = DMIN;
  else
    if (D > DMAX)
      D = DMAX;
  
  //��һ����0~100
  D = (int16_t)(((double)D) / (DMAX - DMIN) * (SMAX - SMIN) + SMIN); //��һ����SMIN~SMAX
  
  if (E < EMIN)
    E = EMIN;
  else
    if (E > EMAX)
      E = EMAX;
  
  E = (int16_t)(((double)(E - EMIN)) / (EMAX - EMIN) * (SMAX - SMIN) + SMIN); //��һ����SMIN~SMAX
  
  /*�����ȵ�ȷ��*/
  /*����PD��ָ������ֵ�����Ч������*/
  if (P > PFF[0] && P < PFF[6])
  {
    if (P <= PFF[1])
    {
      Pn = 1;
      PF[0] = (uint16_t)(FMAX * ((float)(PFF[1] - P) / (PFF[1] - PFF[0])));
    }
    else if (P <= PFF[2])
    {
      Pn = 2;
      PF[0] = (uint16_t)(FMAX * ((float)(PFF[2] - P) / (PFF[2] - PFF[1])));
    }
    else if (P <= PFF[3])
    {
      Pn = 3;
      PF[0] = (uint16_t)(FMAX * ((float)(PFF[3] - P) / (PFF[3] - PFF[2])));
    }
    else if (P <= PFF[4])
    {
      Pn = 4;
      PF[0] = (uint16_t)(FMAX * ((float)(PFF[4] - P) / (PFF[4] - PFF[3])));
    }
    else if (P <= PFF[5])
    {
      Pn = 5;
      PF[0] = (uint16_t)(FMAX * ((float)(PFF[5] - P) / (PFF[5] - PFF[4])));
    }
    else if (P <= PFF[6])
    {
      Pn = 6;
      PF[0] = (uint16_t)(FMAX * ((float)(PFF[6] - P) / (PFF[6] - PFF[5])));
    }
  }
  else if (P <= PFF[0])
  {
    Pn = 1; PF[0] = (uint16_t)(FMAX);
  }
  else if (P >= PFF[6])
  {
    Pn = 6; PF[0] = 0;
  }
  PF[1] = (uint16_t)(FMAX - PF[0]); //P�������ȣ�PF[0]�������ȵĲ���
  
  
  if (D > DFF[0] && D < DFF[6])
  {
    if (D <= DFF[1])
    {
      Dn = 1; DF[0] = (uint16_t)(FMAX * ((float)(DFF[1] - D) / (DFF[1] - DFF[0])));
    }
    else if (D <= DFF[2])
    {
      Dn = 2;
      DF[0] = (uint16_t)(FMAX * ((float)(DFF[2] - D) / (DFF[2] - DFF[1])));
    }
    else if (D <= DFF[3])
    {
      Dn = 3;
      DF[0] = (uint16_t)(FMAX * ((float)(DFF[3] - D) / (DFF[3] - DFF[2])));
    }
    else if (D <= DFF[4])
    {
      Dn = 4;
      DF[0] = (uint16_t)(FMAX * ((float)(DFF[4] - D) / (DFF[4] - DFF[3])));
    }
    else if (D <= DFF[5])
    {
      Dn = 5;
      DF[0] = (uint16_t)(FMAX * ((float)(DFF[5] - D) / (DFF[5] - DFF[4])));
    }
    else if (D <= DFF[6])
    {
      Dn = 6;
      DF[0] = (uint16_t)(FMAX * ((float)(DFF[6] - D) / (DFF[6] - DFF[5])));
    }
  }
  else if (D <= DFF[0])
  {
    Dn = 1;
    DF[0] = (uint16_t)(FMAX);
  }
  else if (D >= DFF[6])
  {
    Dn = 6;
    DF[0] = 0;
  }
  DF[1] = (uint16_t)(FMAX - DF[0]);  //�õ�������
  
  /*ʹ����Χ�Ż���Ĺ����rule[7][7]*/
  /*���ֵʹ��13�������ȣ�����ֵ��UFF[7]ָ��*/
  /*һ�㶼���ĸ�������Ч*/
  Un[0] = rule[Pn - 1][ Dn - 1];
  Un[1] = rule[Pn][ Dn - 1];
  Un[2] = rule[Pn - 1][ Dn];
  Un[3] = rule[Pn][ Dn];
  if (PF[0] <= DF[0])//����R�����������󽻼�
    UF[0] = PF[0];
  else
    UF[0] = DF[0];
  if (PF[1] <= DF[0])
    UF[1] = PF[1];
  else
    UF[1] = DF[0];
  if (PF[0] <= DF[1])
    UF[2] = PF[0];
  else
    UF[2] = DF[1];
  if (PF[1] <= DF[1])
    UF[3] = PF[1];
  else
    UF[3] = DF[1];
  //ͬ���������������ֵ���
  
  if (Un[0] == Un[1])
  {
    if (UF[0] > UF[1])
      UF[1] = 0;
    else
      UF[0] = 0;
  }
  if (Un[0] == Un[2])
  {
    if (UF[0] > UF[2])
      UF[2] = 0;
    else
      UF[0] = 0;
  }
  if (Un[0] == Un[3])
  {
    if (UF[0] > UF[3])
      UF[3] = 0;
    else
      UF[0] = 0;
  }
  if (Un[1] == Un[2])
  {
    if (UF[1] > UF[2])
      UF[2] = 0;
    else
      UF[1] = 0;
  }
  if (Un[1] == Un[3])
  {
    if (UF[1] > UF[3])
      UF[3] = 0;
    else
      UF[1] = 0;
  }
  if (Un[2] == Un[3])
  {
    if (UF[2] > UF[3])
      UF[3] = 0;
    else
      UF[2] = 0;
  }
  
  /*���ķ�(�������ȼ�Ȩƽ��)��ģ��*/
  /*Un[]ԭֵΪ�������������ţ�ת��Ϊ��������ֵ*/
  // if (Un[0] >= 0)
  Un[0] = UFF[Un[0]];
  // else
  //     Un[0] = (int16_t)(-UFF[-Un[0]]);
  // if (Un[1] >= 0)
  Un[1] = UFF[Un[1]];
  //  else
  //    Un[1] = (int16_t)(-UFF[-Un[1]]);
  // if (Un[2] >= 0)
  Un[2] = UFF[Un[2]];
  //  else
  //     Un[2] = (int16_t)(-UFF[-Un[2]]);
  // if (Un[3] >= 0)
  Un[3] = UFF[Un[3]];
  // else
  //     Un[3] = (int16_t)(-UFF[-Un[3]]);
  
  temp1 = UF[0] * Un[0] + UF[1] * Un[1] + UF[2] * Un[2] + UF[3] * Un[3];
  temp2 = UF[0] + UF[1] + UF[2] + UF[3];
  U = (int16_t)(temp1 / temp2);
  return U;
}
