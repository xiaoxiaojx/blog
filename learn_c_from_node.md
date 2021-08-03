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
> 可移植操作系统接口（英语：Portable Operating System Interface，缩写为POSIX）是IEEE为要在各种UNIX操作系统上运行软件，而定义API的一系列互相关联的标准的总称，其正式称呼为IEEE Std 1003，而国际标准名称为ISO/IEC 9945。
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

### sigaction, memset
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

待补充 ...
