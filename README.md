# Network Tx/Rx speed Monitor for LU
## 概述

一个基于Ubuntu18.04平台开发的实时监测网卡收发速率应用，通过读取`/proc/net/dev`文件中对应网卡记录，并截取所需字段进行计算返回结果，原repo地址[在这](https://github.com/gesanqiu/Network-Bandwidth-Monitor)，相关的教程[CPacke生成deb安装包](https://ricardolu.gitbook.io/trantor/cmake-in-action/cpack-sheng-cheng-deb-an-zhuang-bao)。

- 开发平台：Ubuntu 18.04 aarch64
- 编译器：Ubuntu/Linaro 7.5.0-3ubuntu1~18.04

## 文件目录

```shell
.
├── CMakeLists.txt
├── images
│   └── img.png
├── inc
│   └── netUsageMonitor.h
├── lib
│   └── libnetUsageMonitor.so # (ELF 64-bit LSB shared object, ARM aarch64)
├── LICENSE
├── package	# dpkg-deb -b ./ network_bandwidth_cal-1.0.1-Linux.deb
│   ├── DEBIAN
│   │   └── control
│   ├── network_bandwidth_cal-1.0.1-Linux.deb
│   └── usr
│       ├── include
│       │   └── netUsageMonitor.h
│       └── lib
│           └── libnetUsageMonitor.so
├── README.md
└── src
    └── netUsageMonitor.c
```

- `lib`目录下的`libnetUsageMonitor.so`是我在`ARM aarch64`平台下编译出的动态链接库，开发者应该在自己的开发平台上自行编译出所需的`.so`。

## 编译&打包

```shell
mkdir build
cd build
cmake ..
make
make package
```

![img](images/img.png)

## 安装

```shell
dpkg -i network_bandwidth_cal-1.0.1-Linux.deb
```

安装后，在`/usr/lib`和`/usr/include`会生成对应的动态链接库和头文件。

## 使用

```c
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "netUsageMonitor.h"

int main(int argc, char *argv[])
{

    char *net;
    time_t current_time;
    struct netUsage net_usage;

    net = argv[1];

    net_init(net);
    while(true) {
        sleep(3);
        current_time = time(NULL);
        net_update_usage(net, current_time, &net_usage);
        printf("\n[NET TX] LAST Transmit Total: %lld bytes [%f KB].\n", net_usage.net_previous_transmit_total, (double)(net_usage.net_previous_transmit_total / 1024));
        printf("\n[NET TX] NOW  Transmit Total: %lld bytes [%f KB].\n", net_usage.net_current_transmit_total, (double)(net_usage.net_current_transmit_total / 1024));
        printf("\n[NET TX] NOW Transmit Speed: %f bytes/s [%f KB/s].\n", net_usage.net_transmit_speed, (net_usage.net_transmit_speed / 1024));
        printf("\n[NET RX] LAST Receive Total: %lld bytes [%f KB].\n", net_usage.net_previous_receive_total, (double)(net_usage.net_previous_receive_total / 1024));
        printf("\n[NET RX] NOW  Receive Total: %lld bytes [%f KB].\n", net_usage.net_current_receive_total, (double)(net_usage.net_current_receive_total / 1024));
        printf("\n[NET RX] NOW Receive Speed: %f bytes/s [%f KB/s].\n", net_usage.net_receive_speed, (net_usage.net_receive_speed / 1024));
        printf("*********************************************************************************************************\n");
    }
    return 0;
}

```

