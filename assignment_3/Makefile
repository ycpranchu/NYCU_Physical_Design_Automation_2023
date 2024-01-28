CXX := g++
CXXFLAGS := -O3 -march=native -std=c++20 -g

SOURCES := Lab3.cpp sa.cpp

OBJECTS := $(SOURCES:.cpp=.o)

TARGET := Lab3

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)