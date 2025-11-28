CC = gcc
TARGET = easyat
SOURCE = easyat.c

# 调试配置
ifdef DEBUG
    CFLAGS = -Wall -Wextra -std=c99 -g -DDEBUG -O0
else
    CFLAGS = -Wall -Wextra -std=c99 -O2
endif

# 默认目标
all: $(TARGET)

# 编译目标
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# 清理目标
clean:
	rm -f $(TARGET)

# 安装目标（可选）
install: $(TARGET)
	@echo "安装 $(TARGET) 到 /usr/local/bin/..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "✓ 安装完成: /usr/local/bin/$(TARGET)"

# 卸载目标（可选）
uninstall:
	@echo "从系统卸载 $(TARGET)..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "✓ 卸载完成"

# 调试版本
debug:
	$(MAKE) DEBUG=1

# 检查代码风格
check:
	@echo "检查代码编译..."
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCE)

# 显示帮助信息
help:
	@echo "可用的make目标:"
	@echo "  all      - 编译程序 (默认)"
	@echo "  clean    - 清理编译文件"
	@echo "  debug    - 编译调试版本"
	@echo "  install  - 安装到系统"
	@echo "  uninstall- 从系统卸载"
	@echo "  check    - 检查代码语法"
	@echo "  help     - 显示此帮助信息"

.PHONY: all clean install uninstall debug check help