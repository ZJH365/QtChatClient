# QtChatClient (Qt 5.8 / VS2015)

一个可直接拆分为**客户端**和**服务端**两个程序的即时聊天示例，重点是：
- VS2015 可直接打开 `.sln`
- 界面主要用 `.ui`
- 功能先稳定可用，再考虑扩展

## 目录结构

```
QtChatClient/
├── QtChatClient.sln            # VS2015 解决方案（推荐）
├── QtChatClient.pro            # Qt Creator 顶层工程（可选）
├── common/
│   ├── protocol.h
│   └── protocol.cpp            # 协议封包 + AES(CNG) 加密/解密
├── server/
│   ├── QtChatServer.vcxproj    # VS2015 工程
│   ├── server.pro
│   ├── main.cpp
│   ├── serverwindow.h/.cpp/.ui
│   ├── chatserver.h/.cpp
│   └── clientsession.h/.cpp
└── client/
    ├── QtChatClient.vcxproj    # VS2015 工程
    ├── client.pro
    ├── main.cpp
    ├── logindialog.h/.cpp/.ui
    ├── clientwindow.h/.cpp/.ui
    └── chatclient.h/.cpp
```

## 已实现功能

- 客户端：登录 -> 聊天主界面（聊天页/用户页）
- 服务端：账号密码验证、群消息广播
- 持久化：
  - `users.json`（账号）
  - `chat_history.json`（聊天记录）
- 登录成功后返回最近 24 小时聊天记录
- 网络包：JSON + 长度包头 + AES-CBC 加密

## 默认账号

服务端首次启动会自动创建：
- `admin / 123456`
- `demo / demo123`

## 关键兼容说明（本次修复重点）

1. **移除了 OpenSSL 依赖**：不再需要 `openssl/evp.h`、`libeay32.dll`、`ssleay32.dll`。
2. AES 改为 **Windows CNG (`bcrypt`)** 实现，VS2015 默认可用。
3. 新增 **VS2015 `.sln + .vcxproj`**，可以直接在 Visual Studio 中打开编译。
4. `vcxproj` 固定 `PlatformToolset=v140`，并设置 `WindowsTargetPlatformVersion=8.1`，用于规避新 SDK + 老编译器组合导致的兼容问题。

## 在 VS2015 中打开

1. 安装 Qt 5.8（MSVC2015）和 Qt VS Tools 插件。
2. 打开 `QtChatClient.sln`。
3. 先编译运行 `QtChatServer`，再运行 `QtChatClient`。

> 备注：示例为了便于理解，AES key/iv 采用固定值；生产环境请改为安全密钥协商方案。
