CCC=g++
INCLUDE = -I./src -I./src/environment -I./src/policy -I./src/utils
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -O2 -g
LDFLAGS=-lm -lconfig++
EXEC=exe

all : clean compile run

fast : run trajectory

clean :
	rm -f ${EXEC}

compile : main.cpp
	${CCC} ${CCFLAGS} main.cpp -o ${EXEC} ${LDFLAGS}

run :
	./${EXEC}

trajectory :
	python3 plot/trajectory.py

graph :
	python3 plot/graph.py

