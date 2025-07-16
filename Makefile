# 编译器与参数
CXX := g++
CXXFLAGS := -std=c++17 -Wall -fPIC -Iinclude
LDFLAGS := -Lbuild -ldiffie_hellman -lssl -lcrypto -lzmq -pthread

# 目录定义
SRC_DIR := src
INC_DIR := include
TEST_DIR := test
BUILD_DIR := build

# 源文件和对象文件
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# 动态库目标
SO_TARGET := $(BUILD_DIR)/libdiffie_hellman.so

# 测试程序
MAIN_SRC := $(TEST_DIR)/main.cpp
MAIN_BIN := $(BUILD_DIR)/secure_dh_app

SENDER_SRC := $(TEST_DIR)/sender.cpp
SENDER_BIN := $(BUILD_DIR)/sender

RECEIVER_SRC := $(TEST_DIR)/receiver.cpp
RECEIVER_BIN := $(BUILD_DIR)/receiver

# 默认目标
all: $(SO_TARGET) $(MAIN_BIN) $(SENDER_BIN) $(RECEIVER_BIN)

# 构建动态链接库
$(SO_TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -shared -o $@ $^

# 编译所有 .cpp 为 .o（全部带 fPIC）
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译主程序
$(MAIN_BIN): $(MAIN_SRC) $(SO_TARGET)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# 编译 sender
$(SENDER_BIN): $(SENDER_SRC) $(SO_TARGET)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# 编译 receiver
$(RECEIVER_BIN): $(RECEIVER_SRC) $(SO_TARGET)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

LDFLAGS := -Lbuild -ldiffie_hellman -lssl -lcrypto -lzmq -pthread -Wl,-rpath,'$$ORIGIN'


# 运行主程序
run: $(MAIN_BIN)
	LD_LIBRARY_PATH=build ./$(MAIN_BIN)

# 清理构建产物
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.so $(BUILD_DIR)/secure_dh_app $(BUILD_DIR)/sender $(BUILD_DIR)/receiver

.PHONY: all clean run
