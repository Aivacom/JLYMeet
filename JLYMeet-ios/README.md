Use ThunderBolt SDK to realize audio and video functions
======================================

中文版本：[简体中文](README.zh.md)


<br />

Overview
-------------------------------------------------------------
- This article mainly introduces how to use ThunderBolt iOS version SDK to achieve the same channel audio and video continuous wheat function. Based on this scenario, the implementation steps will be briefly explained.

<br />
   
Reference API
-------------------------------------------------------------

### Method interface

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


### Delegate
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


Implementation steps
-------------------------------------------------------------
（1）First initialize the SDK and create a global instance of `ThunderEngine`

```objective-c

    self.engine = [ThunderEngine createEngine:kThunderAppId sceneId:0 delegate:delegate];

```  

（2）Initialize some global configuration

```objective-c

    // Setting area: default value (domestic)
    [_engine setArea:THUNDER_AREA_DEFAULT];

    // Turn on user volume callback, once every 500 milliseconds
    [_engine setAudioVolumeIndication:500 moreThanThd:0 lessThanThd:0 smooth:0];

    // Turn on microphone volume callback
    [_engine enableCaptureVolumeIndication:500 moreThanThd:0 lessThanThd:0 smooth:0];

```

（3）To join a room, you need to configure room properties and start-up configuration as needed before joining the room

```objective-c

    // Set room properties.
    [_engine setMediaMode:THUNDER_CONFIG_NORMAL];     
    [_engine setRoomMode:THUNDER_ROOM_CONFIG_LIVE];   

    // Set audio properties.
    [_engine setAudioConfig:THUNDER_AUDIO_CONFIG_MUSIC_STANDARD 
                 commutMode:THUNDER_COMMUT_MODE_HIGH            
               scenarioMode:THUNDER_SCENARIO_MODE_DEFAULT];     

    // Join room
    [_engine joinRoom:_token roomName:roomId uid:self.localUid];

```

（4）After receiving the successful callback to join the room (`onJoinRoomSuccess: withUid: elapsed:`), publish local audio and video streams and set the start broadcast position

```objective-c

    ThunderVideoEncoderConfiguration* videoEncoderConfiguration = [[ThunderVideoEncoderConfiguration alloc] init];
    // Set the start play method to start the video with wheat
    videoEncoderConfiguration.playType = THUNDERPUBLISH_PLAY_INTERACT;
    // Set video encoding type
    videoEncoderConfiguration.publishMode = THUNDERPUBLISH_VIDEO_MODE_NORMAL;

    // Each time you enter the room, you need to set it again, otherwise the default configuration will be used
    [_engine setVideoEncoderConfig:videoEncoderConfiguration];

    // Start video preview
    [_engine startVideoPreview];
    
    // Push local video stream
    [_engine stopLocalVideoStream:NO];
    
    // Push local audio stream
    [_engine stopLocalAudioStream:NO];

```


（5）Create a local video canvas

```objective-c

    // Create video canvas
    ThunderVideoCanvas *canvas = [[ThunderVideoCanvas alloc] init];

    // The view must be set when the canvas is created
    UIView *view = [[UIView alloc] init];
    view.backgroundColor = [UIColor blackColor];
    canvas.view = view;

    // Set video layout
    [canvas setRenderMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

    // Set user id
    [canvas setUid:uid];

    // Set local canvas
    [_engine setLocalVideoCanvas:canvas];
    // Set the local canvas display mode
    [_engine setLocalCanvasScaleMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

```


（6）Upon receiving the audio from the remote user, the video stream callback notification (`onRemoteAudioStopped`,` onRemoteVideoStopped`), create a remote live view and establish a microphone

```objective-c

    // Create video canvas
    ThunderVideoCanvas *canvas = [[ThunderVideoCanvas alloc] init];

    // The view must be set when the canvas is created
    UIView *view = [[UIView alloc] init];
    view.backgroundColor = [UIColor blackColor];
    canvas.view = view;

    // Set video layout
    [canvas setRenderMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

    // Set user id
    [canvas setUid:uid];

    // Set remote canvas
    [_engine setRemoteVideoCanvas:canvas];
    // Set the remote canvas display mode
    [_engine setRemoteCanvasScaleMode:uid mode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];

```


（7）You can also switch front and rear cameras

```objective-c

    // Successful call returns 0, failed returns <0
    [_engine switchFrontCamera:isFront];

```

（8）Can turn off local audio and video streaming

```objective-c

    if (disabled) {
        [_engine stopVideoPreview];
    } else {
        [_engine startVideoPreview];
    }

    // On or off local video stream
    [_engine stopLocalVideoStream:disabled];


    // On or off local audio stream
    [_engine stopLocalAudioStream:disabled];

```

（9）Can turn off remote audio and video streaming

```objective-c

    [_engine stopRemoteAudioStream:uid stopped:disabled];
    [_engine stopRemoteVideoStream:uid stopped:disabled];

```

（10）You can set the start broadcast position after entering the room

```objective-c

    ThunderVideoEncoderConfiguration* videoEncoderConfiguration = [[ThunderVideoEncoderConfiguration alloc] init];
    // Set the start play method to start the video with wheat
    videoEncoderConfiguration.playType = THUNDERPUBLISH_PLAY_INTERACT;
    // Set video encoding type
    videoEncoderConfiguration.publishMode = THUNDERPUBLISH_VIDEO_MODE_NORMAL;

    // Each time you enter the room, you need to set it again, otherwise the default configuration will be used
    [_engine setVideoEncoderConfig:videoEncoderConfiguration];

```

（11）Leave room

```objective-c

    [_engine leaveRoom];

```

（12）Finally, upon receiving the notification that the program wants to exit, destroy the `ThunderEngine` instance and do some exception handling

```objective-c

    // Prevent users from staying in the room for a while after logging out
    [self.engine leaveRoom];

    // Destroy engine
    [ThunderEngine destroyEngine];

```

