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

@optional
// Asks the delegate for layout mode(UICollectionViewFlexLayoutMode) in the specified section. Default value is UICollectionViewFlexLayoutModeFlow
- (UICollectionViewFlexLayoutMode)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout layoutModeForSection:(NSInteger)section;
// Asks the delegate if all items in one section have the same size. If they are, layout will be faster much more.
- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout hasFixedSize:(CGSize *)fixedSize forSection:(NSInteger)section;
// Asks the delegate if the item should be pleace in the whole completed row.
- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout isFullSpanAtItem:(NSInteger)item forSection:(NSInteger)section;
// Asks the delegate for number of columns in the specified section.
// For FlowLayout, it is better to provide the logical columns and it will help layout manager the memory
- (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout numberOfColumnsInSection:(NSInteger)section;
// Tells the delegate that the header is entering into STICKY mode with it the position
- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerEnterStickyModeAtSection:(NSInteger)section withOriginalPoint:(CGPoint)point;
// Tells the delegate that the header is exitting from STICKY mode
- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerExitStickyModeAtSection:(NSInteger)section;


@end

@interface UICollectionViewFlexLayout : UICollectionViewLayout

// The properties similar to UICollectionViewFlowLayout
@property (nonatomic) CGFloat minimumLineSpacing;
@property (nonatomic) CGFloat minimumInteritemSpacing;
@property (nonatomic) CGSize itemSize;
// @property (nonatomic) CGSize estimatedItemSize; // defaults to CGSizeZero - setting a non-zero size enables cells that self-size via -preferredLayoutAttributesFittingAttributes:
@property (nonatomic) UICollectionViewScrollDirection scrollDirection; // default is UICollectionViewScrollDirectionVertical
@property (nonatomic) CGSize headerReferenceSize;
@property (nonatomic) CGSize footerReferenceSize;
@property (nonatomic) UIEdgeInsets sectionInset;

// Indicates the behavior of among multiple headers.
// If stackedStickyHeaders is NO, the behavior is same as UICollectionViewFloLayout.sectionHeadersPinToVisibleBounds but only specified section set by addStickyHeader will be sticky.
// If stackedStickyHeaders is YES, all of sticky headers above/in current vivible area will pin to the top of UICollectionView bounds, even the section which includes the sticky header is invisible
@property (nonatomic, assign) BOOL stackedStickyHeaders;    // Default is YES. All sticky headers above the current visible cells will be shown even if its section is not in vivible area.
@property (nonatomic, assign) NSInteger pagingSection; // The minimal section in which paging will be fired once the gesture starts. Default is NSNotFound which means paging is not supported.
@property (nonatomic, assign) CGPoint pagingOffset; // Indicates paging offset, all cells(including header, footer, decoration)'s position will be moved with this offset

// Add section with sticky header
- (void)addStickyHeader:(NSInteger)section;
// Remove the specified section with sticky section
- (void)removeStickyHeader:(NSInteger)section;
// Clear all sections with sticky header
- (void)removeAllStickyHeaders;

@end

#endif /* CollectionViewFlexLayout_h */
