## @author Karin Kalman
## @author Michael Müller
## @author Gerhard Sabeditsch
##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-DDEBUG -Wall -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11

OBJECTS=main.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: myfile

myfile: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	$(RM) *.o
 
.PHONY: clean
clean:
	$(RM) *.o *~ myfile

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
