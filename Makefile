# Default flags and library
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE \
         -I/home/trenston/workspace/TLPI/tlpi-book/lib
LDFLAGS = /home/trenston/workspace/TLPI/tlpi-book/libtlpi.a

# Rule: compile any .c file to an executable of the same name
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
