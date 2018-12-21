########################################################################
#
#  DESCRIPTION  : Makefile to compile aphamask and compose applications
# 
#  PLATFORM		: Linux
#
#  DATE	               	NAME	        	  	REASON
#  2nd Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
#                       [C88650674]
#Reference: Ioannis Karamouzas (ioannis@clemson.edu)
########################################################################

#compiler
CC = g++

#auxiliary flags
CFLAGS	= -g

#set platform dependent variables
ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lOpenImageIO -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -L /usr/lib64/ -lglut -lGL -lGLU -lOpenImageIO -lm
  endif
endif

#executable name
PROJECTS = alphamask compose
EXECUTABLE1 = alphamask
EXECUTABLE2 = compose

#list of all .o file for each .cpp file
OBJECTS := alphamask.o
OBJECTS += alphamask_func.o

OBJECTS2 := compose.o
OBJECTS2 += compose_func.o			

#this does the linking step  
all: ${PROJECTS}
${EXECUTABLE1} : ${OBJECTS} 
	${CC} ${CFLAGS} -o ${EXECUTABLE1} ${OBJECTS} ${LDFLAGS}
	
${EXECUTABLE2} : ${OBJECTS2} 
	${CC} ${CFLAGS} -o ${EXECUTABLE2} ${OBJECTS2} ${LDFLAGS}  

#compile the code 
%.o: %.cpp
	${CC} -c ${CFLAGS} $<
	
#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECTS}
