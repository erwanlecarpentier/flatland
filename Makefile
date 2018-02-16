CCC=g++
INCLUDE = -I./src -I./src/environment -I./src/policy -I./src/utils
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -O2 -g
LDFLAGS=-s -lm -lconfig++
EXEC=exe

all : clean compile run trajectory

fast : run trajectory

clean :
	rm -f ${EXEC}

clear :
	clear

compile : clear clear main.cpp
	${CCC} ${CCFLAGS} main.cpp -o ${EXEC} ${LDFLAGS}

run :
	./${EXEC}

trajectory :
	python3 plot/trajectory.py

