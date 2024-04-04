# Tiny_Rpc

## 要完善的地方

>第一次面试，面试官提出要加入相关的测试，比如说多少并发量，这个要写一下ing...
>protobuf返回的是东西大多都是指针，需要将指针改一下，封装成智能智能

自己的第一个项目，主要是学习了在Linux上使用C/C++开发相关的软件的流程。开发的是基于C++的多线程异步Rpc框架，底层书主从Reactor架构，底层是epoll实现的IO多路复用，应用层则是基于protobuf序列化的rpc自定义协议。

- 日志和配置文件的开发
  - 为什么使用`xml`进行文件配置开发，其实就是简单，你也可以使用`json`或者说`yml`进行配置文件，都可以，没啥区别
  - 调用的是库是`third_party/tinyxml`的文件，链接直接使用就行了，注意，这里不能使用指针指针进行包装，因为这里删除父亲的指针，子指针就会进行相关析构。
  - 配置文件放在`conf/rpc.xml`里面，具体配置可以参考xml的配置文件
  - 最终的格式文件是：`INFO 2023年9月3日16时21分37秒 文件名:/home/lzc/test_c++/main.cc:line`，相关行号
  - 日志如何进行处理？
    - 这里是额外的开了一个线程去打印和处理文件，`每一个处理线程都有一个Logger类的对象，会将产生的Logger通过定时任务加入到一个AsyncLogger里面`，AyncLogger中会单独开一个线程。每一个处理线程产生的日志文件，在buffer有定的数量的时候，统一输出到硬盘上，（这里是直接放到定时任务里面了）
  - 相关文件：
    - `src/include/common/log.h`：log日志
    - `src/include/common/log_config.h`: 读取xml配置文件
    - `src/include/common/utils.h`：其他工具函数
- 事件的封装
  - linux中将事件统一抽象成了文件描述符，就是listendfd，这里将这些文件描述符给封装起来。
  - 如果进行操作，其实就是维护了一个`multimap`，这个mutimap会对定时器的事件从小到大进行排序，然后动态维护这个`multimap`，就可以了
  - 相关文件
    - `src/include/common/fd_event.h`：对文件描述符进行封装
    - `src/include/common/net/wakeup_fd_event.h`：继承于上方的文件，对唤醒文件的进一步封装（也就是加入一个读入事件，表示唤醒
- 定时器的封装
  - 网络框架中需要相关的定时器，比如说超时的问题，如果超过这个时间没有得到响应的结果，那么就会报一个超时的任务
    - 问题1：如果判断一个定时任务需要执行
    - 问题2：如何监听这个事件，在指定的事件返回相对应的任务
  - 相关文件
    - `src/include/net/time/time_event.h`：定时器中要执行的任务
    - `src/include/net/time/timer.h`： 定时器，linux上的`timerfd_create`创建出来的`timerfd_create`得到的文件描述符
- 主从Reactor模块的相关配置
  - Reactor模块是项目中的重点，主线程是mainReactor，然后还有四个subReactor，主线程通过epoll监听可读事件，之后accept()获得对应的`clientfd`，然后将这个fd加入到四个subReactor中，然后进行处理相关的IO读写
    - 四个线程如何进行选举？
      - 从零开始，然后依次选举，然后再次从0开始(没有做相关的复杂均衡。。。)
  - 相关文件：
    - `src/include/time/eventloop.h`: EventLoop最主要的模块
    - `src/include/net/io_thread/io_thread.h`：对上方的eventloop再次封装一下
    - `src/include/net/io_thread/io_thread_group.h`：线程组，里面是EventLoop循环，对io_thread的封装
- 内容缓冲区的开发，
  - Buffer的相关开发
  - 内容不可能一字节一字节进行的传输，要开发一个buffer进行传输
  - 注意，这里
  - 相关文件
    - `srcsrc/include/net/tcp/tcp_buffer.h`: tcp_buffer,
- 协议开发
  - 这里要自己设置一个protobuf的协议，protobuf只是序列化的，所以要自定义协议
    - `src/include/net/coder/abstract_coder.h`：virtual基类，编解码器的基类
    - `src/include/net/coder/abstract_protocol.h`：virtual基类，协议的基类
    - `src/include/net/coder/protobuf_coder.h`：继承上方的类，是一个编解码器
    - `src/include/net/coder/protobuf_protocol.h`：继承上方的类，实现了自定义的协议
  - 大致协议如下：
    - 开始符 - 整包长度 - MsigID长度 - MsgID - 方法名长度 - 方法名 - 错误码 - 错误信息长度 - 错误信息 - protobuf序列化数据 - 校验码 - 结束符
    - 数据信息放在了`protobuf_protocol.h`文件中
    - 开始符和结束符用特殊的码来标识，0x02 和 0x03
    - msgid 是 rpc请求的唯一标识符
    - 校验码必须要使用，因为tcp只是保证了数据的达到，但是并没有办法保证数据的正确性
- 客户端/服务端的开发
  - Acceptor的封装
    - `socket() -> bind() -> listen() -> accept()`的流程，封装一下，监听客户端的连接
  - 相关文件：
    - `src/include/net/tcp/ipv4_net_addr.h`：封装了IPv4地址，但是并没有对IPv6进行包装
    - `src/include/net/tcp/tcp_acceptor.h`：Acceptor, 注意，这里要设置成非阻塞的形式
    - `src/include/net/tcp/tcp_connection.h`：读取数据，解析处理，然后返回客户端
    - `src/include/net/tcp/tcp_client.h`：封装TcpClint的代码,其实就是发送消息，注意：返回错误，但是errno = EINPROGRESS，表示正在创建连接，这时候也应该加入到相关队列中

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
6. 重复引用调用导致空指针异常
   1. 这个问题是：在初始化日志的时候，如果说你在初始化日志的时候调用相关日志的方法，那么你就会导致空指针异常，导致寄了
      1. 问题解决差不多，gdb可以运行到错误的地方，然后`backtrace`打出调用栈，发现空指针异常了，（0x00）
      2. 使用万能的pritnf打法进行调试，然后查出问题，不要在这里进行打印，直接使用cout输出文件就行了
7. 当客户但发送连接的话，用户端接受，结果一直触发可写事件（日志一直刷新）
   1. 在结束的时候。。要将事件的可读可写事件取消掉。。。
8. 哎，创建日志的时候，记得一定要创建，排查了一上午。。。，FILE*好像并不会自动创建文件的...真逆天啊

## 线上部署的问题

这里面也有很大的学问

- 服务器绑定的ip地址是0.0.0.0，表示监听的所有的ip地址，如果绑定的是本机的ip地址，那么就会 `bind() error  errno = 99, errno = Cannot assign requested address`这个错误，具体原因不是很清楚。
- 还是老问题，如果说你在这个服务器上部署的话，那么就会遇到编译的问题，如果在这个服务器上运行，还是建议自己手动编译一遍，不要复制粘贴源文件，一开始我的就有相关的问题，尤其是设计到动态链接的时候，挺麻烦的。

## 其他

[grpc快速使用](https://grpc.io/docs/languages/cpp/quickstart)
