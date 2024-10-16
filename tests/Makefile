# all
all: gready_test

# paths
SOURCE = ../src
HEAD = ../headers
TESTS = .
BUILD = ../build
BIN = ../bin

CFLAGS = -g -I$(HEAD)

GROBJ = $(BUILD)/vamana.o $(BUILD)/gready_test.o

$(BUILD)/%.o: $(SOURCE)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(TESTS)/%.cpp | $(BUILD)
	g++ $(CFLAGS) -c $< -o $@

gready_test: $(GROBJ)
	g++ $(CFLAGS) $(GROBJ) -o $(BIN)/gready_test

# io_test: $(COMOBJ)
# 	g++ $(COMOBJ) -o $(BIN)/io_test

# prune_test: $(TESOBJ)
# 	g++ $(TESOBJ) -o $(BIN)/progDelay