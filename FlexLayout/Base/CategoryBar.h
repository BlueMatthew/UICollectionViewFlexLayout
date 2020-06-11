//
//  CategoryBar.h
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef CategoryBar_h
#define CategoryBar_h

#import <UIKit/UIKit.h>

@class SUICategoryBar;

@protocol SUICategoryBarDelegate<NSObject>
@optional

- (void)categoryBar:(nonnull SUICategoryBar *)categoryBar didSelectItemAtIndex:(NSInteger)index;

@end

@interface SUICategoryBar : UICollectionView

@property (nonatomic, weak, nullable) id<SUICategoryBarDelegate> viewDelegate;
@property(nullable, nonatomic, copy) NSArray<UIBarItem *> *items;
@property (nonatomic) CGSize itemSize;

@property (nonatomic, readonly) NSInteger selectedItem;

/*
 The behavior of tintColor for bars has changed on iOS 7.0. It no longer affects the bar's background
 and behaves as described for the tintColor property added to UIView.
 To tint the bar's background, please use -barTintColor.
 */
@property(null_resettable, nonatomic, strong) UIColor *tintColor;
@property(nullable, nonatomic, strong) UIColor *barTintColor API_AVAILABLE(ios(7.0)) UI_APPEARANCE_SELECTOR;  // default is nil



- (nonnull instancetype)initWithFrame:(CGRect)frame;
- (void)selectItemAt:(NSInteger)item animated:(BOOL)animated;

@end

#endif /* CategoryBar_h */
