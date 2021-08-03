## 基础教程

可能大部分同学已经忘记了 C 语言相关的语法及知识，建议先阅读一下基础的语法与概念

- [C 语言教程 | 菜鸟教程](https://www.runoob.com/cprogramming/c-tutorial.html)
- [C++ 教程 | 菜鸟教程](https://www.runoob.com/cplusplus/cpp-tutorial.html)

## 从 Node.js 中学习 C, C++

主要记录一下在 Node 中出现的一些 C 语言知识，一般是没有出现在基础教程中，为学习时搜索出来记录的笔记，也便于后面快速查阅

### __POSIX__, NODE_SHARED_MODE
node 中经常会出现一些宏定义，其值为构建时决定
```
// src/node_main.cc

#if defined(__POSIX__) && defined(NODE_SHARED_MODE)
```

#### __POSIX__
如 __POSIX__ 在 node.gypi 的配置文件 node.gypi, conditions 字段决定, 可认为下面简化为一个 if (OS=="win") else { 'defines': [ '__POSIX__' ] } 语句, 所以非 win 系统 defined(__POSIX__) 都为 true
> 可移植操作系统接口（英语：Portable Operating System Interface，缩写为POSIX）是IEEE为要在各种UNIX操作系统上运行软件，而定义API的一系列互相关联的标准的总称，其正式称呼为IEEE Std 1003，而国际标准名称为ISO/IEC 9945, 查看更多 [可移植操作系统接口](https://zh.wikipedia.org/wiki/%E5%8F%AF%E7%A7%BB%E6%A4%8D%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F%E6%8E%A5%E5%8F%A3)
```
// node.gypi

'conditions': [
	[ 'OS=="win"', {
      'defines!': [
        'NODE_PLATFORM="win"',
      ],
      'defines': [
        'FD_SETSIZE=1024',
        # we need to use node's preferred "win32" rather than gyp's preferred "win"
        'NODE_PLATFORM="win32"',
        # Stop <windows.h> from defining macros that conflict with
        # std::min() and std::max().  We don't use <windows.h> (much)
        # but we still inherit it from uv.h.
        'NOMINMAX',
        '_UNICODE=1',
      ],
      'msvs_precompiled_header': 'tools/msvs/pch/node_pch.h',
      'msvs_precompiled_source': 'tools/msvs/pch/node_pch.cc',
      'sources': [
        '<(_msvs_precompiled_header)',
        '<(_msvs_precompiled_source)',
      ],
    }, { # POSIX
      'defines': [ '__POSIX__' ],
    }]
]
```
#### NODE_SHARED_MODE
和 __POSIX__ 是类似的，在 node.gypi 中定义
```
// node.gypi

[ 'node_shared=="true"', {
  'defines': [
  	'NODE_SHARED_MODE',
  ],
}]
```
那么 node_shared 何时为 true 了，追溯到 configure.py 文件
```
// configure.py

o['variables']['node_shared'] = b(options.shared)

parser.add_option('--shared',
    action='store_true',
    dest='shared',
    help='compile shared library for embedding node in another project. ' +
         '(This mode is not officially supported for regular applications)')
```
所以说我们看见，如果构建 node 时运行的命令 ./configure --shared 带上 --shared 这个版本的 node NODE_SHARED_MODE 就会被定义, 这种情况一般出现你的 c++ 项目需要把 node 作为依赖的时候。

### ((void(*)(int))0)
void(*)(int)代表一个无返回值的且具有一个整型参数的函数指针类型（这里是一个空函数），整个语句表示将“0”强制类型转换为无返回值且具有一个整型参数的函数指针, 查看更多 [#define SIG_DFL ((void(*)(int))0)](https://www.cnblogs.com/liulipeng/p/3555395.html)
```
// signal.h

#define	SIG_DFL		(void (*)(int))0
```

### __attribute__((constructor))和__attribute__((destructor))
例子放在 [demo_NODE_C_CTOR.cpp](https://github.com/xiaoxiaojx/blog/blob/master/demo_NODE_C_CTOR.cpp)
* __attribute__ ((constructor))会使函数在main()函数之前被执行

* __attribute__ ((destructor))会使函数在main()退出后执行


### new 和不 new 的区别
> 查看更多 [C++类实例化的两种方式：new和不new的区别](https://zhuanlan.zhihu.com/p/62106872)
* new创建类对象需要指针接收，一处初始化，多处使用
* new创建类对象使用完需delete销毁
* new创建对象直接使用堆空间，而局部不用new定义类对象则使用栈空间
* new对象指针用途广泛，比如作为函数返回值、函数参数等
* 频繁调用场合并不适合new，就像new申请和释放内存一样
```
A a;  // a存在栈上
A* a = new a();  // a存在堆中

CTest* pTest = new CTest();
delete pTest;
```
如 Node.js 中 . 的使用, 访问命名空间 per_process用::中某个不用 new 的实例的方法用.
```
// src/debug_utils.cc

namespace per_process {
	EnabledDebugList enabled_debug_list;
}

// src/node.cc

per_process::enabled_debug_list.Parse(nullptr);
```
如 Node.js 中 -> 的使用
```
// src/node_options.cc

namespace per_process {
	Mutex cli_options_mutex;
	std::shared_ptr<PerProcessOptions> cli_options{new PerProcessOptions()};
}

// src/node.cc

per_process::cli_options->v8_thread_pool_size)
```


### 点操作符和箭头操作符
> 查看更多 [点操作符和箭头操作符](https://blog.csdn.net/fulima007/article/details/6327067)
* 箭头（->）：左边必须为指针，如 new 实例化的类；
* 点号（.）：左边必须为实体，如结构体，不用 new 的实例。



### sigaction, memset
> 每个线程都有自己独立的signal mask，但所有线程共享进程的signal action。这意味着，你可以在线程中调用pthread_sigmask(不是sigmask)来决定本线程阻塞哪些信号。但你不能调用sigaction来指定单个线程的信号处理方式。如果在某个线程中调用了sigaction处理某个信号，那么这个进程中的未阻塞这个信号的线程在收到这个信号都会按同一种方式处理这个信号。另外，注意子线程的mask是会从主线程继承而来的, 查看更多 [pthread_sigmask sigaction](https://blog.csdn.net/luzubodfgs/article/details/89252482)
```
// src/node_main.cc

// 定义一个结构体 act
struct sigaction act;
// memset: 可以方便的清空一个结构类型的变量或数组, 指针的为NULL, 其他为0
// memset: https://blog.csdn.net/faihung/article/details/90707367
memset(&act, 0, sizeof(act));
// 设置新的信号处理函数
act.sa_handler = SIG_IGN;
// tips: 结构体和函数是可以同名的
// sigaction: 函数的功能是检查或修改与指定信号相关联的处理动作
sigaction(SIGPIPE, &act, nullptr);
```

### getauxval
> getauxval() 函数从辅助函数中检索值 向量，内核的 ELF 二进制加载器使用的一种机制 当程序运行时将某些信息传递给用户空间 执行

个人理解是从内核中获取当前程序的一些基础的信息, 比如传参为 AT_BASE 时是获取程序解释器的基地址, 查看更多 [getauxval(3) — Linux manual page](https://man7.org/linux/man-pages/man3/getauxval.3.html)

```
// src/node_main.cc

#if defined(__linux__)
  // 辅助向量(auxiliary vector)，一个从内核到用户空间的信息交流机制，它一直保持透明。然而，在GNU C库(glibc)2.16发布版中添加了一个新的库函数”getauxval()”
  // http://www.voidcn.com/article/p-flcjdfbd-bu.html https://man7.org/linux/man-pages/man3/getauxval.3.html
  
  node::per_process::linux_at_secure = getauxval(AT_SECURE);
```

### setvbuf
setvbuf: C 库函数 int setvbuf(FILE *stream, char *buffer, int mode, size_t size) 定义流 stream 应如何缓冲。

_IONBF: 无缓冲：不使用缓冲。每个 I/O 操作都被即时写入。buffer 和 size 参数被忽略。

nullptr: C++中有个nullptr的关键字可以用作空指针，既然已经有了定义为0的NULL，为何还要nullptr呢？这是因为定义为0的NULL很容易引起混淆，尤其是函数重载调用时, 查看更多 [C/C++中的NULL与nullptr
](https://blog.csdn.net/u012707739/article/details/77915483)
```
// src/node_main.cc

setvbuf(stdout, nullptr, _IONBF, 0);
setvbuf(stderr, nullptr, _IONBF, 0);
```


待补充 ...
