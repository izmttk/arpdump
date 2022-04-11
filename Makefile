BUILD_DIR := ./build
SRC_DIR := ./src
TEST_DIR := ./test
DEP_DIR := ./include

TARGET_EXEC := arpdump

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')
DEPS := $(shell find $(DEP_DIR) -name '*.h' -or -name '*.hpp')

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# define the C compiler to use
CXX := g++

# define any compile-time flags
CXXFLAGS := -Wall -g -std=c++17 -fdiagnostics-color=always -DSPDLOG_COMPILED_LIB

# define any directories containing header files other than /usr/include
#
INCLUDES := -I./include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS := -L./lib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS := -lspdlog

.PRECIOUS: $(BUILD_DIR)/%.o

all: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LFLAGS) $(LIBS)

%.test: $(filter-out %/main.o,$(OBJS)) $(BUILD_DIR)/$(TEST_DIR)/%.test.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $(BUILD_DIR)/$@ $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
$(BUILD_DIR)/%.o: %.cpp $(DEPS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
