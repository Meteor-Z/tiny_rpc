# tiny_rpc

## 编译相关

编译器： `clang` or `gcc`
C++版本： C++17

### 必装的库

1. 更新子模块:`git submodule update --init --recursive`

### 命令

```shell
git clone https://github.com/Meteor-Z/tiny_rpc.git
cd tiny_rpc 
mkdir build && cd build
cmake .. 
make -j20
```
## 文档生成

代码可以通过doxygen生成文档

```shell
mkdir document
doxygen Doxyfile
```
在`document`里面打开`index.html`即可。