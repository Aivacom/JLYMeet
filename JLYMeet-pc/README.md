## Use ThunderBolt SDK to realize video linking function

*中文版本： [简体中文](README.zh.md)* 

### Overview

- This article mainly introduces how to use ThunderBolt Windows version to realize the function of video linking. First of all, please make sure that ThunderBolt SDK is correctly integrated into your development environment.

### Prepare

(1) Download SDK

(2) Copy the provided header files and library files to the project directory

(3) Add the project's header file directory: project—attributes—configuration attributes—c / c + ± —regular—additional include directory: add the header file storage directory.

(4) Add the path of the lib library referenced by the project: Project-Properties-Configuration Properties-Linker-General-Additional Library Directory: plus the directory where the lib files are stored.

(5) Add the lib file name referenced by the project: project—properties—configuration properties—linker—input—additional dependencies: add the lib file name. 

(6) Copy the provided dll file to the project output directory

### Implementation steps

(1) First create an `IThunderEngine` instance, and perform the initialization operation.

```c++
MediaManager::instance()->getThunderManager() = createEngine();
MediaManager::instance()->getThunderManager()->initialize(appId, sceneId, pHandler);
```

(2) Join the room.

```c++
// setMediaMode => If you do n’t want to modify later, you can set it only once
MediaManager::instance()->getThunderManager()->setMediaMode(PROFILE_NORMAL);  

MediaManager::instance()->getThunderManager()->joinRoom(token.c_str(), token.length(), m_roomName.c_str(), uid.c_str());
```

(3) After successfully entering the room, publish local audio and video streams to join the video link.

```   c++
// set camera
if (MediaManager::instance()->getThunderManager()->getVideoDeviceMgr() != NULL)
{
	MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->startVideoDeviceCapture(m_videoDevicesCombo.GetCurSel());
}

// Set the local view
VideoCanvas canvas;
canvas.renderMode = VIDEO_RENDER_MODE_CLIP_TO_BOUNDS;
sprintf_s(canvas.uid, "%I64d", m_localUid);
canvas.hWnd = m_videoCanvs[0].m_videoCavas->GetSafeHwnd();
MediaManager::instance()->getThunderManager()->setLocalVideoCanvas(canvas);   

// Set encoding configuration
VideoEncoderConfiguration config;
config.playType = VIDEO_PUBLISH_PLAYTYPE_SINGLE;
config.publishMode = VIDEO_PUBLISH_MODE_SMOOTH_DEFINITION;
MediaManager::instance()->getThunderManager()->setVideoEncoderConfig(config);	

// start preview
MediaManager::instance()->getThunderManager()->startVideoPreview();

MediaManager::instance()->getThunderManager()->stopLocalVideoStream(false); // Can send local video
MediaManager::instance()->getThunderManager()->stopLocalAudioStream(false); // Can send local audio

```

(4) Subscribe to remote video and audio after receiving the audio and video stream callback notifications (`onRemoteAudioStopped`,` onRemoteVideoStopped`) of the remote user.

```c++
MediaManager::instance()->getThunderManager()->setRemoteVideoCanvas(canvas);
MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(uid, stop);
```

(5) You can also stop sending local audio and video streams, so that remote users will not hear \ not see your voice \ video.

```c++
MediaManager::instance()->getThunderManager()->stopLocalAudioStream(stop);
MediaManager::instance()->getThunderManager()->stopLocalVideoStream(stop);
```

(6) You can also choose not to receive audio and video streams from remote users.

```c++
MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(uid, stop);
MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(uid, stop);
```

(7) Leave the room.

```c++
MediaManager::instance()->getThunderManager()->stopVideoPreview();
MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->stopVideoDeviceCapture();
MediaManager::instance()->getThunderManager()->leaveRoom();
```

(8) Finally, before ending the program, don't forget to destroy the `IThunderEngine` instance.

```c++
MediaManager::instance()->getThunderManager()->destroyEngine();
```
