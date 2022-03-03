CC = gcc
TARGET = client
INCLUDE = -I ../common/
OBJS = ../common/common.o ./client.o ../common/udp_client.o 

clean:
	rm $(OBJS)
realclean: clean
	rm $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread

../common/common.o : ../common/common.c
	$(CC) -o $@ -c $< $(INCLUDE)

./client.o : ./client.c
	$(CC) -o $@ -c $< $(INCLUDE)

../common/udp_client.o : ../common/udp_client.c
	$(CC) -o $@ -c $< $(INCLUDE)
	
