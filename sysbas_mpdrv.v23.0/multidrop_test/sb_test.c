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
#include <linux/fd.h>
#include <poll.h>
#include <string.h>

/* multi-drop mode related ioctl commands */
#define TIOCSMULTIDROP      0x5470
#define TIOCSMDADDR         0x5471
#define TIOCGMDADDR         0x5472
#define TIOCSENDADDR        0x5473

#define MDMODE_ENABLE   0x2

int serial_fd;
int baudrate;
int delay;
int OpenSerial(char * port_name);
void SerialWriteData(int fd, char *buff, int len);
void SerialWriteWithAddress(int fd, char *buff, int len, int addr);
int ReadSerial(int fd, char * buff);
//===============================================================================	
void usage(void)
{
	printf("Usage: ./sb_test [Port Name] [Baudrate]\n");
	printf("Port Name : /dev/ttyMP0 ~ /dev/ttyMP32\n");
	printf("Baudrate  : 9600, 19200, ... \n");
}

int main(int argc, char ** argv)
{
	char send_buff[128]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int cnt = 0, len, i, mode = 0;
	char recv_buff[512];
	int base_delay = 200;
	int valid_addr = 0x23;
	int invalid_addr = 0x24;
	int addr;

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
			printf("Send test Mode ! \n");
			sleep(1);
			break;
		case 1:
			printf("Recv Test Mode ! \n");
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

	ioctl(serial_fd, TIOCSMDADDR, valid_addr);
	

	addr = valid_addr;
	ioctl(serial_fd, TIOCSENDADDR, addr);

	while(1)
	{
			if (mode == 0) 
			{
		if (++cnt>52)
		{
			cnt = 1;
#if 0
			if (addr == valid_addr)
			{
				addr = invalid_addr;
				printf("send data with invalid address\n");
			}
			else
			{
				addr = valid_addr;
				printf("send data with valid address\n");
			}
			ioctl(serial_fd, TIOCSENDADDR, addr);
#endif
		}
#if 1
			if (addr == valid_addr)
			{
				addr = invalid_addr;
				printf("?\n");
			}
			else
			{
				addr = valid_addr;
				printf("!\n");
			}
			ioctl(serial_fd, TIOCSENDADDR, addr);
//			ioctl(serial_fd, TIOCSENDADDR, 0x23);
#endif

		SerialWriteWithAddress(serial_fd, send_buff, cnt, addr);
		SerialWriteWithAddress(serial_fd, "\n", 1, addr);
		usleep(delay);
			}

			if (mode == 1)
			{
		do {
			len=ReadSerial(serial_fd, recv_buff);
			if (len>0)
			{
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
	ioctl(fd, TIOCSMULTIDROP, MDMODE_ENABLE);

	return fd;
}	


void SerialWriteData(int fd, char *buff, int len)
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

void SerialWriteWithAddress(int fd, char *buff, int len, int addr)
{
	char *p;
	int ret;
	int cnt=0;

	if (len <=0) return;

//	ioctl(fd, TIOCSENDADDR, addr);

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
	fd_set readfds;
	int state;
	struct timeval tv;


	p=buff;

	while(1) 
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		state = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (state <= 0 ) break;

		if (FD_ISSET(fd, &readfds))
		{
			usleep(1);
			len = read(fd, p, buffsize - tot);
			if (len <= 0) break;
			p += len;
			tot += len;
			if (tot >= 256) break;
		}
	}

	return tot;
}
