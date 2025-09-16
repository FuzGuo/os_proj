#!/bin/bash

# ==============================================================================
#  该脚本用于自动挂载 hdc 镜像，从中复制 process.log 文件，然后卸载镜像。
#  请在 exp_3 目录下运行此脚本。
# ==============================================================================

# 设置 -e 选项，如果任何命令失败，脚本将立即退出，防止后续错误。
set -e

# --- 配置路径 ---
# lab 目录相对于当前目录 (exp_3) 的位置
LAB_DIR="../lab"

# 源文件在挂载后的完整路径
SOURCE_FILE="${LAB_DIR}/hdc/etc/process.log"

# 目标目录 (当前目录)
DEST_DIR="."

# --- 脚本主体 ---

echo "INFO: 开始执行自动化脚本..."

# 1. 挂载文件系统
# 使用子 shell (小括号) 来执行挂载命令，
# 这样可以临时切换到 lab 目录而不影响脚本当前的工作目录。
echo "==> 步骤 1/3: 正在挂载 hdc 文件系统..."
(cd "${LAB_DIR}" && sudo ./mount-hdc)
echo "    挂载成功。"

# 2. 复制日志文件
# 检查文件是否存在，以防内核没有生成日志。
if [ -f "${SOURCE_FILE}" ]; then
    echo "==> 步骤 2/3: 正在复制 process.log 文件到当前目录..."
    # 从挂载点复制文件可能需要 sudo 权限
    sudo cp "${SOURCE_FILE}" "${DEST_DIR}"
    # 复制过来的文件所有者是 root，修改为当前运行 sudo 的用户
    sudo chown "$SUDO_USER:$SUDO_USER" "${DEST_DIR}/process.log"
    echo "    复制成功，文件所有者已修改为 $SUDO_USER。"
else
    echo "错误: 未找到日志文件: ${SOURCE_FILE}"
    echo "     请确认您的内核修改是否正确，并且已经重新编译运行。"
fi

# 3. 卸载文件系统
# 同样使用子 shell 来确保在正确的目录下执行卸载命令。
echo "==> 步骤 3/3: 正在卸载 hdc 文件系统..."
# 需要等两秒钟，确保所有文件操作完成
sleep 2
(cd "${LAB_DIR}" && sudo umount hdc)
echo "    卸载成功。"

echo ""
echo "INFO: 脚本执行完毕！process.log 文件已复制到当前目录。"