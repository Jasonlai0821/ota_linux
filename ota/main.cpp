#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <time.h> 
#include <unistd.h>
#include "SkyOTAUpgrade.h"
#include <iostream>
#include "Thread.h"
#include <net/if.h>        //for struct ifreq

#define UPDATE_TIME 4		// check new version timer at 4:00:00
#define INTERVAL_TIME 24	// set the interal time one day

#define MAX_LINE_LENGTH 1024

#define SIZE 16
#define D_SIZE 32

void onStartOTA();

int get_mac(char * mac, int len_limit)    //返回值是实际写入char * mac的字符个数（不包括'\0'）
{
	printf("get_mac: enter\n");
	
    struct ifreq ifreq;
    int sockFd = -1;
    int ioctrlRet = -1;

    if ((sockFd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
    	printf("get_mac: socket error:%d\n", sockFd);
        return -1;
    }
	
	memset(&ifreq,0,sizeof(ifreq));
    strcpy (ifreq.ifr_name, "wlan0");

    if ((ioctrlRet = ioctl (sockFd, SIOCGIFHWADDR, &ifreq)) < 0)
    {
    	printf("get_mac: ioctl error:%d\n", ioctrlRet);
        close(sockFd);
        return -1;
    }
    
    close(sockFd);
	
    printf("get_mac() ifr_hwaddr:%02X:%02X:%02X:%02X:%02X:%02X\n", ifreq.ifr_hwaddr.sa_data[0],
									ifreq.ifr_hwaddr.sa_data[1],
									ifreq.ifr_hwaddr.sa_data[2],
									ifreq.ifr_hwaddr.sa_data[3],
									ifreq.ifr_hwaddr.sa_data[4],
									ifreq.ifr_hwaddr.sa_data[5]);
	
    return snprintf (mac, len_limit, "%02X%02X%02X%02X%02X%02X", 
    									(unsigned char) ifreq.ifr_hwaddr.sa_data[0], 
    									(unsigned char) ifreq.ifr_hwaddr.sa_data[1], 
    									(unsigned char) ifreq.ifr_hwaddr.sa_data[2], 
    									(unsigned char) ifreq.ifr_hwaddr.sa_data[3], 
    									(unsigned char) ifreq.ifr_hwaddr.sa_data[4], 
    									(unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
}

int getLocalVersionInfo(char * data, char * model, char * chip)
{
	int ret = -1;
	FILE *fp;
	char strLine[MAX_LINE_LENGTH];
	if((fp = fopen("/etc/ota_version.conf","r")) == NULL){
		printf("Open /etc/ota_version.conf Falied!"); 
		return ret;
	}
	
	while(fgets(strLine,MAX_LINE_LENGTH,fp) != NULL){
		if(strstr(strLine,"DATE:") != NULL){
			strcpy(data,strLine+5);
		}
		if(strstr(strLine,"SKY_MODEL:") != NULL){
			strcpy(model,strLine+10);
		}
		if(strstr(strLine,"SKY_CHIP:") != NULL){
			strcpy(chip,strLine+9);
		}
	}
	
	if(data[0] != '\0' && model[0] != '\0' && chip[0] != '\0'){
		ret = 1;
	}
	fclose(fp);
	return ret;
}


void onDoTask(int a)
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);

	printf("do task current time is: %d:%d:%d\n",p->tm_hour,p->tm_min,p->tm_sec);
	onStartOTA();
}

void onSetTimer()
{
	long tv;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);

	printf("onSetTimer() current time is: %d:%d:%d\n",p->tm_hour,p->tm_min,p->tm_sec);

	if(p->tm_hour < UPDATE_TIME){
	 	tv = ((UPDATE_TIME - p->tm_hour - 1)* 60 + p->tm_min) * 60;
	}else{
		tv = ( (INTERVAL_TIME + UPDATE_TIME - p->tm_hour - 1) * 60 + p->tm_min) * 60;
	}
	struct itimerval t;
	t.it_interval.tv_usec = 0;   /* ¼ÆÊ±Æ÷ÖØÆô¶¯µÄ¼äÐªÖµ */
    t.it_interval.tv_sec = INTERVAL_TIME*60*60;
	t.it_value.tv_usec = 0;/* ¼ÆÊ±Æ÷°²×°ºóÊ×ÏÈÆô¶¯µÄ³õÊ¼Öµ */
    t.it_value.tv_sec = tv;

	if(setitimer( ITIMER_REAL, &t, NULL) < 0 ){
        perror("onSetTimer() error.\n");
        return;
    }

	signal(SIGALRM, onDoTask);
}

void callback_notify (int status, int param1, int param2)
{
	printf("status: %d,param1:%d,param2:%d\n",status,param1,param2);
	if(status == STATUS_DOWNLOAD_FINISHED || status == STATUS_DOWNLOAD_ERROR || status ==  STATUS_NEW_VERSION_NOT_EXISTED){
		stopDownloading();
		switch(status){
			case STATUS_DOWNLOAD_FINISHED:
				printf("\t**********  recoverySystem ota  **********\n");
				system("recoverySystem ota /userdata/update.img");
				break;
			case STATUS_NEW_VERSION_NOT_EXISTED:
			case STATUS_DOWNLOAD_ERROR:
				printf("\t**********  SKYWORTH OTA NO NEW VERSION  **********\n");
				break;
			default:
				printf("SKYWORTH OTA ERROR");
				break;
		}
		//deInit();
	}
}

void onStartOTA()
{
	char szMac[18];
	char localversion[SIZE];
	char localmodel[D_SIZE];
	char localchip[D_SIZE];
	
	SkyOtaRequestInfo *info = new SkyOtaRequestInfo();
	
	memset(szMac, 0, sizeof(szMac));
	memset(localversion, 0, sizeof(localversion));
	memset(localmodel, 0, sizeof(localmodel));
	memset(localchip, 0, sizeof(localchip));
	
	int ret = getLocalVersionInfo(localversion,localmodel,localchip);
	if(ret > 0){
		strncpy((char*)info->lc_version,localversion,9);
		strncpy((char*)info->model,localmodel,5);
		strncpy((char*)info->chip,localchip,4);
	}else{
		printf("getLocalVersionInfo error!!!\n");
		return;
	}
	
    int nRet = get_mac(szMac, sizeof(szMac));
	if(nRet > 0)
    {
        printf("OTA MAC ADDR: %s\n", szMac);
        strcpy((char*)info->mac,szMac);
    }else{
		printf("OTA MAC ADDR is NULL\n");
		return;
	}
	//strcpy((char*)info->mac,"0e04c5c5d231");
	unsigned char file_path[D_SIZE] = {0};
	strcpy((char*)file_path,"/userdata/update.img");
	info->cb_notify = &callback_notify;

	init(info,file_path,D_SIZE);

	if(isNewlyVersionExisted()){
		system("rm -rf /userdata/update.img");
		startDownloading();
	}else{
		printf("\t**********  SKYWORTH OTA NO NEW VERSION EXIT **********\n");
		//onSetTimer();//start alarm to set next time check version
	}
}

int
main(int argc, char *argv[])
{
	int i = 1;
	long tv;
	time_t timep;
	struct tm *p;
	
	onStartOTA();
	while(i != 1000){
		time(&timep);
		p = localtime(&timep);

		printf("i= %d ,current time is: %d:%d:%d\n",i,p->tm_hour,p->tm_min,p->tm_sec);
		
		if(p->tm_hour < UPDATE_TIME){
			tv = ((UPDATE_TIME - p->tm_hour - 1)* 60 + (60 - p->tm_min -1)) * 60 + (60 - p->tm_sec);
		}else{
			tv = ( (INTERVAL_TIME + UPDATE_TIME - p->tm_hour - 1) * 60 + (60 - p->tm_min -1)) * 60 + (60 - p->tm_sec);
		}
		
		printf("next time is: %d\n",tv);
		os::Thread::sleep(tv*1000);
		onStartOTA();
		i++;
	}
    return 0;
}
