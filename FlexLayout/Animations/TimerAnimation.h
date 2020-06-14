//
//  TimerAnimation.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef TimerAnimation_h
#define TimerAnimation_h

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface TimerAnimation : NSObject


- (nullable instancetype)initWithDuration:(NSTimeInterval)duration animations:(void (^_Nonnull)(CGFloat))animations completion:(void (^_Nullable)(BOOL))completion;

- (void)invalidate;

@end

#endif /* TimerAnimation_h */
