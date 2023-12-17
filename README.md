# r0层通过ZwSetSystemInformation加载驱动

> 测试环境: windows 10 ltsc

## SLoader

内核层源码

## DriverLoadingEx

应用层源码

## 如何使用

1.加载SLoader.sys

2.运行 `DriverLoadingEx.exe C:/Demo.sys` 加载 Demo.sys

3.使用 procexp 查看结果