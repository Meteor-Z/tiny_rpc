# tiny_rpc

## 编译相关

编译环境：Arch Linux

编译器版本： clang 16.06

C++版本： C++20

>注意：用到了c++中的`std::source_location()`，低版本的编译器可能对c++20支持不好，导致查找不到相关的定义。

### 必装的库

1. git submodule update --init --recursive: 更新以下子模块

### 命令

```shell
git clone https://github.com/Meteor-Z/tiny_rpc.git
cd tiny_rpc 
mkdir build && cd build
cmake .. && make
```
