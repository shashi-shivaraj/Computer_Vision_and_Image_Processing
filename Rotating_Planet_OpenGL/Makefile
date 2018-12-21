########################################################################
#
#  DESCRIPTION  : Makefile to compile OpenGL rotating planet application
# 
#  PLATFORM		: Linux
#
#  DATE	               	NAME	        	  	REASON
#  2nd Dec,2018         Shashi Shivaraju        CPSC_6780_Assignment_4
#                       [C88650674]
########################################################################
#compiler
CC      = g++
C       = cpp

#auxiliary flags
CFLAGS  = -g

#set platform dependent variables
ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lOpenImageIO -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS   = -L /usr/lib64/ -lglut -lGL -lGLU -lm
  endif
endif

#executable name
PROJECT		= rotating_planet

#list of all .o file for each .cpp file
OBJECTS = ${PROJECT}.o

#this does the linking step  
${PROJECT}:	${OBJECTS}
	${CC} ${CFLAGS} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

#compile the code 
%.o: %.cpp
	${CC} -c ${CFLAGS} $<

#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECT}
