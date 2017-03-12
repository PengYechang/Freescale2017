#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "gpio.h"
#include "common.h"

#define Nrf24l01_PORT 	 HW_GPIOB

#define Nrf24l01_CSN   	 10
#define Nrf24l01_SCK     11
#define Nrf24l01_MOSI 	 16
#define Nrf24l01_MISO 	 17
#define Nrf24l01_CE      20

//****************************************IO�˿ڶ���***************************************

#define   Read_24L01_MISO     (uint16_t)(GPIO_ReadBit(Nrf24l01_PORT,Nrf24l01_MISO))

#define   Hign_24L01_MOSI    PBout(Nrf24l01_MOSI) = 1
#define   Low_24L01_MOSI   	 PBout(Nrf24l01_MOSI) = 0

#define   Hign_24L01_CSN     PBout(Nrf24l01_CSN) = 1
#define   Low_24L01_CSN      PBout(Nrf24l01_CSN) = 0

#define   Hign_24L01_SCK     PBout(Nrf24l01_SCK) = 1
#define   Low_24L01_SCK      PBout(Nrf24l01_SCK) = 0

#define   Hign_24L01_CE      PBout(Nrf24l01_CE) = 1
#define   Low_24L01_CE       PBout(Nrf24l01_CE) = 0

#define RX_DR sta&0x40
#define TX_DS sta&0x20
#define MAX_RT sta&0x10
//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5    // 5 uints TX address width
#define RX_ADR_WIDTH    5    // 5 uints RX address width
#define TX_PLOAD_WIDTH  5   // 32 uints TX payload
#define RX_PLOAD_WIDTH  5   // 32 uints TX payload

//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define NRF24L01READ_REG        0x00   // ���Ĵ���ָ��
#define NRF24L01WRITE_REG       0x20  // д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61   // ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0   // д��������ָ��
#define FLUSH_TX        0xE1  // ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2   // ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3   // �����ظ�װ������ָ��
//#define NOP             0xFF   // ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������


void NrfInit(void);											//NRF��ʼ��
unsigned char SPI_RW(unsigned char byte);
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf);
void TX_Mode(void);
void Transmit(unsigned char * tx_buf);
void RX_Mode(void);
void NrfTransmit(void);
extern unsigned char Rx_Buf[RX_PLOAD_WIDTH];
extern unsigned char Tx_Buf[RX_PLOAD_WIDTH];

#endif

