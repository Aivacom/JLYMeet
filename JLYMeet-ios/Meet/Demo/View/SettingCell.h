//
//  SettingCell.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/21.
//  Copyright © 2019 JLY. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TextField.h"


@class SettingOption;
@class SettingCell;


@protocol SettingCellDelegate <NSObject>

@optional

- (void)tableViewCellDidBeginEditing:(SettingCell *)cell;
- (void)tableViewCell:(SettingCell *)cell didChangedEditingWithText:(NSString *)text;
- (void)tableViewCell:(SettingCell *)cell didEndEditingWithText:(NSString *)text;

@end


@interface SettingCell : UITableViewCell
@property (nonatomic, readonly, strong) SettingOption *option;
@property (nonatomic, readonly, strong) TextField *detailTextField;
@property (nonatomic, readonly, strong) UIImageView *accessoryImageView;
@property (nonatomic, weak) id <SettingCellDelegate> delegate;


- (void)setupDataWithOption:(SettingOption *)option;


@end
