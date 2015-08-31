/*
 * DJI_Pro_Link.c
 *
 *  Created on: Mar 12, 2015
 *      Author: wuyuwei
 */

#include <stdio.h>
#include <string.h>
#include "DJI_Pro_Link.h"
#include "usart.h"
#include "DJI_Pro_Codec.h"
#include "DJI_Pro_App.h"
static Session_Queue Send_Session_Common_Tab;
static unsigned char Send_Global_Common_Memory[PKG_MAX_SIZE];
static Session_Queue Send_Session_Tab[SESSION_AND_MEM_COUNT];
static Memory_Manage_Unit Send_MMU_Tab[SESSION_AND_MEM_COUNT];
static unsigned char Send_Global_Memory[SESSION_AND_MEM_COUNT * PKG_MAX_SIZE];
u8  RecBuff[160]={0};
u8 i;
ProFrameData_Unit  DataFromMobile;
unsigned char Activation_Ack = 0;//激活成功为1
unsigned char GetControl_Ack = 0;//获取控制权1
//ProHeader RecHeader;ProFrameData_Unit  RecData;

static void Send_Pro_Data(unsigned char *buf)
{
	ProHeader *pHeader = (ProHeader *)buf;
	USART1_SendBuffer(buf,pHeader->length);
}


Memory_Manage_Unit *Request_Send_MMU(unsigned short size)
{
	unsigned char i;
	Memory_Manage_Unit *p2mmu ;

	for(i = 0 ; i < SESSION_AND_MEM_COUNT; i ++)
	{
		if(Send_MMU_Tab[i].usage_flag == 0)
		{
			p2mmu = &Send_MMU_Tab[i];
			p2mmu->usage_flag = 1;
			p2mmu->end_addr = p2mmu->start_addr + size;
			break;
		}
	}

	return p2mmu;
}


void Free_Send_MMU(Memory_Manage_Unit *mmu)
{

	if(mmu->usage_flag == 1)
	{
		mmu->usage_flag = 0;
	}
}

Session_Queue * Request_Send_Session(unsigned short size)
{
	int i;
	Session_Queue *p2session=NULL;
	Memory_Manage_Unit *p2mmu=NULL;

	for(i = 0 ; i < SESSION_AND_MEM_COUNT ; i ++)
	{
		if(Send_Session_Tab[i].usage_flag == 0)
		{
			Send_Session_Tab[i].usage_flag = 1;
			p2session = &Send_Session_Tab[i];
			break;
		}
	}

	p2mmu = Request_Send_MMU(size);
	if(p2mmu == NULL)
	{
		p2session = NULL;
	}
	else
	{
		p2session->mmu = p2mmu;
	}
	return p2session;
}



void Free_Send_Session(Session_Queue * session)
{
	if(session->usage_flag == 1)
	{
		Free_Send_MMU(session->mmu);
		session->usage_flag = 0;
	}
}


void Pro_Link_Setup(void)
{
	unsigned char i;

	for(i = 0; i < SESSION_AND_MEM_COUNT; i ++)
	{
		Send_Session_Tab[i].session_id = i + 2;
		Send_Session_Tab[i].usage_flag = 0;
		Send_Session_Tab[i].pre_seq_num = 0x10000;

		Send_MMU_Tab[i].mmu_index = i;
		Send_MMU_Tab[i].usage_flag = 0;
		Send_MMU_Tab[i].start_addr = (unsigned long)&Send_Global_Memory[i * PKG_MAX_SIZE];
	}

	Send_Session_Common_Tab.usage_flag = 0;
	Send_Session_Common_Tab.session_id = 1;
	

}

void Pro_Config_Comm_Encrypt_Key(const char *key)
{
	sdk_set_encrypt_key_interface(key);
}


int Pro_Send_Interface(ProSendParameter *parameter)
{
	unsigned short ret = 0;
	static unsigned short global_seq_num = 0;
  Session_Queue *p2session = NULL;
	if(parameter->length > PRO_DATA_MAX_SIZE)
	{
		return -1;
	}


	switch(parameter->pkg_type)
	{
	case 0://会话ID
		ret = sdk_encrypt_interface(Send_Global_Common_Memory,parameter->buf,parameter->length,
				0,parameter->need_encrypt,0,global_seq_num ++);
		if(ret == 0)
		{
			return -1;
		}
		Send_Pro_Data(Send_Global_Common_Memory);//通过串口发送
		break;
	case 1:
		if(global_seq_num == Send_Session_Common_Tab.pre_seq_num)
		{
			global_seq_num ++;
		}
		Send_Session_Common_Tab.pre_seq_num = global_seq_num;

		ret = sdk_encrypt_interface(Send_Global_Common_Memory,parameter->buf,parameter->length,
				0,parameter->need_encrypt,1,global_seq_num ++);

		if(ret == 0)
		{
			return -1;
		}

		Send_Pro_Data(Send_Global_Common_Memory);
		break;
	case 2:
		p2session = Request_Send_Session(parameter->length + sizeof(ProHeader) + 4);
		if(p2session)
		{
			if(global_seq_num == p2session->pre_seq_num)
			{
				global_seq_num ++;
			}
			p2session->pre_seq_num = global_seq_num;

			ret = sdk_encrypt_interface((unsigned char*)p2session->mmu->start_addr,parameter->buf,parameter->length,
				0,parameter->need_encrypt,p2session->session_id,global_seq_num ++);
			if(ret == 0)
			{
				Free_Send_Session(p2session);
				return -1;
			}

			Send_Pro_Data((unsigned char*)p2session->mmu->start_addr);
			ret = 0;
		}
		break;
		
	}


	return 0;
}


void Pro_Receive_Interface(void)
{
  int Heard_CRC32=0;
	ProFrameData_Unit  RecData;
	SDKHeader RecHeader ;
	
	memcpy(&RecHeader, RecBuff,sizeof(SDKHeader));
	
	if (RecHeader.sof != _SDK_SOF) return;
	if (RecHeader.version != 0) return;
	if (RecHeader.length > _SDK_MAX_RECV_SIZE) return;
	if (RecHeader.length > sizeof(SDKHeader) && RecHeader.length < _SDK_FULL_DATA_SIZE_MIN) return;
	
	if(sdk_stream_crc16_calc((unsigned char*)&RecHeader, _SDK_HEAD_DATA_LEN)!=RecHeader.head_crc) return;//校验帧头
	
	memcpy(&Heard_CRC32,&RecBuff[RecHeader.length - _SDK_CRC_DATA_SIZE] ,_SDK_CRC_DATA_SIZE);
	if(sdk_stream_crc32_calc((unsigned char*)RecBuff, RecHeader.length - _SDK_CRC_DATA_SIZE)!=Heard_CRC32) return;//整体校验
	
	if(RecHeader.is_ack)
	{
		ProAckParameter Ack;
		Ack.session_id = RecHeader.session_id;
		Ack.seq_num = RecHeader.sequence_number;
		Ack.length = RecHeader.length - _SDK_FULL_DATA_SIZE_MIN;
		Ack.need_encrypt = RecHeader.enc_type;
		
		switch(Ack.session_id)
		{
			case 0x02:
				if(Ack.seq_num==0)
				{
					unsigned short temp = (unsigned short)RecBuff[13]<<RecBuff[12];
					if(temp==0)
						Activation_Ack = 1;
				}
				break;
//			case 0x01:
//				//if(RecHeader.sequence_number==Send_Session_Tab[1].pre_seq_num)
//				{
//					unsigned short temp = (unsigned short)RecBuff[13]<<8||(unsigned short)RecBuff[12];
//					if(temp==0x0002)
//						GetControl_Ack = 1;
//				}
//				break;
		}
		
		
	}
	else
	{
		RecData.CommandSet = RecBuff[12];
		RecData.CommandId = RecBuff[13];
		RecData.dataLen = RecHeader.length - _SDK_FULL_DATA_SIZE_MIN-2;//透传数据的长度
		memcpy(&RecData.data , &RecBuff[14],RecData.dataLen);
		//RecData.readed = 0;  //数据为读取
		
		switch(RecData.CommandSet)
		{
			case 0x02:
				if(RecData.CommandId==0x02)
					memcpy((unsigned char*)&DataFromMobile,(unsigned char*)&RecData,sizeof(RecData));
				break;
			
		}
	}
}
