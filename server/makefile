CC = gcc
TARGET = server
INCLUDE = -I ../common/
OBJS = ./server.o \
	../common/common.o  ../common/sub_reactor.o \
	../common/udp_server.o ../common/thread_pool.o \
	../common/udp_epoll.o
		

clean:
	rm $(OBJS)
realclean: clean
	rm $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread

./server.o : ./server.c
	$(CC) -o $@ -c $< $(INCLUDE)

../common/common.o : ../common/common.c
	$(CC) -o $@ -c $< $(INCLUDE)

../common/udp_server.o : ../common/udp_server.c
	$(CC) -o $@ -c $< $(INCLUDE)

../common/udp_epoll.o : ../common/udp_epoll.c
	$(CC) -o $@ -c $< $(INCLUDE)

../common/sub_reactor.o : ../common/sub_reactor.c
	$(CC) -o $@ -c $< $(INCLUDE)

../common/thread_pool.o : ../common/thread_pool.c
	$(CC) -o $@ -c $< $(INCLUDE)

