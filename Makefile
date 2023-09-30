CXX = g++
# 添加任何必要的编译选项
CFLAGS = -Wall -g
LDFLAGS = -lpthread
SANITISER = 

# 源文件和目标文件列表
SRCDIR = src
OBJDIR = build
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
TARGETS = test-server test-client

# 默认目标
all: $(TARGETS)

# test-server的编译规则
test-server: $(OBJS) $(OBJDIR)/test-server.o
	$(CXX) $(CFLAGS) $(SANITISER) $(OBJS) $(OBJDIR)/test-server.o -o $@ $(LDFLAGS) 

# test-client的编译规则
test-client: $(OBJS) $(OBJDIR)/test-client.o
	$(CXX) $(CFLAGS) $(SANITISER) $(OBJS) $(OBJDIR)/test-client.o -o $@ $(LDFLAGS) 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/test-server.o: test-server.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/test-client.o: test-client.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJS)  $(OBJDIR)/*.o $(TARGETS)


# 伪目标，用于防止与同名文件冲突
.PHONY: all clean
