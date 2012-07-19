##
## @file Makefile
## Betriebssysteme Beispiel 3 Makefile
##
## @author Mihajlo Milanovic <ic11b081@technikum-wien.at>
## @author Florian Mrkvicka  <ic11b048@technikum-wien.at>
## @author Roland Elentner   <ic11b025@technikum-wien.at>
## @date 2012/05/22
##
## @version $Revision: 005 $
##
## URL: $HeadURL$
##
## Last modified: $Author: Roland $
##

##
## ------------------------------------------------------------- variables --
##

CC := gcc
CFLAGS := -I/usr/local/include -Wall -Wextra -Werror -pedantic -g -O3
RM := rm -f
DOXYGEN := doxygen

OBJECTSENDER := sender.o
OBJECTEMPFAENGER := empfaenger.o
OBJECTCOMMON := common.o
HEADERCOMMON := common.h

##
## ----------------------------------------------------------------- rules --
##

%.o : %.c $(HEADERCOMMON)
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

all: sender empfaenger

sender: $(OBJECTSENDER) $(OBJECTCOMMON)
	$(CC) $(CFLAGS) -o $@ $^ -lsem182

empfaenger: $(OBJECTEMPFAENGER) $(OBJECTCOMMON)
	$(CC) $(CFLAGS) -o $@ $^ -lsem182

clean:
	$(RM) $(OBJECTSENDER) $(OBJECTEMPFAENGER) $(OBJECTCOMMON) sender empfaenger

distclean: clean
	$(RM) -r doc

doc:
	$(DOXYGEN) doxygen.dcf

##
## -------- cleanup Semaphore and Shared Memory -----------------------------
##
clear:
	ipcrm -M 0x0008da9c -S 0x0008da9a -S 0x0008da9b ||:
##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
