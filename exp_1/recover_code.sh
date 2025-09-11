#!/bin/bash

# 功能: 将原始备份文件恢复到 linux-0.11 的源码目录中。

set -e

BOOT_DIR="../lab/linux-0.11/boot"
TOOLS_DIR="../lab/linux-0.11/tools"

SRC_BOOTSECT="bootsect.s.bak"
SRC_SETUP="setup.s.bak"
SRC_BUILD="build.c.bak"

DEST_BOOTSECT="$BOOT_DIR/bootsect.s"
DEST_SETUP="$BOOT_DIR/setup.s"
DEST_BUILD="$TOOLS_DIR/build.c"


echo "--- 开始恢复原始文件 ---"

if [ ! -d "$BOOT_DIR" ] || [ ! -d "$TOOLS_DIR" ]; then
    echo "错误：目标目录 '$BOOT_DIR' 或 '$TOOLS_DIR' 不存在。"
    exit 1
fi

if [ ! -f "$SRC_BOOTSECT" ]; then
    echo "警告：备份文件 '$SRC_BOOTSECT' 未找到，跳过恢复。"
else
    echo "正在恢复: '$SRC_BOOTSECT' -> '$DEST_BOOTSECT'"
    cp "$SRC_BOOTSECT" "$DEST_BOOTSECT"
fi

if [ ! -f "$SRC_SETUP" ]; then
    echo "警告：备份文件 '$SRC_SETUP' 未找到，跳过恢复。"
else
    echo "正在恢复: '$SRC_SETUP' -> '$DEST_SETUP'"
    cp "$SRC_SETUP" "$DEST_SETUP"
fi

if [ ! -f "$SRC_BUILD" ]; then
    echo "警告：备份文件 '$SRC_BUILD' 未找到，跳过恢复。"
    echo "请确认你是否已执行第一步创建了该备份文件。"
else
    echo "正在恢复: '$SRC_BUILD' -> '$DEST_BUILD'"
    cp "$SRC_BUILD" "$DEST_BUILD"
fi


echo ""
echo "--- 恢复操作完成 ---"