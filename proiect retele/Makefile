#fisier folosit pentru compilarea serverului&clientului TCP concurent

all:
	gcc Server.c -o Server
	gcc Client.c -o Client
clean:
	rm -f *~ Client Server
test:
	gcc test.c -o test
c:
	./Client 127.0.0.1 2024
cc:
	./Client 127.0.0.1 2728
allt:
	gcc ServerTest.c -o ServerTest
	gcc ClientTest.c -o ClientTest
cleant:
	rm -f ClientTest ServerTest
ct:
	./ClientTest 127.0.0.1 2728
st:
	./ServerTest