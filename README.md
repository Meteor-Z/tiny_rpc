# Tiny_Rpc

使用C/++17版本编写的一个简易的Rpc框架，主要涵盖的内容是客户端和服务端双方进行rpc框架的调用，其中的内容包括但不限于以下内容

1. 日志和配置文件的开发，使用的是xml进行配置，日志采用异步输出日志文件信息
2. Reactor模块进行开发（EventLoop）的开发，mainReactor一个，subReactor 四个，还有一个额外的一个副线程专门打印相关日志
3. 定时器Timer的开发
4. 内容缓冲区的开发，也就是Buffer的相关使用，std::vector&lt;char&gt;的形式进行开发
5. 

## 开发环境

- 机器：WSL(openSUSE)
- 

### 相关恶心的内容

相关代码的时候，耗费事件比较多的地方，有的地方很难，也有的地方很弱智，但是挺耗费事件的。

- 链接顺序：cmake中链接代码库的时候是有链接顺序的，一般来说

## 开发环境

本人是在WSL(openSUSE)上进行开发的，使用的是

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

protobuf版本：3.19.4，别弄高了，弄高了，属实没法链接上去

```shell
# 根据order.proto文件生成文件，输出在当前文件中
protoc --cpp_out=./ order.proto 
```
