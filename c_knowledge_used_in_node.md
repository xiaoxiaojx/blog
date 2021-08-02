## 基础教程

可能大部分同学已经忘记了 C 语言相关的语法及知识，建议先阅读一下基础的语法与概念

- [C 语言教程 | 菜鸟教程](https://www.runoob.com/cprogramming/c-tutorial.html)
- [C++ 教程 | 菜鸟教程](https://www.runoob.com/cplusplus/cpp-tutorial.html)

## Node 用到的 C 知识汇总

主要记录一下在 Node 中出现的一些 C 语言知识，一般是没有出现在基础教程中，是学习时搜索出来做的笔记，也便于后面查阅

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
