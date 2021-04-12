
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>
#include <errno.h>
#include <linux/version.h>
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(ver,rel,seq)     ((ver << 16) | (rel << 8) | seq)
#endif

#define TIOCGNUMOFPORT		0x545F
#define TIOCGGETDEVID		0x5468
#define TIOCGGETBDNO		0x5469
#define TIOCGGETINTERFACE	0x546A
#define TIOCGGETREV		0x546B
#define TIOCGGETNRPORTS		0x546C
#define TIOCGGETPORTTYPE	0x546D
#define RS232           1 
#define RS422PTP        2
#define RS422MD         3
#define RS485NE         4
#define RS485ECHO       5

static char *sw_version = "23.0";
static char *sw_revdate = "2020-01-30";

void MKNOD(char *filename, int major, int minor);

int main(int argc, char *argv[])
{

	int fd,i;
	unsigned int nr_ports[4];
	unsigned int nr_bds;
	unsigned int device_id[4];
	unsigned int rev[4];
	unsigned int interface[4];
	unsigned int port_type[4];
	char list[128];
	char name[128];
	char dev_name[4][32];
	char interface_name[4][32];
	char port_type_name[4][32];
	int n = 0, port = 0;
	int iir;
	
	MKNOD("/dev/ttyMPCON",54,0);
	
	if( (fd = open("/dev/ttyMPCON",O_RDWR)) < 0 ){
	printf("%d,%d,%d\n",ENOENT,ENOTDIR,ENXIO);	
	printf("line= %d\n",errno);

		printf("Can't open /dev/ttyMPCON\n");
		printf("Please check your board in slot..\n");
		return -1;
	}

	nr_bds = ioctl(fd,TIOCGGETBDNO);
	for(i=0;i<nr_bds;i++)
	{
		device_id[i] = ioctl(fd,TIOCGGETDEVID,i);
		interface[i] = ioctl(fd,TIOCGGETINTERFACE,i);
		port_type[i] = ioctl(fd,TIOCGGETPORTTYPE,i);
		rev[i] = ioctl(fd,TIOCGGETREV,i);
		nr_ports[i] = ioctl(fd,TIOCGGETNRPORTS,i);

		switch(device_id[i])
		{
			case 0x4d01: sprintf(dev_name[i],"Multi-1 PCI");break;
			case 0x4d02: sprintf(dev_name[i],"Multi-2 PCI");break;
			case 0x4d04: sprintf(dev_name[i],"Multi-4 PCI");break;
			case 0x4d08: sprintf(dev_name[i],"Multi-8 PCI");break;
			case 0x4d32: sprintf(dev_name[i],"Multi-32 PCI");break;
			case 0x4501: sprintf(dev_name[i],"Multi-1 PCIe");break;
			case 0x4502: sprintf(dev_name[i],"Multi-2 PCIe");break;
			case 0x4504: sprintf(dev_name[i],"Multi-4 PCIe");break;
			case 0x4508: sprintf(dev_name[i],"Multi-8 PCIe");break;
			case 0x4532: sprintf(dev_name[i],"Multi-32 PCIe");break;

			case 0x4e01: sprintf(dev_name[i],"Multi-1 PCIe E");break;
			case 0x4e02: sprintf(dev_name[i],"Multi-2 PCIe E");break;
			case 0x4b02: sprintf(dev_name[i],"Multi-2 PCIe B");break;
			case 0x4b04: sprintf(dev_name[i],"Multi-4 PCIe B");break;
			case 0x4b08: sprintf(dev_name[i],"Multi-8 PCIe B");break;
			case 0x4b32: sprintf(dev_name[i],"Multi-32 PCIe B");break;

			case 0x0004: sprintf(dev_name[i],"Multi-4(GT) PCI");break;
			case 0x0008: sprintf(dev_name[i],"Multi-8(GT) PCI");break;
			case 0x0032: sprintf(dev_name[i],"Multi-32(GT) PCI");break;
			case 0x1501: sprintf(dev_name[i],"Multi-1(GT) PCIe");break;
			case 0x1502: sprintf(dev_name[i],"Multi-2(GT) PCIe");break;
			case 0x1504: sprintf(dev_name[i],"Multi-4(GT) PCIe");break;
			case 0x1508: sprintf(dev_name[i],"Multi-8(GT) PCIe");break;
			case 0x1532: sprintf(dev_name[i],"Multi-32(GT) PCIe");break;

			case 0x4604: sprintf(dev_name[i],"Multi-4M PCI");break;  //modem
		}
		if ((interface[i] & 0xf0)==0x00) 
			sprintf(interface_name[i],"RS232");
		else if ((interface[i] & 0xf0)==0x10)
			sprintf(interface_name[i],"RS422 PTP Mode");
		else if ((interface[i] & 0xf0)==0x20)
			sprintf(interface_name[i],"RS485 None Echo Mode");

		if (port_type[i]==0x01) 
			sprintf(port_type_name[i],"16C55X");
		else if (port_type[i]==0x02) 
			sprintf(port_type_name[i],"16C105X");
		else if (port_type[i]==0x03) 
			sprintf(port_type_name[i],"16C105XA");
		else
			sprintf(port_type_name[i],"UNKNOWN");
		
	}
	close(fd);

	unlink("/dev/ttyMPCON");

	if ( nr_bds <= 0 ){
		printf("No Multiport PCI/PCIe board found..!!\n");
		return -1;
	}
	
	printf("================================================================\n");
	printf("	Enhanced Async Multi-Port(PCI/PCIe) Linux Device Driver	\n");
	printf("	Version : %s	revision: %s\n", sw_version, sw_revdate);
	printf("================================================================\n");
	printf(" %d board(s) installed \n", nr_bds );
	for(i=0;i<nr_bds;i++)
	{
		printf(" Board No.%d : %s (rev %x)\n", i+1, dev_name[i], rev[i]);
		for(port=0;port<nr_ports[i];port++)
		{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
			sprintf(name,"/dev/ttyMP%d",n);
			MKNOD(name,54,n);
#endif
			printf("	/dev/ttyMP%d (%s , %s)\n",n++,interface_name[i], port_type_name[i]);
		}
		
	}
	_exit(0);
			
}

void MKNOD(char *filename, int major, int minor)
{
	unsigned short newmode;

	newmode = 0766 & ~umask(0);

	unlink(filename);

	if(mknod((char *)filename, newmode | S_IFCHR, makedev(major,minor)))
	{
		printf("MKNOD : Can't make device %s\n", filename);
		return;
	}
}
