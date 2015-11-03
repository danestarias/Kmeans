CC=g++ -std=c++11 -Wall -L/home/daniel90/zmq/lib -I/home/daniel90/zmq/include
LDFLAGS= -lpthread -lzmqpp -lzmq -lsodium -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

all: worker serverK serverN

worker: worker.cc
	$(CC) -o worker worker.cc $(LDFLAGS)

serverK: serverK.cc
	$(CC) -o serverK serverK.cc $(LDFLAGS)

serverN: serverN.cc
	$(CC) -o serverN serverN.cc $(LDFLAGS)
	
clean:
	rm -f worker serverK serverN
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/daniel90/zmq/lib 		debian
#export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/home/daniel90/zmq/lib 	IOS

