	#ifndef __BH1750_H
	#define __BH1750_H	 
	#include "sys.h"
	 
	//BH1750???
	#define BH1750_Addr			0x46
	
	//BH1750???
	#define POWER_OFF			0x00
	#define POWER_ON			0x01
	#define MODULE_RESET		0x07
	#define	CONTINUE_H_MODE		0x10
	#define CONTINUE_H_MODE2	0x11
	#define CONTINUE_L_MODE		0x13
	#define ONE_TIME_H_MODE		0x20
	#define ONE_TIME_H_MODE2	0x21
	#define ONE_TIME_L_MODE		0x23
	
	//????
	#define Measure_Mode			CONTINUE_H_MODE
	
	//???	????(??lx)=(High Byte  + Low Byte)/ 1.2 * ????
	#if ((Measure_Mode==CONTINUE_H_MODE2)|(Measure_Mode==ONE_TIME_H_MODE2))
		#define Resolurtion		0.5
	#elif ((Measure_Mode==CONTINUE_H_MODE)|(Measure_Mode==ONE_TIME_H_MODE))
		#define Resolurtion		1
	#elif ((Measure_Mode==CONTINUE_L_MODE)|(Measure_Mode==ONE_TIME_L_MODE))
		#define Resolurtion		4
	#endif
	
	#define BH1750_I2C_WR	0		/* ???bit */
	#define BH1750_I2C_RD	1		/* ???bit */
	
	/* ??I2C?????GPIO??, ???????4?????????SCL?SDA??? */
	#define BH1750_GPIO_PORT_I2C	GPIOA		/* GPIO?? */
	#define BH1750_RCC_I2C_PORT 	RCC_APB2Periph_GPIOA		/* GPIO???? */
	#define BH1750_I2C_SCL_PIN		GPIO_Pin_3		/* ???SCL????GPIO */
	#define BH1750_I2C_SDA_PIN		GPIO_Pin_2
	/* ???SDA????GPIO */
	
	
	/* ????SCL?SDA??,??????????????? */
	#if 0	/* ????: 1 ??GPIO??????IO?? */
		#define BH1750_I2C_SCL_1()  GPIO_SetBits(BH1750_GPIO_PORT_I2C, BH1750_I2C_SCL_PIN)		/* SCL = 1 */
		#define BH1750_I2C_SCL_0()  GPIO_ResetBits(BH1750_GPIO_PORT_I2C, BH1750_I2C_SCL_PIN)		/* SCL = 0 */
		
		#define BH1750_I2C_SDA_1()  GPIO_SetBits(BH1750_GPIO_PORT_I2C, BH1750_I2C_SDA_PIN)		/* SDA = 1 */
		#define BH1750_I2C_SDA_0()  GPIO_ResetBits(BH1750_GPIO_PORT_I2C, BH1750_I2C_SDA_PIN)		/* SDA = 0 */
		
		#define BH1750_I2C_SDA_READ()  GPIO_ReadInputDataBit(BH1750_GPIO_PORT_I2C, BH1750_I2C_SDA_PIN)	/* ?SDA???? */
	#else	/* ???????????????IO?? */
	    /* ??:????,?IAR???????,????????? */
		#define BH1750_I2C_SCL_1()  BH1750_GPIO_PORT_I2C->BSRR = BH1750_I2C_SCL_PIN				/* SCL = 1 */
		#define BH1750_I2C_SCL_0()  BH1750_GPIO_PORT_I2C->BRR = BH1750_I2C_SCL_PIN				/* SCL = 0 */
		
		#define BH1750_I2C_SDA_1()  BH1750_GPIO_PORT_I2C->BSRR = BH1750_I2C_SDA_PIN				/* SDA = 1 */
		#define BH1750_I2C_SDA_0()  BH1750_GPIO_PORT_I2C->BRR = BH1750_I2C_SDA_PIN				/* SDA = 0 */
		
		#define BH1750_I2C_SDA_READ()  ((BH1750_GPIO_PORT_I2C->IDR & BH1750_I2C_SDA_PIN) != 0)	/* ?SDA???? */
	#endif
	
	
	void i2c_Start(void);
	void i2c_Stop(void);
	void i2c_SendByte(uint8_t _ucByte);
	uint8_t i2c_ReadByte(void);
	uint8_t i2c_WaitAck(void);
	void i2c_Ack(void);
	void i2c_NAck(void);
	uint8_t i2c_CheckDevice(uint8_t _Address);
	
	void BH1750_Init(void);			//???IIC???
	float LIght_Intensity(void);	//????????
	uint8_t BH1750_Byte_Write(uint8_t data);
	uint16_t BH1750_Read_Measure(void);
	void BH1750_Power_ON(void);
	void BH1750_Power_OFF(void);
	void BH1750_RESET(void);
			 				    
	#endif

