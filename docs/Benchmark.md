
Benchmark - Run On 2016-07-07
======
环境
------
+ 环境: CentOS 7.1, GCC 4.8.5
+ CPU: Xeon E3-1230 v2 3.30GHz*8 (sender和receiver都只用一个核心)
+ 内存: 24GB (这是总内存，具体使用数根据配置不同而不同)
+ 网络: 千兆网卡 * 1
+ 编译选项: -O2 -g -DNDEBUG -ggdb -Wall -Werror -Wno-unused-local-typedefs -std=gnu++11 -D_POSIX_MT_
+ 配置选项: -DATBUS_MACRO_BUSID_TYPE=uint64_t -DATBUS_MACRO_CONNECTION_BACKLOG=128 -DATBUS_MACRO_CONNECTION_CONFIRM_TIMEOUT=30 -DATBUS_MACRO_DATA_ALIGN_TYPE=uint64_t -DATBUS_MACRO_DATA_NODE_SIZE=128 -DATBUS_MACRO_DATA_SMALL_SIZE=3072 -DATBUS_MACRO_HUGETLB_SIZE=4194304 -DATBUS_MACRO_MSG_LIMIT=65536


测试项                                   |      连接数     |        包长度        |      CPU消耗     |    内存消耗   |    吞吐量     |      QPS
----------------------------------------|----------------|---------------------|-----------------|--------------|--------------|---------------
Linux+本地回环+ipv6+静态缓冲区             |         1      |      8-16384字节     |     90%/100%    |   5.8MB/24MB |    601MB/s   |      95K/s
Linux+本地回环+ipv6+静态缓冲区             |         1      | 8-128字节(模拟ping包) |     48%/100%    |   5.8MB/27MB |    163MB/s   |    2822K/s
Linux+本地回环+ipv6+动态缓冲区(ptmalloc)   |         1      |      8-16384字节     |     90%/100%    |   5.8MB/24MB |    607MB/s   |      96K/s
Linux+本地回环+ipv6+动态缓冲区(ptmalloc)   |         1      | 8-128字节(模拟ping包) |     48%/100%    |   5.8MB/27MB |    165MB/s   |    2857K/s
Linux+共享内存                           |         1      |      8-16384字节     |      98%/98%    |   74MB/74MB  |    1.56GB/s  |     199K/s
Linux+共享内存                           |         1      | 8-128字节(模拟ping包) |     100%/83%    |   74MB/74MB  |    303MB/s   |    5253K/s


压力测试说明
------

1. 动态缓冲区指发送者发送缓冲区使用malloc和free来保存发送中的数据,静态缓冲区指使用内置的内存池来缓存。
2. 由于发送者的CPU消耗会高于接收者（接收者没有任何逻辑，发送者会有一次随机数操作），所以动态缓冲区时其实内存会持续增加。（内存碎片原因，实际测试过程中3分钟由20MB涨到28MB）
3. Windows下除了不支持Unix Socket外，共享内存和ipv4/ipv6连接都是支持的，但是没有跑压力测试。

*PS: 目前没写多连接测试工具，后面有空再写吧*

测试命令如下（需要把tools/script里的脚本拷贝到[构建目录]/tools内）
------

```bash
# Linux+本地回环+ipv6+静态缓冲区:8-16384字节 测试命令
./startup_benchmark_io_stream.sh ipv6://::1:16389 2048 4194304 8096

# Linux+本地回环+ipv6+静态缓冲区:8-128字节(模拟ping包) 测试命令
./startup_benchmark_io_stream.sh ipv6://::1:16389 16 4194304 8096

# Linux+本地回环+动态缓冲区(ptmalloc):8-16384字节 测试命令
./startup_benchmark_io_stream.sh ipv6://::1:16389 2048 4194304 0

# Linux+本地回环+动态缓冲区(ptmalloc):8-128字节(模拟ping包) 测试命令
./startup_benchmark_io_stream.sh ipv6://::1:16389 16 4194304 0

# Linux+共享内存:8-16384字节 测试命令
./startup_benchmark_shm.sh 12345679 2048

# Linux+共享内存:8-128字节(模拟ping包) 测试命令
./startup_benchmark_shm.sh 12345679 16
```


对比tsf4g性能测试报告 - Run On 2014-01-14
------
+ 环境: tlinux 1.0.7 (based on CentOS 6.2), GCC 4.8.2, gperftools 2.1(启用tcmalloc和cpu profile)
+ CPU: Xeon X3440 2.53GHz*8
+ 内存: 8GB (这是总内存，具体使用数根据配置不同而不同)
+ 网络: 千兆网卡 * 1
+ 编译选项: -O2 -g -DNDEBUG -ggdb -Wall -Werror -Wno-unused-local-typedefs -std=gnu++11 -D_POSIX_MT_
+ 配置选项: 无

测试项                                   |      连接数         |        包长度        |      CPU消耗     |    内存消耗   |    吞吐量     |      QPS
----------------------------------------|--------------------|---------------------|-----------------|--------------|--------------|---------------
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |        16KB         |    13%/100%     |     280MB    |   86.4MB/s   |    5.4K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |         8KB         |    13%/100%     |     280MB    |     96MB/s   |    12K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |         4KB         |    13%/100%     |     280MB    |     92MB/s   |    23K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |         2KB         |    15%/100%     |     280MB    |     88MB/s   |    44K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |         1KB         |    16%/100%     |     280MB    |     82MB/s   |    82K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |        512字节       |    22%/100%     |     280MB    |    79.5MB/s  |    159K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |        256字节       |    33%/100%     |     280MB    |    73.5MB/s  |    294K/s
Linux+跨机器转发+ipv4                     | 2(仅一个连接压力测试) |        128字节       |    50%/100%     |     280MB    |    65.75MB/s |    526K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |         32KB         |    100%/100%    |     280MB    |    3.06GB/s  |    98K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |         16KB         |    61%/71%      |     280MB    |    1.59GB/s  |    102K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |          8KB         |    36%/70%      |     280MB    |    1.27GB/s  |    163K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |          4KB         |    40%/73%      |     280MB    |    1.30MB/s  |    333K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |          2KB         |    43%/93%      |     280MB    |    1.08GB/s  |    556K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |          1KB         |    54%/100%     |     280MB    |    977MB/s   |    1000K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |         512字节       |    44%/100%     |     280MB    |    610MB/s   |    1250K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |         256字节       |    42%/100%     |     280MB    |    305MB/s   |    1250K/s
Linux+共享内存                           | 3(仅一个连接压力测试) |         128字节       |    42%/100%     |     280MB    |    174MB/s   |    1429K/s


对比结果分析:

1. atbus的吞吐量基本上高于tbus
2. [共享内存] QPS方面，atbus比tbus的的发送端性能高，接收端相近
