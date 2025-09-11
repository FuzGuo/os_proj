# 操作系统课程设计

## 项目结构

下面是本项目的文件树，一般的文件都有注释，没有注释的是实验包自带的文件。
``` bash
.
├── exp_<实验id>
│   ├── readme.md
│   └── 相应的实验代码，py脚本
├── gcc-3.4.tar.gz #用于安装gcc 3.4 64位/32位版本
├── hit-oslab-linux-20110823.tar.gz # 原始、未经改动的实验包
├── lab #解压后的实验包
│   ├── bochs
│   ├── bochsout.txt
│   ├── dbg-asm
│   ├── dbg-c
│   ├── gdb
│   ├── gdb-cmd.txt
│   ├── hdc
│   ├── hdc-0.11.img 
│   ├── linux-0.11
│   ├── mount-hdc
│   ├── run
│   └── rungdb
└── readme.md #项目总体说明文档
```

## 0. 搭建实验环境
本项目从最开始的压缩包开始`hit-oslab-linux-20110823.tar.gz`,借助gcc-3.4，成功从零搭建出可以运行的实验环境。

解压2011年版本的实验包。
```shell
tar -zxvf hit-oslab-linux-20110823.tar.gz 
```

安装gcc 3.4版本

gcc 3.4版本需要手动安装，我们已经准备好了gcc 3.4的版本。

解压后有两个gcc文件夹，一个amd64版本的，一个i386版本的，前者64位，后者32位，由于目前尚未在32位系统测试过，建议安装amd64版本的。

``` shell
sudo dpkg install ./文件名.deb
sudo apt --fix-broken install #解决依赖问题（如果有的话）
```

bootsect.S 和 setup.S 这两个文件是运行在实模式下的16位汇编代码，需要使用 as86 汇编编译器和 ld86 连接器来处理 。这两个工具包含在 bin86 软件包中 。因此安装bin86。

```
apt-cache search as86 ld86
sudo apt install bin86
```

由于是在64位系统上安装32位程序的编译环境，还需要安装其他32位系统的兼容库。
```
sudo apt install libc6-dev-i386
sudo apt install libsm6:i386
sudo apt install libx11-6:i386
sudo apt install libxpm4:i386
```
到此可以编译并输出了。

提示没有make指令，可以用apt安装。

如果*存在*apt安装失败（无法定位包）的情况，请尝试`sudo apt update`。

在lab/linux-0.11使用下使用`make all`，不要在lab目录下使用。

至此实验环境搭建完毕，`make all`可输出img文件。具体如何挂载、启动Bochs可以参考实验指导书，虽然比较旧，但是搭建完环境后上面的指导还是相对完整的。

## 1.实验一
