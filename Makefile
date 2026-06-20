# IRC Server Makefile

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
LDFLAGS =

# Directories
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files (all potential source files)
ALL_SOURCES = $(SRC_DIR)/main.cpp \
              $(SRC_DIR)/server/Server.cpp \
              $(SRC_DIR)/server/ServerInit.cpp \
              $(SRC_DIR)/server/PollLoop.cpp \
              $(SRC_DIR)/server/AcceptClient.cpp \
              $(SRC_DIR)/server/Disconnect.cpp \
              $(SRC_DIR)/server/Shutdown.cpp \
              $(SRC_DIR)/network/Socket.cpp \
              $(SRC_DIR)/network/Receiver.cpp \
              $(SRC_DIR)/network/Sender.cpp \
              $(SRC_DIR)/network/Buffer.cpp \
              $(SRC_DIR)/network/PacketAssembler.cpp \
              $(SRC_DIR)/models/Client.cpp \
              $(SRC_DIR)/models/Channel.cpp \
              $(SRC_DIR)/parser/Parser.cpp \
              $(SRC_DIR)/parser/Dispatcher.cpp \
              $(SRC_DIR)/commands/Pass.cpp \
              $(SRC_DIR)/commands/Nick.cpp \
              $(SRC_DIR)/commands/User.cpp \
              $(SRC_DIR)/commands/Join.cpp \
              $(SRC_DIR)/commands/Privmsg.cpp \
              $(SRC_DIR)/commands/Topic.cpp \
              $(SRC_DIR)/commands/Invite.cpp \
              $(SRC_DIR)/commands/Kick.cpp \
              $(SRC_DIR)/commands/Mode.cpp \
              $(SRC_DIR)/replies/Reply.cpp \
              $(SRC_DIR)/utils/Utils.cpp \
              $(SRC_DIR)/utils/NumericReplies.cpp

# Filter out empty files - only include non-empty source files
SOURCES = $(shell for file in $(ALL_SOURCES); do if [ -s "$$file" ]; then echo "$$file"; fi; done)

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Output executable
TARGET = $(BIN_DIR)/ircserv

# Default target
all: check-empty $(TARGET)

# Check for empty files and warn
check-empty:
	@empty_files=$$(find $(SRC_DIR) -type f \( -name "*.cpp" -o -name "*.hpp" \) -size 0); \
	if [ -n "$$empty_files" ]; then \
		echo "⚠ WARNING: Found empty files (will be excluded from build):"; \
		echo "$$empty_files" | sed 's/^/  - /'; \
	fi

# Create directories if they don't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/server $(OBJ_DIR)/network $(OBJ_DIR)/models $(OBJ_DIR)/parser $(OBJ_DIR)/commands $(OBJ_DIR)/replies $(OBJ_DIR)/utils

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build successful: $(TARGET)"

# Clean object files and executable
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "✓ Clean complete"

# Rebuild
re: clean all

# Run the server
run: $(TARGET)
	./$(TARGET)

# Test targets
test: $(TARGET)
	@echo "Running tests..."
	@./tests/basic_connection.sh
	@./tests/registration.sh
	@./tests/join.sh
	@./tests/privmsg.sh
	@./tests/mode.sh

# Phony targets
.PHONY: all clean re run test check-empty
