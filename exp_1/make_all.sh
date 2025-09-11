#!/bin/bash

# 设置 -e 选项，如果任何命令失败，脚本将立即退出
set -e

LINUX_DIR="../lab/linux-0.11"

./set_code.sh

echo "--- 开始在目录 '$LINUX_DIR' 中执行 make all ---"

# 检查目录是否存在
if [ ! -d "$LINUX_DIR" ]; then
    echo "错误：目标目录 '$LINUX_DIR' 不存在。"
    exit 1
fi

# 执行 make 命令
make -C "$LINUX_DIR" all

echo "--- make all 执行成功 ---"