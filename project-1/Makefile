all: stucs

stucs: students.o hash.o skiplist.o stucs.o List.o PList.o main.o
	g++ students.o hash.o skiplist.o stucs.o List.o PList.o main.o -o stucs
	
student.o: students.cpp
	g++ -c students.cpp

hash.o: hash.cpp
	g++ -c hash.cpp

skiplist.o: skiplist.cpp
	g++ -c skiplist.cpp

stucs.o: stucs.cpp
	g++ -c stucs.cpp

List.o: List.cpp
	g++ -c List.cpp

PList.o: PList.cpp
	g++ -c PList.cpp
	
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *o stucs