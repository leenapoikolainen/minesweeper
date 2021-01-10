minesweeper: minesweeper.o main.o
	g++ -Wall -g minesweeper.o main.o -o minesweeper

main.o: main.cpp minesweeper.h
	g++ -Wall -g -c main.cpp

minesweeper.o: minesweeper.cpp minesweeper.h
	g++ -Wall -g -c minesweeper.cpp