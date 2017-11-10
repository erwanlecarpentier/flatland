CCC=g++
INCLUDE = -I./src -I./src/environment -I./src/policy
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -O2 -g
LDFLAGS=-lm -lconfig++
EXEC=exe

all : clean compile run

clean :
	rm -f ${EXEC}

compile : main.cpp
	${CCC} ${CCFLAGS} main.cpp -o ${EXEC} ${LDFLAGS}

run :
	./${EXEC}

trajectory :
	python3 plot/trajectory.py

graph :
	python3 plot/comparison.py

