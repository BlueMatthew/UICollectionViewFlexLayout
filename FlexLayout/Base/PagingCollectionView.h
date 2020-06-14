//
//  PagingCollectionView.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef PagingCollectionView_h
#define PagingCollectionView_h

#import <UIKit/UIKit.h>

@class UIPagingCollectionView;

@protocol UIPagingCollectionViewDelegate<NSObject>
@optional

- (BOOL)collectionView:(nonnull UICollectionView *)collectionView pagingShouldBeginAtLocation:(CGPoint)location withTranslation:(CGPoint)translation andVelocity:(CGPoint)velocity onSection:(out NSInteger *_Nullable)section;
- (NSInteger)pageForSection:(NSInteger)section inPagingCollectionView:(nonnull UIPagingCollectionView *)pagingCollectionView;
- (NSInteger)pageSizeForSection:(NSInteger)section inPagingCollectionView:(nonnull UIPagingCollectionView *)pagingCollectionView;

- (void)collectionView:(nonnull UIPagingCollectionView *)pagingCollectionView pagingWithOffset:(CGPoint) offset decelerating:(BOOL)decelerating onSection:(NSInteger)section;
- (void)collectionView:(nonnull UIPagingCollectionView *)pagingCollectionView pagingEndedOnSection:(NSInteger)section toNewPage:(NSInteger)page;

@end


@interface UIPagingCollectionView : UICollectionView

@property (nonatomic, weak, nullable) id <UIPagingCollectionViewDelegate> pagingDelegate;

- (void)enablePagingWithDirection:(UICollectionViewScrollDirection)direction;
- (void)disablePaging;


@end

#endif /* PagingCollectionView_h */
