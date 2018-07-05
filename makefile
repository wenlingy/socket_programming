# makefile 
# Please use the command
#			gmake
all: aws.o client.o serverA.o serverB.o serverC.o

aws.o:	aws.cpp
	g++ -o aws.o aws.cpp -lsocket -lnsl -lresolv

client.o:	client.cpp
	g++ -o client.o client.cpp -lsocket -lnsl -lresolv

serverA.o:	serverA.cpp
	g++ -o serverA.o serverA.cpp -lsocket -lnsl -lresolv
	
serverB.o:  serverB.cpp
	g++ -o serverB.o serverB.cpp -lsocket -lnsl -lresolv
	
serverC.o:  serverC.cpp
	g++ -o serverC.o serverC.cpp -lsocket -lnsl -lresolv
	