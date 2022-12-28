CC = clang++
CFLAGS = -std=c++11 -Wall -Iinclude -g

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

OUTPUT = yi.out

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	rm $(OBJS)

clean:
	rm $(OBJS) $(OUTPUT)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

