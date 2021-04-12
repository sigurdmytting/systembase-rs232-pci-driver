# Systembase Multiport ioctl Program

CC			=	gcc  
RM			=	rm
CP			=  	cp
LIBS		=	-lpthread

TARGET		=	sb_ioctl 
OBJS		=	sb_ioctl.o

all : $(TARGET)

sb_ioctl: sb_ioctl.o 
	$(RM) -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.o
	
	
clean: 
	$(RM) -f $(OBJS) $(TARGET)




