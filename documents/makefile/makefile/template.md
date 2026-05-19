```
ifndef TOOLCHAIN_DIR
    $(error TOOLCHAIN_DIR is undefined)
endif


ver=release

CXX=g++
ifeq ($(ver),debug)
CXXFLAGS=-std=c++11 -Wall -Wextra -g -O0 -DDEBUG
else
CXXFLAGS=-std=c++11 -Wall #-Wextra
endif
BIN=ldpipe

# 除去隐藏目录
ALLDIR=$(shell find . -type d | grep -E '/\.|proto|test' -v)
SRCS=$(foreach DIR,$(ALLDIR), $(wildcard $(DIR)/*.cpp))
OBJS=$(patsubst %.cpp,%.o, $(SRCS))
PROTODIR=$(shell find . -type d | grep -E '/proto$$')
PROTOFILE=$(wildcard $(PROTODIR)/*.proto)
PROTOSRC=$(patsubst %.proto,%.pb.cc,$(PROTOFILE)) $(patsubst %.proto,%.grpc.pb.cc,$(PROTOFILE))
PROTOHEAD=$(patsubst %.cc,%.h,$(PROTOSRC))
PROTOOBJS=$(patsubst %.cc,%.o, $(PROTOSRC))


ALL: $(PROTOOBJS) $(OBJS)
    $(CXX) $(CXXFLAGS) -o $(BIN) $^ -L$(TOOLCHAIN_DIR)/installed/lib -pthread -lprotobuf -lgrpc++

%.o: %.cc %.cpp
    $(CXX) $(CXXFLAGS) -o $@ -c $<

$(PROTOSRC): $(PROTOFILE)
    cd $(PROTODIR);protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $(notdir $(PROTOFILE))
    cd $(PROTODIR);protoc --cpp_out=. $(notdir $(PROTOFILE))


clean:
    rm -f $(OBJS) $(PROTOOBJS) $(BIN)

// $^,所有的依赖目标的集合。以空格分隔。如果在依赖目标中有多个重复的，那个这个变量会去除重复的依赖目标，只保留一份。
// $@,表示规则中的目标文件集。在模式规则中，如果有多个目标，那么，"$@"就是匹配于目标中模式定义的集合。
// $<,依赖目标中的第一个目标名字。如果依赖目标是以模式（即"%"）定义的，那么"$<"将是符合模式的一系列的文件集。注意，其是一个一个取出来的。
```
