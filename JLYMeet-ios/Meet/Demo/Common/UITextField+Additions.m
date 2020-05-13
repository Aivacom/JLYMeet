//
//  UITextField+Additions.m
//  SCClassRoom
//
//  Created by GasparChu on 2020/3/10.
//  Copyright © 2020 JLY. All rights reserved.
//

#import "UITextField+Additions.h"
#import <objc/runtime.h>

@implementation UITextField (Additions)


- (void)setMaxInputLenth:(NSInteger)maxLen
{
    NSString *toBeString = self.text;
    //获取高亮部分
    //Get highlight
    UITextRange *selectedRange = [self markedTextRange];
    UITextPosition *position = [self positionFromPosition:selectedRange.start offset:0];
    
    if (!position || !selectedRange) {
        if (toBeString.length > maxLen) {
            NSRange rangeIndex = [toBeString rangeOfComposedCharacterSequenceAtIndex:maxLen];
            if (rangeIndex.length == 1) {
                self.text = [toBeString substringToIndex:maxLen];
            }
            else {
                NSRange rangeRange = [toBeString rangeOfComposedCharacterSequencesForRange:NSMakeRange(0, maxLen)];
                self.text = [toBeString substringWithRange:rangeRange];
            }
        }
    }
}

- (BOOL)isNum:(NSString *)string
{
    if (0 == string.length) {
        return NO;
    }
    NSString *regex = @"[0-9]*";
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    if ([predicate evaluateWithObject:string]) {
        return YES;
    }
    return NO;
}

#pragma mark - Event
- (void)textEditingChanged:(UITextField *)textField
{
    if (self.maxLength) {
        [textField setMaxInputLenth:self.maxLength];
    }
}

#pragma mark - UITextFieldDelegate
- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    if ((TextFieldInputTypeNum == self.allowInputType) && string.length && ![self isNum:string]) {
        return NO;
    }
    return YES;
}

#pragma mark - Getter & Setter
- (void)setMaxLength:(NSUInteger)maxLength
{
    objc_setAssociatedObject(self, @selector(maxLength), @(maxLength), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self addTarget:self action:@selector(textEditingChanged:) forControlEvents:UIControlEventEditingChanged];
    self.delegate = self;
}

- (NSUInteger)maxLength
{
    NSNumber *maxLength = objc_getAssociatedObject(self, @selector(maxLength));
    return maxLength.unsignedIntValue;
}

- (void)setAllowInputType:(TextFieldInputType)allowInputType
{
    objc_setAssociatedObject(self, @selector(allowInputType), @(allowInputType), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    self.delegate = self;
}

- (TextFieldInputType)allowInputType
{
    NSNumber *maxLength = objc_getAssociatedObject(self, @selector(allowInputType));
    return maxLength.integerValue;
}

@end
