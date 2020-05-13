//
//  ThunderManager.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/7.
//  Copyright © 2019 JLY. All rights reserved.
//


#import <Foundation/Foundation.h>
#import "ThunderEngine.h"
#import "VideoCanvas.h"


@interface ThunderManager : NSObject
@property (nonatomic, readonly, strong) ThunderEngine *engine;            // SDK引擎/SDK engine
@property (nonatomic, readonly, strong) VideoCanvas *localVideoCanvas;  // 视频视图/video canvas
@property (nonatomic, readonly, strong) NSString *logPath;                // 日志路径/log file path
@property (nonatomic, readonly, copy) NSString *appId;                    // AppId
@property (nonatomic, copy) NSString *localUid;                           // 本地用户uid/local user id
@property (nonatomic, copy) NSString *token;                              // token
@property (nonatomic, readonly, assign) ThunderPublishVideoMode publishMode;        // 视频编码类型/Video encoding type



+ (instancetype)sharedManager;

// 初始化SDK
// Initialize SDK
- (void)setupEngineWithDelegate:(id<ThunderEventDelegate>)delegate;

// 销毁SDK
// Destroy SDK
- (void)destroyEngine;

// 请求token
// Request token
- (void)requestTokenWithRoomId:(NSString *)roomId completionHandler:(void (^)(BOOL success))completionHandler;

// 加入房间
// Join the room
- (void)joinRoom:(NSString *)roomId;

// 设置视频编码配置
// Set video encoding configuration
- (void)setupPublishMode;

// 退出房间
// Exit the room
- (void)leaveRoom;

// 开启直播
// Start live streaming
- (void)enableVideoLive;

// 创建本地或者远程视频视图
// Create local or remote video views
- (VideoCanvas *)createVideoCanvasWithUid:(NSString *)uid isLocalCanvas:(BOOL)isLocalCanvas;

// 切换摄像头
// Switch camera
- (NSInteger)switchFrontCamera:(BOOL)isFront;

// 关闭本地视频，包含视频采集和视频流推送
// Close local video, including video capture and video stream push
- (void)disableLocalVideo:(BOOL)disabled;

// 关闭接收远程视频流
// Close receiving remote video stream
- (void)disableRemoteVideo:(NSString *)uid disabled:(BOOL)disabled;

// 关闭本地音频流推送
// On or off local audio streaming
- (void)disableLocalAudio:(BOOL)disabled;

// 关闭接收远程音频流
// On or off receiving remote audio streams
- (void)disableRemoteAudio:(NSString *)uid disabled:(BOOL)disabled;

// 恢复远程用户的音频流和视频流
// Restore audio and video streams of remote users
- (void)recoveryRemoteStream:(NSString *)uid;

// SDK退出房间后依然会保持之前停止流的状态，所以退出房间时对所有流进行恢复
// After exiting the room, the SDK will still maintain the state of the previous stop flow, so all flows are restored when exiting the room
- (void)recoveryAllRemoteStream;

// 更新token
// Update token
- (void)updateToken;

// 切换视频编码类型
// Switch video encoding type
- (void)switchPublishMode:(ThunderPublishVideoMode)publishMode;

//清除canvasView
// Clear canvasView
- (void)clearCanvasViewWithUID:(NSString *)uid;

@end
