# SoftwareLisenceManagementToolKit
generate and validate software lisence, generate QRcode

## 模块设计
1. 二维码信息生成和读取模块(qrcode)
   1. 输入文本，生成二维码，不需要加密
   2. 输入二维码图片地址（？）参考，输出文本

2. RSA的加密和解密模块(lisencemanager)
   1. 传入电脑的SID信息，以及使用时间，以及开始时间，生成加密的文本作为Lisence，**机器指纹信息**
   2. 读取Lisence，并且和当前的电脑的SID信息和时间做匹配，返回是否允许。

3. PC信息读取模块
读取机器指纹信息，并交给字符串
机器指纹信息：
CPU，硬盘

## 参考资料

