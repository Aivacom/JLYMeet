//
//  SettingOption.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/21.
//  Copyright © 2019 JLY. All rights reserved.
//


#import <Foundation/Foundation.h>

@interface SettingOption : NSObject

@property (nonatomic, copy) NSString *title;
@property (nonatomic, copy) NSString *value;
@property (nonatomic, assign) BOOL userInteractionEnabled; // 默认YES/ Default YES
@property (nonatomic, assign) BOOL showAccessoryImageView; // 默认YES/ Default YES



@end
