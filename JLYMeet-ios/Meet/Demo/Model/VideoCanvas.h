//
//  VideoCanvas.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/8.
//  Copyright © 2019 JLY. All rights reserved.
//


#import "ThunderEngine.h"
#import "CanvasStatus.h"

@interface VideoCanvas : ThunderVideoCanvas
// 是否本地Canvas
// Whether local Canvas
@property (nonatomic, assign) BOOL isLocalCanvas;
@property (nonatomic, strong) NSIndexPath *indexPath;
// 用于记录音视频流的状态
// Used to record the status of audio and video streams
@property (nonatomic, strong) CanvasStatus *status;



@end
