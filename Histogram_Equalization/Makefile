########################################################################
#
#  DESCRIPTION  : Makefile for histogram equalization
# 
#  PLATFORM		: Linux
#
#Reference: Ioannis Karamouzas (ioannis@clemson.edu)
########################################################################
#compiler
CC      = g++
C       = cpp

#auxiliary flags
CFLAGS  = -g 
LDFLAGS = -lm

#executable name
PROJECT		= hist_equalization

#list of all .o file for each .cpp file
OBJECTS := ${PROJECT}.o

#this does the linking step 
all: ${PROJECT}
${PROJECT} : ${OBJECTS}  
	${CC} ${CFLAGS} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

#compile the code 
%.o: %.cpp
	${CC} -c ${CFLAGS} $<

#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECT}
