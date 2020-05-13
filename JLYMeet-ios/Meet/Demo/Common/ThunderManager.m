//
//  ThunderManager.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/7.
//  Copyright © 2019 JLY. All rights reserved.
//


#import "ThunderManager.h"
#import "Utils.h"
#import "TokenHelper.h"
#import "CommonMacros.h"



static NSString * const kThunderAppId = @"1470236061"; // SDK AppID
static NSString * const kUserDefaultsKeyPublishMode = @"kUserDefaultsKeyPublishMode";

@interface ThunderManager () <ThunderRtcLogDelegate>
@property (nonatomic, readwrite, strong) ThunderEngine *engine;
@property (nonatomic, readwrite, strong) VideoCanvas *localVideoCanvas;
@property (nonatomic, readwrite, strong) NSString *logPath;
@property (nonatomic, readwrite, assign) ThunderPublishVideoMode publishMode;


@end

@implementation ThunderManager

+ (instancetype)sharedManager {
    static ThunderManager *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}


- (instancetype)init {
    self = [super init];
    if (self) {
        NSNumber *value = [[NSUserDefaults standardUserDefaults] objectForKey:kUserDefaultsKeyPublishMode];
        if (value) {
            _publishMode = value.intValue;
        } else {
            _publishMode = THUNDERPUBLISH_VIDEO_MODE_NORMAL;
        }
    }
    return self;
}


#pragma mark - Public

- (void)setupEngineWithDelegate:(id<ThunderEventDelegate>)delegate {
    self.engine = [ThunderEngine createEngine:kThunderAppId sceneId:0 delegate:delegate];
    
    // 设置区域：默认值（国内）
    // Setting area: default value (domestic)
    [_engine setArea:THUNDER_AREA_DEFAULT];
    
    // 打开用户音量回调，500毫秒回调一次
    // Turn on user volume callback, once every 500 milliseconds
    [_engine setAudioVolumeIndication:500 moreThanThd:0 lessThanThd:0 smooth:0];
    
    // 打开麦克风音量回调
    // Turn on microphone volume callback
    [_engine enableCaptureVolumeIndication:500 moreThanThd:0 lessThanThd:0 smooth:0];


    
    // Handle the exception of not exiting the room when the app exits
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillTerminateNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        [self.engine leaveRoom];
        
        // 销毁引擎
        // Destroy engine
        [ThunderEngine destroyEngine];
    }];
    
    // 设置SDK日志存储路径
    // Set SDK log storage path
    NSString* logPath = NSHomeDirectory();
    self.logPath = [logPath stringByAppendingString:@"/Documents/Log/Thunder"];
    [_engine setLogFilePath: _logPath];
    
    // Debug模式下直接打印日志
    // Print logs directly in Debug mode
#ifdef DEBUG
    [_engine setLogCallback:self];
#endif

}

- (void)destroyEngine {
    // 销毁引擎
    // Destroy engine
    [ThunderEngine destroyEngine];
}


- (void)requestTokenWithRoomId:(NSString *)roomId completionHandler:(void (^)(BOOL success))completionHandler {
    TokenRequestParams *params = [TokenRequestParams defaultParams];
    params.appId = self.appId;
    params.uid = self.localUid;
    params.roomId = roomId;
    [TokenHelper requestTokenWithParams:params completionQueue:dispatch_get_main_queue() completionHandler:^(BOOL success, NSString *token) {
        if (success) {
            self.token = token;
        }
        
        if (completionHandler) {
            completionHandler(success);
        }
    }];
}


- (void)joinRoom:(NSString *)roomId {
    // 设置房间属性。   如果不是指定纯音频模式的话，可以不设置，默认是音视频模式
    // Set room properties. If you do not specify the pure audio mode, you can not set it, the default is the audio and video mode
    [_engine setMediaMode:THUNDER_CONFIG_NORMAL];     // 音视频模式：音视频模式；/mode: audio and video mode;
    [_engine setRoomMode:THUNDER_ROOM_CONFIG_COMMUNICATION];   // 场景模式：直播/Scene mode: live broadcast
    
    // 设置音频属性。
    // Set audio properties
    [_engine setAudioConfig:THUNDER_AUDIO_CONFIG_MUSIC_STANDARD
                 commutMode:THUNDER_COMMUT_MODE_HIGH
               scenarioMode:THUNDER_SCENARIO_MODE_DEFAULT];
    // 加入房间
    // Join room
    [_engine joinRoom:_token roomName:roomId uid:self.localUid];
}

- (void)setupPublishMode {
    ThunderVideoEncoderConfiguration* videoEncoderConfiguration = [[ThunderVideoEncoderConfiguration alloc] init];
    /// 设置开播玩法为视频连麦开播
    // Set the start play method to start the video with wheat
    videoEncoderConfiguration.playType = THUNDERPUBLISH_PLAY_INTERACT;
    // 设置视频编码类型
    // Set video encoding type
    videoEncoderConfiguration.publishMode = _publishMode;
    
    // 每次进房间都需要再次设置，否则会使用默认配置
    // Each time you enter the room, you need to set it again, otherwise the default configuration will be used
    [_engine setVideoEncoderConfig:videoEncoderConfiguration];
}


- (void)leaveRoom {
    [_engine leaveRoom];
}

- (void)enableVideoLive {
    // 开启视频预览，在enableVideoEngine之前调用
    // Open video preview, call before enableVideoEngine
    [_engine startVideoPreview];
    
    // 开启本地视频流发送
    // Enable local video streaming
    [_engine stopLocalVideoStream:NO];
    
    // 打开音频采集，并开播到频道
    // Open audio capture and broadcast to channel
    [_engine stopLocalAudioStream:NO];
}



- (VideoCanvas *)createVideoCanvasWithUid:(NSString *)uid isLocalCanvas:(BOOL)isLocalCanvas {
    // 创建视频视图
    // Create video canvas
    VideoCanvas *canvas = [[VideoCanvas alloc] init];
    canvas.isLocalCanvas = isLocalCanvas;
    
    // 必须创建canvas时设置其view
    // The view must be set when the canvas is created
    UIView *view = [[UIView alloc] init];
    view.backgroundColor = [UIColor blackColor];
    canvas.view = view;
    
    // 设置视频布局
    // Set video layout
    [canvas setRenderMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];
    
    // 设置用户uid
    // Set user id
    [canvas setUid:uid];
    
    if (isLocalCanvas) {
        // 设置本地视图
        // Set Local canvas
        [_engine setLocalVideoCanvas:canvas];
        // 设置本地视图显示模式
        // Set the local canvas display mode
        [_engine setLocalCanvasScaleMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];
        
        self.localVideoCanvas = canvas;

    } else {
        // 设置远端视图
        // Set remote canvas
        [_engine setRemoteVideoCanvas:canvas];
        // 设置远端视图显示模式
        // Set the remote canvas display mode
        [_engine setRemoteCanvasScaleMode:uid mode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];
    }
    
    return canvas;
}

- (void)clearCanvasViewWithUID:(NSString *)uid
{
    // 创建视频视图
    // Create video canvas
    VideoCanvas *canvas = [[VideoCanvas alloc] init];
    
    // 当用户退出房间后，需要将view置为nil，否则用户重新进入房间会导致以前的view指向野指针
    // When the user exits the room, the view needs to be set to nil, otherwise the user will re-enter the room will cause the previous view to point to the wild pointer
    canvas.view = nil;
    
    // 设置视频布局
    // Set video layout
    [canvas setRenderMode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];
    
    // 设置用户uid
    // Set user id
    [canvas setUid:uid];
    
    // 设置远端视图
    // Set remote canvas
    [_engine setRemoteVideoCanvas:canvas];
    // 设置远端视图显示模式
    // Set remote canvas display mode
    [_engine setRemoteCanvasScaleMode:uid mode:THUNDER_RENDER_MODE_CLIP_TO_BOUNDS];
    
}


- (NSInteger)switchFrontCamera:(BOOL)isFront {
    //  调用成功返回 0，失败返回 < 0
    //  Successful call returns 0, failed returns <0
    return [_engine switchFrontCamera:isFront];
}



- (void)disableLocalVideo:(BOOL)disabled {
    if (disabled) {
        [_engine stopVideoPreview];
    } else {
        [_engine startVideoPreview];
    }
    
    // 开关本地视频发送
    // Switch local video sending
    [_engine stopLocalVideoStream:disabled];

}

- (void)disableRemoteVideo:(NSString *)uid disabled:(BOOL)disabled {
    [_engine stopRemoteVideoStream:uid stopped:disabled];
}


- (void)disableLocalAudio:(BOOL)disabled {
    // 开关本地音频流
    // On or off Local audio stream
    [_engine stopLocalAudioStream:disabled];
    
}

- (void)disableRemoteAudio:(NSString *)uid disabled:(BOOL)disabled {
    [_engine stopRemoteAudioStream:uid stopped:disabled];
}

- (void)recoveryRemoteStream:(NSString *)uid {
    [_engine stopRemoteAudioStream:uid stopped:NO];
    [_engine stopRemoteVideoStream:uid stopped:NO];
}

- (void)recoveryAllRemoteStream {
    // 关闭的流进行恢复
    // Resume closed stream
    [_engine stopAllRemoteVideoStreams:YES]; // 为了让下面的stopAllRemoteVideoStreams生效/In order for the following stopAllRemoteVideoStreams to take effect
    [_engine stopAllRemoteVideoStreams:NO];

    [_engine stopAllRemoteAudioStreams:YES]; // 为了让下面的stopAllRemoteVideoStreams生效/In order for the following stopAllRemoteAideoStreams to take effect
    [_engine stopAllRemoteAudioStreams:NO];
}


- (void)updateToken {
    [_engine updateToken:_token];
}


- (void)switchPublishMode:(ThunderPublishVideoMode)publishMode {
    self.publishMode = publishMode;
    [self setupPublishMode];
    
    [[NSUserDefaults standardUserDefaults] setObject:@(_publishMode) forKey:kUserDefaultsKeyPublishMode];
    [[NSUserDefaults standardUserDefaults] synchronize];
}



#pragma mark - ThunderRtcLogDelegate

- (void)onThunderRtcLogWithLevel:(ThunderRtcLogLevel)level message:(nonnull NSString*)msg {
    kLog(@"【RTC】level=%ld, %@", (long)level, msg);
}


#pragma mark - Get and Set

- (NSString *)appId {
    return kThunderAppId;
}

- (NSString *)localUid {
    if (_localUid == nil) {
        _localUid = [Utils generateUid];
    }
    
    return _localUid;
}





@end
