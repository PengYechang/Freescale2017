#ifndef _CCD_H__
#define _CCD_H__

#include "gpio.h"
#include "dma.h"
#include "common.h"
#include "rtc.h"
#include "adc.h"

#define SI_HIGH  {PTB->PDOR |= (1<<(1));}
#define SI_LOW   {PTB->PDOR &= ~(1<<(1));}
#define CLK_HIGH  {PTB->PDOR |= (1<<(2));}
#define CLK_LOW   {PTB->PDOR &= ~(1<<(2));}

extern void SamplingDelay(void);
extern void ccd_init();
extern void startCCD();
extern void ImageCapture(unsigned char * ImageData);
#endif