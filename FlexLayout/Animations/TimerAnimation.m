//
//  TimerAnimation.m
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "TimerAnimation.h"
#import <QuartzCore/QuartzCore.h>

typedef void (^Animations)(CGFloat progress);
typedef void (^Completion)(BOOL finished);

@interface TimerAnimation()
{
    NSTimeInterval  m_duration;
    Animations      m_animations;
    Completion      m_completion;
    CFTimeInterval  m_firstFrameTimestamp;
    
    CADisplayLink   *m_displayLink;
    
    BOOL            m_running;
}
@end

@implementation TimerAnimation

- (nullable instancetype)initWithDuration:(NSTimeInterval)duration animations:(void (^_Nonnull)(CGFloat))animations completion:(void (^_Nullable)(BOOL))completion
{
    if (self = [super init])
    {
        m_duration = duration;
        m_animations = animations;
        m_completion = completion;
        
        m_firstFrameTimestamp = CACurrentMediaTime();
        
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(handleFrame:)];
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
        m_displayLink = displayLink;
        
        m_running = YES;
    }
    
    return self;
}

- (void)invalidate
{
    if (!m_running)
    {
        return;
    }
    
    m_running = NO;
    m_completion(NO);
    CADisplayLink *displayLink = m_displayLink;
    if (nil != displayLink)
    {
        [displayLink invalidate];
        displayLink = nil;
    }
}

- (void)handleFrame:(CADisplayLink *)displayLink
{
    if (!m_running)
    {
        return;
    }
    
    NSTimeInterval elapsed = CACurrentMediaTime() - m_firstFrameTimestamp;
    if (elapsed >= m_duration)
    {
        m_animations(1);
        m_running = NO;
        [displayLink invalidate];
        m_completion(YES);
    }
    else
    {
        m_animations(elapsed / m_duration);
    }
}

@end
