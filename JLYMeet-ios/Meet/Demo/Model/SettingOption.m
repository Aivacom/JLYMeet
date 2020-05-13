//
//  SettingOption.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/21.
//  Copyright © 2019 JLY. All rights reserved.
//


#import "SettingOption.h"

@interface SettingOption ()

@end

@implementation SettingOption

- (instancetype)init {
    self = [super init];
    if (self) {
        _userInteractionEnabled = YES;
        _showAccessoryImageView = YES;
    }
    return self;
}

@end
