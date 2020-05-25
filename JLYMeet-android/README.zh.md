## 使用 ThunderBolt SDK 实现视频连麦功能
*English Version： [English](README.md)*

### 概述
- 本文主要介绍如何使用ThunderBolt Android版本实现视频连麦的功能，首先请确保已将ThunderBolt SDK正确集成到你的开发环境。
- 我们提供全面的用户接入文档，来方便用户实现音视频能力的快速接入，如果您想了解具体集成方法、接口说明、相关场景Demo，可点击如下链接了解：

集成SDK到APP，请参考：[SDK集成方法](https://docs.aivacom.com/cloud/cn/product_category/rtc_service/rt_video_interaction/integration_and_start/integration_and_start_android.html)

API开发手册，请访问： [Android API](https://docs.aivacom.com/cloud/cn/product_category/rtc_service/rt_video_interaction/api/Android/v2.7.0/category.html)

相关Demo下载，请访问：[SDK及Demo下载](https://docs.aivacom.com/download)

### 实现步骤
（1）首先创建一个`IThunderEngine`实例，并进行初始化操作。

```
 thunderEngine = ThunderEngine.createEngine(context, appId, sceneId, handler);
```

（2）加入房间。token需要参考文档从业务服务器获取。

```
//设置业务区域
thunderEngine.setArea(ThunderRtcConstant.AreaType.THUNDER_AREA_DEFAULT);
//设置频道属性
thunderEngine.setRoomConfig(THUNDER_PROFILE_DEFAULT, THUNDER_ROOMCONFIG_LIVE);
//加入房间
thunderEngine.joinRoom(token, channelName, uid);
```

（3）在进入房间成功后，发布本地音、视频流加入视频连麦。

```	
//
//开播 音频
thunderEngine.stopLocalAudioStream(false);
//设置视频模式
thunderEngine.setVideoEncoderConfig();
//设置本地视图缩放模式
thunderEngine.setLocalCanvasScaleMode(THUNDER_RENDER_MODE_CLIP_TO_BOUNDS);
//设置本地预览视图
ThunderVideoCanvas mPreviewView = new ThunderVideoCanvas(mPreviewContainer, THUNDERVIDEOVIEW_SCALE_MODE_ASPECT_FIT, uid);
thunderEngine.setLocalVideoCanvas(mPreviewView);
//开启视频采集
thunderEngine.enableLocalVideoCapture(true);
//开启预览
thunderEngine.startVideoPreview();
//开播 视频
thunderEngine.stopLocalVideoStream(false);

```

（4）在收到远端用户的音频、视频流回调通知（`onRemoteAudioStopped`、`onRemoteVideoStopped`）后订阅远端视频和音频。

```
@Override
public void onRemoteAudioStopped(String uid, boolean muted) {};
@Override
public void onRemoteVideoStopped(String uid, boolean muted) {};
```

（5）您还可以停止发送本地音视频流，这样远端用户将听不到或看不见你的声音或视频。

```
  thunderEngine.stopLocalAudioStream(false);
  thunderEngine.stopLocalVideoStream(false);
```

（6）您也可以选择不接收远端用户的音视频流。

```
  thunderEngine.stopRemoteAudioStream(uid, muted);
  thunderEngine.stopRemoteVideoStream(uid, muted);
```

（7）连麦结束后，退出房间。

```
thunderEngine.leaveRoom();
```
