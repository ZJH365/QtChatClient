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


## 如果“项目打不开”

请按下面顺序排查：

1. 先确认你是用 **VS2015** 打开 `QtChatClient.sln`（不是直接双击 `.vcxproj`）。
2. VS2015 中安装 **Qt VS Tools**，并在 `Qt VS Tools -> Qt Versions` 配置 Qt 5.8 (MSVC2015) 路径。
3. 如果仍提示工程类型不支持：关闭 VS，删除 `.vs/` 目录后重开解决方案。
4. 本仓库 `sln` 已改为标准 VC++ 工程类型 GUID，可在未装 Qt 插件时先打开工程文件（仅构建会失败）。
5. 如果报 `[QtRunWork] Error starting process \bin\uic.exe`，说明 Qt 路径为空：
   - 在 VS2015 里执行：`Qt VS Tools -> Qt Project Settings`，给工程选择 Qt 5.8 (msvc2015)
   - 或设置系统环境变量 `QTDIR=C:\Qt\5.8\msvc2015`，重启 VS 后再编译
6. 如果 VS 里看不到 **Qt Project Settings**：
   - 说明工程未被 Qt VS Tools 识别；本仓库已同时设置：
     - `sln` 使用 Qt VS Tools 项目类型 GUID（`{BC8A1FFA-BEE3-4634-8014-F334798102B3}`）
     - `vcxproj` 使用 `Keyword=Qt4VSv1.0`，并导入 `$(QtMsBuild)\qt.props` / `qt_defaults.props` 与 `qt.targets`
   - 先确认已安装 **Qt VS Tools** 扩展，再右键项目看是否出现 Qt 菜单。

## 在 VS2015 中打开

1. 安装 Qt 5.8（MSVC2015）和 Qt VS Tools 插件。
2. 打开 `QtChatClient.sln`。
3. 先编译运行 `QtChatServer`，再运行 `QtChatClient`。

> 备注：示例为了便于理解，AES key/iv 采用固定值；生产环境请改为安全密钥协商方案。
