
twsal: twsal.o main.o
	g++ twsal.o main.o -o twsal #-Wall

twsal.o: twsal.cpp twsal.h
	g++ -c twsal.cpp -o twsal.o #-Wall

main.o: main.cpp twsal.h
	g++ -c main.cpp -o main.o #-Wall
