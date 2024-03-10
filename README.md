# Tiny_Rpc

## 要完善的地方

>第一次面试，面试官提出要加入相关的测试，比如说多少并发量，这个要写一下ing...

自己的第一个项目，主要是学习了在Linux上使用C/C++开发相关的软件的流程。开发的是基于C++的多线程异步Rpc框架，底层书主从Reactor架构，底层是epoll实现的IO多路复用，应用层则是基于protobuf序列化的rpc自定义协议。

- 日志和配置文件的开发
  - 为什么使用`xml`进行文件配置开发，其实就是简单，你也可以使用`json`或者说`yml`进行配置文件，都可以，没啥区别
  - 调用的是库是`third_party/tinyxml`的文件，链接直接使用就行了，注意，这里不能使用指针指针进行包装，因为这里删除父亲的指针，子指针就会相关析构。
  - 配置文件放在`conf/rpc.xml`里面，具体配置可以参考xml的配置文件
  - 最终的格式文件是：`INFO 2023年9月3日16时21分37秒 文件名:/home/lzc/test_c++/main.cc`
  - 日志如何进行处理？
    - 这里是额外的开了一个线程去打印和处理文件，`每一个处理线程都有一个Logger类的对象，会将产生的Logger通过定时任务加入到一个AsyncLogger里面`，每一个处理线程产生的日志文件，在buffer有定的数量的时候，加入到AsyncLogger里面，然后统一输出到硬盘上，（这里是直接放到定时任务里面了）
  - 相关文件：
    - `src/include/common/log.h`：log日志
    - `src/include/common/config.h`: 读取xml配置文件
    - `src/include/common/utils.h`：其他工具函数
- 主从Reactor模块的相关配置
  - Reactor模块是项目中的重点，主线程是mainReactor，然后还有四个subReactor，主线程通过epoll监听可读事件，之后accept()获得对应的`clientfd`，然后将这个fd加入到四个subReactor中，然后进行处理相关的IO读写
    - 四个线程如何进行选举？
      - 从零开始，然后依次选举，然后再次从0开始
  - 相关文件：
    - 
- 定时器Timer的开发
  - 很多任务需要加入定时器的装置，所以实现了相关内容，
- 内容缓冲区的开发，也就是Buffer的相关使用，std::vector&lt;char&gt;的形式进行开发
-  

## 开发环境

- 环境：linux + clangd + cmake ，均为最新版本，
  - linux： WSL(openSUSE)
  - clangd: 最新版本，没有用的头文件可以直接变成灰色，非常好，可以直接去除掉
  - cmake: 最新版本
- protobuf: 因为链接版本的问题，选用的版本是3.19.6，具体问题可以参考[我这篇文章](https://zhuanlan.zhihu.com/p/679834990)
- fmt: 因为要升级到C++20才能使用std::format，所以这里使用了fmt进行代替
  - `git submodule update --init --recursive`

## 编译安装

```shell
git clone https://github.com/Meteor-Z/tiny_rpc.git
cd tiny_rpc 
mkdir build && cd build
cmake .. 
make -j20 # 开20个job进行编译，否则很慢
```

## 文档生成

```shell
mkdir document # 生成文档
doxygen Doxyfile # 使用doxygen进行生成
```

在`document`里面打开`index.html`即可。

## 生成文件

protobuf版本：3.19.4，高版本链接不上去

```shell
# 根据order.proto文件生成文件，输出在当前文件中
protoc --cpp_out=./ order.proto 
```

## 麻烦的地方

我在写这这个内容的时候，遇到了许多麻烦的事情，这些是我记录的，这些东西有的比较难，有的却很简单，但是都耗费了我很长时间，哎，因为一个小问题，结果浪费了巨长时间，很难受

1. 链接顺序：我以前以为cmake链接的时候，链接顺序随便链接的，但是其实链接的顺序也是有区别的，`越基础的库越应该放在前面`，比如说fmt等库，就应该放在前面，否贼链接的时候就会链接失败
   1. 改成方法：将fmt等库放在前面即可
2. 函数调用的死锁问题，`函数重复调用，导致死锁`
   1. 这个问题可以使用可重复性的锁，就是可以一直加锁的那个，但是我觉得当项目代码一多，那么这个锁稍不注意，就会出错
   2. 最终的解决方案是：干脆不写这个函数了，直接放在上面，虽然啰嗦一下，但是能运行
      1. 如果写了这个函数，调用了这个函数，可能会有一下错误，
         1. 当函数不小心调用的时候，可能会出现多线程错误
         2. 以后维护的时候，如果说这个地方应该加锁，一加，结果寄了，不好调试
         3. 还有一种方法是直接写在注释里面，但是感觉不是很清晰
3. protobuf链接失败，因为protobuf设置的时候，没有相关config，导致需要find_package进行链接
   1. 可以参考我写的[这篇文章](https://zhuanlan.zhihu.com/p/679834990)
4. 调试的时候，如果异常断开，这时候就会处于time_wait状态，导致连接不上
   1. 正常来说，不会出现这个情况，因为异常断开的话，是ctrl+c这样断开了，那么就会出现一场，如果是正常退出，不会出现这个异常，
   2. 但是如果真的要解决这个问题，好像是可以使用`setsockopt()`进行端口复用解决
5. 队列没有锁，导致多个线程访问buffer文件，因为`.top()`访问没有文件的信息的时候，就会出现UB的行为，这时候就会出错
   1. 很简单，直接加锁就行了
6. 