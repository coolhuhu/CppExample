# UDP Network Simulator - Claude Code Guide

## 项目概述
这是一个用C++17实现的UDP网络代理程序，用于模拟网络通信中的异常情况，包括丢包、延迟、抖动、乱序等。

## 核心文件结构
- `network_simulator.h/cpp` - 核心网络模拟器类
- `config_manager.h/cpp` - 配置管理系统
- `main.cpp` - 主程序入口
- `CMakeLists.txt` - 构建配置

## 关键特性
- **丢包模拟**: 可配置0-100%的丢包率
- **延迟模拟**: 基础延迟 + 随机抖动
- **乱序模拟**: 数据包乱序发送
- **实时统计**: 收发包统计、延迟统计
- **灵活配置**: 命令行参数和配置文件支持
- **高性能**: 基于Asio异步IO模型

## 构建命令
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 使用示例
```bash
# 基本使用
./bin/udp_simulator --listen-port 8080 --target-port 8081

# 模拟5%丢包
./bin/udp_simulator --packet-loss 5

# 模拟延迟和抖动
./bin/udp_simulator --delay-rate 20 --base-delay 50 --max-jitter 100

# 使用配置文件
./bin/udp_simulator --config config.txt
```

## 架构设计要点
1. **NetworkSimulator**: 主类，处理UDP代理和网络异常模拟
2. **异步IO**: 使用Asio的异步socket操作
3. **多线程**: IO线程 + 数据包处理线程
4. **线程安全**: 使用mutex和atomic保证线程安全
5. **配置系统**: 支持命令行和配置文件

## 网络异常模拟算法
- **丢包**: 随机数生成器根据概率丢弃
- **延迟**: 数据包放入延迟队列，定时发送
- **抖动**: 基础延迟 + 随机抖动时间
- **乱序**: 随机延迟某些数据包

## 依赖要求
- C++17
- CMake 3.10+
- Asio (通过FetchContent自动下载)

## 扩展性考虑
- 模块化设计，易于添加新的网络异常类型
- 配置系统灵活，支持多种配置方式
- 统计和日志系统可插拔