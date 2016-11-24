CCFLAGS = -g -pedantic -Wall -std=c++11 -IE:/libs/SFML-2.3.2/include
LDFLAGS =  

STATIC_LIBS=
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