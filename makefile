
main: main.o XRequestManager.o
	g++ -std=c++11 -pthread -g -Wall -lX11 main.o XRequestManager.o -o main

main.o: main.cpp 
	g++ -std=c++11 `pkg-config --cflags glib-2.0` -g -Wall -c main.cpp

XRequestManager.o: XRequestManager.cpp
	g++ -std=c++11 `pkg-config --cflags glib-2.0` -g -Wall -c XRequestManager.cpp

clean:
	rm *.o main
