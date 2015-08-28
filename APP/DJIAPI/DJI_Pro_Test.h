/*
 * DJI_Pro_Test.h
 *
 *  Created on: 7 Apr, 2015
 *      Author: wuyuwei
 */

#ifndef DJI_PRO_TEST_H_
#define DJI_PRO_TEST_H_
#include "DJI_Pro_Codec.h"
#include "DJI_Pro_Link.h"
#include "DJI_Pro_App.h"
/* external functions */
void DJI_Pro_Test_Setup(void);
unsigned char DJI_Onboard_API_Activation(void);
unsigned char DJI_Onboard_API_Control(unsigned char arg);
void DJI_Onboard_API_UAV_Control(unsigned char data);
void DJI_Onboard_API_Ctr(unsigned char flagmode,unsigned int n,float a,float b,float c,float d);
void DJI_Onboard_API_Ctr_drawcube(void);
void DJI_Onboard_API_Ctr_drawcircle(void);
extern void DJI_Onboard_API_String(unsigned char*data);
extern void DJI_Onboard_API_DataTran(unsigned char *data,int len);
#endif /* DJI_PRO_TEST_H_ */
