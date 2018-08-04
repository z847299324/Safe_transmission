CC = g++ 
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lpthread -lcrypto
FILE = Codec.cpp RespondCodec.cpp threadpool.c Asn1_Factory.cpp main.cpp BaseASN1.cpp RequestCodec.cpp serverop.cpp SequenceASN1.cpp ItcastLog.cpp epoll2.cpp
OBJS = Codec.o RespondCodec.o threadpool.o Asn1_Factory.o main.o BaseASN1.o RequestCodec.o serverop.o SequenceASN1.o ItcastLog.o epoll2.o
#FILE = Codec.cpp RespondCodec.cpp threadpool.c Asn1_Factory.cpp main.cpp BaseASN1.cpp RequestCodec.cpp serverop.cpp

main:$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $^
%.o:%.c %.cpp
	    $(CC) -o $@ $^ -std=c++11
.PHONY:clean
	clean:
	    @rm -f *.o $(TARGET)


