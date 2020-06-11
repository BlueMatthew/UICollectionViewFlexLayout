//
//  CategoryBarItemViewCell.m
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "CategoryBarItemViewCell.h"

@interface SUICategoryBarItemViewCell()
{
    UILabel *m_view;
}

@end

@implementation SUICategoryBarItemViewCell
@synthesize barItem = m_barItem;
@synthesize selectedState = m_selectedState;

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        m_selectedState = NO;
        
        m_view = [[UILabel alloc] initWithFrame:self.contentView.bounds];
        m_view.textAlignment = NSTextAlignmentCenter;
        [self.contentView addSubview:m_view];
        
        [self updateStyle];
    }
    
    return self;
}

- (void)updateStyle
{
    NSDictionary<NSAttributedStringKey, id> *textAttributes = nil;
    
    if (nil != m_barItem)
    {
        UIControlState state = m_selectedState ? UIControlStateFocused : UIControlStateNormal;
        textAttributes = [m_barItem titleTextAttributesForState:state];
    }
    
    // Update UI
    UIColor *color = nil;
    if (nil != textAttributes)
    {
        color = (UIColor *)[textAttributes objectForKey:NSForegroundColorAttributeName];
    }
    
    if (nil != color)
    {
        m_view.textColor = color;
    }
    else
    {
        m_view.textColor = [UIColor blackColor];
    }
    
    UIFont *font = nil;
    if (nil != textAttributes)
    {
        font = (UIFont *)[textAttributes objectForKey:NSFontAttributeName];
    }
    m_view.font = font;
    
}

- (void)setSelectedState:(BOOL)selectedState
{
    // BOOL oldState = m_selectedState;
    m_selectedState = selectedState;
    
    // if (oldState != selectedState)
    {
        [self updateStyle];
    }
}

- (void)setBarItem:(UIBarItem *)barItem
{
    if (m_barItem == barItem)
    {
        return;
    }
    
    m_barItem = barItem;
    
    m_view.text = barItem.title;
    
    [self updateStyle];
}

- (void)setBackgroundColor:(UIColor *)backgroundColor
{
    [super setBackgroundColor:backgroundColor];
    m_view.backgroundColor = backgroundColor;
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    
    m_view.frame = self.contentView.bounds;
}

- (void)prepareForReuse
{
    m_view.text = @"";
    m_barItem = nil;
    
    m_selectedState = NO;
    
    [self updateStyle];
}

@end
