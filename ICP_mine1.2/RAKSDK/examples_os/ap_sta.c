/**
******************************************************************************
* @file    ap_sta.c
* @author  RAK439 module Design Team
* @version V1.0.2
* @date    13-Jun-2015
* @brief   RAK439 module OS Demo ap_sta Application C File.
*
*          This file contains:
*           -network api function,how to creat ap or sta,easy config
* 
******************************************************************************
**/
#include "rw_app.h"

//wifi configs
//#define CONFIG_WIFI_AP_SSID "RAK_AP"
//#define CONFIG_WIFI_AP_PASSWORD "1234567890"
//#define CONFIG_WIFI_STA_SSID "TP_LINK_Soft"
//#define CONFIG_WIFI_STA_PASSWORD "Soft12345678"
char* Wifi_AP_SSID = "RAK_AP";
char* Wifi_AP_PASSWORD = "1234567890";
char* Wifi_STA_SSID = "TP_LINK_Soft";
char* Wifi_STA_PASSWORD = "Soft12345678";
rw_IpConfig_t Wifi_AP_IPConfig_t={
	.addr = 0xC0A80701,
	.mask = 0xFFFFFF00,
	.gw   = 0xC0A80701,
	.svr1 = 0xC0A80701
//	.svr2
};
//wifi configs end

RW_APP_CTX         app_demo_ctx;

void rw_appdemo_context_init(void)
{
	int        i =0;

	memset((void* )&app_demo_ctx, 0, sizeof(app_demo_ctx));
	app_demo_ctx.ltcps_sockfd = INVAILD_SOCK_FD;
	app_demo_ctx.ludps_sockfd = INVAILD_SOCK_FD;
	app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD;
	app_demo_ctx.udpc_sockfd = INVAILD_SOCK_FD; 
	app_demo_ctx.easy_sockfd = INVAILD_SOCK_FD;

	for(i=0; i<ALLOW_MAX_NUMS; i++)
	{
		app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
	}
}

int rw_network_init(rw_WlanConnect_t* pconn, rw_IpConfigMode_t ipmode, rw_IpConfig_t* ipconfig)
{
	int             ret = 0;
	unsigned int TimeOut_t = 0;
	while ((ret =rw_wlanConnect(pconn)) != RW_OK) {
		DPRINTF("%s\r\n",pconn->ssid);
		DPRINTF("rw_wlanConnect error =%d\r\n", ret);
		//while(1);
        delay_ms(5);
	}
	while(app_demo_ctx.rw_connect_status != STATUS_OK){
		TimeOut_t++;
		if(TimeOut_t == 1000)
		{
			DPRINTF("app_demo_ctx.rw_connect_status != STATUS_OK TimeOut_t %d\r\n",TimeOut_t);
			TimeOut_t = 0;
		}
		delay_ms(5);
	}

	while ((ret =rw_ipConfig(ipconfig, ipmode)) != RW_OK){
		DPRINTF("rw_ipConfig error =%d\r\n", ret);
		//while(1);    
        delay_ms(5);
	}
    app_demo_ctx.rw_ipquery_status = STATUS_OK;
//	DPRINTF("addr = 0X%X\r\n", ipconfig->addr);
	uint8_t my_addr[4] = {0};//对地址进行十进制转化
	my_addr[0] = (uint8_t)(ipconfig->addr >> 24);
	my_addr[1] = (uint8_t)(ipconfig->addr >> 16);
	my_addr[2] = (uint8_t)(ipconfig->addr >> 8);
	my_addr[3] = (uint8_t)(ipconfig->addr >> 0) ;
	DPRINTF("ipquery success addr = 0x%x(%d.%d.%d.%d)\r\n", ipconfig->addr,my_addr[0],my_addr[1],my_addr[2],my_addr[3]);//可以显示十进制的地址

	if(pconn->role_mode ==ROLE_AP){	
		if ((ret =rw_ipConfig(ipconfig, DHCP_SERVER)) != RW_OK){
			DPRINTF("rw_ipConfig error =%d\r\n", ret);
			//while(1);    
		}	
	}	
	if (ipmode != DHCP_CLIENT) {
		app_demo_ctx.rw_ipquery_status =STATUS_OK; 
	}
	return RW_OK;
}

int rw_network_startAP(void)
{
	int    ret =RW_OK;

	memset(&conn, 0, sizeof(conn));
	conn.role_mode = ROLE_AP;
//	conn.ssid = "RAK_AP";
//	conn.psk = "1234567890";
	conn.ssid = Wifi_AP_SSID;
	conn.psk = Wifi_AP_PASSWORD;
	conn.pmk = NULL;    //set NULL
	conn.channel = 6;
	conn.sec_mode = RW_SEC_TYPE_SEC;
	conn.auth_mode = RW_AUTH_TYPE_AUTO;

	rw_IpConfig_t ipconfig;
//	ipconfig.addr = 0xC0A80701;
//	ipconfig.mask = 0xFFFFFF00;
//	ipconfig.gw   = 0xC0A80701;
//	ipconfig.svr1 = 0xC0A80701;
	ipconfig.addr = Wifi_AP_IPConfig_t.addr;
	ipconfig.mask = Wifi_AP_IPConfig_t.mask;
	ipconfig.gw   = Wifi_AP_IPConfig_t.gw;
	ipconfig.svr1 = Wifi_AP_IPConfig_t.svr1;
	ret =rw_network_init(&conn, IP_CONFIG_STATIC, &ipconfig);  
	if (ret != RW_OK) {
		DPRINTF("rw_startAP error =%d\r\n", ret);
		while(1);
	}  
	return  ret;
}

int rw_network_startSTA(void)
{
	int                ret =RW_OK;
	rw_IpConfig_t      ipinfo;
	//    uint8_t ap_pmk[32]={0xa0, 0xd6, 0xd8, 0x52, 0x4b, 0x98, 0xf5, 0x5b, 0xd3, 0x6b, 0xa2, 0xb7, 0x59, 0x0a, 0x27, 0xfb, 0xf8, 0xe8, 0x51, 0xb0, 0x21, 0x31, 0x7c, 0x3f, 0xb8, 0xc5, 0x27, 0x21, 0x53, 0xd2, 0x42, 0x89};
	//    uint8_t ap_pmk[32]={0xFD, 0xC1, 0x78, 0xE3, 0x47, 0xAB, 0x07, 0xB9, 0xA0, 0xd3, 0xa0, 0xec, 0x00, 0xce, 0x0d, 0x3c, 0x30, 0x98, 0x1c, 0xa2, 0x14, 0xb5, 0xe1, 0xfc, 0xde, 0xa9, 0xa8, 0xca, 0x9d, 0xae, 0xd6, 0x6d};	

	memset(&conn, 0, sizeof(conn));
	conn.role_mode = ROLE_STA;
//	conn.ssid = "app-test";//"RAK_Wireless"  "1HEA-PC_Network_4" "mw_raktest""D-Link_DIR-600M"
//	conn.psk = "aptest12345";
	conn.ssid = Wifi_STA_SSID;//"RAK_Wireless"  "1HEA-PC_Network_4" "mw_raktest""D-Link_DIR-600M"
	conn.psk = Wifi_STA_PASSWORD;
	conn.pmk = NULL; //ap_pmk;NULL;
	conn.channel = 0;
	conn.sec_mode = RW_SEC_TYPE_SEC;
	conn.auth_mode = RW_AUTH_TYPE_AUTO;

	ret =rw_network_init(&conn, DHCP_CLIENT, &ipinfo);	
	if (ret != RW_OK) {
		DPRINTF("rw_startSTA error =%d\r\n", ret);
		while(1);
	}  
	return  ret;
}

int rw_network_startConfig(rw_ConfigMode_t mode)
{
	int    ret =RW_OK;

	app_demo_ctx.rw_easywps_status =STATUS_INIT;

	ret =rw_startEasyWps(mode);
	if (ret != RW_OK) {
		DPRINTF("rw_startEasyWps error =%d\r\n", ret);
		while(1);
	}  
	return  ret;
}

