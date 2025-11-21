CXX = g++
CXXFLAGS = -g
LDFLAGS = -lmysqlcppconnx

# acha todod arquivos com cpp, mesmo em subpasta
SRCS = $(shell find . -name '*.cpp')

# cria os arquivos .o
OBJS = $(SRCS:.cpp=.o)

# nome do programa
TARGET = a.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

# conecta os .o com os .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
