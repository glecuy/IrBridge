#*****************************************************************************
#
# File : Makefile
# ----
#
# Copyright Statement :
# ------------------
#
# Project :
# -------
#
# Description :
# -----------
# This file defines rules to
#
# -------
#
# Modification History :
# --------------------
#
# Date      Major Version   Author          Comment
# ----      -------------   ------          -------
#
#*****************************************************************************

CC=g++
CFLAGS=-W -Wall
LDFLAGS=
EXEC=IrCapture

all: $(EXEC)

$(EXEC): IrCapture.o
	$(CC) -o $@ $^ $(LDFLAGS)

#main.o: pages2c.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf $(EXEC)
	
