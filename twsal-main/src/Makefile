
twsal: twsal.o main.o encode_string.o
	g++ twsal.o main.o encode_string.o -o twsal

twsal.o: twsal.cpp twsal.h
	g++ -c twsal.cpp -o twsal.o

encode_string.o: encode_string.cpp encode_string.h
	g++ -c encode_string.cpp -o encode_string.o

main.o: main.cpp twsal.h
	g++ -c main.cpp -o main.o

clean:
	rm *.o
	rm twsal
