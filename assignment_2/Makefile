CXX := g++
CXXFLAGS := -O3 -std=c++11

Floorplan:Floorplan.o
	g++ -g -o Floorplan Floorplan.o

Floorplan.o:Floorplan.cpp
	$(CXX) $(CXXFLAGS) -g Floorplan.cpp -c 

clean:
	rm -f *.o