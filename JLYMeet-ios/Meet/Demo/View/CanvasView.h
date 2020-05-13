//
//  CanvasView.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/6.
//  Copyright © 2019 JLY. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ThunderEngine.h"


@class CanvasView;
@class VideoCanvas;


@protocol CanvasViewDelegate <NSObject>

@optional

- (void)canvasViewDidTapped:(CanvasView *)canvasView;


@end

@interface CanvasView : UIView
@property (nonatomic, readonly, strong) VideoCanvas *canvas;
@property (nonatomic, weak) id <CanvasViewDelegate> delegate;



- (void)enableTapEvent;

- (void)setupWithCanvas:(VideoCanvas *)canvas;

- (void)setVolume:(NSUInteger)volume;

- (void)setTxQuality:(ThunderLiveRtcNetworkQuality)txQuality;

- (void)handleNoVolumeScene;


@end
