alltests: gready_test graph_test io_test prune_test vamana_test filtered_test

# paths
SOURCE = ../src
HEAD = ../headers
TESTS = .
BUILD = ../build
BIN = ../bin

CFLAGS = -g -I$(HEAD)

GRPOBJ = $(BUILD)/graph_test.o $(BUILD)/graph.o	$(BUILD)/vamana-utils.o $(BUILD)/vamana.o $(BUILD)/io.o

GROBJ = $(BUILD)/vamana.o $(BUILD)/io.o $(BUILD)/gready_test.o $(BUILD)/graph.o $(BUILD)/vamana-utils.o

COMOBJ = $(BUILD)/io.o $(BUILD)/graph.o $(BUILD)/io_test.o $(BUILD)/filtered-vamana.o $(BUILD)/vamana-utils.o $(BUILD)/vamana.o

PROBJ = $(BUILD)/vamana.o $(BUILD)/io.o $(BUILD)/prune_test.o $(BUILD)/graph.o $(BUILD)/vamana-utils.o

VAMOBJ = $(BUILD)/vamana.o $(BUILD)/vamana_test.o $(BUILD)/graph.o $(BUILD)/io.o $(BUILD)/vamana-utils.o

FILOBJ = $(BUILD)/filtered-vamana.o $(BUILD)/io.o $(BUILD)/filtered_test.o $(BUILD)/graph.o $(BUILD)/vamana-utils.o	$(BUILD)/vamana.o

$(BUILD)/%.o: $(SOURCE)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(TESTS)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

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