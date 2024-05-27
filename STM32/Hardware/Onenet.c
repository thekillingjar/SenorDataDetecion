/**
 ************************************************************
 ************************************************************
 ************************************************************
 *	�ļ����� 	onenet.c
 *
 *	���ߣ� 		С��ѧǶ��ʽ
 *
 *	���ڣ� 		2024-04-10
 *
 *	�汾�� 		V1.1
 *
 *	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
 *
 *	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
 *				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
 ************************************************************
 ************************************************************
 ************************************************************
 **/

// �����豸
#include "ESP8266.h"
#include "BEEP.h"
// Э���ļ�
#include "Onenet.h"
#include "Mqtt.h"

// Ӳ������
#include "led.h"
#include "dht11.h"
#include "delay.h"
#include "Usart.h"
// C��
#include <string.h>
#include <stdio.h>

// json��
#include "cjson.h"

u8 flag;
// u8 netbuf[50];
u8 LED0_FLAG;
u8 LED1_FLAG;
extern u16 adcy;
char buf[1024];
u8 tmp_data[1024];
u8 alarm_topic[100];
u8 post_topic[100];
u8 replay_topic[100];
u8 set_topic[100];
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	0-�ɹ�	��0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
void Topic_Init()
{
	sprintf((char *)post_topic, "$sys/%s/%s/thing/property/post", PROID, DEVID);
	sprintf((char *)replay_topic, "$sys/%s/%s/thing/property/set_reply", PROID, DEVID);
	sprintf((char *)alarm_topic, "$sys/%s/%s/thing/event/post", PROID, DEVID);
	sprintf((char *)set_topic, "$sys/%s/%s/thing/property/set", PROID, DEVID);
}
_Bool OneNet_DevLink(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	unsigned char *dataPtr;

	_Bool status = 1;

	if (MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0) // �޸�clean_session=1
	{
		ESP8266_Clear();
		recv_len = 0;
		search_flag = 0;
		ipd_len = 0;
		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ�ƽ̨
		u1_printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
		dataPtr = ESP8266_GetIPD(250); // �ȴ�ƽ̨��Ӧ
		//		u1_printf("%s", dataPtr);
		if (dataPtr != NULL) // ���ƽ̨�������ݲ�Ϊ����
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK) //	MQTT���ݽ��������жϣ�connack���ģ�
			{
				switch (MQTT_UnPacketConnectAck(dataPtr)) // ��ӡ�Ƿ����ӳɹ�������ʧ�ܵ�ԭ��
				{
				case 0:
					u1_printf("Tips: connect success\r\n");
					status = 0;
					break;

				case 1:
					u1_printf("WARN:	connect failed:protocol error\r\n");
					break;
				case 2:

					u1_printf("WARN:	connect failed: illegal clientid\r\n");
					break;
				case 3:
					u1_printf("WARN:	connect failed: broker error\r\n");
					break;
				case 4:
					u1_printf("WARN:	connect failed:pwd or name error\r\n");
					break;
				case 5:
					u1_printf("WARN:	connect failed: illegal link\r\n");
					break;

				default:
					u1_printf("ERR: connect failed: unknowed error\r\n");
					break;
				}
			}
		}
		else
		{
			u1_printf("ERROR\r\n");
		}
		MQTT_DeleteBuffer(&mqttPacket); // ɾ��
	}
	else
	{
		u1_printf("WARN:	MQTT_PacketConnect Failed\r\n");
	}
	delay_ms(500);
	return status;
}
uint16_t OneNet_FillBuf(char *buf, u8 temp, u8 humi, float mq2, float bh1750,
												u8 temp_alarm, u8 humi_alarm, u8 smoke_alarm, u8 light_alarm,
												float temp_min, float temp_max, float humi_min, float humi_max,
												float smoke_min, float smoke_max, float light_min, float light_max)
{

	char text[32];

	strcpy(buf, "{\"id\":\"123\",\"version\": \"1.0\",\"params\":{");
	memset(text, 0, sizeof(text));
	sprintf(text, "\"temp\":{\"value\":%d},", temp);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"humi\":{\"value\":%d},", humi);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"smoke\":{\"value\":%f},", mq2);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"light\":{\"value\":%f},", bh1750);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"temp_min\":{\"value\":%f},", temp_min);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"temp_max\":{\"value\":%f},", temp_max);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"humi_min\":{\"value\":%f},", humi_min);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"humi_max\":{\"value\":%f},", humi_max);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"smoke_min\":{\"value\":%f},", smoke_min);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"smoke_max\":{\"value\":%f},", smoke_max);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"light_min\":{\"value\":%f},", light_min);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"light_max\":{\"value\":%f},", light_max);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	if (temp_alarm == 1)
		sprintf(text, "\"temp_alarm\":{\"value\":true},");
	else
		sprintf(text, "\"temp_alarm\":{\"value\":false},");
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	if (humi_alarm == 1)
		sprintf(text, "\"humi_alarm\":{\"value\":true},");
	else
		sprintf(text, "\"humi_alarm\":{\"value\":false},");
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	if (smoke_alarm == 1)
		sprintf(text, "\"smoke_alarm\":{\"value\":true},");
	else
		sprintf(text, "\"smoke_alarm\":{\"value\":false},");
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	if (light_alarm == 1)
		sprintf(text, "\"light_alarm\":{\"value\":true}");
	else
		sprintf(text, "\"light_alarm\":{\"value\":false}");
	strcat(buf, text);

	strcat(buf, "}}");
	// u1_printf("%s", buf);
	return strlen(buf);
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNet_SendData(u8 temp, u8 humi, float mq2, float bh1750,
										 u8 temp_alarm, u8 humi_alarm, u8 smoke_alarm, u8 light_alarm,
										 float temp_min, float temp_max, float humi_min, float humi_max,
										 float smoke_min, float smoke_max, float light_min, float light_max)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���
	mqttPacket._data = tmp_data;
	mqttPacket._size = 1024;
	uint16_t body_len = 0, i = 0;
	u8 ret = -1;
	memset(buf, 0, sizeof(buf)); // �����������

	body_len = OneNet_FillBuf(buf, temp, humi, mq2, bh1750,
														temp_alarm, humi_alarm, smoke_alarm, light_alarm,
														temp_min, temp_max, humi_min, humi_max,
														smoke_min, smoke_max, light_min, light_max);
	u1_printf("body_len:%d\r\n", body_len);
	if (body_len)
	{
		ret = MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket, post_topic);
		u1_printf("ret:%d\r\n", ret);
		if (ret == 0) // ���
		{

			for (; i < body_len; i++)
			{
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ����ݵ�ƽ̨
			MQTT_DeleteBuffer(&mqttPacket);											 // ɾ��
		}
		else
			;
		//			u1_printf(  "WARN:EDP_NewBuffer Failed\r\n");
	}
}

void OneNet_Reply(u32 msg_id)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	char buf[256];

	short body_len = 0, i = 0;
	memset(buf, 0, sizeof(buf)); // �����������

	sprintf(buf, "{\"id\":\"%d\",\"code\": \"200\",\"msg\":\"sucess\"}", msg_id);
	body_len = strlen(buf); // ��ȡ��ǰ��Ҫ���͵����������ܳ���
	if (body_len)
	{
		if (MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket, replay_topic) == 0) // ���
		{
			for (; i < body_len; i++)
			{
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ����ݵ�ƽ̨
			MQTT_DeleteBuffer(&mqttPacket);											 // ɾ��
		}
		else
			;
		//			u1_printf(  "WARN:EDP_NewBuffer Failed\r\n");
	}
}

void OneNet_Alarm(u8 kind, u8 flag)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	char buf[256];

	short body_len = 0, i = 0;
	memset(buf, 0, sizeof(buf)); // �����������
	char clear[10];
	if (flag)
		sprintf(clear, "true");
	else
		sprintf(clear, "false");
	sprintf(buf, "{\"id\":\"123\",\"version\": \"1.0\",\"params\":{\"emergency\":{\"value\":{\"kind\": %d, \"clear\": %s}}}}", kind, clear);
	body_len = strlen(buf); // ��ȡ��ǰ��Ҫ���͵����������ܳ���
	if (body_len)
	{
		if (MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket, alarm_topic) == 0) // ���
		{
			for (; i < body_len; i++)
			{
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ����ݵ�ƽ̨
			MQTT_DeleteBuffer(&mqttPacket);											 // ɾ��
		}
		else
			;
		//			u1_printf(  "WARN:EDP_NewBuffer Failed\r\n");
	}
}
//==========================================================
//	�������ƣ�	OneNET_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0}; // Э���

	// UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);

	if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len); // ��ƽ̨���Ͷ�������

		MQTT_DeleteBuffer(&mqtt_packet); // ɾ��
	}
}

//==========================================================
//	�������ƣ�	OneNET_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNET_Subscribe(void)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0}; // Э���

	char topic_buf[56];
	const char *topic = topic_buf;

	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVID);
	//	u1_printf("%s", topic);
	// UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);

	if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len); // ��ƽ̨���Ͷ�������
		u1_printf("sub success\r\n");
		MQTT_DeleteBuffer(&mqtt_packet); // ɾ��
	}
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

	char *req_payload = NULL;
	char *cmdid_topic = NULL;

	unsigned short topic_len = 0;
	unsigned short req_len = 0;

	unsigned char qos = 0;
	static unsigned short pkt_id = 0;

	unsigned char type = 0;

	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	cJSON *root, *params, *temperature, *humidity;
	type = MQTT_UnPacketRecv(cmd);
	switch (type)
	{
	case MQTT_PKT_PUBLISH: // ���յ�Publish��Ϣ

		result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len); // ���topic����Ϣ��
		if (result == 0)
		{
			// ��ӡ�յ�����Ϣ
			u1_printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
			dataPtr = strchr(req_payload, ':'); // ����'}'

			if (dataPtr != NULL && result != -1) // ����ҵ���
			{
				dataPtr++;

				while (*dataPtr >= '0' && *dataPtr <= '9') // �ж��Ƿ����·��������������
				{
					numBuf[num++] = *dataPtr++;
				}
				numBuf[num] = 0;

				num = atoi((const char *)numBuf); // תΪ��ֵ��ʽ
				u1_printf("%d", num);
				if (strstr((char *)req_payload, "alarm"))
				{
					if (num == 0)
						alarm = 0;
					else if (num == 1)
						alarm = 1;
				}
			}
		}

		break;

	case MQTT_PKT_PUBACK: // ����Publish��Ϣ��ƽ̨�ظ���Ack

		if (MQTT_UnPacketPublishAck(cmd) == 0)
			//				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");

			break;

	case MQTT_PKT_SUBACK: // ����Subscribe��Ϣ��Ack

		if (MQTT_UnPacketSubscribe(cmd) == 0)
			;
		//				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");

		break;

	default:
		result = -1;
		break;
	}

	ESP8266_Clear(); // ��ջ���

	if (result == -1)
		return;

	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}
/*�ж�onetnet�Ƿ����ӳɹ�*/
/*0 OK ;  !0 error*/
uint8_t onenet_state()
{
	return OneNet_DevLink();
}
