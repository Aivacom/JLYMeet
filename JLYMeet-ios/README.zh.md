使用 ThunderBolt SDK 实现同频道音视频连麦功能
======================================

English Version： [English](README.md)

<br />

概述
-------------------------------------------------------------
- 本文主要介绍如何使用ThunderBolt iOS版本SDK实现同频道音视频连麦功能，基于该场景，将对实现步骤做简要说明。

<br />

引用API
-------------------------------------------------------------

### 方法接口

* `createEngine:sceneId:delegate:`
* `setArea:`
* `setAudioVolumeIndication:moreThanThd:lessThanThd:smooth:`
* `enableCaptureVolumeIndication:moreThanThd:lessThanThd:smooth:`
* `setLogFilePath:`
* `setLogCallback:`   

<br />

* `setMediaMode:`
* `setRoomMode:`
* `setAudioConfig:commutMode:scenarioMode:`
* `setVideoEncoderConfig:`
* `joinRoom:roomName:uid:`  

<br />

* `startVideoPreview`
* `enableVideoEngine`
* `enableAudioEngine`

<br />

* `setRenderMode:`
* `setUid:`
* `setLocalVideoCanvas:`
* `setLocalCanvasScaleMode:`
* `setRemoteVideoCanvas:`
* `setRemoteCanvasScaleMode:mode:`   

<br />

* `switchFrontCamera:`   

<br />

* `stopVideoPreview`   
* `stopLocalVideoStream:`
* `stopLocalAudioStream:`
* `stopRemoteVideoStream:stopped:`
* `stopRemoteAudioStream:stopped:`   

<br />

* `stopAllRemoteVideoStreams:`
* `stopAllRemoteAudioStreams:`   

<br />

* `updateToken:`   

<br />

* `leaveRoom`   

<br />

* `destroyEngine`   

<br />
<br />


### 代理接口
* `thunderEngine:onJoinRoomSuccess:withUid:elapsed:`
* `thunderEngine:onLeaveRoomWithStats:`
* `thunderEngine:onPlayVolumeIndication:totalVolume:`
* `thunderEngine:onCaptureVolumeIndication:cpt:micVolume:`
* `thunderEngine:onRemoteAudioStopped:byUid:`
* `thunderEngine:onRemoteVideoStopped:byUid:`
* `thunderEngine:onRoomStats:`
* `thunderEngine:onTokenWillExpire:`
* `thunderEngineTokenRequest:`
* `thunderEngine:onNetworkQuality:txQuality:rxQuality:`


<br />
<br />


实现步骤
-------------------------------------------------------------
（1）首先初始化SDK并创建一个`ThunderEngine`全局实例

```objective-c

    self.engine = [ThunderEngine createEngine:kThunderAppId sceneId:0 delegate:delegate];

```  

（2）初始化一些全局配置

```objective-c

    // 设置区域：默认值（国内）
    [_engine setArea:THUNDER_AREA_DEFAULT];

    // 打开用户音量回调，500毫秒回调一次
    [_engine setAudioVolumeIndication:500 moreThanThd:0 lessThanThd:0 smooth:0];

    // 打开麦克风音量回调
    [_engine enableCaptureVolumeIndication:500 moreThanThd:0 lessThanThd:0 smooth:0];

```

（3）加入房间，需在加入房间前根据需要配置房间属性和开播配置等

```objective-c

    // 设置房间属性。   如果不是指定纯音频模式的话，可以不设置，默认是音视频模式
    [_engine setMediaMode:THUNDER_CONFIG_NORMAL];     // 音视频模式：音视频模式；
    [_engine setRoomMode:THUNDER_ROOM_CONFIG_LIVE];   // 场景模式：直播

    // 设置音频属性。
    [_engine setAudioConfig:THUNDER_AUDIO_CONFIG_MUSIC_STANDARD // 采样率，码率，编码模式和声道数：44.1 KHz采样率，音乐编码, 双声道，编码码率约 40；
                 commutMode:THUNDER_COMMUT_MODE_HIGH            // 交互模式：强交互模式
               scenarioMode:THUNDER_SCENARIO_MODE_DEFAULT];     // 场景模式：默认


    // 加入房间
    [_engine joinRoom:_token roomName:roomId uid:self.localUid];

```

（4）在收到加入房间成功回调后（`onJoinRoomSuccess:withUid:elapsed:`），设置开播参数，发布本地音、视频流

```objective-c

    ThunderVideoEncoderConfiguration* videoEncoderConfiguration = [[ThunderVideoEncoderConfiguration alloc] init];
    // 设置开播玩法为视频连麦开播
    videoEncoderConfiguration.playType = THUNDERPUBLISH_PLAY_INTERACT;
    // 设置视频编码类型
    videoEncoderConfiguration.publishMode = THUNDERPUBLISH_VIDEO_MODE_NORMAL;

    // 每次进房间都需要再次设置，否则会使用默认配置
    [_engine setVideoEncoderConfig:videoEncoderConfiguration];

    // 开启视频预览
    [_engine startVideoPreview];
    
    // 开启本地视频流发送
    [_engine stopLocalVideoStream:NO];
    
    
    // 开启本地音频流发送
    [_engine stopLocalAudioStream:NO];

```


（5）创建本地直播视图

```objective-c

    // 创建视频视图
    ThunderVideoCanvas *canvas = [[ThunderVideoCanvas alloc] init];

    // 必须创建canvas时设置其view
    UIView *view = [[UIView alloc] init];
    view.backgroundColor = [UIColor blackColor];
    canvas.view = view;

    // 设置视频布局
    [canvas setRenderMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

    // 设置用户uid
    [canvas setUid:uid];

    // 设置本地视图
    [_engine setLocalVideoCanvas:canvas];
    // 设置本地视图显示模式
    [_engine setLocalCanvasScaleMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

```


（6）在收到远端用户的音，视频流回调通知（`onRemoteAudioStopped`，`onRemoteVideoStopped`），创建远端直播视图建立连麦

```objective-c

    // 创建视频视图
    ThunderVideoCanvas *canvas = [[ThunderVideoCanvas alloc] init];

    // 必须创建canvas时设置其view
    UIView *view = [[UIView alloc] init];
    view.backgroundColor = [UIColor blackColor];
    canvas.view = view;

    // 设置视频布局
    [canvas setRenderMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

    // 设置用户uid
    [canvas setUid:uid];

    // 设置远端视图
    [_engine setRemoteVideoCanvas:canvas];
    // 设置远端视图显示模式
    [_engine setRemoteCanvasScaleMode:uid mode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

```


（7）您还可以切换前后置摄像头

```objective-c

    //  调用成功返回 0，失败返回 < 0
    [_engine switchFrontCamera:isFront];

```

（8）可以关闭本地音，视频流

```objective-c

    if (disabled) {
        [_engine stopVideoPreview];
    } else {
        [_engine startVideoPreview];
    }

    // 开关本地视频发送
    [_engine stopLocalVideoStream:disabled];


    // 开关本地音频流
    [_engine stopLocalAudioStream:disabled];

```

（9）可以关闭远端音，视频流

```objective-c

    [_engine stopRemoteAudioStream:uid stopped:disabled];
    [_engine stopRemoteVideoStream:uid stopped:disabled];

```

（10）进入房间成功后，可以实时切换开播档位

```objective-c

    ThunderVideoEncoderConfiguration* videoEncoderConfiguration = [[ThunderVideoEncoderConfiguration alloc] init];
    // 设置开播玩法为视频连麦开播
    videoEncoderConfiguration.playType = THUNDERPUBLISH_PLAY_INTERACT;
    // 设置视频编码类型
    videoEncoderConfiguration.publishMode = THUNDERPUBLISH_VIDEO_MODE_NORMAL;

    // 每次进房间都需要再次设置，否则会使用默认配置
    [_engine setVideoEncoderConfig:videoEncoderConfiguration];

```

（11）连麦结束后离开房间

```objective-c

    [_engine leaveRoom];

```

（12）最后在收到程序要退出的通知时销毁`ThunderEngine`实例并做一些异常处理

```objective-c

    // 防止退出后，用户还停留在房间一段时间
    [self.engine leaveRoom];

    // 销毁引擎
    [ThunderEngine destroyEngine];

```

