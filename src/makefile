
LIBDIR=-L/home/christian/lib/cplex-12.7.0/lib/x86-64_linux/static_pic -L/home/christian/lib/cplex-12.7.0/concert/lib/x86-64_linux/static_pic
INCDIR=-I/home/christian/lib/cplex-12.7.0/concert/include/ -I/home/christian/lib/cplex-12.7.0/include/

CCFLAGS=-O3 -pedantic -Wall -std=c++11 -DIL_STD $(INCDIR)
LDFLAGS=$(LIBDIR)

STATIC_LIBS=-lconcert -lilocplex -lcplexdistmip -lcplex -lpthread
DYNAMIC_LIBS=

CC = g++
NAME = ttp

HEADERS := $(wildcard *.h)  $(wildcard */*.h)

SRCS := $(wildcard *.cpp)  $(wildcard */*.cpp)

OBJS=$(SRCS:.cpp=.o)

DEPS=$(SRCS:.cpp=.d)


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $^ $(LDFLAGS) $(STATIC_LIBS)
	mv $(NAME) ..

.SUFFIXES: .o .cpp .d
.cpp.o:
	$(CC) -c $(CCFLAGS) -o $@ $<

.cpp.d:
	$(CC) -MM $(CCFLAGS) -MF $@ $<
		
clean:
	rm -f $(OBJS) $(DEPS) ../$(NAME)

-include $(DEPS)