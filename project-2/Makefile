all: shapes handler circle semicircle ring square ellipse

shapes: shapes.o functions.o
	g++ shapes.o functions.o -o shapes

handler: handler.o
	g++ handler.o -o handler

circle: circle.o
	g++ circle.o -o circle

semicircle: semicircle.o
	g++ semicircle.o -o semicircle

ring: ring.o
	g++ ring.o -o ring

square: square.o
	g++ square.o -o square

ellipse: ellipse.o
	g++ ellipse.o -o ellipse

shapes.o: shapes.cpp
	g++ -c shapes.cpp

functions.o: functions.cpp
	g++ -c functions.cpp

handler.o: handler.cpp
	g++ -c handler.cpp

circle.o: circle.cpp
	g++ -c circle.cpp

semicircle.o: semicircle.cpp
	g++ -c semicircle.cpp

ring.o: ring.cpp
	g++ -c ring.cpp

square.o: square.cpp
	g++ -c square.cpp

ellipse.o: ellipse.cpp
	g++ -c ellipse.cpp

clean: 
	rm *o shapes handler circle semicircle ring square ellipse
