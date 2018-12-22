########################################################################
#
#  DESCRIPTION  : Makefile to Active Contour
# 
#  PLATFORM		: Linux
#
#Reference: Ioannis Karamouzas (ioannis@clemson.edu)
########################################################################
#compiler
CC      = gcc
C       = c

#auxiliary flags
CFLAGS  = -g 
LDFLAGS = -lm

#executable name
PROJECT		= active_contour

#list of all .o file for each .cpp file
OBJECTS := ${PROJECT}.o 
OBJECTS += functions.o

#this does the linking step 
all: ${PROJECT}
${PROJECT} : ${OBJECTS}  
	${CC} ${CFLAGS} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

#compile the code 
%.o: %.c
	${CC} -c ${CFLAGS} $<

#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECT}
