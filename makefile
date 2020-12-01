OUTPUT_DIR = build
LIBRARY = $(OUTPUT_DIR)/ppa_lib.a
EXE = $(OUTPUT_DIR)/example

CXX = g++
CXXFLAGS = -std=c++11 -g -O3
CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -pedantic

# Macro to expand files recursively: parameters $1 -  directory, $2 - extension, i.e. cpp
rwildcard = $(wildcard $(addprefix $1/*.,$2)) $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2))
dir_guard = @mkdir -p $(@D)

SRCS = $(call rwildcard, src, cpp)
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

# Object compilation rule
$(OUTPUT_DIR)/%.o: %.cpp
	$(dir_guard)
	$(CXX) -c -o $@ $(CXXFLAGS) $<

# Executable compilation rule
$(EXE): $(OBJS)
	$(CXX) -o $(EXE) $(OBJS)

# Archiving rule
$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $^

all: $(LIBRARY) $(EXE)

clean:
	rm -rf $(OUTPUT_DIR)

rebuild: clean all
