CXX := g++
CXXFLAGS := -O3 -std=c++17 -Werror=unused-variable

SOURCES := main.cpp Channel_Routing.cpp
HEADERS := main.h Channel_Routing.h

OBJDIR := objs
OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
 
TARGET := Lab4

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)
