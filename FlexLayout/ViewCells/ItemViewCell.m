//
//  ItemViewCell.m
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "ItemViewCell.h"
#import "UIUtility.h"

@interface SUIItemViewCell()
{
    UILabel *m_view;
}

@end

@implementation SUIItemViewCell

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        self.contentView.layoutMargins = UIEdgeInsetsMake(0, 8, 0, 0);
        
        m_view = [[UILabel alloc] initWithFrame:CGRectInset(self.contentView.bounds, 10, 0)];
        
        [self.contentView addSubview:m_view];
    }
    
    return self;
}

- (void)setBackgroundColor:(UIColor *)backgroundColor
{
    [super setBackgroundColor:backgroundColor];
    m_view.backgroundColor = backgroundColor;
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];

    m_view.frame = CGRectInset(self.contentView.bounds, 10, 0);
}

- (void)setText:(nullable NSString *)text
{
    m_view.text = text;
}

- (void)prepareForReuse
{
    self.backgroundColor = nil;
    m_view.text = @"";
}

- (void)updateDataSource:(NSMutableDictionary *)item
{
    NSNumber *bgColor = (NSNumber *)[item objectForKey:@"bgColor"];
    if (nil != bgColor)
    {
        self.backgroundColor = UIColorFromRGB([bgColor unsignedLongValue]);
    }
    m_view.text = (NSString *)[item objectForKey:@"text"];
}


@end
