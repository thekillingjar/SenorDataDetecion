#include "bh1750.h"
#include "sys.h"
#include "Usart.h"

static void I2C_BH1750_GPIOConfig(void);



static void i2c_Delay(void)
{
	uint8_t i;

	for (i = 0; i < 10; i++);
}


void i2c_Start(void)
{
	
	BH1750_I2C_SDA_1();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_0();
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
}

void i2c_Stop(void)
{
	BH1750_I2C_SDA_0();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_1();
}


void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			BH1750_I2C_SDA_1();
		}
		else
		{
			BH1750_I2C_SDA_0();
		}
		i2c_Delay();
		BH1750_I2C_SCL_1();
		i2c_Delay();	
		BH1750_I2C_SCL_0();
		if (i == 7)
		{
			 BH1750_I2C_SDA_1();
		}
		_ucByte <<= 1;
		i2c_Delay();
	}
}


uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ???1?bit????bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		BH1750_I2C_SCL_1();
		i2c_Delay();
		if (BH1750_I2C_SDA_READ())
		{
			value++;
		}
		BH1750_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}


uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	BH1750_I2C_SDA_1();
	i2c_Delay();
	BH1750_I2C_SCL_1();	
	i2c_Delay();
	if (BH1750_I2C_SDA_READ())
		re = 1;
	else
		re = 0;
	BH1750_I2C_SCL_0();
	i2c_Delay();
	return re;
}


void i2c_Ack(void)
{
	BH1750_I2C_SDA_0();	
	i2c_Delay();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
	BH1750_I2C_SDA_1();
}


void i2c_NAck(void)
{
	BH1750_I2C_SDA_1();
	i2c_Delay();
	BH1750_I2C_SCL_1();	
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();	
}


static void I2C_BH1750_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BH1750_RCC_I2C_PORT, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BH1750_I2C_SCL_PIN | BH1750_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(BH1750_GPIO_PORT_I2C, &GPIO_InitStructure);


	i2c_Stop();
}

uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	i2c_Start();		

	i2c_SendByte(_Address | BH1750_I2C_WR);
	ucAck = i2c_WaitAck();	

	i2c_Stop();		

	return ucAck;
}


uint8_t BH1750_Byte_Write(uint8_t data)
{
	i2c_Start();

	i2c_SendByte(BH1750_Addr|0);
	if(i2c_WaitAck()==1)
		return 1;

	i2c_SendByte(data);
	if(i2c_WaitAck()==1)
		return 2;
	i2c_Stop();
	return 0;
}


uint16_t BH1750_Read_Measure(void)
{
	uint16_t receive_data=0; 
	i2c_Start();

	i2c_SendByte(BH1750_Addr|1);
	if(i2c_WaitAck()==1)
		return 0;

	receive_data=i2c_ReadByte();
	i2c_Ack();

	receive_data=(receive_data<<8)+i2c_ReadByte();
	i2c_NAck();
	i2c_Stop();
	return receive_data;	
}



void BH1750_Power_ON(void)
{
	uint8_t x;
	x = BH1750_Byte_Write(POWER_ON);
	u1_printf("x:%d\r\n", x);
}


void BH1750_Power_OFF(void)
{
	BH1750_Byte_Write(POWER_OFF);
}


void BH1750_RESET(void)
{
	BH1750_Byte_Write(MODULE_RESET);
}


void BH1750_Init(void)
{
	I2C_BH1750_GPIOConfig();	
	
	BH1750_Power_ON();	
	//BH1750_RESET();		
	BH1750_Byte_Write(Measure_Mode);
	//SysTick_Delay_ms(120);
}


float LIght_Intensity(void)
{
	return (float)(BH1750_Read_Measure()/1.1f*Resolurtion);
}

