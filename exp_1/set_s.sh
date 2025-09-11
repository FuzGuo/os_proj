#!/bin/bash

# 功能: 将修改过的汇编文件部署到 linux-0.11 的源码目录中。

# 设置 -e 选项，如果任何命令执行失败，脚本将立即退出。
set -e

DEST_DIR="../lab/linux-0.11/boot"
SRC_BOOTSECT="bootsect.s"
SRC_SETUP="setup.s"
DEST_BOOTSECT="$DEST_DIR/bootsect.s"
DEST_SETUP="$DEST_DIR/setup.s"

echo "--- 开始部署修改后的汇编文件 ---"

if [ ! -d "$DEST_DIR" ]; then
    echo "错误：目标目录 '$DEST_DIR' 不存在。请检查你的项目结构。"
    exit 1
fi

if [ ! -f "$SRC_BOOTSECT" ]; then
    echo "错误：源文件 '$SRC_BOOTSECT' 未在当前目录找到，无法部署。"
    echo "请确保你已经在 exp_1 目录中创建了该文件。"
else
    echo "正在部署: '$SRC_BOOTSECT' -> '$DEST_BOOTSECT'"
    cp "$SRC_BOOTSECT" "$DEST_BOOTSECT"
fi

if [ ! -f "$SRC_SETUP" ]; then
    echo "错误：源文件 '$SRC_SETUP' 未在当前目录找到，无法部署。"
    echo "请确保你已经在 exp_1 目录中创建了该文件。"
else
    echo "正在部署: '$SRC_SETUP' -> '$DEST_SETUP'"
    cp "$SRC_SETUP" "$DEST_SETUP"
fi

echo ""
echo "--- 部署操作完成 ---"