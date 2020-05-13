//
//  SettingViewController.m
//  JLYMeet
//
//  Created by iPhuan on 2019/8/21.
//  Copyright © 2019 JLY. All rights reserved.
//

#import "SettingViewController.h"
#import "UIViewController+BaseViewController.h"
#import "SettingCell.h"
#import "ThunderEngine.h"
#import "SettingOption.h"
#import "ThunderManager.h"
#import "UIViewController+AlertController.h"
#import "MBProgressHUD+HUD.h"



static NSString * const kSettingCellReuseIdentifier = @"SettingCell";


@interface SettingViewController () <UITableViewDelegate, UITableViewDataSource>
@property (nonatomic, strong) UITableView *tableView;
@property (nonatomic, copy) NSArray *options;
@property (nonatomic, assign) BOOL hasJoinedRoom;


@end

@implementation SettingViewController

#pragma mark - Life cycle
- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"设置";
    [self setupCommonSetting];
    [self setBackBarButtonItemAction:@selector(onBackBarButtonClick:)];
    
    [self setupSubviews];
}


#pragma mark - Private

- (void)setupSubviews {
    self.tableView = [[UITableView alloc]initWithFrame:self.view.bounds];
    _tableView.delegate = self;
    _tableView.dataSource = self;
    _tableView.scrollEnabled = NO;
    _tableView.contentInset = UIEdgeInsetsMake(10, 0, 0, 0);
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.view addSubview:_tableView];
    _tableView.tableFooterView = [[UIView alloc] init];
    [_tableView registerClass:[SettingCell class] forCellReuseIdentifier:kSettingCellReuseIdentifier];
}

#pragma mark - Public

- (instancetype)initWithLiveStatus:(BOOL)hasJoinedRoom {
    self = [super init];
    if (self) {
        _hasJoinedRoom = hasJoinedRoom;
    }
    
    return self;
}



#pragma mark - Action
- (void)onBackBarButtonClick:(id)sender {

    SettingOption *uidOption = self.options[0];
    if (uidOption.value.length == 0) {
        [MBProgressHUD showToast:@"请输入UID"];
        return;
    }
    
    [self.navigationController popViewControllerAnimated:YES];

    
    
    self.thunderManager.localUid = uidOption.value;
    
    // 实时修改视频编码类型
    // Modify video encoding type in real time
    SettingOption *publishModeOption = self.options[1];
    ThunderPublishVideoMode publishMode = [self publishModeForTitle:publishModeOption.value];
    if (self.thunderManager.publishMode != publishMode) {
        [self.thunderManager switchPublishMode:publishMode];
    }
}


#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.options.count;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    SettingCell *cell = [tableView dequeueReusableCellWithIdentifier:kSettingCellReuseIdentifier forIndexPath:indexPath];
    SettingOption *option = self.options[indexPath.row];
    [cell setupDataWithOption:option];
    
    return cell;
}


#pragma mark - Table view delegate

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 68;
}

- (BOOL)tableView:(UITableView *)tableView shouldHighlightRowAtIndexPath:(NSIndexPath *)indexPath {
    SettingOption *option = self.options[indexPath.row];
    return option.showAccessoryImageView;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [_tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    if (indexPath.row == 1) {
        [self showPublishModeActionSheetAtIndexPath:indexPath];
    }
    
}

- (void)showPublishModeActionSheetAtIndexPath:(NSIndexPath *)indexPath {
    UIAlertController *alertController = [self actionSheetWithTitle:nil message:nil handler:^(UIAlertAction *action, NSUInteger index) {
        if (index != 4) {
            SettingCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
            cell.detailTextField.text = action.title;
            cell.option.value = action.title;
        }
    } otherActionTitles:@"流畅", @"标清", @"高清", @"超清", nil];
    
    [self presentAlertController:alertController atIndexPath:indexPath];
}

- (void)presentAlertController:(UIAlertController *)alertController atIndexPath:(NSIndexPath *)indexPath {
    if (@available(iOS 13.0, *)) {
        alertController.overrideUserInterfaceStyle = UIUserInterfaceStyleLight;
    }
    
    
    // 对iPad做处理
    // Handle iPad
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        UIPopoverPresentationController *popPresenter = alertController.popoverPresentationController;
        SettingCell *cell = [_tableView cellForRowAtIndexPath:indexPath];
        popPresenter.sourceView = cell;
        popPresenter.sourceRect = cell.accessoryImageView.frame;
        popPresenter.permittedArrowDirections = UIPopoverArrowDirectionUp;
    }
    
    [self presentViewController:alertController animated:YES completion:nil];
}




#pragma mark - Get and Set

- (ThunderManager *)thunderManager {
    return [ThunderManager sharedManager];
}

- (NSArray *)options {
    if (_options == nil) {
        SettingOption *option1 = [[SettingOption alloc] init];
        option1.title = @"UID";
        option1.value = self.thunderManager.localUid;
        option1.userInteractionEnabled = !_hasJoinedRoom;
        option1.showAccessoryImageView = NO;
        
        SettingOption *option2 = [[SettingOption alloc] init];
        option2.title = @"开播档位";
        option2.value = [self titleForPublishMode:self.thunderManager.publishMode];
        
        _options = [@[option1, option2] copy];
    }
    
    return _options;
}


- (ThunderPublishVideoMode)publishModeForTitle:(NSString *)title {
    if ([title isEqualToString:@"流畅"]) {
        return THUNDERPUBLISH_VIDEO_MODE_FLUENCY;
    } else if ([title isEqualToString:@"标清"]) {
        return THUNDERPUBLISH_VIDEO_MODE_NORMAL;
    } else if ([title isEqualToString:@"高清"]) {
        return THUNDERPUBLISH_VIDEO_MODE_HIGHQULITY;
    } else if ([title isEqualToString:@"超清"]) {
        return THUNDERPUBLISH_VIDEO_MODE_SUPERQULITY;
    }
    return THUNDERPUBLISH_VIDEO_MODE_DEFAULT;
}


- (NSString *)titleForPublishMode:(ThunderPublishVideoMode)publishMode {
    if (publishMode == THUNDERPUBLISH_VIDEO_MODE_FLUENCY) {
        return @"流畅";
    } else if (publishMode == THUNDERPUBLISH_VIDEO_MODE_NORMAL) {
        return @"标清";
    } else if (publishMode == THUNDERPUBLISH_VIDEO_MODE_HIGHQULITY) {
        return @"高清";
    } else if (publishMode == THUNDERPUBLISH_VIDEO_MODE_SUPERQULITY) {
        return @"超清";
    }
    return @"";
}



@end
