# CXX      ?= g++
# CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -O2
CXX      = clang++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2 -fms-extensions
CPPFLAGS += -I. -MMD -MP
LDFLAGS  ?=
LDLIBS   ?=

TARGET   := main
BUILDDIR := build
SRCS     := $(shell find . -name '*.cpp' -not -path './$(BUILDDIR)/*')
OBJS     := $(SRCS:./%.cpp=$(BUILDDIR)/%.o)
DEPS     := $(OBJS:.o=.d)

.PHONY: all run debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

# debug: CXXFLAGS := -std=c++20 -Wall -Wextra -g -O0
debug: CXXFLAGS := -std=c++20 -Wall -Wextra -g -O0 -fms-extensions
debug: clean $(TARGET)

clean:
	rm -rf $(BUILDDIR) $(TARGET)

-include $(DEPS)
