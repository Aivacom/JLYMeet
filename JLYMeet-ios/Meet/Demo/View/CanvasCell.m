//
//  CanvasCell.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/7.
//  Copyright © 2019 JLY. All rights reserved.
//

#import "CanvasCell.h"
#import "Masonry.h"
#import "CanvasView.h"




@interface CanvasCell ()
@property (nonatomic, readwrite, strong) CanvasView *canvasView;


@end

@implementation CanvasCell

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setupSubviews];
    }
    return self;
}

#pragma mark - Private

- (void)setupSubviews {
    self.canvasView = [[CanvasView alloc] init];
    
    [self.contentView addSubview:_canvasView];
    [_canvasView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.top.bottom.mas_equalTo(self.contentView );
    }];
}



@end
