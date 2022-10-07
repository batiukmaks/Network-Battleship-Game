TARGET_SERVER = run_server
TARGET_CLIENT = run_client

SRC = src
INC = include
BIN = bin

SOURCE = $(wildcard $(SRC)/*.cpp)
OBJECT_SERVER =  bin/run_server.o bin/Server.o bin/SocketManager.o
OBJECT_CLIENT =  bin/run_client.o bin/Client.o bin/SocketManager.o
OBJECT = $(OBJECT_SERVER) $(OBJECT_CLIENT)

CXX = g++
CXXFLAGS = -Wall -I$(INC)

all: $(BIN)/$(TARGET_SERVER) $(BIN)/$(TARGET_CLIENT)

$(BIN)/$(TARGET_SERVER): $(OBJECT_SERVER)
	$(CXX) -o $@ $^

$(BIN)/$(TARGET_CLIENT): $(OBJECT_CLIENT)
	$(CXX) -o $@ $^

$(BIN)/%.o : $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: help run_server run_client clean

help: 
	@echo "src: $(SOURCE)"
	@echo "obj_server: $(OBJECT_SERVER)"
	@echo "obj_client: $(OBJECT_CLIENT)"

run_server:
	$(BIN)/$(TARGET_SERVER)

run_client:
	$(BIN)/$(TARGET_CLIENT)

clean:
	rm -f $(OBJECT) $(BIN)/$(TARGET_SERVER) $(BIN)/$(TARGET_CLIENT)