alltests: gready_test graph_test io_test

# paths
SOURCE = ../src
HEAD = ../headers
TESTS = .
BUILD = ../build
BIN = ../bin

CFLAGS = -g -I$(HEAD)

GRPOBJ = $(BUILD)/graph_test.o $(BUILD)/graph.o

GROBJ = $(BUILD)/vamana.o $(BUILD)/gready_test.o $(BUILD)/graph.o

COMOBJ = $(BUILD)/io.o $(BUILD)/graph.o $(BUILD)/vamana.o $(BUILD)/io_test.o

$(BUILD)/%.o: $(SOURCE)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(TESTS)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

gready_test: $(GROBJ)
	g++ $(CFLAGS) $(GROBJ) -o $(BIN)/gready_test

graph_test: $(GRPOBJ)
	g++ $(CFLAGS) $(GRPOBJ) -o $(BIN)/graph_test

io_test: $(COMOBJ)
	g++ $(CFLAGS) $(COMOBJ) -o $(BIN)/io_test

# prune_test: $(TESOBJ)
# 	g++ $(TESOBJ) -o $(BIN)/progDelay