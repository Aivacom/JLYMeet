//
//  CanvasCell.h
//  JLYMeet
//
//  Created by iPhuan on 2019/8/7.
//  Copyright © 2019 JLY. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ThunderEngine.h"

@class CanvasView;


@interface CanvasCell : UICollectionViewCell

@property (nonatomic, readonly, strong) CanvasView *canvasView;


@end
