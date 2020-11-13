---
layout: post
title: JavaScript 二进制 (上篇)
date: 2020-11-04 15:32:24.000000000 +09:00
---

### 前情提要
> 在 JavaScript 处理二进制数据方面上常接触的只有 Node 读写文件的 Buffer 对象, 脑海中只知道 ES6 也有 ArrayBuffer 相关的二进制处理的对象, 具体两者之间有何关系及 ArrayBuffer 的相关知识, 进阶用法都算是知识盲区了, 所以打算这次好好梳理学习一下, 顺便写个博客记录。

```
// 扫盲之前 😂
type ArrayBuffer = Array<Buffer>
```


### ArrayBuffer
> 2011 年 2 月发布 ArrayBuffer 对象、TypedArray 视图和 DataView 视图是 JavaScript 操作二进制数据的接口。让 JavaScript 有了操作二进制的能力, 后面 ES6 将它们纳入了 ECMAScript 规格，并且增加了新的方法。它们都是以数组的语法处理二进制数据，所以统称为二进制数组。了解更多阅读 [ECMAScript 6 入门 ArrayBuffer](https://es6.ruanyifeng.com/#docs/arraybuffer#)。

#### 知识摘要
1. ArrayBuffer对象：代表内存之中的一段二进制数据，可以通过“视图”进行操作。“视图”部署了数组接口，这意味着，可以用数组的方法操作内存。

2. TypedArray视图：共包括 9 种类型的视图，比如Uint8Array（无符号 8 位整数）数组视图, Int16Array（16 位整数）数组视图, Float32Array（32 位浮点数）数组视图等等。

3. DataView视图：可以自定义复合格式的视图，比如第一个字节是 Uint8（无符号 8 位整数）、第二、三个字节是 Int16（16 位整数）、第四个字节开始是 Float32（32 位浮点数）等等，此外还可以自定义字节序。

#### 使用的例子

```
// TypedArray 视图
const buffer = new ArrayBuffer(12);
const x1 = new Int32Array(buffer);
x1[0] = 1;

// DataView 视图
const buf = new ArrayBuffer(32);
const dataView = new DataView(buf);
dataView.getUint8(0) // 0
```

#### 小结
> DataView 自定义复合格式的视图的接口更像 Buffer, 或许 Buffer 是基于此实现, 如下可以看看接口对比

```
// 阅读完基础知识后 🤔
DataView == Buffer ?

// DataView 接口列表
- getInt8：读取 1 个字节，返回一个 8 位整数。
- getUint8：读取 1 个字节，返回一个无符号的 8 位整数。
- getInt16：读取 2 个字节，返回一个 16 位整数。
- getUint16：读取 2 个字节，返回一个无符号的 16 位整数。
- getInt32：读取 4 个字节，返回一个 32 位整数。
- getUint32：读取 4 个字节，返回一个无符号的 32 位整数。
- getFloat32：读取 4 个字节，返回一个 32 位浮点数。
- getFloat64：读取 8 个字节，返回一个 64 位浮点数。

- setInt8：写入 1 个字节的 8 位整数。
- setUint8：写入 1 个字节的 8 位无符号整数。
- setInt16：写入 2 个字节的 16 位整数。

// Buffer 接口列表
- buf.readDoubleBE([offset])
- buf.readDoubleLE([offset])
- buf.readFloatBE([offset])
- buf.readFloatLE([offset])
- buf.readInt8([offset])
- buf.readInt16BE([offset])
- buf.readInt16LE([offset])
- buf.readInt32BE([offset])
- buf.readInt32LE([offset])
- buf.readIntBE(offset, byteLength)
- buf.readIntLE(offset, byteLength)
- buf.readUInt8([offset])
- ...

- buf.writeDoubleBE(value[, offset])
- buf.writeDoubleLE(value[, offset])
- buf.writeFloatBE(value[, offset])
- ...
```


###  Buffer
#### 代码实现
> 那么我们可以瞧瞧 Buffer 的代码实现了, Buffer 对象由 Buffer.form 构造而成，注意通过 new 关键字构造用法已经废弃, 以下代码基于 [nodejs/node](https://github.com/nodejs/node), version: 564fcb8

```
// lib/buffer.js#290

Buffer.from = function from(value, encodingOrOffset, length) {
  if (typeof value === 'string')
    return fromString(value, encodingOrOffset);

  if (typeof value === 'object' && value !== null) {
    if (isAnyArrayBuffer(value))
      return fromArrayBuffer(value, encodingOrOffset, length);
   
   ...   
}
```

> 简化下来发现工厂模式 Buffer.from 方法最终都是返回了一个 FastBuffer 实例

```
Buffer.from(str[, encoding]) -> fromString -> new FastBuffer()
Buffer.from(arrayBuffer[, byteOffset[, length]]) -> fromArrayBuffer -> new FastBuffer()
```

```
// 真相大白后 😱
👇 Buffer 尽然是 Uint8Array（无符号 8 位整数）数组视图 👇
```

```
// lib/internal/buffer.js#952
class FastBuffer extends Uint8Array {}
```

> 那么 Uint8Array 的 Buffer 是如何实现读写其他 Int16， Uint16, Float32, Float64 接口 ? 去瞧瞧 
buf.writeUInt16BE 的实现吧

```
// lib/internal/buffer.js#828
function writeU_Int16BE(buf, value, offset, min, max) {
  value = +value;
  checkInt(value, min, max, buf, offset, 1);

  buf[offset++] = (value >>> 8); // 按位操作符
  buf[offset++] = value;
  return offset;
}

function writeUInt16BE(value, offset = 0) {
  return writeU_Int16BE(this, value, offset, 0, 0xffff);
}
```

> 这里先简单回顾一下 [JavaScript 按位操作符](https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Operators/Bitwise_Operators), 如上 value >>> 8 是向右移了 8位, 我们可以看看下面数字9(十进制)向右移到两位的例子

```
     9 (base 10): 00000000000000000000000000001001 (base 2)
                  --------------------------------
9 >>> 2 (base 10): 00000000000000000000000000000010 (base 2) = 2 (base 10)
```

> 回到 writeU_Int16BE，原来是写 16 位时, 会通过占用 2 个字节的形式各存储 8 位, 举个例子, 如 16 位的 1000000000000000(32768), 如上的代码第一个字节存储的是向右移了 8 位的 10000000(128), 第二个字节直接赋值给 1000000000000000, 第二个字节超出了 8 位怎么办 ? 超出会截掉头部 ?

```
const b = new ArrayBuffer(2)

const v = new Uint8Array(b)
undefined
v[0] = 128
128
v[1] = 32768
32768
v
Uint8Array(2) [128, 0]
```

> 如上的代码运算也验证了猜想, 第二个字节头部超出的 8 位被截掉了。 原来 Buffer 实现的其他位数操作接口都是被修饰抹平后的。

#### 字节序
> 仔细发现 Buffer 比 DataView 多了一些接口, 比如有 ***字节序*** 的概念, 如 buf.writeInt32BE, buf.writeInt32LE, DataView 只有 setInt32

> 计算机硬件有两种储存数据的方式：大端字节序（big endian）和小端字节序（little endian）。
举例来说，数值0x2211使用两个字节储存：高位字节是0x22，低位字节是0x11。

```
大端字节序：高位字节在前，低位字节在后，这是人类读写数值的方法。
小端字节序：低位字节在前，高位字节在后，即以0x1122形式储存。
```

> 我的理解是用高字节写就用高字节读, 低字节写就用高低字节读就行, 不然存储的 AB, 读出来的是 BA 。了解更多阅读 [理解字节序](http://www.ruanyifeng.com/blog/2016/11/byte-order.html)。

### 使用场景
> 在浏览器端操作二进制会比较少见, 这不 Webpack v5 发布不久, 这里还是讲讲 Node 上的实践, 其中 Webpack v5 的 ***本地持久缓存*** 功能里二进制相关的操作是可以好好学一下的。

> 如何通俗的解释这个功能点了? 大家知道首次编译后, 修改代码后再次的编译耗时是非常短的, 这是 MemoryCachePlugin 的功能, 它能在编译后把需要都数据都缓存在内存中, 接下来的编译当文件的时间戳没有变化都是直接返回上一次的运行结果。在一个运行时里面往内存读取数据还是比较简单的, 内存缓存相关的这一点就细不说了。

> 那么如何理解 IdleFileCachePlugin 实现的持久缓存功能? 简单说就是把本次的编译的数据写入文件中, 当本次编译结束进程退出, 后面重新 npm start 的时候在会从缓存文件中把各个模块上次编译结果读取出来以此来减少大量的编译时间。

> 大量的文件操作当然是直接操作二进制比较快, 对于 JavaScript 的 5 种基本数据类型 Undefined、Null、Boolean、Number、String 以及复杂的数据类型 Object 的序列化与反序列化, 代码压缩等 Webpack 做的各种策略还是能好好讲解一下的。

> 这部分的代码讲解就放在下一篇文章吧 ... 

![下次见～](https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1605297615876&di=a8600be2381d57666a014d9b63692976&imgtype=0&src=http%3A%2F%2Fpic4.zhimg.com%2F50%2Fv2-48f30f9964e3207b7c45aa54428e803b_hd.jpg)

> 关于本篇文章作者理解有误或其他讨论 [Issues](https://github.com/xiaoxiaojx/blog/issues) 见 ~


