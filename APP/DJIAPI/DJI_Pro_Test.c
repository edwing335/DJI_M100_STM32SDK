/*
 ============================================================================
 Name        : DJI_Pro_Test.cpp
 Author      : Wu Yuwei
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

/* SDK */
#include <stdio.h>
#include <stdlib.h>
#include "DJI_Pro_Test.h"
#include "BSP_Config.H"
/* MATH for_example */
#include <math.h>

/* parameter */
#define C_EARTH (double) 6378137.0
#define C_PI	(double) 3.141592653589793
static const char *key;
static activation_data_t activation_msg = {14,2,1,""};

void DJI_Pro_Test_Setup(void)
{

	activation_msg.app_id =1021920;
	activation_msg.app_api_level = 2;
	activation_msg.app_ver = 1;
	memcpy(activation_msg.app_bundle_id,"1234567890123456789012", 32);
	key = "c35852be595a4be9f6d6610b7402f40d0d9ceeb3fa3e198e6d6381c6acc7fd03";
	Pro_Config_Comm_Encrypt_Key(key);
	Pro_Link_Setup();
}

unsigned char DJI_Onboard_API_Activation(void)
{
	App_Send_Data( 2, 0, MY_ACTIVATION_SET, API_USER_ACTIVATION,(unsigned char*)&activation_msg,sizeof(activation_msg));
	delay_ms(500);
	return Activation_Ack;
}

unsigned char DJI_Onboard_API_Control(unsigned char arg)
{
		unsigned char send_data = arg;
		App_Send_Data(1,1,MY_CTRL_CMD_SET,API_OPEN_SERIAL,(unsigned char*)&send_data,sizeof(send_data));
		delay_ms(500);
		return GetControl_Ack;
}

void DJI_Onboard_API_UAV_Control(unsigned char arg)
{
	unsigned char send_data = arg;
	App_Complex_Send_Cmd(send_data);	
}

void DJI_Onboard_API_Ctr(unsigned char flagmode,unsigned int n,float a,float b,float c,float d)
{
		api_ctrl_without_sensor_data_t send_data={0};
		unsigned char j;
		send_data.ctrl_flag = flagmode;
		send_data.roll_or_x = a;
		send_data.pitch_or_y = b;
		send_data.thr_z = c; //m/s
		send_data.yaw = d;
		for (j=0;j<n;j++)
		{ 
			App_Send_Data(0, 0, MY_CTRL_CMD_SET, API_CTRL_REQUEST, (unsigned char*)&send_data, sizeof(send_data));
			delay_ms(1);
		}
}

void DJI_Onboard_API_Ctr_drawcube(void)
{
	DJI_Onboard_API_Ctr(0x48,100,2,0,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,2,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,-2,0,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,-2,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,2,0,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,-2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,2,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,-2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,-2,0,0,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,-2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,0,2,0);
	delay_ms(400);
	DJI_Onboard_API_Ctr(0x48,100,0,-2,0,0);
	delay_ms(400);		
}

void DJI_Onboard_API_Ctr_drawcircle(void)
{
	api_ctrl_without_sensor_data_t send_data={0};
  unsigned int j;
     for(j=0;j<3000;j++)
    {
			send_data.ctrl_flag = 0x50;
			send_data.roll_or_x = 1*cos(C_PI*0.002*j);
			send_data.pitch_or_y = 1*sin(C_PI*0.002*j);
			send_data.thr_z = 10; //m/s
			send_data.yaw = 0;
			App_Send_Data(0, 0, MY_CTRL_CMD_SET, API_CTRL_REQUEST, (unsigned char*)&send_data, sizeof(send_data));
			delay_ms(1);
		}
}

void DJI_Onboard_API_String(unsigned char *data)
{
	char tran_buf[100]={0};//注意!!!!如果发送的数据大于1024byte会丢失
	
	memcpy(&tran_buf[0], data,strlen((unsigned char*)data));
	tran_buf[strlen((unsigned char*)data)]=0;
	
	App_Send_Data( 0, 0, MY_ACTIVATION_SET, API_DATA_TRANSMISSION,(unsigned char*)&tran_buf,strlen(tran_buf)+1);
}

void DJI_Onboard_API_DataTran(unsigned char *data,int len)
{
	char tran_buf[100]={0};//注意!!!!如果发送的数据大于1024byte会丢失
	memcpy(&tran_buf[0], data,len);
	
	App_Send_Data( 0, 0, MY_ACTIVATION_SET, API_DATA_TRANSMISSION,(unsigned char*)&tran_buf,len);
}



