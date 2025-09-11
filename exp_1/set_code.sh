#!/bin/bash

# 功能: 将修改过的汇编文件和 build.c 部署到 linux-0.11 的源码目录中。

set -e


BOOT_DIR="../lab/linux-0.11/boot"
TOOLS_DIR="../lab/linux-0.11/tools"

SRC_BOOTSECT="bootsect.s"
SRC_SETUP="setup.s"
SRC_BUILD="build.c"

DEST_BOOTSECT="$BOOT_DIR/bootsect.s"
DEST_SETUP="$BOOT_DIR/setup.s"
DEST_BUILD="$TOOLS_DIR/build.c"


echo "--- 开始部署修改后的文件 ---"

if [ ! -d "$BOOT_DIR" ] || [ ! -d "$TOOLS_DIR" ]; then
    echo "错误：目标目录 '$BOOT_DIR' 或 '$TOOLS_DIR' 不存在。"
    exit 1
fi

if [ ! -f "$SRC_BOOTSECT" ]; then
    echo "警告：源文件 '$SRC_BOOTSECT' 未找到，跳过部署。"
else
    echo "正在部署: '$SRC_BOOTSECT' -> '$DEST_BOOTSECT'"
    cp "$SRC_BOOTSECT" "$DEST_BOOTSECT"
fi

if [ ! -f "$SRC_SETUP" ]; then
    echo "警告：源文件 '$SRC_SETUP' 未找到，跳过部署。"
else
    echo "正在部署: '$SRC_SETUP' -> '$DEST_SETUP'"
    cp "$SRC_SETUP" "$DEST_SETUP"
fi

if [ ! -f "$SRC_BUILD" ]; then
    echo "警告：源文件 '$SRC_BUILD' 未找到，跳过部署。"
else
    echo "正在部署: '$SRC_BUILD' -> '$DEST_BUILD'"
    cp "$SRC_BUILD" "$DEST_BUILD"
fi


echo ""
echo "--- 部署操作完成 ---"