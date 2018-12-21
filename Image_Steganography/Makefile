########################################################################
#
#  DESCRIPTION  : Makefile to compile Steganography application
# 
#  PLATFORM		: Linux
#
#  DATE	               	NAME	        	  	REASON
#  20th Nov,2018        Shashi Shivaraju        CPSC_6040_Final_Project
#                       [C88650674]
#Reference: Ioannis Karamouzas (ioannis@clemson.edu)
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
    LDFLAGS   = -L /usr/lib64/ -lglut -lGL -lGLU -lOpenImageIO -lm
  endif
endif

#executable name
PROJECT		= stegano

#list of all .o file for each .cpp file
OBJECTS = ${PROJECT}.o manager.o matrix.o huffman.o

#this does the linking step  
${PROJECT}:	${OBJECTS}
	${CC} ${CFLAGS} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

#compile the code 
%.o: %.cpp
	${CC} -c ${CFLAGS} $<

#this will clean up all temporary files created by make all
clean:
	rm -f core.* *.o *~ ${PROJECT}
