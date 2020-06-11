//
//  ViewController.m
//  WaterfallLayout
//
//  Created by Matthew Shi on 2020/6/10.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "ViewController.h"
#import "FlexListView.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)loadView
{
    CGRect frame = [[UIScreen mainScreen] bounds];
    
    self.view = [[SUIFlexListView alloc] initWithFrame:frame];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}


@end
