# 数据生成框架 v2.0.0

竞赛题目测试数据批量生成工具，支持 Windows / Linux / macOS。

## 快速开始

```bash
# 1. 配置 data.txt 和 data.cpp
# 2. 编译主程序
g++ -O2 -o main.exe main.cpp

# 普通模式：生成数据 + 标程答案
main.exe

# 对拍模式：检验 user.cpp 与 std.cpp 是否一致
main.exe --check

# 对拍 + 计时
main.exe --check --time

# 指定其他用户程序
main.exe --check --user mysol.cpp
```

## 文件结构

```
├── main.cpp        主控程序
├── data.cpp        数据生成器（修改底部标注区域）
├── std.cpp         标准程序（参考答案）
├── function.hpp    工具函数库
├── data.txt        数据规模配置
├── help.txt        详细使用说明
└── data/
    └── <题目名>/
        ├── <题目名>1.in
        ├── <题目名>1.out
        └── ...
```

## 主要更新（v2.0.0）

- **随机数**：换用 `mt19937_64`，彻底修复大范围随机偏差问题
- **对拍**：`main.exe --check` 自动对比标程与用户程序，输出失败列表
- **计时**：`--time` 统计每组数据耗时，方便卡时间复杂度
- **新函数**：连通图、DAG、随机排列、随机字符串、DSU结构体等
- **跨平台**：去除 `<windows.h>` 依赖，支持 Linux/macOS
- **彩色输出**：OK/FAIL 一目了然

详见 `help.txt`。
