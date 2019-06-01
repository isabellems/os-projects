all: restaurant customer waiter doorman

restaurant: restaurant.o
	g++ -g restaurant.o -o restaurant -pthread -lrt

customer: customer.o
	g++ -g customer.o -o customer -pthread -lrt

waiter: waiter.o
	g++ -g waiter.o -o waiter -pthread

doorman: doorman.o
	g++ -g doorman.o -o doorman -pthread

clean:
	rm *o restaurant customer waiter doorman
