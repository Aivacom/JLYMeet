//
//  ToolBar.h
//  SCloudLive
//
//  Created by iPhuan on 2019/8/20.
//  Copyright © 2019 JLY. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ToolBar;

@protocol ToolBarDelegate <NSObject>

@optional


- (void)toolBarDidTapOnCameraSwitchButton:(ToolBar *)toolBar;
- (void)toolBarDidTapOnSettingButton:(ToolBar *)toolBar;
- (void)toolBarDidTapOnLogButton:(ToolBar *)toolBar;
- (void)toolBarDidTapOnFeedbackButton:(ToolBar *)toolBar;

@end

    

@interface ToolBar : UIView
@property (nonatomic, weak) id <ToolBarDelegate> delegate;


- (void)updateToolButtonsStatusWithLiveStatus:(BOOL)hasJoinedRoom;

@end
