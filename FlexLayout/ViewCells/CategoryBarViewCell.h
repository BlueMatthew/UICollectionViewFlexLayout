//
//  CategoryBarViewCell.h
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef CategoryBarViewCell_h
#define CategoryBarViewCell_h

#import <UIKit/UIKit.h>

@interface SUICategoryBarViewCell : UICollectionViewCell

@property(nullable, nonatomic, strong) UIView *categoryBar;

- (nullable UIView *)detachCategoryBar;
- (void)attachCategoryBar:(nonnull UIView *)categoryBar;

@end

#endif /* CategoryBarViewCell_h */
