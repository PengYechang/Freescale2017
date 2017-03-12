#include "NRF24L01.h"

unsigned char  sta;   //״̬��־
unsigned char  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //���ص�ַ
unsigned char  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //���յ�ַ
//unsigned char  Tx_Buf[TX_PLOAD_WIDTH]= {0x12,0x23,0x34};           //NRF������
unsigned char  Tx_Buf[TX_PLOAD_WIDTH]= {0}; //�������ݻ�����
unsigned char  Rx_Buf[RX_PLOAD_WIDTH]= {0}; //�������ݻ�����

void NrfInit(void)
{
	/***************************��������******************************/
	GPIO_QuickInit(Nrf24l01_PORT,Nrf24l01_MISO,kGPIO_Mode_IPU);
	/***************************�������******************************/
	GPIO_QuickInit(Nrf24l01_PORT,Nrf24l01_CSN,kGPIO_Mode_OPP);
	GPIO_QuickInit(Nrf24l01_PORT,Nrf24l01_SCK,kGPIO_Mode_OPP);
	GPIO_QuickInit(Nrf24l01_PORT,Nrf24l01_MOSI,kGPIO_Mode_OPP);
	GPIO_QuickInit(Nrf24l01_PORT,Nrf24l01_CE,kGPIO_Mode_OPP);
    Hign_24L01_CSN;
    Low_24L01_CE;    		//CE�͵�ƽѡ������
    Low_24L01_SCK;  		//������ʱSCKΪ�͵�ƽ
    DelayMs(250);
}
/************************************IO ��ģ��SPI���� ����************************************************/
unsigned char SPI_RW(unsigned char byte)
{
    unsigned char bit_ctr;
    for(bit_ctr=0; bit_ctr<8; bit_ctr++)
    {
        if(byte&0x80)
        {
            Hign_24L01_MOSI;
        }
        else
        {
            Low_24L01_MOSI;
        }
        byte=(byte<<1);
        Hign_24L01_SCK;        //������
        if(Read_24L01_MISO)  //MISO
            byte+=1;         //ͬʱ��һ������������պͷ���
        Low_24L01_SCK;      //�½���
    }
    return(byte);         //�����յ�������
}

unsigned char SPI_RW_Reg  (unsigned char  reg,unsigned char value) // ��Ĵ���REGдһ���ֽڣ�ͬʱ����״̬�ֽ�
{
    unsigned char status;
    Low_24L01_CSN;
    status=SPI_RW(reg);
    SPI_RW(value);
    Hign_24L01_CSN;
    return(status);
}
unsigned char SPI_Read (unsigned char  reg )
{
    unsigned char reg_val;
    Low_24L01_CSN;
    SPI_RW(reg);
    reg_val=SPI_RW(0);
    Hign_24L01_CSN;
    return(reg_val);
}
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
    unsigned char status,byte_ctr;
    Low_24L01_CSN;
    status = SPI_RW(reg);    // Select register to write to and read status byte
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
        SPI_RW(*pBuf++);
    Hign_24L01_CSN;              // Set CSN high again
    return(status);          // return nRF24L01 status byte
}

/*******************************��*****��*****ģ*****ʽ*****��*****��*************************************/
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
{
    unsigned char status,uchar_ctr;

    Low_24L01_CSN;
    status = SPI_RW(reg);         // Select register to write to and read status unsigned char

    for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++)
        pBuf[uchar_ctr] = SPI_RW(0);    //
    Hign_24L01_CSN;
    return(status);                    // return nRF24L01 status uchar
}
/******************************************************************************************************/
//������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//���ܣ����ݶ�ȡ�����rx_buf���ջ�������
/******************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
    unsigned char revale=0;
    sta=SPI_Read(STATUS); // ��ȡ״̬�Ĵ������ж����ݽ���״��
    if(RX_DR)    // �ж��Ƿ���յ�����
    {
        SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
        revale =1;   //��ȡ������ɱ�־
    }
    SPI_RW_Reg(NRF24L01WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
    return revale;
}
void TX_Mode(void)
{
    Low_24L01_CE;
    SPI_RW_Reg(FLUSH_TX,0x00);
    SPI_Write_Buf(NRF24L01WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
    SPI_Write_Buf(NRF24L01WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
    SPI_RW_Reg(NRF24L01WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
    SPI_RW_Reg(NRF24L01WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
    SPI_RW_Reg(NRF24L01WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...1a
    SPI_RW_Reg(NRF24L01WRITE_REG + RF_CH, 40);        // Select RF channel 40
    SPI_RW_Reg(NRF24L01WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
    SPI_RW_Reg(NRF24L01WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ2�ֽ�
    SPI_RW_Reg(NRF24L01WRITE_REG + CONFIG, 0x0e);
    Hign_24L01_CE;
    DelayMs(100);
}
void Transmit(unsigned char * tx_buf)
{
    Low_24L01_CE;
    SPI_Write_Buf(NRF24L01WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
    SPI_RW_Reg(FLUSH_TX,0x00);
    SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // װ������
    SPI_RW_Reg(NRF24L01WRITE_REG + CONFIG, 0x0e);      // IRQ�շ�����ж���Ӧ��16λCRC��������
    Hign_24L01_CE;
}
/****************************************************************************************************/
//������void RX_Mode(void)
//���ܣ����ݽ�������
/****************************************************************************************************/
void RX_Mode(void)
{
    Low_24L01_CE;
    SPI_RW_Reg(FLUSH_RX,0x00);
    SPI_Write_Buf(NRF24L01WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack

    SPI_RW_Reg(NRF24L01WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
    SPI_RW_Reg(NRF24L01WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
    SPI_RW_Reg(NRF24L01WRITE_REG + RF_CH, 40);        // Select RF channel 40
    SPI_RW_Reg(NRF24L01WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ2�ֽ�
    SPI_RW_Reg(NRF24L01WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
    SPI_RW_Reg(NRF24L01WRITE_REG + CONFIG, 0x0F);
    Hign_24L01_CE;
    DelayMs(130);   								  //����û��Ҳ���ԣ�AVR 22.1184M
}

void NrfTransmit(void)
{
    Transmit(Tx_Buf);
    sta=SPI_Read(NRF24L01READ_REG +  STATUS);
    if(TX_DS)
    {
        SPI_RW_Reg(NRF24L01WRITE_REG + STATUS,sta);
    }
    if(MAX_RT)  //����Ƿ��ͳ�ʱ
    {
        SPI_RW_Reg(NRF24L01WRITE_REG + STATUS,sta);
    }
}
