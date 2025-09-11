## 实验一

本次实验文档结构为。

注意，sh文件都需要在exp_1目录环境下执行，不然会出bug。
如果运行不了sh，运行 `chmod +x *.sh` 以解决问题。

```bash
exp_1
├── bootsect.s  # 完成任务的版本
├── bootsect.s.bak  # 原始文件备份
├── build.c  # 完成任务的版本
├── build.c.bak  # 原始文件备份
├── make_all.sh  # 可从exp_1目录下直接make all 目录lab/linux-0.11，集成了下面set_code.sh的功能
├── make_clean.sh # 同上，不过是make clean
├── readme.md  # readme文档
├── recover_code.sh  # 恢复原始备份bootsect.s、setup.s到lab/linux-0.11/boot
├── set_code.sh  # 设置修改后的bootsect.s、setup.s到lab/linux-0.11/boot
├── setup.s   # 完成任务的版本
└── setup.s.bak # 原始文件备份
```

所有的.bak文件都是未修改文件的备份，运行recover_code.sh就可以恢复备份。

运行set_code.sh可以把你自己的不带.bak文件后缀的部署到lab/linux-0.11中。

注意，make_all.sh内部已经集成了set_code.sh，没必要在运行之前重复执行。