## 实验一

本次实验文档结构为。

注意，sh文件都需要在exp_1目录环境下执行，不然会出bug。
```bash
exp_1
├── bootsect.s  # 完成任务的版本
├── bootsect.s.bak  # 原始文件备份
├── make_all.sh  # 可从exp_1目录下直接make all 目录lab/linux-0.11
├── make_clean.sh # 同上，不过是make clean
├── readme.md  # readme文档
├── recover_s.sh  # 恢复原始备份bootsect.s、setup.s到lab/linux-0.11/boot
├── set_s.sh  # 设置修改后的bootsect.s、setup.s到lab/linux-0.11/boot
├── setup.s   # 完成任务的版本
└── setup.s.bak # 原始文件备份
```
