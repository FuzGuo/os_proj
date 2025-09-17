#!/bin/bash

set -e

LAB_DIR="../lab"
SOURCE_FILE="${LAB_DIR}/hdc/etc/process.log"
DEST_DIR="."

echo "INFO: 开始提取 process.log..."

echo "INFO: 正在挂载 hdc..."
(cd "${LAB_DIR}" && sudo ./mount-hdc)

if [ -f "${SOURCE_FILE}" ]; then
    echo "INFO: 正在复制 process.log..."
    sudo cp "${SOURCE_FILE}" "${DEST_DIR}"
    sudo chown "$SUDO_USER:$SUDO_USER" "${DEST_DIR}/process.log"
else
    echo "错误: 未找到日志文件: ${SOURCE_FILE}"
fi

echo "INFO: 正在卸载 hdc..."
sleep 2
(cd "${LAB_DIR}" && sudo umount hdc)

echo "INFO: 脚本执行完毕。"