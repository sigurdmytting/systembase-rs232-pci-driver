#
# Systembase Multiport Test Program


CC			=	gcc  
RM			=	rm
CP			=  	cp
LIBS		=	-lpthread

TARGET		=	sb_test 
OBJS		=	sb_test.o

all : $(TARGET)

sb_test: sb_test.o 
	$(RM) -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.o
	
	
clean: 
	$(RM) -f $(OBJS) $(TARGET)




