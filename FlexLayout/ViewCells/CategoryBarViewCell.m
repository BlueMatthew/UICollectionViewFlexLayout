//
//  CategoryBarViewCell.m
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "CategoryBarViewCell.h"

@interface SUICategoryBarViewCell()
{
}

@end

@implementation SUICategoryBarViewCell
@synthesize categoryBar = m_categoryBar;

- (void)setBackgroundColor:(UIColor *)backgroundColor
{
    [super setBackgroundColor:backgroundColor];
    m_categoryBar.backgroundColor = backgroundColor;
}

- (void)setFrame:(CGRect)frame
{
    if (CGRectEqualToRect(self.frame, frame))
    {
        return;
    }
    
    [super setFrame:frame];
    
    m_categoryBar.frame = self.bounds;
}

- (UIView *)detachCategoryBar
{
    UIView *categoryBar = m_categoryBar;
    m_categoryBar = nil;
    
    return categoryBar;
}

- (void)attachCategoryBar:(nonnull UIView *)categoryBar
{
    UIView *thisCategoryBar = m_categoryBar;
    if (nil != thisCategoryBar)
    {
        [thisCategoryBar removeFromSuperview];
    }
    
    if (nil != categoryBar.superview && self.contentView != categoryBar.superview)
    {
        [categoryBar removeFromSuperview];
    }
    
    m_categoryBar = categoryBar;
    m_categoryBar.frame = self.bounds;
    
    if (nil == categoryBar.superview)
    {
        [self.contentView addSubview:m_categoryBar];
    }
}

@end
