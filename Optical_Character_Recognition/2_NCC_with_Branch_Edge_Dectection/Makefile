########################################################################
#
#  DESCRIPTION  : Makefile to build letter detection
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

#executable name
PROJECT		= letter_detection

#list of all .o file for each .cpp file
OBJECTS = ${PROJECT}.o

#this does the linking step  
${PROJECT}:	${PROJECT}.o
	${CC} ${CFLAGS} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

#compile the code 
%.o: %.c
	${CC} -c ${CFLAGS} $<

#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECT}
