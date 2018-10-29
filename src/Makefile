#
# COMPUTER SCIENCE 244
# Makefile for Tutorial 6 Question 1
# (c) 2018 University of Stellenbosch
#

NAME     = fadd
ASM_SRC  = fadd.asm
C_OBJS   = faddtest.o
ASM_OBJS = fadd.o
OBJS     = $(ASM_OBJS) $(C_OBJS)

# flags
DEBUG    = -ggdb
OPTIMISE = -O0
TARGET   = -m32
WARNINGS = -Wall
CFLAGS   = $(DEBUG) $(OPTIMISE) $(WARNINGS) $(TARGET)
DFLAGS   = # nothing yet....
ASMFLAGS = -f elf32

# commands
ASM      = nasm
CC       = gcc
RM       = rm -f
COMPILE  = $(CC) $(CFLAGS) $(DFLAGS)
ASSEMBLE = $(ASM) $(ASMFLAGS)

# rules ########################################################################

all: clean $(NAME)

$(NAME): $(OBJS)
	$(COMPILE) -o $@ $(OBJS)

$(ASM_OBJS): $(ASM_SRC)
	$(ASSEMBLE) $(ASM_SRC)

clean:
	$(RM) *.o *.map *.s *.bin
	$(RM) $(NAME)

