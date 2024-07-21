# -- config
CXX ?= g++
# -- end config

INCLUDE = -Iinclude/ -I.
CXXFLAGS = -std=c++20 -g -Og -Wall $(INCLUDE)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRCS = $(call rwildcard,src/lib/,*.cpp)
OBJS = $(SRCS:src/lib/%.cpp=build/lib/%.o)
DEPS = $(OBJS:%.o=%.d)

SONAME = libalgotrade.so

TESTBOT_SRCS = $(call rwildcard,src/testbot/,*.cpp)
TESTBOT_OBJS = $(TESTBOT_SRCS:src/testbot/%.cpp=build/testbot/%.o)
TESTBOT_DEPS = $(TESTBOT_OBJS:%.o=%.d)

.PHONY: default
default: lib testbot

.PHONY: lib
lib: build/lib/$(SONAME)

.PHONY: testbot
testbot: build/testbot/testbot

.PHONY: clean
clean:
	rm -rf build/

build/lib/:
	mkdir -p build/lib

build/testbot/:
	mkdir -p build/testbot

build/lib/$(SONAME): build/lib/ $(OBJS)
	$(CXX) $(CXXFLAGS) -lcpr -shared -o $@ $(OBJS)

build/testbot/testbot: build/testbot/ $(TESTBOT_OBJS) build/lib/$(SONAME)
	$(CXX) $(CXXFLAGS) -Lbuild/lib/ -lyaml-cpp -lalgotrade -o $@ $(TESTBOT_OBJS)

-include $(DEPS)
-include $(TESTBOT_DEPS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -fPIC -MMD -c $< -o $@
