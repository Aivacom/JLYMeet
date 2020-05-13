## 使用 ThunderBolt SDK 实现视频连麦功能

*English Version：[english](/README.md)*

### 概述

- 本文主要介绍如何使用ThunderBolt Windows版本实现视频连麦的功能。首先请确保已将ThunderBolt SDK正确集成到你的开发环境

### 准备
(1) 下载 SDK

(2) 复制提供的头文件和库文件到工程目录下

(3) 添加工程的头文件目录：工程—属性—配置属性—c/c+±–常规—附加包含目录：加上头文件存放目录。

(4) 添加工程引用的lib库路径：工程—属性—配置属性—链接器—常规—附加库目录：加上lib文件存放目录。

(5) 添加工程引用的lib文件名：工程—属性—配置属性—链接器—输入—附加依赖项：加上lib文件名。

(6) 复制提供的dll文件到工程输出目录下

### 实现步骤
(1) 首先创建一个`IThunderEngine`实例，并进行初始化操作。

```
MediaManager::instance()->getThunderManager() = createEngine();
MediaManager::instance()->getThunderManager()->initialize(appId, sceneId, pHandler);
```

(2) 加入房间。

```
setMediaMode => If you do n’t want to modify later, you can set it only once
MediaManager::instance()->getThunderManager()->setMediaMode(PROFILE_NORMAL);  

MediaManager::instance()->getThunderManager()->joinRoom(token.c_str(), token.length(), m_roomName.c_str(), uid.c_str());
```

(3) 在进入房间成功后，发布本地音、视频流加入视频连麦。

```   
// 设置摄像头
if (MediaManager::instance()->getThunderManager()->getVideoDeviceMgr() != NULL)
{
	MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->startVideoDeviceCapture(m_videoDevicesCombo.GetCurSel());
}

// 设置本地视图
VideoCanvas canvas;
canvas.renderMode = VIDEO_RENDER_MODE_CLIP_TO_BOUNDS;
sprintf_s(canvas.uid, "%I64d", m_localUid);
canvas.hWnd = m_videoCanvs[0].m_videoCavas->GetSafeHwnd();
MediaManager::instance()->getThunderManager()->setLocalVideoCanvas(canvas);   

// 设置编码配置
VideoEncoderConfiguration config;
config.playType = VIDEO_PUBLISH_PLAYTYPE_SINGLE;
config.publishMode = VIDEO_PUBLISH_MODE_SMOOTH_DEFINITION;
MediaManager::instance()->getThunderManager()->setVideoEncoderConfig(config);	

// 打开视频预览
MediaManager::instance()->getThunderManager()->startVideoPreview();

MediaManager::instance()->getThunderManager()->stopLocalVideoStream(false); // 发送本地视频
MediaManager::instance()->getThunderManager()->stopLocalAudioStream(false); // 发送本地音频

```

(4) 在收到远端用户的音频、视频流回调通知（`onRemoteAudioStopped`、`onRemoteVideoStopped`）后订阅远端视频和音频。

```
MediaManager::instance()->getThunderManager()->setRemoteVideoCanvas(canvas);
MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(uid, stop);
```

(5) 您还可以停止发送本地音视频流，这样远端用户将听不到\看不见你的声音\视频。

```
MediaManager::instance()->getThunderManager()->stopLocalAudioStream(stop);
MediaManager::instance()->getThunderManager()->stopLocalVideoStream(stop);
```

(6) 您也可以选择不接收远端用户的音视频流。

```
MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(uid, stop);
MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(uid, stop);
```

(7) 连麦结束后，退出房间。

```
MediaManager::instance()->getThunderManager()->stopVideoPreview();
MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->stopVideoDeviceCapture();
MediaManager::instance()->getThunderManager()->leaveRoom();
```

(8) 最后，结束程序前，别忘了销毁`IThunderEngine`实例。

```
MediaManager::instance()->getThunderManager()->destroyEngine();
```