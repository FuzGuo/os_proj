#!/bin/bash

# 功能: 将原始备份文件恢复到 linux-0.11 的源码目录中。

# 设置 -e 选项，如果任何命令执行失败，脚本将立即退出，确保操作的原子性。
set -e

DEST_DIR="../lab/linux-0.11/boot"
SRC_BOOTSECT="bootsect.s.bak"
SRC_SETUP="setup.s.bak"
DEST_BOOTSECT="$DEST_DIR/bootsect.s"
DEST_SETUP="$DEST_DIR/setup.s"

echo "--- 开始恢复原始汇编文件 ---"

if [ ! -d "$DEST_DIR" ]; then
    echo "错误：目标目录 '$DEST_DIR' 不存在。请检查你的项目结构。"
    exit 1
fi


if [ ! -f "$SRC_BOOTSECT" ]; then
    echo "错误：源文件 '$SRC_BOOTSECT' 未在当前目录找到，无法恢复。"
else
    echo "正在恢复: '$SRC_BOOTSECT' -> '$DEST_BOOTSECT'"
    cp "$SRC_BOOTSECT" "$DEST_BOOTSECT"
fi

if [ ! -f "$SRC_SETUP" ]; then
    echo "错误：源文件 '$SRC_SETUP' 未在当前目录找到，无法恢复。"
else
    echo "正在恢复: '$SRC_SETUP' -> '$DEST_SETUP'"
    cp "$SRC_SETUP" "$DEST_SETUP"
fi

echo ""
echo "--- 恢复操作完成 ---"