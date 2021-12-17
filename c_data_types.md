## 基础数据类型
<table class="wikitable">
<tbody><tr>
<th>关键字</th>
<th><a href="/wiki/%E5%AD%97%E8%8A%82" title="字节">字节（字节）</a></th>
<th>范围</th>
<th>格式化字符串</th>
<th>硬件层面的类型</th>
<th>备注
</th></tr>
<tr>
<td><code>char</code></td>
<td>1bytes</td>
<td>通常为-128至127或0至255，与体系结构相关</td>
<td>%c
</td>
<td>字节(Byte)
</td>
<td>大多数情况下即<code>signed char；</code>
<p>在极少数1byte&nbsp;!= 8bit或不使用ASCII字符集的机器类型上范围可能会更大或更小。其它类型同理。
</p>
</td></tr>
<tr>
<td><code>unsigned char</code></td>
<td>1bytes</td>
<td>通常为0至255</td>
<td>%c、%hhu
</td>
<td>字节
</td>
<td>
</td></tr>
<tr>
<td><code>signed char</code></td>
<td>1bytes</td>
<td>通常为-128至127</td>
<td>%c、%hhd、%hhi
</td>
<td>字节
</td>
<td>
</td></tr>
<tr>
<td><code>int</code></td>
<td>2bytes(16位系统) 或<br>4bytes
</td>
<td>-32768至32767或<br>-2147483648至2147483647</td>
<td>%i、%d
</td>
<td>字(Word)或双字(Double Word)
</td>
<td>即<code>signed int</code>(但用于bit-field时，int可能被视为signed int，也可能被视为unsigned int)
</td></tr>
<tr>
<td><code>unsigned int</code></td>
<td>2bytes 或<br>4bytes</td>
<td>0至65535 或<br>0至4294967295</td>
<td>%u
</td>
<td>字或双字
</td>
<td>
</td></tr>
<tr>
<td><code>signed int</code></td>
<td>2bytes 或<br>4bytes
</td>
<td>-32768至32767 或<br>-2147483648至2147483647</td>
<td>%i、%d
</td>
<td>字或双字
</td>
<td>
</td></tr>
<tr>
<td><code>short int</code></td>
<td>2bytes</td>
<td>-32768至32767</td>
<td>%hi、%hd
</td>
<td>字
</td>
<td>即<code>signed short</code>
</td></tr>
<tr>
<td><code>unsigned short</code></td>
<td>2bytes</td>
<td>0至65535</td>
<td>%hu
</td>
<td>字
</td>
<td>
</td></tr>
<tr>
<td><code>signed short</code></td>
<td>2bytes</td>
<td>-32768至32767</td>
<td>%hi、%hd
</td>
<td>字
</td>
<td>
</td></tr>
<tr>
<td><code>long int</code></td>
<td>4bytes 或<br>8bytes<sup id="cite_ref-1" class="reference"><a href="#cite_note-1">[1]</a></sup></td>
<td>-2147483648至2147483647 或<br>-9223372036854775808至9223372036854775807</td>
<td>%li、%ld
</td>
<td>长整数(Long Integer)
</td>
<td>即<code>signed long</code>
</td></tr>
<tr>
<td><code>unsigned long</code></td>
<td>4bytes 或<br>8bytes</td>
<td>0至4294967295 或<br>0至18446744073709551615</td>
<td>%lu
</td>
<td>整数(Unsigned Integer)或
<p>长整数(Unsigned Long Integer)
</p>
</td>
<td>依赖于实现
</td></tr>
<tr>
<td><code>signed long</code></td>
<td>4bytes或<br>8bytes</td>
<td>-2147483648至2147483647 或<br>-9223372036854775808至9223372036854775807</td>
<td>%li、%ld
</td>
<td>整数(Signed Integer)或
<p>长整数(Signed Long Integer)
</p>
</td>
<td>依赖于实现
</td></tr>
<tr>
<td><code>long long</code></td>
<td>8bytes</td>
<td>-9223372036854775808至9223372036854775807</td>
<td>%lli、%lld
</td>
<td>长整数(Long Integer)
</td>
<td>
</td></tr>
<tr>
<td><code>unsigned long long</code></td>
<td>8bytes</td>
<td>0至18446744073709551615</td>
<td>%llu
</td>
<td>长整数(Unsigned Long Integer)
</td>
<td>
</td></tr>
<tr>
<td><code>float</code></td>
<td>4bytes</td>
<td>2.939x10<sup>−38</sup>至3.403x10<sup>+38</sup> (7 sf)</td>
<td>%f、%e、%g
</td>
<td>浮点数(Float)
</td>
<td>
</td></tr>
<tr>
<td><code>double</code></td>
<td>8bytes</td>
<td>5.563x10<sup>−309</sup>至1.798x10<sup>+308</sup> (15 sf)</td>
<td>%lf、%e、%g
</td>
<td>双精度浮点型(Double Float)
</td>
<td>
</td></tr>
<tr>
<td><code>long double</code></td>
<td>10bytes或<br>16bytes</td>
<td>7.065x10<sup>-9865</sup>至1.415x10<sup>9864</sup> (18 sf或33 sf)</td>
<td>%lf、%le、%lg
</td>
<td>双精度浮点型(Double Float)
</td>
<td>在大多数平台上的实现与<code>double</code>相同，实现由编译器定义。
</td></tr>
<tr>
<td><code>_Bool</code>
</td>
<td>1byte
</td>
<td>0或1
</td>
<td>%i、%d
</td>
<td>布尔型(Boolean)
</td>
<td>
</td></tr></tbody></table>

## char 类型
char 类型用于存储字符（如，字母或标点），但从技术层面看，char是整数类型。因为char类型实际上存储的是整数而不是字符。计算机使用数字编码来处理字符，即用特定的整数表示特定的字符。美国最常用的编码是ASCLL编码。例如，在ASCLL编码中，整数65代表大写字母A。因此，存储字母A实际存储的是整数65。

标准ASCLL码的范围是0～127，只需7位二进制数即可表示。通常，char类型被定义为8位的存储单元，因此容纳标准ASCII码绰绰有余。许多其他系统（如IMB PC和苹果macOS）还提供ASCLL码，也是在8位的表示范围之内。一般而言，C语言会保证char类型足够大，以存储系统（实现C语言的系统）的基本字符集。

## 可移植类型
C语言提供了许多有用的整数类型。但是，某些类型名在不同系统中的功能不一样。C99新增了两个头文件stdint.h和inttypes.h，以确保C语言的类型名在各系统中的功能相同。

C语言为现有类型创建了更多类型名。这些新的类型名定义在stdint.h头文件中。例如，int32_t表示整型类型的宽度正好是32位。在使用32位int的系统中，头文件会把int32_t作为int的别名。不同的系统也可以定义定义相同的类型名。例如，int为16位、long为32位的系统会把int32_t作为long的别名。然后，使用int32_t类型编写程序，并包含stdint.h头文件时，编译器会把int或long替换成与当前系统匹配的类型。

## 延伸阅读
### 字符编码
计算机刚刚发明时，只支持ascii码，也就是说只支持英文，随着计算机在全球兴起，各国创建了属于自己的编码来显示本国文字，中文首先使用GB2132编码，它收录了6763个汉字，平日里我们工作学习大概只会用到3千个汉字，因此日常使用已经足够，GBK收录了21003个汉字，远超日常汉字使用需求，不管是日用还是商用都能轻松搞定，因此从windows95开始就将GBK作为默认汉字编码，而GB18030收录了27533个汉字，为汉字研究、古籍整理等领域提供了统一的信息平台基础，平时根本使用不到这种编码。这些中文编码本身兼容ascii，并采用变长方式记录，英文使用一个字节，常用汉字使用2个字节，罕见字使用四个字节。后来随着全球文化不断交流，人们迫切需要一种全球统一的编码能够统一世界各地字符，再也不会因为地域不同而出现乱码，这时Unicode字符集就诞生了，也称为统一码，万国码。新出来的操作系统其内核本身就支持Unicode，由万国码的名称就可以想象这是一个多么庞大的字符集，为了兼容所有地区的文字，也考虑到空间和性能，Unicode提供了3种编码方案：
* utf-8 变长编码方案，使用1-6个字节来储存
* utf-32 定长编码方案，始终使用4个字节来储存
* utf-16 介于变长和定长之间的平衡方案，使用2个或4个字节来储存

utf-8由于是变长方案，类似GB2132和GBK量体裁衣，最节省空间，但要通过第一个字节决定采用几个字节储存，编码最复杂，且由于变长要定位文字，就得从第一个字符开始计算，性能最低。utf-32由于是定长方案，字节数固定因此无需解码，性能最高但最浪费空间。utf-16是个怪胎，它将常用的字放在编号0 ~ FFFF之间，不用进行编码转换，对于不常用字的都放在10000~10FFFF编号之后，因此自然的解决变长的问题。注意对于这3种编码，只有utf-8兼容ascii，utf-32和utf-16都不支持单字节，由于utf-8最省流量，兼容性好，后来解码性能也得到了很大改善，同时新出来的硬件也越来越强，性能已不成问题，因此很多纯文本、源代码、网页、配置文件等都采用utf-8编码，从而代替了原来简陋的ascii。再来看看utf-16，对于常见字2个字节已经完全够用，很少会用到4个字节，因此通常也将utf-16划分为定长，一些操作系统和代码编译器直接不支持4字节的utf-16。Unicode还分为大端和小端，大端就是将高位的字节放在低地址表示，后缀为BE；小端就是将高位的字节放在高地址表示，后缀为LE，没有指定后缀，即不知道其是大小端，所以其开始的两个字节表示该字节数组是大端还是小端，FE FF表示大端，FF FE表示小端。Windows内核使用utf-16，linux，mac，ios内核使用的是utf-8，我们就不去争论谁好谁坏了。另外虽然windows内核为utf-16，但为了更好的本地化，控制面板提供了区域选项，如果设置为简体就是GBK编码，在win10中，控制台和记事本默认编码为gbk，其它第三方软件就不好说了，它们默认编码各不相同。

## 参考
* [数据类型 (C语言)](https://zh.wikipedia.org/wiki/%E6%95%B0%E6%8D%AE%E7%B1%BB%E5%9E%8B_(C%E8%AF%AD%E8%A8%80))
* [可移植类型：stdint.h和inttypes.h](https://blog.csdn.net/WarEric/article/details/76407920)
* [在C语言中使用中文，本地化全攻略](https://blog.csdn.net/jaj2003/article/details/109802900)
* [C语言处理中文字符](http://forum.becomingcelia.com/blog/c/2018-12-31-14)
