//
//  SettingCell.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/21.
//  Copyright © 2019 JLY. All rights reserved.
//

#import "SettingCell.h"
#import "Masonry.h"
#import "SettingOption.h"
#import "CommonMacros.h"



@interface SettingCell () <UITextFieldDelegate>
@property (nonatomic, readwrite, strong) SettingOption *option;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, readwrite, strong) TextField *detailTextField;
@property (nonatomic, readwrite, strong) UIImageView *accessoryImageView;
@property (nonatomic, strong) UITapGestureRecognizer *tap;

@end

@implementation SettingCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        [self setupSubviews];
    }
    return self;
}


#pragma mark - Private

- (void)setupSubviews {
    self.selectedBackgroundView = [[UIView alloc] init];
    
    UIView *selectedView = [[UIView alloc] init];
    selectedView.backgroundColor = kColorHex(@"#EDEEF5");
    selectedView.layer.cornerRadius = 4;
    selectedView.layer.masksToBounds = YES;
    selectedView.layer.borderWidth = 1;
    selectedView.layer.borderColor = kColorHex(@"#E6E6E6").CGColor;
    
    [self.selectedBackgroundView addSubview:selectedView];
    [selectedView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.mas_equalTo(20);
        make.right.mas_equalTo(-20);
        make.top.mas_equalTo(10);
        make.bottom.mas_equalTo(-10);
    }];

    
    self.contentView.layer.cornerRadius = 4;
    self.contentView.layer.masksToBounds = YES;
    self.contentView.layer.borderWidth = 1;
    self.contentView.layer.borderColor = kColorHex(@"#E6E6E6").CGColor;
    
    [self.contentView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.mas_equalTo(20);
        make.right.mas_equalTo(-20);
        make.top.mas_equalTo(10);
        make.bottom.mas_equalTo(-10);
    }];
    
    
    self.titleLabel = [[UILabel alloc] init];
    _titleLabel.font = [UIFont systemFontOfSize:15];
    _titleLabel.textColor = [UIColor blackColor];
    _titleLabel.textAlignment = NSTextAlignmentLeft;
    
    [self.contentView addSubview:_titleLabel];
    [_titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.mas_equalTo(15);
        make.width.mas_equalTo(80);
        make.height.mas_equalTo(self.contentView);
        make.centerY.mas_equalTo(self.contentView);
    }];
    
    
    self.detailTextField = [[TextField alloc] init];
    _detailTextField.userInteractionEnabled = NO;
    _detailTextField.returnKeyType = UIReturnKeyDone;
    _detailTextField.keyboardType = UIKeyboardTypeNumberPad;
    _detailTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
    _detailTextField.font = [UIFont systemFontOfSize:15];
    _detailTextField.textColor = kColorHex(@"#666666");
    _detailTextField.textAlignment = NSTextAlignmentRight;
    _detailTextField.canPaste = NO;
    _detailTextField.maxInputLength = 6;
    _detailTextField.delegate = self;
    [_detailTextField addTarget:self action:@selector(textFieldDidChangedEditing:) forControlEvents:UIControlEventEditingChanged];
    
    
    
    [self.contentView addSubview:_detailTextField];
    [_detailTextField mas_makeConstraints:^(MASConstraintMaker *make) {
        make.right.mas_equalTo(-14);
        make.left.mas_equalTo(self.titleLabel.mas_right).offset(30);
        make.height.mas_equalTo(self.contentView);
        make.centerY.mas_equalTo(self.contentView);
    }];
    
    
    self.accessoryImageView = [[UIImageView alloc] initWithImage:kImageNamed(@"ic_more")];
    
    [self.contentView addSubview:_accessoryImageView];
    [_accessoryImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.right.mas_equalTo(-10);
        make.width.height.mas_equalTo(16);
        make.centerY.mas_equalTo(self.contentView);
    }];
    
}


#pragma mark - UIControlEventEditingChanged

- (void)textFieldDidChangedEditing:(UITextField *)textField {
    self.option.value = textField.text;
    
    if (_delegate && [_delegate respondsToSelector:@selector(tableViewCell:didChangedEditingWithText:)]) {
        [_delegate tableViewCell:self didChangedEditingWithText:textField.text];
    }
}

#pragma mark - UITextFieldDelegate

- (void)textFieldDidBeginEditing:(UITextField *)textField {
    if (_delegate && [_delegate respondsToSelector:@selector(tableViewCellDidBeginEditing:)]) {
        [_delegate tableViewCellDidBeginEditing:self];
    }
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
    self.option.value = textField.text;
    
    if (_delegate && [_delegate respondsToSelector:@selector(tableViewCell:didEndEditingWithText:)]) {
        [_delegate tableViewCell:self didEndEditingWithText:textField.text];
    }
}




#pragma mark - Public
- (void)setupDataWithOption:(SettingOption *)option {
    self.option = option;
    
    _titleLabel.text = _option.title;
    _detailTextField.text = _option.value;
    
    self.userInteractionEnabled = _option.userInteractionEnabled;
    
    _accessoryImageView.hidden = !_option.showAccessoryImageView;
    
    if (_option.showAccessoryImageView) {
        _detailTextField.userInteractionEnabled = NO;
        
        if (_tap) {
            [self.contentView removeGestureRecognizer:_tap];
        }
        
    } else {
        _detailTextField.userInteractionEnabled = YES;
        
        if (_tap == nil) {
            self.tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapOnCell)];
            [self.contentView addGestureRecognizer:_tap];
        }
    }
    
    [_detailTextField mas_updateConstraints:^(MASConstraintMaker *make) {
        if (self.option.showAccessoryImageView) {
            make.right.mas_equalTo(-31);
        } else {
            make.right.mas_equalTo(-14);
        }
    }];
}

- (void)tapOnCell {
    [_detailTextField becomeFirstResponder];
}



@end

