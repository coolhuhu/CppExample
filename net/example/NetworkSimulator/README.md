# UDP Network Simulator

一个用C++实现的UDP网络代理程序，用于模拟网络通信中的异常情况，包括丢包、延迟、抖动、乱序等。

## 功能特性

- **丢包模拟**: 可配置的丢包率
- **延迟模拟**: 可配置的延迟率和基础延迟
- **抖动模拟**: 可配置的抖动率和最大抖动时间
- **乱序模拟**: 可配置的乱序率
- **实时统计**: 显示收发包统计信息
- **详细日志**: 可选的详细日志记录
- **配置管理**: 支持命令行参数和配置文件
- **高性能**: 基于Asio异步IO模型

## 编译

```bash
mkdir build
cd build
cmake ..
make
```

## 使用方法

### 命令行参数

```bash
./udp_simulator [options]
```

选项：
- `-h, --help`: 显示帮助信息
- `-c, --config <file>`: 从配置文件加载设置
- `--listen-host <host>`: 监听地址 (默认: 0.0.0.0)
- `--listen-port <port>`: 监听端口 (默认: 8080)
- `--target-host <host>`: 目标地址 (默认: 127.0.0.1)
- `--target-port <port>`: 目标端口 (默认: 8081)
- `--packet-loss <rate>`: 丢包率 (0-100%)
- `--delay-rate <rate>`: 延迟率 (0-100%)
- `--jitter-rate <rate>`: 抖动率 (0-100%)
- `--reorder-rate <rate>`: 乱序率 (0-100%)
- `--base-delay <ms>`: 基础延迟 (毫秒)
- `--max-jitter <ms>`: 最大抖动 (毫秒)
- `--no-log`: 禁用日志
- `--no-stats`: 禁用统计

### 示例

1. **基本使用**:
```bash
./udp_simulator --listen-port 8080 --target-port 8081
```

2. **模拟5%丢包**:
```bash
./udp_simulator --packet-loss 5
```

3. **模拟延迟和抖动**:
```bash
./udp_simulator --delay-rate 20 --base-delay 50 --max-jitter 100
```

4. **使用配置文件**:
```bash
./udp_simulator --config config.txt
```

### 配置文件格式

```
# 注释以#开头
listen_host=0.0.0.0
listen_port=8080
target_host=127.0.0.1
target_port=8081
packet_loss_rate=5%
delay_rate=10%
jitter_rate=15%
reorder_rate=5%
base_delay=50ms
max_jitter=100ms
enable_logging=true
enable_statistics=true
```

## 架构设计

### 核心类

1. **NetworkSimulator**: 主类，负责UDP代理和网络异常模拟
2. **NetworkConfig**: 配置结构体，定义网络模拟参数
3. **NetworkStats**: 统计结构体，记录网络传输统计信息
4. **ConfigManager**: 配置管理器，处理命令行参数和配置文件

### 工作流程

1. 监听指定端口接收UDP数据包
2. 根据配置的概率决定是否丢弃数据包
3. 根据配置的概率决定是否延迟数据包
4. 根据配置的概率决定是否乱序发送数据包
5. 转发数据包到目标地址
6. 记录统计信息和日志

### 异常模拟算法

- **丢包**: 使用随机数生成器，根据配置的概率丢弃数据包
- **延迟**: 将数据包放入延迟队列，在指定时间后发送
- **抖动**: 在基础延迟上添加随机抖动时间
- **乱序**: 随机延迟某些数据包的发送时间

## 性能优化

- 使用异步IO模型，避免阻塞
- 多线程处理数据包
- 高效的数据结构管理延迟队列
- 原子操作保证统计数据的线程安全

## 扩展性

- 模块化设计，易于添加新的网络异常类型
- 配置系统灵活，支持多种配置方式
- 可插拔的日志和统计系统

## 依赖

- C++17 或更高版本
- CMake 3.10 或更高版本
- Asio 网络库 (通过FetchContent自动下载)