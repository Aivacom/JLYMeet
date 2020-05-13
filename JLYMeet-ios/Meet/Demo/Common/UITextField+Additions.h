//
//  UITextField+Additions.h
//  SCClassRoom
//
//  Created by GasparChu on 2020/3/10.
//  Copyright © 2020 JLY. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN
// 允许输入的类型
// Types allowed
typedef NS_ENUM(NSInteger, TextFieldInputType) {
    TextFieldInputTypeNum = 1,    // 只允许输入数字/Only numbers are allowed
};


@interface UITextField (Additions)<UITextFieldDelegate>

@property (nonatomic, assign) NSUInteger maxLength; // 输入最大长度/Enter the maximum length
@property (nonatomic, assign) TextFieldInputType allowInputType;  // 允许输入的类型/Types allowed

@end

NS_ASSUME_NONNULL_END
