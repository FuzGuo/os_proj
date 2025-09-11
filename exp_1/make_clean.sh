#!/bin/bash

# 功能: 对 linux-0.11 项目执行 make clean 命令。

# 设置 -e 选项，如果任何命令失败，脚本将立即退出
set -e

# Linux-0.11 源码目录
LINUX_DIR="../lab/linux-0.11"

echo "--- 开始在目录 '$LINUX_DIR' 中执行 make clean ---"

# 检查目录是否存在
if [ ! -d "$LINUX_DIR" ]; then
    echo "错误：目标目录 '$LINUX_DIR' 不存在。"
    exit 1
fi

# 执行 make clean 命令
make -C "$LINUX_DIR" clean

echo "--- make clean 执行成功 ---"