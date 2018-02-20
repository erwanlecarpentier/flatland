CCC=g++
INCLUDE = -I./src -I./src/environment -I./src/policy -I./src/utils
#INCLUDESP=-I/opt/DMIA/EIGEN/eigen/include/eigen3 -I/opt/DMIA/EIGEN/libconfig/include -L/opt/DMIA/EIGEN/libconfig/lib# for serv-prol1
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -O2 -g
#CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} ${INCLUDESP} -O2 -g# for serv-prol1
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

