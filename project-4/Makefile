all: mydiz

mydiz: main.o Mydiz.o Queue.o FileQueue.o BF.o DinodeList.o FileSpace.o BfsQ.o
	g++ main.o Mydiz.o Queue.o FileQueue.o BF.o DinodeList.o FileSpace.o BfsQ.o -o mydiz

main.o: main.cpp
	g++ -c main.cpp

Queue.o: Queue.cpp
	g++ -c Queue.cpp

FileQueue.o: FileQueue.cpp
	g++ -c FileQueue.cpp

BfsQ.o: BfsQ.cpp
	g++ -c BfsQ.cpp

BF.o: BF.cpp
	g++ -c BF.cpp

Mydiz.o: Mydiz.cpp
	g++ -c Mydiz.cpp

DinodeList.o: DinodeList.cpp
	g++ -c DinodeList.cpp

FileSpace.o: FileSpace.cpp
	g++ -c FileSpace.cpp

clean:
	rm *o mydiz
