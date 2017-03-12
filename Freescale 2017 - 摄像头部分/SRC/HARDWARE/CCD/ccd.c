#include "ccd.h"

void ccd_init(){
	GPIO_QuickInit(HW_GPIOB,1,kGPIO_Mode_OPP); //ADC1 SI
	GPIO_QuickInit(HW_GPIOB,2,kGPIO_Mode_OPP); //ADC1 CLK
	ADC_QuickInit(ADC0_SE8_PB0,kADC_SingleDiff8or9);
	
	
	GPIO_QuickInit(HW_GPIOB,3,kGPIO_Mode_OPP); //ADC2 SI
	GPIO_QuickInit(HW_GPIOB,4,kGPIO_Mode_OPP); //ADC2 CLK
	ADC_QuickInit(ADC1_SE11_PB05,kADC_SingleDiff8or9);
	
	startCCD(); //Æô¶¯CCD
}

void startCCD(){
   unsigned char i;

    SI_HIGH;            /* SI  = 1 */
    SamplingDelay();
    CLK_HIGH;           /* CLK = 1 */
    SamplingDelay();
    SI_LOW;            /* SI  = 0 */
    SamplingDelay();
    CLK_LOW;           /* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_HIGH;       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        CLK_LOW;       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_HIGH;           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_LOW;           /* CLK = 0 */	
}

void ImageCapture(unsigned char * ImageData) {

    unsigned char i;
    extern uint8_t AtemP ;

    SI_HIGH;            /* SI  = 1 */
    SamplingDelay();
    CLK_HIGH;           /* CLK = 1 */
    SamplingDelay();
    SI_LOW;            /* SI  = 0 */
    SamplingDelay();

    //Delay 10us for sample the first pixel
    /**/
    for(i = 0; i < 80; i++)
    {
      SamplingDelay() ;  //200ns
    }

    //Sampling Pixel 1

    *ImageData = ADC_QuickReadValue(ADC0_SE8_PB0);
    ImageData ++ ;
    CLK_LOW;           /* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_HIGH;       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        //Sampling Pixel 2~128

       *ImageData =  ADC_QuickReadValue(ADC0_SE8_PB0);
        ImageData ++ ;
        CLK_LOW;       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_HIGH;           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_LOW;           /* CLK = 0 */
}


void SamplingDelay(void){
   volatile uint8_t i ;
   for(i=0;i<1;i++) {
    __asm("nop");
    __asm("nop");
	 }
   
}