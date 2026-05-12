# flight-price-monitor-cpp 工程模块文档

## 1. 概述

### 1.1 模块定位
Flight Price Monitor 是一个基于 C++ 的航班价格监控系统，用于实时追踪航班价格变化并发送通知。

### 1.2 代码路径
```
/home/nio/git/ns/neo/AI/projects/ns/flight-price-monitor-cpp
```

### 1.3 依赖关系
| 依赖 | 类型 | 版本 | 说明 |
|------|------|------|------|
| Crow | Web框架 | v1.0+ | 轻量级 C++ HTTP 服务器 |
| SQLite3 | 数据库 | 3.x | 嵌入式数据库 |
| libcurl | HTTP客户端 | 7.x | 网络请求 |

### 1.4 功能特性
- ✅ 航班价格实时监控
- ✅ 价格变化通知（邮件/日志）
- ✅ RESTful API 接口
- ✅ SQLite 数据持久化

---

## 2. 架构设计

### 2.1 核心组件

```
┌─────────────────────────────────────────────────────────────┐
│                    Flight Price Monitor                     │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐    ┌───────────────┐    ┌───────────────┐ │
│  │   HTTP API  │    │ Flight Service│    │  Notification │ │
│  │  (Crow)     │───▶│   (核心逻辑)   │───▶│   Service     │ │
│  └─────────────┘    └───────────────┘    └───────────────┘ │
│         │                  │                      │        │
│         ▼                  ▼                      ▼        │
│  ┌─────────────┐    ┌───────────────┐                     │
│  │   Routes    │    │   Database    │                     │
│  │  (API定义)  │    │   (SQLite)    │                     │
│  └─────────────┘    └───────────────┘                     │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 组件说明

| 组件 | 职责 | 文件位置 |
|------|------|----------|
| **HTTP API** | 提供 RESTful 接口 | `src/main.cpp` |
| **Flight Service** | 航班价格查询与监控逻辑 | `src/flight_service.cpp` |
| **Notification Service** | 价格变化通知 | `src/notification_service.cpp` |
| **Database** | 数据持久化存储 | `src/database.cpp` |

### 2.3 数据流

```
客户端请求 → HTTP API → Flight Service → 外部API/数据库 → 返回响应
                                        ↓
                              Notification Service → 邮件/日志通知
```

### 2.4 接口说明

| 接口 | 方法 | 路径 | 描述 |
|------|------|------|------|
| 添加监控任务 | POST | `/api/tasks` | 创建新的航班监控任务 |
| 获取任务列表 | GET | `/api/tasks` | 获取所有监控任务 |
| 获取任务详情 | GET | `/api/tasks/{id}` | 获取单个任务详情 |
| 更新任务 | PUT | `/api/tasks/{id}` | 更新监控任务 |
| 删除任务 | DELETE | `/api/tasks/{id}` | 删除监控任务 |
| 手动检查价格 | POST | `/api/tasks/{id}/check` | 立即检查价格 |

---

## 3. 编译环节

### 3.1 编译命令

```bash
# 创建构建目录
mkdir -p build && cd build

# 配置编译
cmake ..

# 编译项目
make -j$(nproc)
```

### 3.2 编译产物

| 文件 | 路径 | 说明 |
|------|------|------|
| `flight_monitor` | `build/flight_monitor` | 主程序可执行文件 |

### 3.3 环境要求

- **编译器**: GCC 8.0+ 或 Clang 9.0+
- **CMake**: 3.10+
- **系统**: Linux/macOS

---

## 4. 运行环节

### 4.1 运行命令

```bash
# 进入构建目录
cd build

# 运行程序（默认端口 18080）
./flight_monitor

# 指定端口运行
./flight_monitor --port 8080
```

### 4.2 环境变量

| 变量名 | 说明 | 默认值 |
|--------|------|--------|
| `DB_PATH` | SQLite 数据库路径 | `./flight_monitor.db` |
| `LOG_LEVEL` | 日志级别 | `INFO` |

---

## 5. 验证环节

### 5.1 API 测试

```bash
# 添加监控任务
curl -X POST http://localhost:18080/api/tasks \
  -H "Content-Type: application/json" \
  -d '{"flight_number": "CA1234", "origin": "PEK", "destination": "SHA", "target_price": 500}'

# 获取任务列表
curl http://localhost:18080/api/tasks

# 检查任务价格
curl -X POST http://localhost:18080/api/tasks/1/check
```

### 5.2 验证标准

| 验证项 | 标准 |
|--------|------|
| API 响应 | HTTP 200 OK |
| 数据库写入 | 任务成功存储 |
| 价格检查 | 返回当前价格 |
| 通知触发 | 价格低于目标时发送通知 |

---

## 6. 部署环节

### 6.1 部署脚本

```bash
# 部署到生产环境
#!/bin/bash
set -e

# 停止旧服务
pkill -f flight_monitor || true

# 复制新二进制
cp build/flight_monitor /opt/flight-monitor/

# 启动服务
nohup /opt/flight-monitor/flight_monitor > /var/log/flight-monitor.log 2>&1 &
```

### 6.2 部署目标

- **开发环境**: 本地运行
- **测试环境**: 测试服务器
- **生产环境**: 生产服务器

---

## 7. 调试方法

### 7.1 日志查看

```bash
# 查看运行日志
tail -f /var/log/flight-monitor.log

# 查看数据库记录
sqlite3 flight_monitor.db "SELECT * FROM tasks;"
```

### 7.2 常见问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 端口被占用 | 18080 端口已被使用 | 更换端口 `--port 8081` |
| 数据库连接失败 | 数据库文件权限问题 | 检查文件权限 |
| API 响应超时 | 外部航班 API 问题 | 检查网络连接 |

---

## 8. 版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0.0 | 2026-05-12 | 初始版本，支持基本监控功能 |

---

## 9. GitHub 仓库

- **仓库地址**: `https://github.com/MichaelLeoWang/flight-price-monitor-cpp`
- **分支**: `master`
- **最后同步**: 2026-05-12