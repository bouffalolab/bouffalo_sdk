# NetHub Userspace Tools Makefile
# 支持 C 语言版本和 Expect 脚本版本的统一构建

.PHONY: all clean install uninstall expect c build-prepare help check-deps test

# 默认目标：构建 C 版本并设置 Expect 脚本权限
all: build-prepare expect c

# 构建准备
build-prepare:
	@echo "准备构建环境..."
	@mkdir -p easyat/src easyat/scripts easyat/docs easyat/tests tools
	@chmod +x connect_and_setip
	@echo "✓ 构建环境准备完成"

# C 语言版本构建
c:
	@echo "构建 C 语言版本..."
	@cd easyat/src && $(MAKE)
	@echo "✓ C 语言版本构建完成"

# Expect 脚本版本设置
expect:
	@echo "设置 Expect 脚本版本..."
	@chmod +x easyat/scripts/connect
	@chmod +x easyat/scripts/get_link_status
	@chmod +x easyat/scripts/wifi_scan
	@echo "✓ Expect 脚本版本设置完成"

# 只构建 C 版本
c-only: build-prepare
	@cd easyat/src && $(MAKE)

# 只设置 Expect 版本
expect-only: build-prepare expect

# 清理构建文件
clean:
	@echo "清理构建文件..."
	@cd easyat/src && $(MAKE) clean 2>/dev/null || true
	@rm -f easyat/src/easyat
	@echo "✓ 构建文件清理完成"

# 安装到系统
install: all
	@echo "安装 EasyAT 工具到系统..."
	@# 创建安装目录
	@sudo mkdir -p /usr/local/bin
	@sudo mkdir -p /usr/local/share/easyat
	@sudo mkdir -p /usr/local/share/doc/easyat

	@# 安装 C 版本可执行文件
	@if [ -f easyat/src/easyat ]; then \
		sudo cp easyat/src/easyat /usr/local/bin/easyat; \
		echo "✓ C 版本已安装: /usr/local/bin/easyat"; \
	fi

	@# 安装 connect_and_setip 脚本
	@sudo cp connect_and_setip /usr/local/bin/connect_and_setip; \
		echo "✓ connect_and_setip 脚本已安装: /usr/local/bin/connect_and_setip"

	@# 安装 Expect 脚本
	@sudo mkdir -p /usr/local/share/easyat/scripts; \
		sudo cp easyat/scripts/* /usr/local/share/easyat/scripts/; \
		echo "✓ Expect 脚本已安装: /usr/local/share/easyat/scripts/"

	@# 安装文档
	@sudo cp -r easyat/docs /usr/local/share/doc/easyat/; \
		echo "✓ 文档已安装: /usr/local/share/doc/easyat/"

	@# 创建 Expect 版本的符号链接
	@sudo ln -sf /usr/local/share/easyat/scripts/connect /usr/local/bin/easyat-connect; \
		sudo ln -sf /usr/local/share/easyat/scripts/get_link_status /usr/local/bin/easyat-status; \
		sudo ln -sf /usr/local/share/easyat/scripts/wifi_scan /usr/local/bin/easyat-scan; \
		echo "✓ Expect 脚本链接已创建"

	@echo "✓ 安装完成"

# 从系统卸载
uninstall:
	@echo "从系统卸载 EasyAT 工具..."
	@sudo rm -f /usr/local/bin/easyat
	@sudo rm -f /usr/local/bin/connect_and_setip
	@sudo rm -f /usr/local/bin/easyat-connect
	@sudo rm -f /usr/local/bin/easyat-status
	@sudo rm -f /usr/local/bin/easyat-scan
	@sudo rm -rf /usr/local/share/easyat
	@sudo rm -rf /usr/local/share/doc/easyat
	@echo "✓ 卸载完成"

# 检查依赖
check-deps:
	@echo "检查系统依赖..."
	@# 检查 C 编译器
	@if command -v gcc >/dev/null 2>&1; then \
		echo "✓ GCC 编译器: $$(gcc --version | head -n1)"; \
	else \
		echo "✗ 未找到 GCC 编译器，请安装 build-essential"; \
	fi

	@# 检查 Make
	@if command -v make >/dev/null 2>&1; then \
		echo "✓ Make 工具: $$(make --version | head -n1)"; \
	else \
		echo "✗ 未找到 Make 工具"; \
	fi

	@# 检查 Expect
	@if command -v expect >/dev/null 2>&1; then \
		echo "✓ Expect 解释器: $$(expect -v)"; \
	else \
		echo "✗ 未找到 Expect 解释器，请安装 expect"; \
	fi

	@# 检查串口设备
	@if [ -e "/dev/ttyHD0" ]; then \
		echo "✓ 串口设备: /dev/ttyHD0"; \
		if [ -r "/dev/ttyHD0" ] && [ -w "/dev/ttyHD0" ]; then \
			echo "✓ 串口设备权限正常"; \
		else \
			echo "⚠ 串口设备权限不足，请将用户添加到 dialout 组"; \
		fi; \
	else \
		echo "⚠ 未找到串口设备 /dev/ttyHD0，请确认驱动已加载"; \
	fi

# 测试构建
test: all
	@echo "执行功能测试..."
	@# 测试 C 版本编译
	@if [ -f easyat/src/easyat ]; then \
		echo "✓ C 版本编译测试通过"; \
	else \
		echo "✗ C 版本编译测试失败"; \
		exit 1; \
	fi

	@# 测试 Expect 脚本权限
	@if [ -x easyat/scripts/connect ] && \
	   [ -x easyat/scripts/get_link_status ] && \
	   [ -x easyat/scripts/wifi_scan ]; then \
		echo "✓ Expect 脚本权限测试通过"; \
	else \
		echo "✗ Expect 脚本权限测试失败"; \
		exit 1; \
	fi

	@# 测试 connect_and_setip 脚本
	@if [ -x connect_and_setip ]; then \
		echo "✓ connect_and_setip 脚本测试通过"; \
	else \
		echo "✗ connect_and_setip 脚本测试失败"; \
		exit 1; \
	fi

	@echo "✓ 所有测试通过"

# 开发者工具
dev-setup:
	@echo "设置开发环境..."
	@# 安装开发依赖
	@echo "请安装以下开发依赖:"
	@echo "  sudo apt-get install build-essential expect"
	@echo "  sudo apt-get install gdb valgrind  # 可选调试工具"
	@# 创建开发配置
	@echo "DEBUG=1" > easyat/src/.debug
	@echo "# 开发配置已创建，运行 'make dev-clean' 可移除"

dev-clean:
	@echo "清理开发环境..."
	@rm -f easyat/src/.debug
	@$(MAKE) clean

# 打包发布
dist:
	@echo "创建发布包..."
	@# 获取版本信息
	@VERSION=$$(date +%Y%m%d); \
	DIST_NAME="easyat-$$VERSION"; \
	mkdir -p dist/$$DIST_NAME; \
	cp -r easyat scripts docs README.md connect_and_setip Makefile dist/$$DIST_NAME/; \
	cd dist && tar -czf $$DIST_NAME.tar.gz $$DIST_NAME/; \
	echo "✓ 发布包已创建: dist/$$DIST_NAME.tar.gz"

# 显示帮助信息
help:
	@echo "NetHub Userspace Tools 构建系统"
	@echo ""
	@echo "可用目标:"
	@echo "  all          - 构建所有版本（默认）"
	@echo "  c            - 只构建 C 语言版本"
	@echo "  expect       - 只设置 Expect 脚本版本"
	@echo "  c-only       - 只构建 C 版本（不包括脚本）"
	@echo "  expect-only  - 只设置 Expect 版本"
	@echo "  clean        - 清理构建文件"
	@echo "  install      - 安装到系统 (/usr/local)"
	@echo "  uninstall    - 从系统卸载"
	@echo "  check-deps   - 检查系统依赖"
	@echo "  test         - 执行功能测试"
	@echo "  dev-setup    - 设置开发环境"
	@echo "  dev-clean    - 清理开发环境"
	@echo "  dist         - 创建发布包"
	@echo "  help         - 显示此帮助信息"
	@echo ""
	@echo "使用示例:"
	@echo "  make                    # 构建所有版本"
	@echo "  make c                  # 只构建 C 版本"
	@echo "  make install            # 构建并安装"
	@echo "  make check-deps         # 检查依赖"
	@echo "  make test               # 运行测试"
	@echo ""
	@echo "环境变量:"
	@echo "  DEBUG=1                 - 启用调试模式"
	@echo "  PREFIX=/path            - 自定义安装路径"