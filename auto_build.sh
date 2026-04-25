#!/bin/bash

set -e

BUILD_ONLY=false
INSTALL_ONLY=false

case "${1:-}" in 
    --build)
        BUILD_ONLY=true
        ;;
    --install)
        INSTALL_ONLY=true
        ;;
    "")
        ;;
    *)
        echo "用法: $0 [--build|--install]"
        echo "  --build     仅构建编译"
        echo "  --install   仅安装"
        echo "  (无参数)    构建 + 安装"
        exit 1
        ;;
esac

PROJECT_DIR="$(pwd)"
BUILD_DIR="$PROJECT_DIR/build"


case "$BUILD_DIR" in
    "/"|"/*"|"/bin"|"/bin/"*|"/etc"|"/etc/"*|"/usr"|"/usr/"*|"/home"|"~")
    echo "✴ 拒绝删除系统目录:$BUILD_DIR"
    exit 1
    ;;
esac

# ================构建====================
if [ "$INSTALL_ONLY" = false ]; then

    if [ -d "$BUILD_DIR" ]; then
        echo "✴ 删除旧的构建目录:$BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi



    echo "✴ 配置CMake项目..."
    cmake -B "$(pwd)/build"
    echo "✴ 编译项目..."
    cmake --build "$(pwd)/build" -j16
    echo "✴ 构建完成"
fi

# ========== 安装 ==========
if [ "$BUILD_ONLY" = false ]; then
    echo "✴ 安装..."
    sudo cmake --install build
    echo "✴ 安装完毕"
    sudo ldconfig
fi