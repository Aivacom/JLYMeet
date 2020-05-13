//
//  CanvasStatus.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/9.
//  Copyright © 2019 JLY. All rights reserved.
//


#import <Foundation/Foundation.h>

typedef NS_ENUM(NSUInteger, RemoteStreamType) {
    RemoteStreamTypeAudio = 0,        // 音频流/Audio streaming
    RemoteStreamTypeVideo,            // 视频流/Video streaming
};

@interface CanvasStatus : NSObject

// 用户uid
// user id
@property (nonatomic, copy) NSString *uid;
// 用于记录连麦用户是否音频和视频都已断开，本地视频不受该参数控制
// It is used to record whether the audio and video have been disconnected by the microphone user, the local video is not controlled by this parameter
@property (nonatomic, assign) NSUInteger livingRetainCount;
// 是否已关闭视频流发送
// Whether the video streaming has been turned off
@property (nonatomic, assign) BOOL isVideoStreamStoped;
// 是否已关闭音频流发送
// Whether audio streaming is turned off
@property (nonatomic, assign) BOOL isAudioStreamStoped;
// 是否为全屏模式
// Whether it is full screen mode
@property (nonatomic, assign) BOOL isFullScreen;
// 是否弹出ActionSheet
// Whether to pop up ActionSheet
@property (nonatomic, assign) BOOL isShowActionSheet;
// 是否远程音频流已关闭，只针对远程用户自己关闭音频流
// Whether the remote audio stream is closed, only to close the audio stream for the remote user
@property (nonatomic, assign) BOOL isRemoteCanvasAudioStreamStoped;

- (void)updateStatusOnRemoteStreamStopped:(BOOL)stopped remoteStreamType:(RemoteStreamType)streamType;


@end
