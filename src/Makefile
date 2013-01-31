CC     := clang
CFLAGS := -Wall -O3 -D__DEBUG__
MPICC  := mpicc

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: $(BINS)

%: %.c
	$(MPICC) -o ../bin/$@ $(CFLAGS) $<
