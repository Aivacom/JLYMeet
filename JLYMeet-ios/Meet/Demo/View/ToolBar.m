//
//  ToolBar.m
//  SCloudLive
//
//  Created by iPhuan on 2019/8/20.
//  Copyright © 2019 JLY. All rights reserved.
//

#import "ToolBar.h"
#import "Masonry.h"
#import "CommonMacros.h"



@interface ToolBar ()
@property (nonatomic, strong) UIButton *cameraSwitchButton;
@property (nonatomic, strong) UIButton *settingButton;
@property (nonatomic, strong) UIButton *logButton;

@end

@implementation ToolBar

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:CGRectMake(0, 0, kScreenWidth, 60)];
    if (self) {
        [self setupSubviews];
    }
    return self;
}

#pragma mark - Private

- (void)setupSubviews {
    self.backgroundColor = [UIColor whiteColor];
    
    // 功能按钮
    self.cameraSwitchButton = [[UIButton alloc] init];
    [_cameraSwitchButton setBackgroundImage:kImageNamed(@"toolbar_btn_camera_switch") forState:UIControlStateNormal];
    [_cameraSwitchButton setBackgroundImage:kImageNamed(@"toolbar_btn_camera_switch_tap") forState:UIControlStateHighlighted];
    [_cameraSwitchButton setBackgroundImage:kImageNamed(@"toolbar_btn_camera_switch_tap") forState:UIControlStateDisabled];
    
    [_cameraSwitchButton addTarget:self action:@selector(onCameraSwitchButtonClick) forControlEvents:UIControlEventTouchUpInside];
    
    [self addSubview:_cameraSwitchButton];
    [_cameraSwitchButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.height.mas_equalTo(36);
        make.left.mas_equalTo(12);
        make.bottom.mas_equalTo(-12);
    }];
    
    
    
    self.settingButton = [[UIButton alloc] init];
    [_settingButton setBackgroundImage:kImageNamed(@"toolbar_btn_setting") forState:UIControlStateNormal];
    [_settingButton setBackgroundImage:kImageNamed(@"toolbar_btn_setting_tap") forState:UIControlStateHighlighted];
    [_logButton setBackgroundImage:kImageNamed(@"toolbar_btn_setting_tap") forState:UIControlStateDisabled];

    
    [_settingButton addTarget:self action:@selector(onSettingButtonClick) forControlEvents:UIControlEventTouchUpInside];
    
    [self addSubview:_settingButton];
    [_settingButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.height.mas_equalTo(36);
        make.left.mas_equalTo(self.cameraSwitchButton.mas_right).offset(12);
        make.bottom.mas_equalTo(-12);
    }];
    
    
    
    UIButton *feedbackButton = [[UIButton alloc] init];
    [feedbackButton setBackgroundImage:kImageNamed(@"toolbar_btn_report") forState:UIControlStateNormal];
    [feedbackButton setBackgroundImage:kImageNamed(@"toolbar_btn_report_tap") forState:UIControlStateHighlighted];
    
    
    [feedbackButton addTarget:self action:@selector(onFeedbackButtonClick) forControlEvents:UIControlEventTouchUpInside];
    
    [self addSubview:feedbackButton];
    [feedbackButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.height.mas_equalTo(36);
        make.right.mas_equalTo(-12);
        make.bottom.mas_equalTo(-12);
    }];
    
    
    self.logButton = [[UIButton alloc] init];
    [_logButton setBackgroundImage:kImageNamed(@"toolbar_btn_log") forState:UIControlStateNormal];
    [_logButton setBackgroundImage:kImageNamed(@"toolbar_btn_log_tap") forState:UIControlStateHighlighted];
    [_logButton setBackgroundImage:kImageNamed(@"toolbar_btn_log_tap") forState:UIControlStateDisabled];
    
    [_logButton addTarget:self action:@selector(onLogButtonClick) forControlEvents:UIControlEventTouchUpInside];
    
    [self addSubview:_logButton];
    [_logButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.height.mas_equalTo(36);
        make.right.mas_equalTo(feedbackButton.mas_left).offset(-12);
        make.bottom.mas_equalTo(-12);
    }];
    
    
    [self updateToolButtonsStatusWithLiveStatus:NO];
}


#pragma mark - Action

- (void)onCameraSwitchButtonClick {
    if (_delegate && [_delegate respondsToSelector:@selector(toolBarDidTapOnCameraSwitchButton:)]) {
        [_delegate toolBarDidTapOnCameraSwitchButton:self];
    }
}


- (void)onSettingButtonClick {
    if (_delegate && [_delegate respondsToSelector:@selector(toolBarDidTapOnSettingButton:)]) {
        [_delegate toolBarDidTapOnSettingButton:self];
    }
}


- (void)onLogButtonClick {
    if (_delegate && [_delegate respondsToSelector:@selector(toolBarDidTapOnLogButton:)]) {
        [_delegate toolBarDidTapOnLogButton:self];
    }
}

- (void)onFeedbackButtonClick {
    if (_delegate && [_delegate respondsToSelector:@selector(toolBarDidTapOnFeedbackButton:)]) {
        [_delegate toolBarDidTapOnFeedbackButton:self];
    }
}


#pragma mark - Public

- (void)updateToolButtonsStatusWithLiveStatus:(BOOL)hasJoinedRoom {
    _cameraSwitchButton.enabled = hasJoinedRoom;
    _logButton.enabled = hasJoinedRoom;
}




@end
