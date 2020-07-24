//
//  ImageItemViewCell.m
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "ImageItemViewCell.h"
#import "UIUtility.h"

#define VERTICAL_SPACING                        8
#define HORIZONTAL_SPACING                      8
#define HORIZONTAL_SPACING_BWTN_BTNS            4
#define TOOL_BTN_SIZE                           24

@interface SUIImageItemViewCell()
{
    UILabel *m_view;
    UIImageView *m_imageView;
    UIColor     *m_imageColor;
    NSString    *m_imageUrl;
    
    NSInteger   m_updateId;
}

@end

@implementation SUIImageItemViewCell
@synthesize fullLineMode = m_fullLineMode;


- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        m_view = [[UILabel alloc] initWithFrame:CGRectZero];
        m_view.numberOfLines = 0;
        [self.contentView addSubview:m_view];
        
        m_imageView = [[UIImageView alloc] initWithFrame:CGRectZero];
        [self.contentView addSubview:m_imageView];
        
        m_updateId = 0;
    }
    
    return self;
}

-(void)setFullLineMode:(BOOL)fullLineMode
{
    if (m_fullLineMode != fullLineMode)
    {
        m_fullLineMode = fullLineMode;
        [self setNeedsLayout];
    }
}

-(void)layoutSubviews
{
    [super layoutSubviews];
    
    CGRect rect = CGRectNull;
    
    if (self.fullLineMode)
    {
        CGFloat height = self.bounds.size.height;
        rect = CGRectMake(height + 8, 0, self.contentView.bounds.size.width - height - 8, height);
        m_view.frame = rect;
        
        rect = CGRectMake(8.0, 8.0, self.contentView.bounds.size.height - 16.0, height - 16.0);
        m_imageView.frame = rect;
    }
    else
    {
        CGFloat width = self.contentView.bounds.size.width;
        rect = CGRectMake(8.0, width, width - 8.0, self.contentView.bounds.size.height - width);
        m_view.frame = rect;
        
        rect = CGRectMake(8.0, 8.0, width - 16.0, width - 16.0);
        m_imageView.frame = rect;
    }
}

- (void)setBackgroundColor:(UIColor *)backgroundColor
{
    [super setBackgroundColor:backgroundColor];
    
    CGFloat red = 0, green = 0, blue = 0, alpha = 0;
    if ([backgroundColor getRed:&red green:&green blue:&blue alpha:&alpha])
    {
        m_view.textColor = [UIColor colorWithRed:(1 - red) green:(1 - green) blue:(1 - blue) alpha:1];
    }
    
    m_view.backgroundColor = backgroundColor;
}

- (void)setFrame:(CGRect)frame
{
    if (CGRectEqualToRect(self.frame, frame))
    {
        return;
    }
    
    [super setFrame:frame];
    
    /*
    CGRect rect = CGRectNull;
    if (self.fullLineMode)
    {
        CGFloat height = self.bounds.size.height;
        rect = CGRectMake(height, 8, self.bounds.size.width - height - 8, height);
        m_view.frame = rect;
        
        rect = CGRectMake(8.0, 8.0, self.bounds.size.height - 16.0, height - 16.0);
        m_imageView.frame = rect;
    }
    else
    {
        CGFloat width = self.bounds.size.width;
        rect = CGRectMake(0.0, width + 8.0, width, self.bounds.size.height - width - 8.0);
        m_view.frame = rect;
        
        rect = CGRectMake(8.0, 8.0, width - 16.0, width - 16.0);
        m_imageView.frame = rect;
    }
     */
}

- (void)setText:(NSString *)text
{
    m_view.text = text;
}

- (void)setImageUrl:(NSString *)imageUrl
{
    m_imageUrl = imageUrl;
    
    if (nil != imageUrl && ![m_imageUrl isEqualToString:@""])
    {
        // m_imageView.
    }
}

- (void)setImageUrl:(NSString *)imageUrl andColor:(UIColor *)color withDelay:(BOOL)needDelay
{
    [self setImageUrl:imageUrl];
    m_imageColor = color;
    
    m_updateId ++;
    
    if (nil == imageUrl || [m_imageUrl isEqualToString:@""])
    {
        if (needDelay)
        {
            m_imageView.layer.borderWidth = 4;
            m_imageView.layer.borderColor = [color CGColor];
            
            NSTimeInterval timeInterval = (arc4random() % 10) / 5.0;    // 0-2s
            __block NSInteger updateId = m_updateId;
            
            // Delay execution of my block for 10 seconds.
            __weak __typeof(self) weakSelf = self;
            __block UIColor *localColor = [color copy];
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, timeInterval * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                __strong __typeof(weakSelf) strongSelf = weakSelf;
                if (strongSelf)
                {
                    if (updateId == strongSelf->m_updateId)
                    {
                        [strongSelf setImageColor:localColor];
                    }
                }
            });
        }
        else
        {
            [self setImageColor:color];
        }
    }
}

- (void)setImageColor:(UIColor *)color
{
    if (m_imageColor == color)
    {
        m_imageView.backgroundColor = color;
    }
}

- (void)prepareForReuse
{
    m_view.text = @"";
    m_imageColor = nil;
    m_imageView.image = nil;
    m_imageView.backgroundColor = nil;
    m_imageView.layer.borderWidth = 0;
    m_imageView.layer.borderColor = nil;
}

- (void)applyLayoutAttributes:(UICollectionViewLayoutAttributes *)layoutAttributes
{
    [super applyLayoutAttributes:layoutAttributes];
    
    self.frame = CGRectApplyAffineTransform(layoutAttributes.frame, layoutAttributes.transform);
}

- (void)updateDataSource:(ItemData *)item
{
    NSNumber *bgColor = item.backgroundColor;
    if (nil != bgColor)
    {
        self.backgroundColor = UIColorFromRGB([bgColor unsignedLongValue]);
    }

    self.text = item.text;

    NSString *url = item.url;
    // self.imageUrl = url;
    
    NSNumber *imageColor = item.imageColor;
    BOOL displayed = item.displayed;
    if (!displayed)
    {
        item.displayed = YES;
    }
    [self setImageUrl:url andColor:UIColorFromRGB([imageColor unsignedLongValue]) withDelay:(!displayed)];
}

@end
