# SoftwareLisenceManagementToolkit

本项目用于为软件许可管理程序提供基础的工具，包括Lisence证书的生成函数和验证函数。

## 总体设计

1. 机器指纹信息以及时间信息的获取模块(PCInfo)
   1. cpuInfo函数：获取CPU的序列号，并以字符串返回
   2. macAddr函数：获取MAC地址，并且以字符串返回
2. AES算法的加密和解密模块(AESEncryption)
3. Lisence整数的生成和验证模块(lisencemanager)
   1. generateLisence函数：传入电脑的指纹信息，许可时间，当前时间，生成加密的文本作为Lisence
   2. validateLisence函数：输入Lisence字符串，调用解密算法，并且获取机器信息，进行比对和判断Lisence证书的有效性

## 主要函数设计

## generateLisence函数

输入机器的CPU序列号、MAC地址信息，以及许可时间，函数自动获取调用时的时间戳。根据以上的四项信息生成初始的Lisence文本。然后调用AES加密算法，将文本进行加密并且返回其字符串。

## validateLisence函数

输入加密的Lisence字符串，函数自动获取上述三项信息。逐个判断CPU序列号、MAC地址是否吻合，以及是否在许可的时间内。将判断的结果以枚举类型`validity`返回。

| 返回值 | 结果            |
| ------ | --------------- |
| 0      | 证书有效        |
| 10     | CPU序列号不符合 |
| 20     | MAC地址不符合   |
| 30     | 证书已过期      |

## 说明

已对Linux和Windows系统进行适配，并且具有32和64位平台的跨平台特性。

## 参考资料

### 1.Github

* [JQQRCode C++ Library](https://github.com/188080501/JQQRCode)

* [zxing C++ Library](https://github.com/nu-book/zxing-cpp)(Apache Lisence Version 2.0)

### 2.CSDN

* [Windows/Linux获取Mac地址和CPU序列号实现](https://blog.csdn.net/fengbingchun/article/details/108874436)

* [C++如何运行Windows cmd命令并获取返回值(有效)]
