## Use ThunderBolt SDK to realize video connection function
*中文版本： [简体中文](README.zh.md)*
### Overview
- This article mainly introduces how to use ThunderBolt Android version to realize the function of video connection. First of all, please make sure that ThunderBolt SDK is correctly integrated into your development environment.

We provide comprehensive user access documentation so that users can quickly access audio and video capabilities.If you want to know the specific integration method, interface description, and related scenario Demo, you can click the following link to learn:

> Integrate SDK to APP, please click:[SDK integration instructions](https://docs.aivacom.com/cloud/cn/product_category/rtc_service/rt_video_interaction/integration_and_start/integration_and_start_android.html)

> API development manual, please click: [Android API](https://docs.aivacom.com/cloud/cn/product_category/rtc_service/rt_video_interaction/api/Android/v2.7.0/category.html)

> For related Demo download, please click: [SDK and Demo Download](https://docs.aivacom.com/download)

### Implementation steps
（1）Create an instance of `IThunderEngine` and initialize it.

```
 thunderEngine = ThunderEngine.createEngine(context, appId, sceneId, handler);
```

（2）Join the room. The token needs to be obtained from the business server by referring to the documentation.

```
//set business area
thunderEngine.setArea(ThunderRtcConstant.AreaType.THUNDER_AREA_DEFAULT);
//set Room properties
thunderEngine.setRoomConfig(THUNDER_PROFILE_DEFAULT, THUNDER_ROOMCONFIG_LIVE);
//join room
thunderEngine.joinRoom(token, channelName, uid);	
```

（3）After entering the room successfully, publish local audio and video streams to join the video connection.

```	

//start push audio
thunderEngine.stopLocalAudioStream(false);
//Set video encoding configuration
thunderEngine.setVideoEncoderConfig();
//Set local view scale mode
thunderEngine.setLocalCanvasScaleMode(THUNDER_RENDER_MODE_CLIP_TO_BOUNDS);
//Set local preview
ThunderVideoCanvas mPreviewView = new ThunderVideoCanvas(mPreviewContainer, THUNDERVIDEOVIEW_SCALE_MODE_ASPECT_FIT, uid);
thunderEngine.setLocalVideoCanvas(mPreviewView);
//Start video capture
thunderEngine.enableLocalVideoCapture(true);
//Start preview
thunderEngine.startVideoPreview();
//start push video
thunderEngine.stopLocalVideoStream(false);

```

（4）Subscribe to remote video and audio after receiving the audio and video stream callback (`onRemoteAudioStopped`,` onRemoteVideoStopped`) of the remote user.

```
@Override
public void onRemoteAudioStopped(String uid, boolean muted) {};
@Override
public void onRemoteVideoStopped(String uid, boolean muted) {};
```

（5）You can also stop sending local audio and video streams so that other users will not hear your voice or not see your video.

```
  thunderEngine.stopLocalAudioStream(false);
  thunderEngine.stopLocalVideoStream(false);
```

（6）You can also choose not to receive audio and video streams from remote users.

```
  thunderEngine.stopRemoteAudioStream(uid, muted);
  thunderEngine.stopRemoteVideoStream(uid, muted);
```

（7）After video connection ended, exit the room.

```
thunderEngine.leaveRoom();
```
