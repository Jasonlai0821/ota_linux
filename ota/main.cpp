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
#include <pthread.h>

#define UPDATE_TIME 4		// check new version timer at 4:00:00
#define INTERVAL_TIME 24	// set the interal time one day

#define MAX_LINE_LENGTH 1024

#define SIZE 16
#define D_SIZE 32

#define OTA_STATUS              "ota_upgrade_status"

#define BUFFER_SIZE_4M 4 * 1024 * 1024
#define BUFFER_SIZE_4K 4 * 1024

#define OTA_BUFFER_INFO 0
#define OTA_BUFFER_DATA 1


//static char Block_Buffer0[BUFFER_SIZE_4M] = {0};
//static char Block_Buffer1[BUFFER_SIZE_4M] = {0};

static char Block_Buffer0[BUFFER_SIZE_4M+4] = {0};
static char Block_Buffer1[BUFFER_SIZE_4M+4] = {0};

static int buf_pos = 0;
static int buf_type = OTA_BUFFER_INFO; //type 0: ota info, 1: ota data
static long rev_size = 0;
static long total_size = 0;
static long rec_total_size = 0;
static long write_pos =0;


#define DOWNLOAD_FILE_LOCAL 0

static FILE *fp = NULL;

void onStartOTA();

typedef unsigned int   uint32_t;


static uint32_t _crc32 (uint32_t crc, char* buf, uint32_t len)
{
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

    static const uint32_t crc_table[] =
    {
        0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
        0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
        0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
        0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
        0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
        0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
        0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
        0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
        0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
        0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
        0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
        0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
        0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
        0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
        0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
        0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
        0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
        0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
        0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
        0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
        0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
        0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
        0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
        0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
        0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
        0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
        0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
        0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
        0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
        0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
        0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
        0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
        0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
        0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
        0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
        0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
        0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
        0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
        0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
        0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
        0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
        0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
        0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
        0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
        0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
        0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
        0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
        0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
        0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
        0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
        0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
        0x2d02ef8dL
    };

    crc = crc ^ 0xffffffffL;
    while(len >= 8)
    {
        DO8(buf);
        len -= 8;
    }
    if(len)
    {
        do
        {
            DO1(buf);
        }
        while(--len);
    }

    return crc ^ 0xffffffffL;
}


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

void * thread_update (void *arg)
{
	char * buffer;
	long write_lenth = 0;
	long need_write_lenth = 0;
	//printf("thread_update() buf_pos: %d,buf_type: %d\n",buf_pos,buf_type);
	if(buf_pos == 1){//处理数据Block_Buffer0
		buffer = &Block_Buffer0[0];
	}else if(buf_pos == 0){//处理数据Block_Buffer1
		buffer = &Block_Buffer1[0];
	}

	if(buf_type == OTA_BUFFER_INFO){
		//write_lenth = BUFFER_SIZE_4K+4;
		write_lenth = BUFFER_SIZE_4K;
	}else if(buf_type == OTA_BUFFER_DATA){
		if(rec_total_size == total_size){
			write_lenth = rev_size;
		}else{
			write_lenth = BUFFER_SIZE_4M;
		}
	}

	if(buf_type == OTA_BUFFER_INFO){
		//ms_parse_otainfo(buffer);
	}else if(buf_type == OTA_BUFFER_DATA){
		printf("thread_update() rec_total_size: %ld,total_size: %ld,write_lenth: %ld,offset: %ld\n",rec_total_size,total_size,write_lenth,rec_total_size-write_lenth-BUFFER_SIZE_4K);
		if(rec_total_size == total_size){
			need_write_lenth = write_lenth -4;//最后一个数据包中最后的4Byte是crc校验值
		}else{
			need_write_lenth = write_lenth;
		}
		//ms_ota_update_start(rec_total_size-write_lenth-BUFFER_SIZE_4K,buffer,need_write_lenth);
		#if DOWNLOAD_FILE_LOCAL
		if(fp != NULL)
		{
			fwrite(buffer, 1, write_lenth, fp);
		}
		#endif
		if(rec_total_size == total_size){
			//ms_setenv(OTA_STATUS, "0");
	    	usleep(1000 * 1000);
	    	system("reboot");
	    	printf("thread_update() the last buffer writed finished\n");
		}
	}
}


void * thread_update0 (void *arg)
{
	char * buffer;
	long write_lenth = 0;
	long need_write_lenth = 0;
	uint32_t crc32 = 0;
	uint32_t _PackageCrc32 = 0;
	//printf("thread_update() buf_pos: %d,buf_type: %d\n",buf_pos,buf_type);
	if(buf_pos == 1){//处理数据Block_Buffer0
		buffer = &Block_Buffer0[0];
	}else if(buf_pos == 0){//处理数据Block_Buffer1
		buffer = &Block_Buffer1[0];
	}

	if(buf_type == OTA_BUFFER_INFO){
		write_lenth = BUFFER_SIZE_4K;
	}else if(buf_type == OTA_BUFFER_DATA){
		if(rec_total_size == total_size){
			write_lenth = rev_size;
		}else{
			write_lenth = BUFFER_SIZE_4M;
		}
	}

	if(rec_total_size == total_size){
		need_write_lenth = write_lenth -4;//最后一个数据包中最后的4Byte是crc校验值
	}else{
		need_write_lenth = write_lenth;
	}
	_PackageCrc32 = _crc32(_PackageCrc32, buffer, need_write_lenth);
	memcpy(&crc32, buffer + need_write_lenth, sizeof(crc32));
	
	printf("Info CRC check PackageCrc32: %08X, crc32: %08X\n", _PackageCrc32, crc32);
	if (_PackageCrc32 != crc32) {
		printf("Info CRC check error, PackageCrc32: %08X, crc32: %08X\n", _PackageCrc32, crc32);
		printf("Info CRC check error, should reboot to receiver data\n");
		system("reboot");
	}
	
	if(buf_type == OTA_BUFFER_INFO){
		//ms_parse_otainfo(buffer);
		write_pos += write_lenth;
	}else if(buf_type == OTA_BUFFER_DATA){
		printf("thread_update() rec_total_size: %ld,total_size: %ld,write_lenth: %ld,offset: %ld\n",rec_total_size,total_size,write_lenth,write_pos);
		//ms_ota_update_start(write_pos,buffer,need_write_lenth);
		write_pos += need_write_lenth;
		
		#if DOWNLOAD_FILE_LOCAL
		if(fp != NULL)
		{
			fwrite(buffer, 1, write_lenth, fp);
		}
		#endif
		if(rec_total_size == total_size){
			//ms_setenv(OTA_STATUS, "0");
	    	usleep(1000 * 1000);
	    	system("reboot");
	    	printf("thread_update() the last buffer writed finished\n");
		}
	}
}


void processBuf_Thread()
{
	pthread_t tid;
	printf("processBuf_Thread()\n");
    if(pthread_create(&tid, NULL, thread_update0, NULL) == -1)
    {
        printf("fail to pthread_create");
        return;
    }

    void * result;
    if(pthread_join(tid, &result) == -1){
        printf("fail to pthread_join");
        return;
    }
}

/*
	ota.bin文件结构：文件开头4K数据是ota info，后续的数据是ota data
*/

void processReceiverBuffer(void* data, int length)
{
	const char * buffer = (char*)data;
	long max_size =0;
	if(length == 0 || data == NULL){
		return;
	}

	rec_total_size += length;
	
	if(buf_type == OTA_BUFFER_INFO){//先处理ota info 4K的数据量
		max_size = BUFFER_SIZE_4K+4;//4 is for crc32
		//max_size = BUFFER_SIZE_4K;//4 is for crc32
	}else if(buf_type == OTA_BUFFER_DATA){
		//max_size = BUFFER_SIZE_4M;
		max_size = BUFFER_SIZE_4M+4;
	}

	//printf("processReceiverBuffer max_size: %ld,length: %d,rev_size: %ld\n",max_size,length,rev_size);
	
	if((rev_size + length) > max_size){//data数据已经超过max_size
		int need_len = max_size - rev_size;
		//printf("processReceiverBuffer buf_pos: %d\n",buf_pos);
		if(buf_pos == 0){
			char *pos = &Block_Buffer0[rev_size];
			memcpy(pos,buffer,need_len);
			//memset(Block_Buffer1,0,BUFFER_SIZE_4M);
			memset(Block_Buffer1,0,BUFFER_SIZE_4M+4);
			pos = &Block_Buffer1[0];
			memcpy(pos,buffer+need_len,length-need_len);
			buf_pos = 1;
			rev_size =length-need_len;	
			//fwrite(Block_Buffer0, 1, max_size, fp);
		}else if(buf_pos == 1){
			char *pos = &Block_Buffer1[rev_size];
			memcpy(pos,buffer,need_len);
			//memset(Block_Buffer0,0,BUFFER_SIZE_4M);
			memset(Block_Buffer0,0,BUFFER_SIZE_4M+4);
			pos = &Block_Buffer0[0];
			memcpy(pos,buffer+need_len,length-need_len);
			buf_pos = 0;
			rev_size =length-need_len;
			//fwrite(Block_Buffer1, 1, max_size, fp);
		}
		
		//另起线程来将数据写入nand flash
		processBuf_Thread();
		//printf("processReceiverBuffer buf_type: %d\n",buf_type);
		if(buf_type == OTA_BUFFER_INFO){
			buf_type = OTA_BUFFER_DATA;
		}
	}else{
		if(buf_pos == 0){
			char *pos = &Block_Buffer0[rev_size];
			memcpy(pos,buffer,length);
			rev_size += length;
		}else if(buf_pos == 1){
			char *pos = &Block_Buffer1[rev_size];
			memcpy(pos,buffer,length);
			rev_size += length;
		}
		//printf("processReceiverBuffer buf_pos: %d,rev_size: %d\n",buf_pos,rev_size);
		
		//处理最后一个数据包，下载完毕
		if(rec_total_size == total_size){
			printf("processReceiverBuffer the last data size:%ld \n",rev_size);
			if(buf_pos == 0){
				buf_pos = 1;
			}else if(buf_pos == 1){
				buf_pos = 0;
			}
			processBuf_Thread();
		}
	}
	
}

void callback_notify (int status, int param1, void* param2)
{
	//printf("status: %d,param1:%d\n",status,param1);
	if(status == STATUS_DOWNLOAD_FINISHED || status == STATUS_DOWNLOAD_ERROR || status ==  STATUS_NEW_VERSION_NOT_EXISTED){
		stopDownloading();
		switch(status){
			case STATUS_DOWNLOAD_FINISHED:
				printf("\t**********  recoverySystem ota  **********\n");
				//system("recoverySystem ota /userdata/update.img");
				#if DOWNLOAD_FILE_LOCAL
				if(fp != NULL)
			    {
			        fclose(fp);
			        fp = NULL;
			    }
				#endif
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
	}else if(status == STATUS_NEW_VERSION_EXISTED){
		printf("\t**********  SKYWORTH OTA NEW VERSION EXISTED **********\n");
		char *filename = (char*)(param2);
		printf("OTA Download path:%s \n",filename);
		#if DOWNLOAD_FILE_LOCAL
		fp = fopen("/data/tmp.img", "wb");
		if(fp == NULL){
			printf("OTA Download path /data/tmp.img open failed\n");
		}else{
			printf("OTA Download path /data/tmp.img open success\n");
		}
		#endif
		total_size = param1;
	}else if(status == STATUS_DOWNLOAD_PROGRESS){
		//printf("\t**********  SKYWORTH OTA PROGESS =%d **********\n",param1);
		processReceiverBuffer((char*)param2, param1);
		//fwrite((char*)param2, 1, param1, fp);
	}
}

void onStartOTA()
{
	char szMac[18];
	char localversion[SIZE];
	char localmodel[D_SIZE];
	char localchip[D_SIZE];
	char upgrade_status[4] = {0};
	uint8_t runMode = 0; // 0: normal; 1: recovery

	buf_pos = 0;
	buf_type = OTA_BUFFER_INFO; //type 0: ota info, 1: ota data
	rev_size = 0;
	total_size = 0;
	rec_total_size = 0;
	write_pos =0;

	/*prepare_ota(); //准备ota环境，读取相关环境变量
	if (ms_getenv(OTA_STATUS, upgrade_status) != 0) {
        printf("[OTA] get %s fail \n", upgrade_status);
        return ;
    }
    
    runMode = atoi(upgrade_status);*/
	
	//SkyOtaRequestInfo *info = (SkyOtaRequestInfo*)malloc(sizeof(SkyOtaRequestInfo) * 1);
	SkyOtaRequestInfo *info = new SkyOtaRequestInfo();
	
	memset(szMac, 0, sizeof(szMac));
	memset(localversion, 0, sizeof(localversion));
	memset(localmodel, 0, sizeof(localmodel));
	memset(localchip, 0, sizeof(localchip));
	
	int ret = getLocalVersionInfo(localversion,localmodel,localchip);
	if(ret > 0){
		strncpy((char*)info->lc_version,localversion,strlen(localversion)-1);
		strncpy((char*)info->model,localmodel,strlen(localmodel)-1);
		strncpy((char*)info->chip,localchip,strlen(localchip)-1);
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
	strcpy((char*)file_path,"/app/update.img");
	info->cb_notify = &callback_notify;

	init(info,file_path,D_SIZE);

	if(runMode ==0){
		if(isNewlyVersionExisted()){
			//ms_setenv(OTA_STATUS, "1");//当前模式为：normal,设置相应模式recovery,然后重启
			printf("reboot to enter recovery mode for downloading img\n");
			usleep(10*1000);
        	system("reboot");
		}else{
			printf("\t**********  SKYWORTH OTA NO NEW VERSION EXIT **********\n");
		}
	}else{
		#if DOWNLOAD_FILE_LOCAL
		system("rm -rf /app/update.img");
		system("rm -rf /data/tmp.img");
		#endif
		startDownloading();
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
		
		printf("next time is: %ld\n",tv);
		os::Thread::sleep(tv*1000);
		onStartOTA();
		i++;
	}
    return 0;
}
