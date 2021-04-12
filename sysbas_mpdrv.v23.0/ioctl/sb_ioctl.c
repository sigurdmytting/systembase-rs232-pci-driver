
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <memory.h>

#define GETDEEPFIFO             0x54AA
#define SETDEEPFIFO             0x54AB
#define SETFCR                  0x54BA
#define SETTTR			0x54B1
#define SETRTR			0x54B2
#define GETTTR			0x54B3
#define GETRTR			0x54B4

#define TIOCGGETNRPORTS         0x546C //number of port for arg
#define TIOCGNUMOFPORT          0x545F //number of all port
#define TIOCGGETDEVID           0x5468
#define TIOCGGETBDNO            0x5469 //number of board
#define TIOCGGETINTERFACE       0x546A
#define TIOCGGETREV             0x546B
#define TIOCGGETPORTTYPE        0x546D

#define TIOCSMULTIECHO          0x5440
#define TIOCSPTPNOECHO          0x5441

#define ENABLE	1
#define DISABLE	0
int fcr[256];
void Usage(void)
{
	printf("Usage     : ./sb_ioctl [Port Name] [Command] [Value]\n");
	printf("Port Name : /dev/ttyMP0 ~ /dev/ttyMP256\n");
	printf("Command   : -D > Print All Port Infomation(256Fifo,FCR,TTR,RTR)\n");
	printf("          : -F > Setting [Port Name] of 256-Byte FIFO to [0/1] (0:disable, 1:enable)\n");
	printf("	  : -T > Setting [Port Name] of TxTriggerLevel to [value]\n");
	printf("	  : -R > Setting [Port Name] of RxTriggerLevel to [value]\n");
	printf("Value 	  : Value is Decimal Number\n");
}


void ReadFCR(int maxPort)
{
	FILE *fp;
        int ret,i,fd;
        char tmp[100];
	char temp;
	fpos_t fa;
	
	for(i=0;i<maxPort;i++)
		fcr[i] = 1;

        if((fp=fopen("/etc/sb_mp","r+"))<0){
                printf("Can't Open /etc/sb_mp");
                return;
        }
	fseek(fp,17,SEEK_SET);
	
	for(i=0;i<maxPort;i++){
		ret = fscanf(fp,"%x,",&fcr[i]);
	}

	fclose(fp);

}

void GetInfo(int maxPort)
{
	int i,ret,fd;
	char tmp[100];

	printf("===DEEPFIFO INFO===");
	for(i=0;i<maxPort;i++){
                sprintf(tmp,"/dev/ttyMP%d",i);
                 if( (fd = open(tmp,O_RDWR)) < 0 ){
                        printf("Can't open /dev/ttyMP%d",i);
                         printf("Please check your board in slot..\n");
                        return ;
                 }
		if((i%8)==0)
			printf("\n");
                ret = ioctl(fd,GETDEEPFIFO,10);
                printf("MP%d=%x\t",i,ret);
                close(fd);
        }
	printf("\n\n\n");
	
	printf("===TTRINFO===");
        for(i=0;i<maxPort;i++){
                sprintf(tmp,"/dev/ttyMP%d",i);
                 if( (fd = open(tmp,O_RDWR)) < 0 ){
                        printf("Can't open /dev/ttyMP%d",i);
                         printf("Please check your board in slot..\n");
                        return ;
                 }
		if((i%8)==0)
                        printf("\n");
                ret = ioctl(fd,GETTTR,10);
                printf("MP%d=%x\t",i,ret);
                close(fd);
        }
        printf("\n\n\n");

	printf("===RTRINFO===");
        for(i=0;i<maxPort;i++){
                sprintf(tmp,"/dev/ttyMP%d",i);
                 if( (fd = open(tmp,O_RDWR)) < 0 ){
                        printf("Can't open /dev/ttyMP%d",i);
                         printf("Please check your board in slot..\n");
                        return ;
                 }
		if((i%8)==0)
                        printf("\n");
                ret = ioctl(fd,GETRTR,10);
                printf("MP%d=%x\t",i,ret);
                close(fd);
        }
        printf("\n\n\n");

	printf("===FCRINFO===");
        for(i=0;i<maxPort;i++){
		if((i%8)==0)
                        printf("\n");
                printf("MP%d=%x\t",i,fcr[i]);
        }
        printf("\n");

}
void Save(int maxPort)
{
	FILE *fp;
	int ret,i,fd;
	char tmp[100];

	if((fp=fopen("/etc/sb_mp","r+"))<0){
		printf("Can't Open /etc/sb_mp");
		return;
	}
	fprintf(fp,"#!/bin/bash\n\n");

        fprintf(fp,"fcr=");
        for(i=0;i<maxPort;i++){
                if((i+1)==maxPort)
                        fprintf(fp,"%x",fcr[i]);
                else
                        fprintf(fp,"%x,",fcr[i]);
        }


	/* SAVE DEEP FIFO SETTING */
	fprintf(fp,"\n\ndeep=");
	for(i=0;i<maxPort;i++){
                sprintf(tmp,"/dev/ttyMP%d",i);
                 if( (fd = open(tmp,O_RDWR)) < 0 ){
                        printf("Can't open /dev/ttyMP%d",i);
                         printf("Please check your board in slot..\n");
                        return ;
                 }
                ret = ioctl(fd,GETDEEPFIFO,10);
		if((i+1)==maxPort)
			fprintf(fp,"%x",ret);
		else
			fprintf(fp,"%x,",ret);
                close(fd);
        }
	
	/* SAVE TTR SETTING */
	fprintf(fp,"\n\nttr=");
        for(i=0;i<maxPort;i++){
                sprintf(tmp,"/dev/ttyMP%d",i);
                 if( (fd = open(tmp,O_RDWR)) < 0 ){
                        printf("Can't open /dev/ttyMP%d",i);
                         printf("Please check your board in slot..\n");
                        return ;
                 }
                ret = ioctl(fd,GETTTR,0);
                if((i+1)==maxPort)
                        fprintf(fp,"%x",ret);
                else
                        fprintf(fp,"%x,",ret);
                close(fd);
        }

	/* SAVE RTR SETTING */
        fprintf(fp,"\n\nrtr=");
        for(i=0;i<maxPort;i++){
                sprintf(tmp,"/dev/ttyMP%d",i);
                 if( (fd = open(tmp,O_RDWR)) < 0 ){
                        printf("Can't open /dev/ttyMP%d",i);
                         printf("Please check your board in slot..\n");
                        return ;
                 }
                ret = ioctl(fd,GETRTR,0);
                if((i+1)==maxPort)
                        fprintf(fp,"%x",ret);
                else
                        fprintf(fp,"%x,",ret);
                close(fd);
        }
	

	fprintf(fp,"\n\nmodprobe golden_tulip deep=$deep rtr=$rtr ttr=$ttr fcr_arr=$fcr\n\n");
	fclose(fp);

}

void SET_PORT(char *port, int cmd, int value)
{
	int fd, ret;
	int pt;
	int hex, dec;

	if((fd=open(port,O_RDWR))<0){
                printf("Can't open %s\n",port);
                printf("Please check your board in slot..\n");
                     return ;
         }

    switch (cmd) {
        case 0:
        	if (value == 1) {
	       		ret = ioctl(fd,SETDEEPFIFO,ENABLE);
		    } else if (value == 0) {
	       		ret = ioctl(fd,SETDEEPFIFO,DISABLE);
		    }
            break;
        case 1:
		    if (0 <= value && value <= 255) {
			    ret = ioctl(fd,SETTTR,value);
		    } else {
			    printf("Input is out of range\n");
                goto err;
		    }
            break;
        case 2:
		    if (0 <= value && value <= 255) {
                ret = ioctl(fd,SETRTR,value);
            }
            else{
                printf("Input is out of range\n");
                goto err;
            }
    }
	printf("ret = %d\n",ret);
err:
    close(fd);
}

int main(int argc, char *argv[])
{
 	int ret,all_port;
	int fd,i;
	int hex;
    char c;
    int flags = 0;
    int fifo_enable;
    int tx_trig;
    int rx_trig;

	if( (fd = open("/dev/ttyMP0",O_RDWR)) < 0 ){
                printf("Can't open /dev/ttyMP0\n");
                printf("Please check your board in slot or Root Permmision.\n");
                return -1;
        }
        all_port = ioctl(fd,TIOCGNUMOFPORT,0);

	close(fd);

    if (argc == 1) {
        Usage();
        exit(0);
    }
	
	ReadFCR(all_port);

    while( (c = getopt(argc, argv, "F:T:R:D")) != -1) {
        switch(c) {
            case 'F':
                flags |= 0x1;
                fifo_enable = atoi(optarg);
                break;
            case 'T':
                flags |= 0x2;
                tx_trig = atoi(optarg);
                break;
            case 'R':
                flags |= 0x4;
                rx_trig = atoi(optarg);
                break;
            case 'D':
                GetInfo(all_port);
                exit(0);
                break;
            case '?':
            default:
                Usage();
                exit(0);
                break;
        }
    }
	
    if (flags & 0x1) {
        SET_PORT(argv[optind], 0, fifo_enable);
    }
    if (flags & 0x2) {
        SET_PORT(argv[optind], 1, tx_trig);
    }
    if (flags & 0x4) {
        SET_PORT(argv[optind], 2, rx_trig);
    }
     
	Save(all_port);

	return 0;	
			
}
