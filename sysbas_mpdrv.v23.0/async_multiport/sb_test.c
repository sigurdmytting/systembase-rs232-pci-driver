#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <poll.h>
#include <string.h>

int serial_fd;
int baudrate;
int delay;
int OpenSerial(char * port_name);
void SerialWrite(int fd, char *buff, int len);
int ReadSerial(int fd, char * buff);
//===============================================================================	
void usage(void)
{
	printf("Usage: ./sb_test [Port Name] [Baudrate] [TestMode]\n");
	printf("Port Name : /dev/ttyMP0 ~ /dev/ttyMP32\n");
	printf("Baudrate  : 9600, 19200, ... \n");
	printf("TestMode  : 0(Loopback) 1(Send) 2(Recv) \n");
}

int main(int argc, char ** argv)
{
	char send_buff[128]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int cnt = 0, len, i, mode = 0;
	char recv_buff[512];
	int base_delay = 100;

	if (argc != 4 )
	{
		usage();
		return 0;
	}

	baudrate = atoi(argv[2]);
	mode = atoi(argv[3]);

	switch(baudrate)
	{
		case    600: baudrate=   B600; delay = base_delay * 6400; break;
		case   1200: baudrate=  B1200; delay = base_delay * 3200; break;
		case   2400: baudrate=  B2400; delay = base_delay * 1600; break;
		case   4800: baudrate=  B4800; delay = base_delay * 800 ; break;
		case   9600: baudrate=  B9600; delay = base_delay * 400; break;
		case  19200: baudrate= B19200; delay = base_delay * 200 ; break;
		case  38400: baudrate= B38400; delay = base_delay * 96 ; break;
		case  57600: baudrate= B57600; delay = base_delay * 48 ; break;
		case 115200: baudrate=B115200; delay = base_delay * 24 ; break;
		case 230400: baudrate=B230400; delay = base_delay * 12  ; break;
		case 460800: baudrate=B460800; delay = base_delay * 6  ; break;
		case 921600: baudrate=B921600; delay = base_delay * 3; break;
		default: usage(); return 0;
	}

	switch(mode)
	{
		case 0:
			printf("Loopback Test Mode ! \n");
			sleep(1);
			break;
		case 1:
			printf("Send Test Mode ! \n");
			sleep(1);
			break;
		case 2:
			printf("Receive Test Mode ! \n");
			sleep(1);
			break;
		default:
			usage();
			return 0;
	}
	
	
	if ((serial_fd = OpenSerial(argv[1]))<0)
	{
		printf("Serial Open Error: %s\n", argv[1]);
		return 0;
	}
	

	while(1)
	{
		if (++cnt>52)
			cnt = 1;

		if (mode !=2)
		{
			SerialWrite(serial_fd,send_buff,cnt);
			SerialWrite(serial_fd,"\n",1);
		}
		usleep(delay);

		if (mode !=1)
		{
			do {
				len=ReadSerial(serial_fd, recv_buff);
				if (len>0)
				{
					int i;
					for(i=0;i<len;i++)
						printf("%c",recv_buff[i]);
				}
				else
				{
					
		usleep(delay);
					int i;
					for(i=0;i<len;i++)
						printf("%c",recv_buff[i]);
				}
			}while(len>0);
		}

	}
	return 0;
}

int OpenSerial(char * port_name)
{
	int fd;
	struct termios newtio;

	bzero(&newtio, sizeof(newtio));

	if ((fd=open(port_name, O_RDWR|O_NOCTTY|O_NDELAY))==-1)
		return -1;

	newtio.c_cflag = baudrate|CS8|CLOCAL|CREAD;
	newtio.c_cflag |= HUPCL;
	newtio.c_iflag |= (IGNPAR | IGNBRK);
	newtio.c_iflag &= ~(IGNCR|ICRNL);

	newtio.c_oflag &= ~(ONLCR);
	newtio.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	return fd;
}	


void SerialWrite(int fd, char *buff, int len)
{
	char *p;
	int ret;
	int cnt=0;

	if (len <=0) return;

	p=buff;
	do {
		ret = write(fd, p, len);
		if(ret>0)
		{
			len -= ret;
			p += ret;
		}
		if (len>0)
		{
			if (++cnt>20) break;;
			usleep(1);
		}
	}while(len);

	return;
}

int ReadSerial(int fd, char * buff)
{
	int len, tot = 0;
	char *p;
	int buffsize = 256;

	p=buff;
	len=read(fd, p, buffsize);
	if (len <= 0) return 0;

	do {
		p += len;
		tot += len;
		if (tot >= 500) break;
		usleep(1);
		len = read(fd, p, buffsize - tot);
	}while (len>0);

	return tot;
}

