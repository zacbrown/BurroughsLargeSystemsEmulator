all: main.o parser.o reader.o translator.o useful.o
	g++ -o ../bin/compiler-mac main.o parser.o reader.o useful.o translator.o

main.o: main.cpp parser.h reader.h translator.h useful.h
	g++ -c main.cpp

useful.o: useful.cpp useful.h
	g++ -c useful.cpp

parser.o: parser.cpp parser.h translator.h reader.h
	g++ -c parser.cpp

reader.o: reader.cpp reader.h useful.h
	g++ -c reader.cpp

translator.o: translator.cpp translator.h parser.h
	g++ -c translator.cpp
