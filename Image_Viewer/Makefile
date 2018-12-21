########################################################################
#
#  DESCRIPTION  : Makefile to compile Image View application
# 
#  PLATFORM		: Linux
#
#  DATE	               	NAME	        	  	REASON
#  5th Sep,2018         Shashi Shivaraju        CPSC_6040_HW_01
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
PROJECT = imgview

#list of all .o file for each .cpp file
OBJECTS := imageview.o
OBJECTS += imgview_func.o		

#this does the linking step  
all: ${PROJECT}
${PROJECT} : ${OBJECTS} 
	${CC} ${CFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS} 

#compile the code 
%.o: %.cpp
	${CC} -c ${CFLAGS} $<
	
#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECT}
