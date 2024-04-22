//
//  LWWQXRootViewController.h
//  nc1020
//
//  Created by eric on 15/8/20.
//  Copyright (c) 2015年 rainyx. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LWKeyboardView.h"
#import "LWWQXArchiveManager.h"

@interface LWWQXRootViewController : UIViewController

- (instancetype)init NS_UNAVAILABLE;

- (instancetype)initWithArchive:(LWWQXArchive *)archive;

@end
