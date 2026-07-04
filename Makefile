CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -Iinclude

SRC = src/Process.cpp src/SchedulerQueue.cpp src/MLQScheduler.cpp src/FileIO.cpp src/main.cpp
TARGET = mlq_simulator

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	rm -f outputs/*.txt

.PHONY: all clean
