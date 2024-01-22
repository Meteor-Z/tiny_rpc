# tiny_rpc

## 环境

编译器： `clang` or `gcc`
C++版本： C++17

## 必装的库

- 更新子模块:`git submodule update --init --recursive`
- 安装protubuf
  - 以Ubuntu为例:`sudo apt install libprotobuf-dev`

## 相关的命令

### 编译安装
```shell
git clone https://github.com/Meteor-Z/tiny_rpc.git
cd tiny_rpc 
mkdir build && cd build
cmake .. 
make -j20
```
### 文档生成

代码可以通过doxygen生成文档

```shell
mkdir document
doxygen Doxyfile
```
在`document`里面打开`index.html`即可。

### 生成protobuf文件

```shell
# 根据order.proto文件生成文件，输出在当前文件中
protoc --cpp_out=./ order.proto 
```