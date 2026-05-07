# =============================================================================
# Caissa — Portable Makefile (Linux, macOS, Windows cmd, Git Bash / MSYS)
# =============================================================================
# Build targets work on every supported shell. `test` and `format` shell
# out to bash / clang-format respectively — install those tools (or run
# `make test` from Git Bash on Windows) if you need them.
#
# Targets:
#   make            -> builds both binaries
#   make chess      -> core-only binary (what graders run)
#   make chess-ai   -> core + AI engine binary
#   make clean      -> remove build/ and bin/
#   make run        -> build and run chess
#   make run-ai     -> build and run chess-ai
#   make test       -> pipe game files through bin/chess and diff outputs
#   make format     -> clang-format -i over src/
#   make format-check -> dry-run, fail on any reformatting needed
#   make doc        -> generate HTML class docs into docs/api/ (Doxygen)
# =============================================================================

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -Isrc
LDFLAGS  ?=

# --- Platform / shell detection -------------------------------------------
# Windows sets OS=Windows_NT regardless of shell. MSYSTEM is set under
# MSYS / Git Bash. Use that to tell apart cmd.exe vs. POSIX-like shells.
ifeq ($(OS),Windows_NT)
    EXE := .exe
    ifeq ($(MSYSTEM),)
        # plain Windows cmd.exe
        RM_BUILD = if exist build rmdir /S /Q build
        RM_BIN   = if exist bin rmdir /S /Q bin
        MKDIR    = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
        EXEC     = $(subst /,\,$(1))
    else
        # Git Bash / MSYS on Windows — POSIX tools available
        RM_BUILD = rm -rf build
        RM_BIN   = rm -rf bin
        MKDIR    = mkdir -p $(1)
        EXEC     = ./$(1)
    endif
else
    EXE :=
    RM_BUILD = rm -rf build
    RM_BIN   = rm -rf bin
    MKDIR    = mkdir -p $(1)
    EXEC     = ./$(1)
endif

BUILD_DIR := build
BIN_DIR   := bin

# --- Source layout ---------------------------------------------------------
CORE_SRCS   := $(wildcard src/core/*.cpp) $(wildcard src/core/pieces/*.cpp)
ENGINE_SRCS := $(wildcard src/engine/*.cpp)

# --- Object files (mirror src/ tree under build/) -------------------------
CORE_OBJS   := $(CORE_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
ENGINE_OBJS := $(ENGINE_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ    := $(BUILD_DIR)/main.o
MAIN_AI_OBJ := $(BUILD_DIR)/main_ai.o

CHESS_BIN    := $(BIN_DIR)/chess$(EXE)
CHESS_AI_BIN := $(BIN_DIR)/chess-ai$(EXE)

# --- Default target --------------------------------------------------------
.PHONY: all
all: chess chess-ai

# --- Convenience phony targets pointing at the real binary files ----------
.PHONY: chess chess-ai
chess:    $(CHESS_BIN)
chess-ai: $(CHESS_AI_BIN)

# --- Link rules ------------------------------------------------------------
# chess: core-only. Engine sources are NOT linked, so deleting src/engine/
# leaves this build working.
$(CHESS_BIN): $(CORE_OBJS) $(MAIN_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(CHESS_AI_BIN): $(CORE_OBJS) $(ENGINE_OBJS) $(MAIN_AI_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# --- Compile rules: src/foo/bar.cpp -> build/foo/bar.o --------------------
$(BUILD_DIR)/%.o: src/%.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# main.cpp is compiled twice: once plain (chess), once with -DWITH_AI.
$(MAIN_OBJ): src/main.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(MAIN_AI_OBJ): src/main.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -DWITH_AI -MMD -MP -c $< -o $@

# --- Bookkeeping -----------------------------------------------------------
$(BIN_DIR):
	@$(call MKDIR,$(BIN_DIR))

.PHONY: clean
clean:
	$(RM_BUILD)
	$(RM_BIN)

.PHONY: run run-ai
run: $(CHESS_BIN)
	$(call EXEC,$(CHESS_BIN))

run-ai: $(CHESS_AI_BIN)
	$(call EXEC,$(CHESS_AI_BIN))

# --- Test target -----------------------------------------------------------
# Pipes every tests/games/**/*.txt file into bin/chess and diffs the final
# stdout line against the `# expected:` line in the file.
# Requires bash. On Windows, use Git Bash (or just run `make test` from a
# Git Bash shell). On plain cmd.exe make will surface a "bash not found"
# error from the OS — install Git for Windows.
.PHONY: test
test: $(CHESS_BIN)
	bash tests/run_tests.sh $(CHESS_BIN)

# --- Format targets --------------------------------------------------------
# Both targets just invoke clang-format. If it is not on PATH, the shell
# (bash or cmd) emits its standard "command not found" message and make
# halts — no platform-specific check needed.
# Install: `winget install LLVM.LLVM` (Windows), `apt install clang-format`,
# `brew install clang-format`, etc.
SRC_FILES := $(wildcard src/*.cpp src/*.h \
                        src/core/*.cpp src/core/*.h \
                        src/core/pieces/*.cpp src/core/pieces/*.h \
                        src/engine/*.cpp src/engine/*.h)

.PHONY: format format-check
format:
	clang-format -i $(SRC_FILES)

format-check:
	clang-format --dry-run --Werror $(SRC_FILES)

# --- Documentation target --------------------------------------------------
# Generates HTML class documentation under docs/api/ via Doxygen.
# Requires `doxygen` on PATH; if missing, the shell surfaces its own error.
.PHONY: doc
doc:
	doxygen Doxyfile

# Auto-include generated dependency files
-include $(CORE_OBJS:.o=.d) $(ENGINE_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(MAIN_AI_OBJ:.o=.d)
