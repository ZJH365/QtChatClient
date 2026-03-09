# QtChatClient (Qt 5.8 / VS2015)

一个可直接拆分为**客户端**和**服务端**两个程序的即时聊天示例，目标是简单、可读、可运行。

## 目录结构

```
QtChatClient/
├── QtChatClient.pro            # 顶层 subdirs 工程
├── common/
│   ├── protocol.h
│   └── protocol.cpp            # 协议封包 + AES 加密/解密
├── server/
│   ├── server.pro
│   ├── main.cpp
│   ├── serverwindow.h/.cpp/.ui # 服务端窗口（端口、日志）
│   ├── chatserver.h/.cpp       # 登录验证、广播、历史持久化
│   └── clientsession.h/.cpp    # 单连接会话处理
└── client/
    ├── client.pro
    ├── main.cpp
    ├── logindialog.h/.cpp/.ui  # 登录窗口
    ├── clientwindow.h/.cpp/.ui # 主窗口（聊天页/用户页）
    └── chatclient.h/.cpp       # 客户端网络通信
```

## 功能

- 客户端：
  - 启动先显示登录窗口（IP、端口、用户名、密码）
  - 登录成功进入主界面：左侧导航（聊天/用户）、中间聊天列表、右侧聊天或个人信息页
  - 支持发送/接收文本消息
- 服务端：
  - 验证用户名密码（`users.json`）
  - 登录成功后返回最近 24 小时聊天记录
  - 接收并广播群聊消息给所有在线用户
  - 聊天记录持久化到 `chat_history.json`
- 网络层：
  - 全部数据使用 JSON 组织
  - 全部 TCP 包体使用 AES-256-CBC 加密（OpenSSL EVP）

## 默认账号

服务端首次启动会自动创建 `users.json`：

- `admin / 123456`
- `demo / demo123`

## 编译说明（Qt 5.8 + VS2015）

1. 确保 Qt 5.8 对应 MSVC2015 套件已安装。
2. 确保运行目录下可找到 OpenSSL 动态库（`libeay32.dll`、`ssleay32.dll`）。
3. 用 Qt Creator 打开：
   - 顶层 `QtChatClient.pro`（推荐）
   - 或分别打开 `server/server.pro` 与 `client/client.pro`
4. 先启动服务端，再启动客户端登录。

> 说明：示例使用固定 AES key/iv，便于新手理解流程；生产环境请改为安全密钥协商方案。
