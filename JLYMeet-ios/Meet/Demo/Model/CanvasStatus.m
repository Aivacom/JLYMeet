//
//  CanvasStatus.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/9.
//  Copyright © 2019 JLY. All rights reserved.
//


#import "CanvasStatus.h"

@interface CanvasStatus ()

@end

@implementation CanvasStatus


- (void)updateStatusOnRemoteStreamStopped:(BOOL)stopped remoteStreamType:(RemoteStreamType)streamType {
    if (stopped) {
        // 保留计算减1
        // calculation minus 1
        _livingRetainCount--;
    } else {
        // 保留计算加1
        // calculation plus 1
        _livingRetainCount++;
    }
    
    if (streamType == RemoteStreamTypeAudio) {
        self.isRemoteCanvasAudioStreamStoped = stopped;
    }
}

@end
