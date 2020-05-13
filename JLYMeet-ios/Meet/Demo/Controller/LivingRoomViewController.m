//
//  LivingRoomViewController.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/6.
//  Copyright © 2019 JLY. All rights reserved.
//

#import "LivingRoomViewController.h"
#import "CommonMacros.h"
#import "Masonry.h"
#import "CanvasCell.h"
#import "ThunderManager.h"
#import "Utils.h"
#import "VideoCanvas.h"
#import "CanvasView.h"
#import "FeedbackViewController.h"
#import "TextField.h"
#import "FeedbackManager.h"
#import "ToolBar.h"
#import "UIViewController+BaseViewController.h"
#import "UIImage+Additions.h"
#import "SettingViewController.h"
#import "MBProgressHUD+HUD.h"
#import "UIViewController+AlertController.h"
#import "UITextField+Additions.h"


static const NSUInteger kMaxInputLength = 8;          // uid输入框最大输入字符数/uid input box maximum number of input characters
static const NSUInteger kMaxLiveConnection = 4;       // 最多允许连接的用户数/Maximum number of users allowed to connect
static const NSUInteger kNumberOfCanvasInRow = 2;     // 每行展示多少个Canvas/How many Canvas to display per line
static NSString * const kCanvasCellReuseIdentifier = @"CanvasCell";

static dispatch_semaphore_t _semaphore;


static NSString * const kFeedbackAppId = @"JLYMeet-ios"; // 对接反馈系统AppID/Docking feedback system AppID


@interface LivingRoomViewController () <ThunderEventDelegate, UICollectionViewDelegateFlowLayout, UICollectionViewDataSource, CanvasViewDelegate, ToolBarDelegate>
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) TextField *roomIDTextField;
@property (nonatomic, strong) UIButton *joinButton;
@property (nonatomic, strong) CanvasView *fullScreenCanvasView;
@property (nonatomic, strong) ToolBar *toolBar;
@property (nonatomic, strong) UITextView *logTextView;
@property (nonatomic, strong) NSMutableArray<VideoCanvas *> *livingCanvases;   // 当前连接的用户Canvas/Canvas of currently connected user
@property (nonatomic, strong) NSMutableArray<CanvasStatus *> *waitingUsersQueue; // 等待加入连接的用户/Users waiting to join the connection
@property (nonatomic, strong) NSMutableDictionary *allUsers;   // 当前所有在房间的用户（只要有流推送就在记录），用于存储livingCanvases和waitingUsersQueue里的所有元素/All current users in the room (as long as there is streaming push records), used to store all elements in livingCanvases and waitingUsersQueue

@property (nonatomic, assign) BOOL hasJoinedRoom;       // 是否已加入房间/Whether you have joined the room
@property (nonatomic, assign) BOOL isUseFrontCamera;    // 是否使用的前置摄像头/Whether to use the front camera

@property (nonatomic, strong) UIAlertController *actionSheet;





@end

@implementation LivingRoomViewController

#pragma mark - Life cycle
- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupBaseSetting];
    [self setupNavigationBarWithBarTintColor:[UIColor whiteColor] titleColor:kColorHex(@"#333333") titleFont:[UIFont boldSystemFontOfSize:17] eliminateSeparatorLine:YES];
    self.navigationController.navigationBar.hidden = YES;
    
    if (@available(iOS 13.0, *)) {
        self.navigationController.overrideUserInterfaceStyle = UIUserInterfaceStyleLight;
    }

    
    _semaphore = dispatch_semaphore_create(1);
    _isUseFrontCamera = YES;
    
    // 初始化SDK
    // Initialize SDK
    [self.thunderManager setupEngineWithDelegate:self];
    
    [self setupSubviews];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    self.navigationController.navigationBar.hidden = NO;
}


#pragma mark - SetupSubviews

- (void)setupSubviews {
    
    // 输入框
    // Inputview
    UIView *inputView = [[UIView alloc] init];
    inputView.backgroundColor = kColorHex(@"#E0E7FE");
    inputView.clipsToBounds = YES;
    inputView.layer.cornerRadius = 20.0f;
    
    [self.view addSubview:inputView];
    [inputView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.mas_equalTo(40 + kStatusBarHeight);
        make.left.mas_equalTo(16);
        make.right.mas_equalTo(-16);
        make.height.mas_equalTo(40);
    }];
    
    self.joinButton = [[UIButton alloc] init];
    _joinButton.titleLabel.font = [UIFont boldSystemFontOfSize:14];
    [_joinButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    UIImage *normalImage = [UIImage imageWithColor:kColorHex(@"#6485F9")];
    UIImage *highlightedImage = [UIImage imageWithColor:kColorHex(@"#3A61ED")];
    
    [_joinButton setBackgroundImage:normalImage forState:UIControlStateNormal];
    [_joinButton setBackgroundImage:highlightedImage forState:UIControlStateHighlighted];
    
    [_joinButton setTitle:@"进入房间" forState:UIControlStateNormal];
    [_joinButton addTarget:self action:@selector(onJoinButtonClick) forControlEvents:UIControlEventTouchUpInside];
    
    [inputView addSubview:_joinButton];
    [_joinButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.right.bottom.mas_equalTo(inputView);
        make.width.mas_equalTo(96);
    }];

    
    
    self.roomIDTextField = [[TextField alloc] init];
    _roomIDTextField.returnKeyType = UIReturnKeyDone;
    _roomIDTextField.keyboardType = UIKeyboardTypeNumberPad;
    _roomIDTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
    _roomIDTextField.placeholder = @"请输入房间ID";
    _roomIDTextField.font = [UIFont systemFontOfSize:14];
    _roomIDTextField.textColor = [[UIColor blackColor] colorWithAlphaComponent:0.3];
    _roomIDTextField.canPaste = NO;
    _roomIDTextField.maxLength = kMaxInputLength;
    _roomIDTextField.allowInputType = TextFieldInputTypeNum;

    
    [inputView addSubview:_roomIDTextField];
    [_roomIDTextField mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.bottom.mas_equalTo(inputView);
        make.left.mas_equalTo(12);
        make.right.mas_equalTo(self.joinButton.mas_left).offset(-5);
    }];
    
    // 直播视图
    // Live view
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    layout.minimumLineSpacing = 0.5;
    layout.minimumInteritemSpacing = 0.5;
    
    self.collectionView = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:layout];
    _collectionView.delegate = self;
    _collectionView.dataSource = self;
    _collectionView.alwaysBounceVertical = YES;
    _collectionView.backgroundColor = [UIColor whiteColor];
    _collectionView.scrollEnabled = NO;
    
    [_collectionView registerClass:[CanvasCell class] forCellWithReuseIdentifier:kCanvasCellReuseIdentifier];
    
    [self.view addSubview:_collectionView];
    [_collectionView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.mas_equalTo(inputView.mas_bottom).offset(48);
        // 展示所有Cell，使其不进行cell重用
        // Show all Cells so that they will not be reused
        make.height.mas_equalTo((kScreenWidth/kNumberOfCanvasInRow) * (kMaxLiveConnection/kNumberOfCanvasInRow));
        make.left.right.mas_equalTo(self.view);
    }];
    
    
    // toolBar
    self.toolBar = [[ToolBar alloc] init];
    _toolBar.delegate = self;
    
    [self.view addSubview:_toolBar];
    
    [_toolBar mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.mas_equalTo(self.view);
        make.height.mas_equalTo(60);
        make.bottom.mas_equalTo(-kBottomOffset);
    }];
}





#pragma mark - Action

- (void)onJoinButtonClick {
    if (_hasJoinedRoom) {
        // 退出房间
        // Exit the room
        [self.thunderManager leaveRoom];
    } else {
        if (_roomIDTextField.text.length == 0) {
            [MBProgressHUD showToast:@"请输入房间ID" ];
            return;
        }
        
        [_roomIDTextField resignFirstResponder];
        
        // 请求权限
        // Request permission
        [Utils requestMediaAccessInViewController:self completionHandler:^(BOOL isAvailable) {
            if (isAvailable) {
                [self requestToken];
            }
        }];
    }
}

- (void)requestToken {
    [MBProgressHUD showActivityIndicator];
    
    // 请求token
    // Request token
    [self.thunderManager requestTokenWithRoomId:_roomIDTextField.text completionHandler:^(BOOL success) {
        if (success) {
            // 进入房间
            // Enter the room
            [self.thunderManager joinRoom:self.roomIDTextField.text];
            
            // 设置进房间超时时间，30S后如还未收到进房间回调，隐藏loading
            // Set the timeout period for entering the room. If the callback for entering the room has not been received after 30S, hide the loading
            [self performSelector:@selector(joinRoomTimeoutHandle) withObject:nil afterDelay:30];
        } else {
            [MBProgressHUD showToast:@"未获取到token，请稍后重试" ];
        }
    }];
}

- (void)joinRoomTimeoutHandle {
    [MBProgressHUD hideActivityIndicator];
}



#pragma mark - ToolBarDelegate

- (void)toolBarDidTapOnCameraSwitchButton:(ToolBar *)toolBar {
    BOOL result = [self.thunderManager switchFrontCamera:!_isUseFrontCamera];
    if (result == 0) {
        _isUseFrontCamera = !_isUseFrontCamera;
    } else {
        [MBProgressHUD showToast:@"切换摄像头失败，请稍后重试"];
    }
}

- (void)toolBarDidTapOnSettingButton:(ToolBar *)toolBar {
    SettingViewController *settingViewController = [[SettingViewController alloc] initWithLiveStatus:_hasJoinedRoom];
    [self.navigationController pushViewController:settingViewController animated:YES];
    
}

- (void)toolBarDidTapOnLogButton:(ToolBar *)toolBar {
    if (self.logTextView.hidden) {
        self.logTextView.hidden = NO;
        [self.view bringSubviewToFront:self.logTextView];
    } else {
        self.logTextView.hidden = YES;
    }
}

- (void)toolBarDidTapOnFeedbackButton:(ToolBar *)toolBar {
    [self onFeedbackButtonClick];
}



#pragma mark - ThunderEventDelegate

/*!
 @brief 进入房间回调/Enter room callback
 @param room 房间名/Room name
 @param uid 用户id/User id
 @elapsed 未实现/none
 */
- (void)thunderEngine: (ThunderEngine* _Nonnull)engine onJoinRoomSuccess:(nonnull NSString* )room withUid:(nonnull NSString*)uid elapsed:(NSInteger)elapsed {
    [MBProgressHUD hideActivityIndicator];
    // 取消超时操作
    // Cancel timeout operation
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(joinRoomTimeoutHandle) object:nil];
    
    //设置视频编码配置
    //Set video encoding configuration
    [self.thunderManager setupPublishMode];
    
    _hasJoinedRoom = YES;
    _roomIDTextField.enabled = NO;
    [_joinButton setTitle:@"退出房间" forState:UIControlStateNormal];
    [_toolBar updateToolButtonsStatusWithLiveStatus:YES];
    
    // 开播
    // Start broadcasting
    [self.thunderManager enableVideoLive];
    
    // 设置屏幕常亮
    // Setting screen is always on
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
    // 加锁，对livingCanvases和waitingUsersQueue的数据处理需要加锁
    // add semaphore，Data processing for livingCanvases and waitingUsersQueue requires locking
    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
    
    // 创建本地视频视图，本地视频视图的canvasStatus主要用于判定本地用户是否关闭音频和视频
    // Create a local video view. The canvasStatus of the local video view is mainly used to determine whether the local user turns off the audio and video
    VideoCanvas *localCanvas = [self.thunderManager createVideoCanvasWithUid:self.localUid isLocalCanvas:YES];
    CanvasStatus *canvasStatus = [[CanvasStatus alloc] init];
    canvasStatus.uid = self.localUid;
    canvasStatus.isAudioStreamStoped = NO;
    canvasStatus.isVideoStreamStoped = NO;
    localCanvas.status = canvasStatus;

    // 有可能进入房间时已收到其他用户音视频流的通知
    // It is possible that when entering the room, other users' audio and video streaming notifications have been received
    [self.livingCanvases insertObject:localCanvas atIndex:0];
    self.allUsers[self.localUid] = localCanvas;
    
    [_collectionView reloadData];
    
    dispatch_semaphore_signal(_semaphore);
    
}

/*!
 @brief 离开房间/Leave room
 */
- (void)thunderEngine: (ThunderEngine* _Nonnull)engine onLeaveRoomWithStats:(ThunderRtcRoomStats* _Nonnull)stats {
    _hasJoinedRoom = NO;
    [_joinButton setTitle:@"进入房间" forState:UIControlStateNormal];
    _roomIDTextField.enabled = YES;
    [_toolBar updateToolButtonsStatusWithLiveStatus:NO];
    
    // 如果正在全屏，则隐藏掉全屏视图
    // If you are in full screen, hide the full screen view
    self.fullScreenCanvasView.hidden = YES;
    [self.fullScreenCanvasView setupWithCanvas:nil];
    
    // 如果显示log，隐藏log
    // If the log is being displayed, hide the log
    self.logTextView.hidden = YES;
    self.logTextView.text = nil;

    // 移除所有存储数据
    // Remove all stored data
    [self.livingCanvases removeAllObjects];
    [self.waitingUsersQueue removeAllObjects];
    [self.allUsers removeAllObjects];
    [_collectionView reloadData];
    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    
    // SDK退出房间后依然会保持之前停止流的状态，所以退出房间时对所有流进行恢复
    // After exiting the room, the SDK will still maintain the state of the previous stop flow, so all flows are restored when exiting the room
    [self.thunderManager recoveryAllRemoteStream];
    
    // 恢复使用前置摄像头
    // Return to front camera
    [self.thunderManager switchFrontCamera:YES];
    
    // 移除token
    // Remove token
    self.thunderManager.token = nil;
}


/*!
 @brief sdk鉴权结果/sdk authentication result
 @param sdkAuthResult 参见ThunderRtcSdkAuthResult/sdkAuthResult See ThunderRtcSdkAuthResult
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine sdkAuthResult:(ThunderRtcSdkAuthResult)sdkAuthResult {
    
}


/*!
 @brief 说话声音音量提示回调/Speaking voice volume prompt callback
 @param speakers 用户Id-用户音量（未实现，音量=totalVolume）/User Id-user volume (not implemented, volume = totalVolume)
 @param totalVolume 混音后总音量/Total volume after mixing
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine onPlayVolumeIndication:(NSArray<ThunderRtcAudioVolumeInfo *> * _Nonnull)speakers
          totalVolume:(NSInteger)totalVolume {
    [speakers enumerateObjectsUsingBlock:^(ThunderRtcAudioVolumeInfo *obj, NSUInteger idx, BOOL *stop) {
        VideoCanvas *canvas = self.allUsers[obj.uid];
        
        if (canvas && [canvas isKindOfClass:[VideoCanvas class]]) {
            // 全屏模式下更新全屏视图
            // Update full screen view in full screen mode
            if (canvas.status.isFullScreen) {
                [self.fullScreenCanvasView setVolume:obj.volume];
            } else {
                CanvasCell *cell = (CanvasCell *)[self.collectionView cellForItemAtIndexPath:canvas.indexPath];
                [cell.canvasView setVolume:obj.volume];
            }
        }
    }];
}

/*!
 @brief 采集声音音量提示回调/Collected sound volume prompt callback
 @param totalVolume 采集总音量（包含麦克风采集和文件播放）/Collect the total volume (including microphone collection and file playback)
 @param cpt 采集时间戳/Collection time stamp
 @param micVolume 麦克风采集音量/Microphone collection volume
 @
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine onCaptureVolumeIndication:(NSInteger)totalVolume cpt:(NSUInteger)cpt micVolume:(NSInteger)micVolume {
    VideoCanvas *localCanvas = self.allUsers[self.localUid];
    
    // 全屏模式下更新全屏视图
    // Update full screen view in full screen mode
    if (localCanvas.status.isFullScreen) {
        [self.fullScreenCanvasView setVolume:micVolume];
    } else {
        CanvasCell *cell = (CanvasCell *)[self.collectionView cellForItemAtIndexPath:localCanvas.indexPath];
        [cell.canvasView setVolume:micVolume];
    }
}


/*!
 @brief 某个Uid用户的音频流状态变化回调/A user's audio stream status change callback
 @param stopped 流是否已经断开（YES:断开 NO:连接）/Whether the stream has been disconnected (YES: disconnected NO: connected)
 @param uid 对应的uid/user id
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine onRemoteAudioStopped:(BOOL)stopped byUid:(nonnull NSString *)uid {
    [self handleRemoteStreamOnStopped:stopped uid:uid streamType:RemoteStreamTypeAudio];
}


/*!
 @brief 某个Uid用户的视频流状态变化回调/Callback of a Uid user's video stream status change
 @param stopped 流是否已经断开（YES:断开 NO:连接）/Whether the stream has been disconnected (YES: disconnected NO: connected)
 @param uid 对应的uid/user id
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine onRemoteVideoStopped:(BOOL)stopped byUid:(nonnull NSString *)uid {
    [self handleRemoteStreamOnStopped:stopped uid:uid streamType:RemoteStreamTypeVideo];
}


- (void)handleRemoteStreamOnStopped:(BOOL)stopped uid:(NSString *)uid streamType:(RemoteStreamType)streamType {
    // 加锁
    // add semaphore
    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);

    if (stopped) {
        [self handleStoppedRemoteStreamWithUid:uid streamType:streamType];
    } else {
        [self handleStartedRemoteStreamWithUid:uid streamType:streamType];
    }
    dispatch_semaphore_signal(_semaphore);

}


- (void)handleStoppedRemoteStreamWithUid:(NSString *)uid streamType:(RemoteStreamType)streamType {
    id obj = self.allUsers[uid];
    
    if ([obj isKindOfClass:[VideoCanvas class]]) {
        VideoCanvas *canvas = (VideoCanvas *)obj;
        [canvas.status updateStatusOnRemoteStreamStopped:YES remoteStreamType:streamType];
        
        // 当音视频流都没有时删除连麦用户
        // Delete users when there is no audio and video stream
        if (canvas.status.livingRetainCount == 0) {
            // 如果正在弹窗操作，关闭弹窗操作
            // If the pop-up operation is in progress, close the pop-up operation
            if (canvas.status.isShowActionSheet && _actionSheet.presentingViewController) {
                [self dismissViewControllerAnimated:YES completion:nil];
            }
            
            // 如果正在全屏，则隐藏掉全屏视图再更新
            // If you are in full screen, hide the full screen view and update
            if (canvas.status.isFullScreen) {
                self.fullScreenCanvasView.hidden = YES;
                [self.fullScreenCanvasView setupWithCanvas:nil];
            }
            
            // 恢复对远程用户音频流和视频流的设置，避免远程用户退出后再次进入房间还保持上次的mute操作
            // Restore the settings of the audio stream and video stream of the remote user to prevent the remote user from re-entering the room after exiting and maintain the previous mute operation
            [self.thunderManager recoveryRemoteStream:canvas.uid];
            
            [self.livingCanvases removeObject:canvas];
            [self.allUsers removeObjectForKey:uid];
            [self.thunderManager clearCanvasViewWithUID:uid];
            
            if (self.waitingUsersQueue.count >0) {
                // 将排队用户加入连麦
                // Add queued users
                CanvasStatus *canvasStatus = self.waitingUsersQueue.firstObject;
                VideoCanvas *remoteCanvas = [self.thunderManager createVideoCanvasWithUid:canvasStatus.uid isLocalCanvas:NO];
                remoteCanvas.status = canvasStatus;
                
                NSString *willJoinUid = canvasStatus.uid;
                
                // 从排队中移除
                // Remove from the queue
                [self.waitingUsersQueue removeObject:canvasStatus];
                [self.allUsers removeObjectForKey:willJoinUid];
                
                // 添加到连麦用户中
                // Add users
                [self.livingCanvases addObject:remoteCanvas];
                self.allUsers[willJoinUid] = remoteCanvas;
            }
            
            // 更新视图
            // Update view
            [_collectionView reloadData];
            
        } else {
            // 处理静音的情况
            // Handling silent situations
            if (canvas.status.isFullScreen) {
                [self.fullScreenCanvasView handleNoVolumeScene];
            } else {
                [_collectionView reloadData];
            }
        }
        

        
        
        // 在排队中
        // In line
    } else if ([obj isKindOfClass:[CanvasStatus class]]) {
        CanvasStatus *canvasStatus = (CanvasStatus *)obj;
        [canvasStatus updateStatusOnRemoteStreamStopped:YES remoteStreamType:streamType];
        
        // 当音视频流都没有时删除排队用户
        // Delete queued users when there are no audio and video streams
        if (canvasStatus.livingRetainCount == 0) {
            [self.waitingUsersQueue removeObject:canvasStatus];
            [self.allUsers removeObjectForKey:uid];
        }
    }
}

- (void)handleStartedRemoteStreamWithUid:(NSString *)uid streamType:(RemoteStreamType)streamType {
    id obj = self.allUsers[uid];
    
    // 说明已创建视图或者已在排队中
    // Indicates that the view has been created or is in the queue
    if (obj) {
        
        CanvasStatus *canvasStatus = nil;
        if ([obj isKindOfClass:[VideoCanvas class]]) {
            VideoCanvas *canvas = (VideoCanvas *)obj;
            canvasStatus = canvas.status;
            
        } else if ([obj isKindOfClass:[CanvasStatus class]]) {
            canvasStatus = (CanvasStatus *)obj;
        }
        
        // 更新音视频流的的状态
        // Update the status of audio and video streams
        [canvasStatus updateStatusOnRemoteStreamStopped:NO remoteStreamType:streamType];

    } else {
        CanvasStatus *canvasStatus = [[CanvasStatus alloc] init];
        canvasStatus.uid = uid;
        // 默认设置远程用户为关闭状态，解决后来用户无法收到之前用户自己关闭音频流通知的问题
        // By default, the remote user is set to the closed state, which solves the problem that the user cannot receive the notification of the user before closing the audio stream later
        canvasStatus.isRemoteCanvasAudioStreamStoped = YES;
        
        // 更新音视频流的的状态
        // Update the status of audio and video streams
        [canvasStatus updateStatusOnRemoteStreamStopped:NO remoteStreamType:streamType];
        
        // There are already 4 people in line with Mai, otherwise mic connection
        if (self.livingCanvases.count == kMaxLiveConnection) {
            [self.waitingUsersQueue addObject:canvasStatus];
            self.allUsers[uid] = canvasStatus;
        } else {
            VideoCanvas *remoteCanvas = [self.thunderManager createVideoCanvasWithUid:uid isLocalCanvas:NO];
            remoteCanvas.status = canvasStatus;
            [self.livingCanvases addObject:remoteCanvas];
            self.allUsers[uid] = remoteCanvas;
            
            [_collectionView reloadData];
        }
    }
}



/*!
 @brief 上下行流量通知 每2s通知一次/Upstream and downstream traffic notifications every 2s
 @param stats 上行下流通统计信息/Upstream and downstream statistics
 */
- (void)thunderEngine:(ThunderEngine *_Nonnull)engine onRoomStats:(nonnull RoomStats*)stats {
    if (!self.logTextView.hidden) {
        self.logTextView.text = [NSString stringWithFormat:@"链路发送码率：%dbps\n链路接收码率：%dbps\n音频包发送码率：%dbps\n音频包接收码率：%dbps\n视频包发送码率：%dbps\n视频包接收码率：%dbps", stats.txBitrate, stats.rxBitrate, stats.txAudioBitrate, stats.rxAudioBitrate, stats.txVideoBitrate, stats.rxVideoBitrate, nil];
    }
}



/*!
 @brief 鉴权服务即将过期回调/Authentication service is about to expire callback
 @param token 即将服务失效的Token/Token that is about to expire
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine onTokenWillExpire:(nonnull NSString*)token {
    kLog(@"Token will expire");
    [self updateToken];
}

/*!
 @brief 鉴权过期回调/Authentication expired callback
 */
- (void)thunderEngineTokenRequest:(ThunderEngine * _Nonnull)engine {
    kLog(@"Token expired");
    
    // 如果onTokenWillExpire更新失败补救请求一次token
    // If onTokenWillExpire update fails, remedial request a token
    [self updateToken];
}

- (void)updateToken {
    // token即将过期时或者已经过期时重新请求
    // Re-request when the token is about to expire or has expired
    self.thunderManager.token = nil;
    [self.thunderManager requestTokenWithRoomId:_roomIDTextField.text completionHandler:^(BOOL success) {
        if (success) {
            // 更新token
            [self.thunderManager updateToken];
        }
    }];
}



/*!
 @brief 网路上下行质量报告回调/Callback for network upstream and downstream quality reports
 @param uid 表示该回调报告的是持有该id的用户的网络质量，当uid为0时，返回的是本地用户的网络质量/Indicates that the callback reports the network quality of the user holding the id. When uid is 0, the network quality of the local user is returned
 @param txQuality 该用户的上行网络质量，参见ThunderLiveRtcNetworkQuality/The user's upstream network quality, see ThunderLiveRtcNetworkQuality
 @param rxQuality 该用户的下行网络质量，参见ThunderLiveRtcNetworkQuality/The user's downlink network quality, see ThunderLiveRtcNetworkQuality
 */
- (void)thunderEngine:(ThunderEngine * _Nonnull)engine onNetworkQuality:(nonnull NSString*)uid txQuality:(ThunderLiveRtcNetworkQuality)txQuality rxQuality:(ThunderLiveRtcNetworkQuality)rxQuality {
    NSString *userUid = uid;
    if ([uid isEqualToString:@"0"]) {
        userUid = self.localUid;
    }
    
    // 只对连麦用户进行处理
    // Only deal with connectioin users
    VideoCanvas *canvas = self.allUsers[userUid];
    if (canvas && [canvas isKindOfClass:[VideoCanvas class]]) {
        // 全屏模式下更新全屏视图
        // Update full screen view in full screen mode
        if (canvas.status.isFullScreen) {
            [self.fullScreenCanvasView setTxQuality:txQuality];
        } else {
            CanvasCell *cell = (CanvasCell *)[self.collectionView cellForItemAtIndexPath:canvas.indexPath];
            [cell.canvasView setTxQuality:txQuality];
        }
    }
    
}





#pragma mark - UICollectionViewDataSource
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return kMaxLiveConnection;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    CanvasCell *cell =[collectionView dequeueReusableCellWithReuseIdentifier:kCanvasCellReuseIdentifier forIndexPath:indexPath];
    
    if (indexPath.row >= self.livingCanvases.count) {
        [cell.canvasView setupWithCanvas:nil];
    } else {
        VideoCanvas *canvas = self.livingCanvases[indexPath.row];
        // 全屏时不在cell中更新
        // Not updated in cell when full screen
        if (canvas.status.isFullScreen) {
            [cell.canvasView setupWithCanvas:nil];
        } else {
            canvas.indexPath = indexPath;
            [cell.canvasView setupWithCanvas:canvas];
        }
    }
    
    return cell;
}


#pragma mark - UICollectionViewDelegateFlowLayout
-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    CGFloat totalWidth = kScreenWidth - (kNumberOfCanvasInRow - 1) * 0.5;
    CGFloat width = totalWidth  / kNumberOfCanvasInRow;
    return  CGSizeMake(width, width);
}



#pragma mark - UICollectionViewDelegate
- (BOOL)collectionView:(UICollectionView *)collectionView shouldHighlightItemAtIndexPath:(NSIndexPath *)indexPath {
    if (indexPath.row >= self.livingCanvases.count) {
        return NO;
    }
    
    return YES;
}


- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    CanvasCell *cell = (CanvasCell *)[collectionView cellForItemAtIndexPath:indexPath];
    VideoCanvas *canvas = cell.canvasView.canvas;
    
    if (canvas.isLocalCanvas) {
        // canvasView和canvas同时传递，是保证这两个值在被reloadData时能一一对应
        // CanvasView and canvas are passed at the same time, which is to ensure that these two values can correspond to each other when reloadData
        [self showOperationActionSheetForLocalCanvas:canvas canvasView:cell.canvasView];
    } else {
        [self showOperationActionSheetForRemoteCanvas:canvas canvasView:cell.canvasView];
    }
}

- (void)showOperationActionSheetForLocalCanvas:(VideoCanvas *)canvas canvasView:(CanvasView *)canvasView {
    NSString *actionTitle1 = canvas.status.isFullScreen?@"恢复":@"放大";
    NSString *actionTitle2 = canvas.status.isVideoStreamStoped?@"打开摄像头":@"关闭摄像头";
    NSString *actionTitle3 = canvas.status.isAudioStreamStoped?@"打开麦克风":@"关闭麦克风";
    
    self.actionSheet = [self actionSheetWithTitle:nil message:nil handler:^(UIAlertAction *action, NSUInteger index) {
        canvas.status.isShowActionSheet = NO;
    
        switch (index) {
            case 0:
                [self updateFullScreenCanvasViewStatusWithCanvas:canvas];
                break;
            case 1:
                canvas.status.isVideoStreamStoped = !canvas.status.isVideoStreamStoped;
                [self.thunderManager disableLocalVideo:canvas.status.isVideoStreamStoped];
                break;
            case 2:
                canvas.status.isAudioStreamStoped = !canvas.status.isAudioStreamStoped;
                [self.thunderManager disableLocalAudio:canvas.status.isAudioStreamStoped];
                // 处理静音场景
                // Handling silent scenes
                [canvasView handleNoVolumeScene];
                break;
        }
    } otherActionTitles:actionTitle1, actionTitle2, actionTitle3, nil];
    
    [self showActionSheetInCanvasView:canvasView];
    canvas.status.isShowActionSheet = YES;

}




- (void)showOperationActionSheetForRemoteCanvas:(VideoCanvas *)canvas canvasView:(CanvasView *)canvasView {
    NSString *actionTitle1 = canvas.status.isFullScreen?@"恢复":@"放大";
    NSString *actionTitle2 = canvas.status.isVideoStreamStoped?@"打开视频":@"关闭视频";
    NSString *actionTitle3 = canvas.status.isAudioStreamStoped?@"打开音频":@"关闭音频";
    
    self.actionSheet = [self actionSheetWithTitle:nil message:nil handler:^(UIAlertAction *action, NSUInteger index) {
        canvas.status.isShowActionSheet = NO;
        
        switch (index) {
            case 0:
                [self updateFullScreenCanvasViewStatusWithCanvas:canvas];
                break;
            case 1:
                canvas.status.isVideoStreamStoped = !canvas.status.isVideoStreamStoped;
                [self.thunderManager disableRemoteVideo:canvas.uid disabled:canvas.status.isVideoStreamStoped];
                break;
            case 2:
                canvas.status.isAudioStreamStoped = !canvas.status.isAudioStreamStoped;
                [self.thunderManager disableRemoteAudio:canvas.uid disabled:canvas.status.isAudioStreamStoped];
                
                // 处理静音场景
                // Handling silent scenes
                [canvasView handleNoVolumeScene];
                break;
        }
    } otherActionTitles:actionTitle1, actionTitle2, actionTitle3, nil];
    
    [self showActionSheetInCanvasView:canvasView];
    canvas.status.isShowActionSheet = YES;
}

- (void)showActionSheetInCanvasView:(CanvasView *)canvasView {
    
    // 强行设置浅色
    // Force light
    if (@available(iOS 13.0, *)) {
        self.actionSheet.overrideUserInterfaceStyle = UIUserInterfaceStyleLight;
    }
    
    // 对iPad做处理
    // Handle iPad
    if (self.actionSheet.preferredStyle == UIAlertControllerStyleActionSheet && [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        UIPopoverPresentationController *popPresenter = self.actionSheet.popoverPresentationController;
        popPresenter.sourceView = canvasView;
        popPresenter.sourceRect = CGRectMake(canvasView.bounds.size.width / 2, canvasView.bounds.size.height / 2, 0, 0);
        popPresenter.permittedArrowDirections = UIPopoverArrowDirectionUp;
    }
    
    [self presentViewController: self.actionSheet animated:YES completion:nil];
}


- (void)updateFullScreenCanvasViewStatusWithCanvas:(VideoCanvas *)canvas {
    if (canvas.status.isFullScreen) {
        self.fullScreenCanvasView.hidden = YES;
        canvas.status.isFullScreen = !canvas.status.isFullScreen;
        // 重新全部更新
        // Update all again
        [self.collectionView reloadData];
        
    } else {
        canvas.status.isFullScreen = !canvas.status.isFullScreen;
        [self.fullScreenCanvasView setupWithCanvas:canvas];
        self.fullScreenCanvasView.hidden = NO;
    }
}


#pragma mark - CanvasViewDelegate

- (void)canvasViewDidTapped:(CanvasView *)canvasView {
    VideoCanvas *canvas = canvasView.canvas;
    
    if (canvas.isLocalCanvas) {
        [self showOperationActionSheetForLocalCanvas:canvas canvasView:canvasView];
    } else {
        [self showOperationActionSheetForRemoteCanvas:canvas canvasView:canvasView];
    }
}


#pragma mark - Feedback


- (void)onFeedbackButtonClick {
    [self setupFeedbackManagerOnce];
    
    FeedbackViewController *feedbackViewController = [[FeedbackViewController alloc] initWithUid:self.localUid];
    UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:feedbackViewController];
    [self presentViewController:navController animated:YES completion:nil];
}

- (void)setupFeedbackManagerOnce {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [FeedbackManager sharedManager].appId = kFeedbackAppId;
        [FeedbackManager sharedManager].appSceneName = @"meet";
        [FeedbackManager sharedManager].functionDesc = @"1、实现同房间连麦互动\n2、具备麦克风静音，摄像头切换，mute音视频流等功能";
        [FeedbackManager sharedManager].logFilePath = self.thunderManager.logPath;
    });
}



#pragma mark - Get and Set

- (ThunderManager *)thunderManager {
    return [ThunderManager sharedManager];
}


- (NSString *)localUid {
    return [ThunderManager sharedManager].localUid;
}

- (NSMutableArray *)livingCanvases {
    if (_livingCanvases == nil) {
        _livingCanvases = [[NSMutableArray alloc] initWithCapacity:kMaxLiveConnection];
    }
    
    return _livingCanvases;
}


- (NSMutableArray *)waitingUsersQueue {
    if (_waitingUsersQueue == nil) {
        _waitingUsersQueue = [[NSMutableArray alloc] init];
    }
    
    return _waitingUsersQueue;
}

- (NSMutableDictionary *)allUsers {
    if (_allUsers == nil) {
        _allUsers = [[NSMutableDictionary alloc] init];
    }
    return _allUsers;
}

- (CanvasView *)fullScreenCanvasView {
    if (_fullScreenCanvasView == nil) {
        _fullScreenCanvasView = [[CanvasView alloc] init];
        _fullScreenCanvasView.delegate = self;
        [_fullScreenCanvasView enableTapEvent];
        _fullScreenCanvasView.hidden = NO;
        
        [self.view addSubview:_fullScreenCanvasView];
        
        [_fullScreenCanvasView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.top.bottom.mas_equalTo(self.collectionView);
        }];
    }
    
    return _fullScreenCanvasView;
}

- (UITextView *)logTextView {
    if (_logTextView == nil) {
        _logTextView = [[UITextView alloc] init];
        _logTextView.editable = NO;
        _logTextView.hidden = YES;
        _logTextView.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.5];
        _logTextView.textColor = [UIColor whiteColor];
        
        [self.view addSubview:_logTextView];
        [_logTextView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.mas_equalTo(self.view);
            make.bottom.mas_equalTo(self.collectionView.mas_bottom);
            make.width.mas_equalTo(175);
            make.height.mas_equalTo(206);
        }];
    }
    
    return _logTextView;
}


@end

