CC = clang
INC_DIR = ./inc
SRC_DIR = ./src
BIN_DIR = ./bin

EXACUTABLE = main
SOURCE = $(EXACUTABLE).c

.PHONY: all clean launch

#all: $(EXACUTABLE) 
all: 
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(EXACUTABLE) $(SRC_DIR)/$(SOURCE)
#main: $(SOURCE)/%.c
#	$(CC) -o $(BIN_FOLDER)/$@ $<

clean:
	rm -rf $(BIN_DIR)/*

launch:
	./$(BIN_DIR)/$(EXACUTABLE)