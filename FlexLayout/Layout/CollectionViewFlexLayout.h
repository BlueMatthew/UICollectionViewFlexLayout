//
//  CollectionViewFlexLayout.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef CollectionViewFlexLayout_h
#define CollectionViewFlexLayout_h

#import <UIKit/UIKit.h>

@class UICollectionViewFlexLayout;


typedef NS_ENUM(NSUInteger, UICollectionViewFlexLayoutMode) {
    UICollectionViewFlexLayoutModeFlow,
    UICollectionViewFlexLayoutModeWaterfall,
};


@protocol UICollectionViewDelegateFlexLayout <UICollectionViewDelegateFlowLayout>

- (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout numberOfColumnsInSection:(NSInteger)section;

- (UICollectionViewFlexLayoutMode)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout layoutModeForSection:(NSInteger)section;

- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerEnterStickyModeAtSection:(NSInteger)section withOriginalPoint:(CGPoint)point;
- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerExitStickyModeAtSection:(NSInteger)section;

- (NSInteger)pageForCollectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout;
- (NSInteger)pageSizeForCollectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout;
- (NSInteger)pageForCollectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout atSection:(NSInteger)section;
- (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout numberOfPagingSectionsForPage:(NSInteger)page;
- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout contentOffset:(out CGPoint *)contentOffset forPage:(NSInteger)page;

@end

@interface UICollectionViewFlexLayout : UICollectionViewLayout

@property (nonatomic) CGFloat minimumLineSpacing;
@property (nonatomic) CGFloat minimumInteritemSpacing;
@property (nonatomic) CGSize itemSize;
@property (nonatomic) CGSize estimatedItemSize; // defaults to CGSizeZero - setting a non-zero size enables cells that self-size via -preferredLayoutAttributesFittingAttributes:
@property (nonatomic) UICollectionViewScrollDirection scrollDirection; // default is UICollectionViewScrollDirectionVertical
@property (nonatomic) CGSize headerReferenceSize;
@property (nonatomic) CGSize footerReferenceSize;
@property (nonatomic) UIEdgeInsets sectionInset;


@property (nonatomic, assign) BOOL stackedStickyHeaders;    // Default is YES. All sticky headers above the current visible cells will be shown even if its section is not in vivible area.
@property (nonatomic, assign) NSInteger pagingSection; // Default is NSNotFound. The minimal section after the header sections.
@property (nonatomic, assign) CGPoint pagingOffset;

- (void)addStickyHeader:(NSInteger)section;
- (void)removeStickyHeader:(NSInteger)section;
- (void)removeAllStickyHeaders;

- (void)setPagingOffset:(CGPoint)pagingOffset withDraggingMode:(BOOL)draggingMode;

@end

#endif /* CollectionViewWaterfallLayout_h */
