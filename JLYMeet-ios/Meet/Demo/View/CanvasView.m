//
//  CanvasView.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/6.
//  Copyright © 2019 JLY. All rights reserved.
//

#import "CanvasView.h"
#import "Masonry.h"
#import "CommonMacros.h"
#import "VideoCanvas.h"
#import "ThunderManager.h"



@interface CanvasView ()
@property (nonatomic, readwrite, strong) VideoCanvas *canvas;
@property (nonatomic, strong) UILabel *signalLabel;
@property (nonatomic, strong) UILabel *uidLabel;
@property (nonatomic, strong) UIImageView *volumeImageView;



@end

@implementation CanvasView

#pragma mark - init

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setupSubviews];
    }
    return self;
}


#pragma mark - Private

- (void)setupSubviews {
    self.backgroundColor = kColorHex(@"#EDEFF5");
    
    self.signalLabel = [[UILabel alloc] init];
    _signalLabel.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.5];
    _signalLabel.layer.cornerRadius = 10.0f;
    _signalLabel.clipsToBounds = YES;
    _signalLabel.textColor = [UIColor whiteColor];
    _signalLabel.font = [UIFont systemFontOfSize:10];
    _signalLabel.textAlignment = NSTextAlignmentCenter;
    _signalLabel.text = @"网络信号较弱";
    _signalLabel.hidden = YES;
    
    [self addSubview:_signalLabel];
    [_signalLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.mas_equalTo(8);
        make.right.mas_equalTo(-8);
        make.width.mas_equalTo(80);
        make.height.mas_equalTo(20);
    }];
    
    
    self.uidLabel = [[UILabel alloc] init];
    _uidLabel.textColor = [UIColor whiteColor];
    _uidLabel.font = [UIFont systemFontOfSize:10];
    
    [self addSubview:_uidLabel];
    [_uidLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.mas_equalTo(9);
        make.bottom.mas_equalTo(-8);
        make.height.mas_equalTo(10);
    }];
    
    
    self.volumeImageView = [[UIImageView alloc] initWithImage:kImageNamed(@"volume_no")];
    
    [self addSubview:_volumeImageView];
    [_volumeImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.right.mas_equalTo(-7);
        make.bottom.mas_equalTo(-8);
        make.width.mas_equalTo(16);
        make.height.mas_equalTo(12);
    }];
}



- (void)topOnCanvasView {
    if (_delegate && [_delegate respondsToSelector:@selector(canvasViewDidTapped:)]) {
        [_delegate canvasViewDidTapped:self];
    }
}


#pragma mark - Public

- (void)enableTapEvent {
    UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(topOnCanvasView)];
    [self addGestureRecognizer:tapGestureRecognizer];
}



- (void)setupWithCanvas:(VideoCanvas *)canvas {
    
    // 移除原来的视频视图，必须是在当前cell上才移除，否则有可能移除已经添加在别的cell上的视图
    // To remove the original video view, it must be removed on the current cell, otherwise it is possible to remove the view that has been added on another cell
    if ([_canvas.view isDescendantOfView:self]) {
        [_canvas.view removeFromSuperview];
    }
    
    
    self.canvas = canvas;
    
    if (_canvas) {
        // 设置视频视图View
        // Set Video View
        _uidLabel.text = _canvas.uid;

        
        // 设置静音
        if (_canvas.status.isAudioStreamStoped || _canvas.status.isRemoteCanvasAudioStreamStoped) {
            [self setNoVolume];
        } else {
            [self setDefaultVolume];
        }
        
        // 有可能该视频视图还未从原来的父视图中移除
        // It is possible that the video view has not been removed from the original parent view
        [_canvas.view removeFromSuperview];
        
        [self insertSubview:_canvas.view belowSubview:_signalLabel];
        [_canvas.view mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.top.bottom.mas_equalTo(self);
        }];
        
        
    } else {
        _uidLabel.text = @"";
        _volumeImageView.hidden = YES;
    }
    
    _signalLabel.hidden = YES;
}


- (void)setVolume:(NSUInteger)volume {
    // 自己关闭音频流或者关闭远程的音频流后不再接受音量的回调
    // Close the audio stream yourself or close the remote audio stream and no longer accept the volume callback
    if (_canvas.status.isAudioStreamStoped || _canvas.status.isRemoteCanvasAudioStreamStoped) {
        return;
    }
    
    _volumeImageView.hidden = NO;
    
    if (volume >= 0 && volume <=30) {
        _volumeImageView.image = kImageNamed(@"volume_1");
    } else if (volume > 31 && volume <=60) {
        _volumeImageView.image = kImageNamed(@"volume_2");
    } else if (volume > 61 && volume <=100) {
        _volumeImageView.image = kImageNamed(@"volume_3");
    }
}

- (void)setDefaultVolume {
    _volumeImageView.hidden = NO;
    _volumeImageView.image = kImageNamed(@"volume_1");
}

- (void)setNoVolume {
    _volumeImageView.hidden = NO;
    _volumeImageView.image = kImageNamed(@"volume_no");
}

- (void)setTxQuality:(ThunderLiveRtcNetworkQuality)txQuality {
    _signalLabel.hidden = txQuality != THUNDER_SDK_NETWORK_QUALITY_VBAD;
}

- (void)handleNoVolumeScene {
    // 本地用户自己关的和主动关闭远程用户的以及远程用户自己关闭的都显示静音图标
    // The mute icon is displayed by the local user and the remote user who actively closes it and the remote user closes it
    if (self.canvas.status.isAudioStreamStoped || self.canvas.status.isRemoteCanvasAudioStreamStoped) {
        [self setNoVolume];
    }
    
}

@end
