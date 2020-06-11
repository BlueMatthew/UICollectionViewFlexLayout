//
//  CollectionViewFlexLayoutInvalidationContext.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/10.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef CollectionViewFlexLayoutInvalidationContext_h
#define CollectionViewFlexLayoutInvalidationContext_h

#import <UIKit/UIKit.h>

@interface UICollectionViewFlexLayoutInvalidationContext : UICollectionViewFlowLayoutInvalidationContext
@property (nonatomic, assign) BOOL invalidateOffset; // Paging Or Sticky

@end

#endif /* CollectionViewFlexLayoutInvalidationContext_h */
