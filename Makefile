TARGET_EXEC ?= a.out 

BUILD_DIR ?= ./build
SRC_DIRS ?= ./classes ./lib

SRCS := $(shell find $(SRC_DIRS) -name "[!.]*.cpp" -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -g -std=c++17 -O2 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include 
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lfreetype
CC = g++
CXX = g++

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

run:
	./build/a.out

crun: $(BUILD_DIR)/$(TARGET_EXEC)
	./build/a.out

profile: CPPFLAGS += -pg
profile: LDFLAGS += -pg
profile: $(BUILD_DIR)/$(TARGET_EXEC)

stacktrace: CPPFLAGS += -v -da -Q
stacktrace: $(BUILD_DIR)/$(TARGET_EXEC)
