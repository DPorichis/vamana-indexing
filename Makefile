alltests: gready_test graph_test io_test prune_test vamana_test filtered_test
all: gready_test graph_test io_test prune_test vamana_test filtered_test project

# paths
SOURCE = ./src
HEAD = ./headers
TESTS = ./tests
BUILD = ./build
BIN = ./bin

CFLAGS = -g -I$(HEAD) -pthread -O3 -ffast-math -std=c++17

PROJOBJ = $(BUILD)/vamana.o $(BUILD)/io.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/main.o $(BUILD)/vamana-utils.o $(BUILD)/filtered-vamana.o $(BUILD)/stiched-vamana.o

GRPOBJ = $(BUILD)/graph_test.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/vamana-utils.o

GROBJ = $(BUILD)/vamana.o $(BUILD)/io.o $(BUILD)/gready_test.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/vamana-utils.o

COMOBJ = $(BUILD)/io.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/vamana.o $(BUILD)/io_test.o  $(BUILD)/vamana-utils.o $(BUILD)/filtered-vamana.o

PROBJ = $(BUILD)/vamana.o $(BUILD)/filtered-vamana.o $(BUILD)/stiched-vamana.o $(BUILD)/io.o $(BUILD)/prune_test.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/vamana-utils.o

VAMOBJ = $(BUILD)/vamana.o $(BUILD)/vamana_test.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/io.o $(BUILD)/vamana-utils.o $(BUILD)/filtered-vamana.o $(BUILD)/stiched-vamana.o

FILOBJ = $(BUILD)/filtered-vamana.o $(BUILD)/io.o $(BUILD)/filtered_test.o $(BUILD)/graph.o $(BUILD)/dist-cache.o $(BUILD)/vamana-utils.o

$(BUILD)/%.o: $(SOURCE)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(TESTS)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

project: $(PROJOBJ)
	g++ $(CFLAGS) $(PROJOBJ) -o $(BIN)/project

gready_test: $(GROBJ)
	g++ $(CFLAGS) $(GROBJ) -o $(BIN)/gready_test

graph_test: $(GRPOBJ)
	g++ $(CFLAGS) $(GRPOBJ) -o $(BIN)/graph_test

prune_test: $(PROBJ)
	g++ $(CFLAGS) $(PROBJ) -o $(BIN)/prune_test

io_test: $(COMOBJ)
	g++ $(CFLAGS) $(COMOBJ) -o $(BIN)/io_test

vamana_test: $(VAMOBJ)
	g++ $(CFLAGS) $(VAMOBJ) -o $(BIN)/vamana_test

filtered_test: $(FILOBJ)
	g++ $(CFLAGS) $(FILOBJ) -o $(BIN)/filtered_test

clean:
	rm -rf $(BUILD)/*.o $(BIN)/*

run_tests: alltests
	$(BIN)/gready_test
	$(BIN)/graph_test
	$(BIN)/io_test
	$(BIN)/prune_test
	$(BIN)/filtered_test
	$(BIN)/vamana_test

run_valgrind_tests: alltests
	cd tests
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/gready_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/graph_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/io_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/prune_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/filtered_test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/vamana_test

run: project
	$(BIN)/project -config ./config.txt

run_timed: project
	time $(BIN)/project -config ./config.txt