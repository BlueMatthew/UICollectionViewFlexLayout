//
//  FlexListView.mm
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "FlexListView.h"
#import "CollectionViewFlexLayout.h"
#import "CategoryBarViewCell.h"
#import "ItemViewCell.h"
#import "InfoViewCell.h"
#import "ImageItemViewCell.h"
#import "FlexListViewDataSource.h"
#import "CategoryBar.h"
#import "UIUtility.h"
#include <map>

#define SIZE_MAIN_IMAGE_WIDTH       282
#define SIZE_MAIN_IMAGE_HEIGHT      282

#define REUSE_ID_NAVBAR             "navbar"
#define REUSE_ID_ENTRY              "entry"
#define REUSE_ID_CATBAR             "catbar"
#define REUSE_ID_LOADMORE           "loadMore"
#define REUSE_ID_ITEM1              "item1"
#define REUSE_ID_ITEM2              "item2"

#define REUSE_ID_INFO_HEADER        "info_header"
#define REUSE_ID_INFO_FOOTER        "info_footer"

#define CONTENT_INSET                    10

#define SECTION_INDEX_ENTRY             0
#define SECTION_INDEX_TEST1             1
#define SECTION_INDEX_TEST2             2
#define SECTION_INDEX_CATBAR            3
#define SECTION_INDEX_ITEM1             4
#define SECTION_INDEX_ITEM2             5

@interface SUIFlexListView() <UICollectionViewDelegateFlexLayout, UICollectionViewDataSource, SUICategoryBarDelegate>
{
    NSInteger                       m_page;
    SUICategoryBar                  *m_categoryBarView;
    
    UITapGestureRecognizer          *m_tapGesture;
    
    BOOL                            m_isCategoryBarSticky;
    CGFloat                         m_minPagingTop;
    
    // DataSource
    DataSource                      *m_dataSource;
    
    std::map<NSInteger, CGPoint>    m_pageContexts;    // Category Page -> contentOffset, should be reset when the data is updated
}

@end

@implementation SUIFlexListView

#pragma mark Construction Functions

- (nonnull instancetype)initWithFrame:(CGRect)frame;
{
    UICollectionViewFlexLayout *layout = [[UICollectionViewFlexLayout alloc] init];
    
    [layout setScrollDirection:UICollectionViewScrollDirectionVertical];
    
    layout.minimumInteritemSpacing = 0.0;
    layout.minimumLineSpacing = 0.0;
    // layout.pagingSection = SECTION_INDEX_ITEM1;
    // layout.sectionHeadersPinToVisibleBounds = YES;
    NSIndexSet *sections = [DataSource getSectionsForStickyHeader];
    [sections enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL * _Nonnull stop) {
        [layout addStickyHeader:idx];
    }];
    [layout setStackedStickyHeaders:YES];
    
    if (self = [super initWithFrame:frame collectionViewLayout:layout])
    {
        if (@available(iOS 11.0, *))
        {
            self.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
        
        self.dataSource = self;
        self.delegate = self;
        // self.pagingDelegate = self;
        
        // [self enablePagingWithDirection:UICollectionViewScrollDirectionHorizontal];
        
        if (@available(iOS 10.0, *))
        {
            self.prefetchingEnabled = NO;   // avoid crashing ?
        }
        self.backgroundColor = [UIColor colorWithRed:245.0 / 255.0 green:245.0 / 255.0 blue:245.0 / 255.0 alpha:1.0];
        self.showsVerticalScrollIndicator = NO;
        self.contentInset = UIEdgeInsetsMake(CONTENT_INSET, CONTENT_INSET, CONTENT_INSET, CONTENT_INSET);

        [self registerClass:[SUIItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_NAVBAR];
        [self registerClass:[SUIItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ENTRY];
        [self registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM1];
        [self registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM2];
        
        [self registerClass:[SUIItemViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_NAVBAR];
        [self registerClass:[SUICategoryBarViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_CATBAR];
        [self registerClass:[SUIItemViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionFooter withReuseIdentifier:@REUSE_ID_LOADMORE];
        [self registerClass:[SUIInfoViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_INFO_HEADER];
        [self registerClass:[SUIInfoViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionFooter withReuseIdentifier:@REUSE_ID_INFO_FOOTER];
        
        m_isCategoryBarSticky = NO;
        m_minPagingTop = CGFLOAT_MAX;
        
        [self initializeDataSource];
        
        [self performSelector:@selector(reloadData) withObject:nil afterDelay:0];
        
        m_tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapView:)];
        m_tapGesture.numberOfTouchesRequired=1;
    }
    
    return self;
}

#pragma mark DataSource Functions

- (void)initializeDataSource
{
    m_dataSource = [[DataSource alloc] initWithFrame:self.bounds insets:self.contentInset numberOfPages:[DataSource numberOfPages]];
}

#pragma mark Paging Functions

- (void)buildCategoryBarWithFrame:(CGRect)frame withBarItems:(NSArray<UIBarItem *> *)barItems
{
    if (nil == m_categoryBarView)
    {
        m_categoryBarView = [[SUICategoryBar alloc] initWithFrame:frame];
        
        m_categoryBarView.viewDelegate = self;
        m_categoryBarView.itemSize = CGSizeMake(m_categoryBarView.bounds.size.width / 4, m_categoryBarView.bounds.size.height);
        [m_categoryBarView setItems:barItems];
    }
    
    if (m_page != m_categoryBarView.selectedItem)
    {
        [m_categoryBarView selectItemAt:m_page animated:NO];
    }
}

- (CGRect)visibleRectForScrollableSction:(NSInteger)section
{
    if (nil == m_categoryBarView || nil == m_categoryBarView.superview)
    {
        return CGRectZero;
    }
    
    CGRect frame = [self convertRect:m_categoryBarView.bounds fromView:m_categoryBarView];
    return CGRectMake(0, CGRectGetMaxY(frame), self.bounds.size.width, self.bounds.size.height - CGRectGetMaxY(frame) + self.contentOffset.y);
}

- (UICollectionView *)buildCollectionViewAtPage:(NSInteger)page forView:(UIView *)parentView withFrame:(CGRect)frame
{
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    
    //Set Scrolling Direction
    layout.scrollDirection = ((UICollectionViewFlowLayout *)self.collectionViewLayout).scrollDirection;
    layout.minimumInteritemSpacing = 0.0;
    layout.minimumLineSpacing = 0.0;

    UICollectionView *collectionView = nil;
    collectionView = [[UICollectionView alloc] initWithFrame:frame collectionViewLayout:layout];
    
    collectionView.backgroundColor = [self.backgroundColor copy];
    collectionView.backgroundColor = [UIColor yellowColor];
    if (@available(iOS 11.0, *))
    {
        collectionView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
    }
    collectionView.scrollEnabled = NO;
    collectionView.bounces = NO;
    collectionView.alwaysBounceVertical = NO;
    collectionView.showsVerticalScrollIndicator = NO;
    
    collectionView.tag = page;
    
    [collectionView registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM1];
    [collectionView registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM2];

#ifdef DEBUG
    // collectionView.layer.borderColor = [[UIColor redColor] CGColor];
    // collectionView.layer.borderWidth = 3;
#endif
    
    CGPoint contentOffset = CGPointZero;
    if (m_isCategoryBarSticky)
    {
        std::map<NSInteger, CGPoint>::const_iterator it = m_pageContexts.find(page);
        if (it != m_pageContexts.end())
        {
            contentOffset = it->second;
            // contentOffset.y -= (m_minPagingTop - ITEM_HEIGHT_CATBAR);
        }
    }
    
    collectionView.dataSource = self;
    collectionView.delegate = self;
    
    [collectionView reloadData];
    [collectionView performBatchUpdates:^{
    } completion:^(BOOL finished) {
        [collectionView setContentOffset:contentOffset animated:NO];
    }];

    return collectionView;
}

- (void)switchPage:(NSInteger)page
{
   // Save ContentOffset for current page
    m_pageContexts[m_page] = self.contentOffset;
    
    // Switch to new page
    m_page = page;
    __block BOOL contentOffsetInvalidated = NO;
    __block CGPoint contentOffset = CGPointZero;
    if (m_isCategoryBarSticky)
    {
        std::map<NSInteger, CGPoint>::const_iterator it = m_pageContexts.find(page);
        contentOffset = (it != m_pageContexts.end()) ? it->second : CGPointMake(0, m_minPagingTop - m_categoryBarView.frame.size.height);
        if (!CGPointEqualToPoint(contentOffset, self.contentOffset))
        {
            contentOffsetInvalidated = YES;
        }
    }
    
    // NSMutableArray<NSNumber *> *sections = [m_sections objectForKey:@(m_page)];
    /*
    NSMutableArray<NSNumber *> *sections = [m_sections objectForKey: [NSString stringWithFormat:@"%ld",m_page]];
    __block NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSetWithIndexesInRange:NSMakeRange(SECTION_INDEX_ITEM1, sections.count - SECTION_INDEX_ITEM1)];
    // __block UICollectionViewFlexLayout *layout = (UICollectionViewFlexLayout *)self.collectionViewLayout;
    
    [self performBatchUpdates:^{
        [UIView performWithoutAnimation:^{
            [self reloadSections:indexSet];
            // layout.pagingOffset = CGPointZero;
        }];
    } completion:^(BOOL finished) {
        if (contentOffsetInvalidated)
        {
            self.contentOffset = contentOffset;
        }
        // [self cleanPagingViews];
        
        [self->m_categoryBarView selectItemAt:page animated:YES];
    }];
     */
}

#pragma mark UICollectionViewDataSource Implementation
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    NSInteger page = (collectionView == self) ? m_page : collectionView.tag;
    return [m_dataSource numberOfSectionsForPage:page];
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    NSInteger numberOfItemsInSection = 0;
    if (collectionView == self)
    {
        numberOfItemsInSection = [m_dataSource numberOfItemsAtSection:section forPage:m_page];
    }
    else
    {
        NSInteger page = collectionView.tag;
        numberOfItemsInSection = [m_dataSource numberOfItemsAtSection:(section + SECTION_INDEX_ITEM1) forPage:page];
    }
    
    return numberOfItemsInSection;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        return [m_dataSource sizeForItem:indexPath forPage:m_page];
    }
    else
    {
        NSInteger page = collectionView.tag;
        return [m_dataSource sizeForItem:[NSIndexPath indexPathForItem:indexPath.item inSection:(indexPath.section + SECTION_INDEX_ITEM1)] forPage:page];
    }
    return CGSizeZero;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section
{
    if (collectionView == self)
    {
        return [m_dataSource headerSizeAtSection:section forPage:m_page];
    }
    
    return CGSizeZero;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForFooterInSection:(NSInteger)section
{
    if (collectionView == self)
    {
        if (SECTION_INDEX_ENTRY == section)
        {
            // return CGSizeMake(self.bounds.size.width, ITEM_HEIGHT_LOADMORE);
        }
    }
    
    return CGSizeZero;
}

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section
{
    NSInteger sectionIndex = (self == collectionView) ? section : (section + SECTION_INDEX_ITEM1);
    NSInteger page = (self == collectionView) ? m_page : collectionView.tag;
    return [m_dataSource lineSpaceAtSection:sectionIndex forPage:page];
}

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section
{
    NSInteger sectionIndex = (self == collectionView) ? section : (section + SECTION_INDEX_ITEM1);
    NSInteger page = (self == collectionView) ? m_page : collectionView.tag;
    return [m_dataSource interitemSpaceAtSection:sectionIndex forPage:page];
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section
{
    NSInteger sectionIndex = (self == collectionView) ? section : (section + SECTION_INDEX_ITEM1);
    NSInteger page = (self == collectionView) ? m_page : collectionView.tag;
    return [m_dataSource insetsAtSection:sectionIndex forPage:page];
}

- (UICollectionReusableView *)collectionView:(UICollectionView *)collectionView viewForSupplementaryElementOfKind:(NSString *)kind atIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        if ([kind isEqualToString:UICollectionElementKindSectionHeader])
        {
            if (SECTION_INDEX_ENTRY == indexPath.section)
            {
                SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_NAVBAR forIndexPath:indexPath];
                ItemData *header = [m_dataSource headerAt:indexPath.section forPage:m_page];
                
                [cell updateDataSource:header];

                return cell;
            }
            else if (SECTION_INDEX_CATBAR == indexPath.section)
            {
                 SUICategoryBarViewCell *cell = (SUICategoryBarViewCell *)[collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_CATBAR forIndexPath:indexPath];
                ItemData *header = [m_dataSource headerAt:indexPath.section forPage:m_page];
                
                cell.backgroundColor = UIColorFromRGB([header.backgroundColor unsignedLongLongValue]);
                [self buildCategoryBarWithFrame:cell.bounds withBarItems:header.barItems];
                m_categoryBarView.backgroundColor = cell.backgroundColor;

                return cell;
            }
        }
        else if ([kind isEqualToString:UICollectionElementKindSectionFooter])
        {
            if (SECTION_INDEX_ENTRY == indexPath.section)
            {
                SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionFooter withReuseIdentifier:@REUSE_ID_LOADMORE forIndexPath:indexPath];
                ItemData *itemData = [m_dataSource footerAt:indexPath.section forPage:m_page];
                [cell updateDataSource:itemData];
                
                return cell;
            }
        }
    }
    
    return nil;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger page = (collectionView == self) ? m_page : collectionView.tag;
    NSInteger sectionIndex = (collectionView == self) ? indexPath.section : (indexPath.section + SECTION_INDEX_ITEM1);

    SectionData *sectionData = [m_dataSource sectionAt:sectionIndex forPage:page];
    ItemData *itemData = [sectionData itemAt:indexPath.item];
    if (sectionData.sectionId == SECTION_INDEX_ITEM2) // MUST check it first
    {
        SUIImageItemViewCell *cell = (SUIImageItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ITEM2 forIndexPath:indexPath];
        cell.fullLineMode = (sectionData.columns == 1);
        [cell updateDataSource:itemData];
        return cell;
    }
    else if (sectionData.sectionId == SECTION_INDEX_ITEM1) // MUST check it first
    {
        SUIImageItemViewCell *cell = (SUIImageItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ITEM1 forIndexPath:indexPath];
        cell.fullLineMode = (sectionData.columns == 1);
        [cell updateDataSource:itemData];
        return cell;
    }
    else if (sectionData.sectionId == SECTION_INDEX_TEST2)
    {
        SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ENTRY forIndexPath:indexPath];
        [cell updateDataSource:itemData];
        [cell addGestureRecognizer:m_tapGesture];
        return cell;
    }
    else if (sectionData.sectionId == SECTION_INDEX_TEST1)
    {
        SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ENTRY forIndexPath:indexPath];
        [cell updateDataSource:itemData];
        return cell;
    }
    else if (sectionData.sectionId == SECTION_INDEX_ENTRY)
    {
        SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ENTRY forIndexPath:indexPath];
        [cell updateDataSource:itemData];
        return cell;
    }
    
    return nil;
}

- (void)collectionView:(UICollectionView *)collectionView willDisplaySupplementaryView:(UICollectionReusableView *)view forElementKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        if ([elementKind isEqualToString:UICollectionElementKindSectionHeader])
        {
            if (SECTION_INDEX_CATBAR == indexPath.section)
            {
                if ([view isKindOfClass:[SUICategoryBarViewCell class]])
                {
                    SUICategoryBarViewCell *cell = (SUICategoryBarViewCell *)view;
                    // [self buildCategoryBarWithFrame:cell.bounds];
                    [cell attachCategoryBar:m_categoryBarView];
                }
            }
        }
    }
}

- (void)collectionView:(UICollectionView *)collectionView didEndDisplayingSupplementaryView:(UICollectionReusableView *)view forElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        if ([elementKind isEqualToString:UICollectionElementKindSectionHeader])
        {
            if (SECTION_INDEX_CATBAR == indexPath.section)
            {
                if ([view isKindOfClass:[SUICategoryBarViewCell class]])
                {
                    SUICategoryBarViewCell *cell = (SUICategoryBarViewCell *)view;
                    [cell detachCategoryBar];
                }
            }
        }
    }
}

/*
 #pragma mark UIPagingCollectionViewDelegate Implementation
- (BOOL)collectionView:(UICollectionView *)collectionView pagingShouldBeginAtLocation:(CGPoint)location withTranslation:(CGPoint)translation andVelocity:(CGPoint)velocity onSection:(out NSInteger *)section
{
    BOOL shouldBegin = NO;
    
    CGRect rect = [self visibleRectForScrollableSction:SECTION_INDEX_ITEM1];
    if (location.y > CGRectGetMinY(rect) && location.y <= CGRectGetMaxY(rect))
    {
        *section = SECTION_INDEX_ITEM1;
        shouldBegin = YES;
    }
    
    return shouldBegin;
}

- (NSInteger)pageForSection:(NSInteger)section inPagingCollectionView:(UIPagingCollectionView *)pagingCollectionView
{
    if (pagingCollectionView == self)
    {
        return (section == SECTION_INDEX_ITEM) ? m_page : 0;
    }
    
    return 0;
}

- (NSInteger)pageSizeForSection:(NSInteger)section inPagingCollectionView:(UIPagingCollectionView *)pagingCollectionView
{
    if (pagingCollectionView == self)
    {
        return (section == SECTION_INDEX_ITEM) ? m_barItems.count : 0;
    }
    
    return 0;
}

- (UIView *)pagingCollectionView:(UIPagingCollectionView *)pagingCollectionView viewForPage:(NSInteger)page inSection:(NSInteger)section
{
    CGRect frame = [self visibleRectForScrollableSction:section];

    UICollectionView *collectionView = [self buildCollectionViewAtPage:page forView:self withFrame:frame];
    return collectionView;
}

- (BOOL)collectionView:(UIPagingCollectionView *)pagingCollectionView pagingEndedOnSection:(NSInteger)section toNewPage:(NSInteger)page
{
    if (pagingCollectionView == self)
    {
        if (SECTION_INDEX_ITEM == section)
        {
            if (m_page != page)
            {
                // [pagingCollectionView resetSwipedViews];
                // [self resetSwipedViews];
                [self switchPage:page];
            }
        }
    }
    
    return NO;
}

- (void)collectionView:(nonnull UIPagingCollectionView *)pagingCollectionView pagingWithOffset:(CGPoint) offset decelerating:(BOOL)decelerating onSection:(NSInteger)section
{
    UICollectionViewPagingLayout *layout = (UICollectionViewPagingLayout *)self.collectionViewLayout;
    
    layout.pagingOffset = offset;
}
 */

#pragma mark UICollectionViewPagingLayoutDelegate Implementation


- (UICollectionViewFlexLayoutMode)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout layoutModeForSection:(NSInteger)section
{
    NSInteger page = (collectionView == self) ? m_page : collectionView.tag;
    NSInteger sectionIndex = (collectionView == self) ? section : (section + SECTION_INDEX_ITEM1);
    SectionData *sectionData = [m_dataSource sectionAt:sectionIndex forPage:page];
    return (sectionData.sectionId == SECTION_INDEX_ITEM1) ? UICollectionViewFlexLayoutModeWaterfall : UICollectionViewFlexLayoutModeFlow;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout numberOfColumnsInSection:(NSInteger)section
{
    NSInteger page = (collectionView == self) ? m_page : collectionView.tag;
    NSInteger sectionIndex = (collectionView == self) ? section : (section + SECTION_INDEX_ITEM1);
    SectionData *sectionData = [m_dataSource sectionAt:sectionIndex forPage:page];
    return sectionData.columns;
}

- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerEnterStickyModeAtSection:(NSInteger)section withOriginalPoint:(CGPoint)point
{
    if (collectionView == self)
    {
        if (SECTION_INDEX_CATBAR == section)
        {
            m_isCategoryBarSticky = YES;
            m_minPagingTop = point.y + m_categoryBarView.frame.size.height;
        }
    }
}

- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerExitStickyModeAtSection:(NSInteger)section
{
    if (collectionView == self)
    {
        if (SECTION_INDEX_CATBAR == section)
        {
            m_isCategoryBarSticky = NO;
            m_minPagingTop = CGFLOAT_MAX;
            m_pageContexts.clear();
        }
    }
}



#pragma mark UserActions

-(void)tapView:(UITapGestureRecognizer *)gesture
{
    NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSet];
    
    NSInteger section1 = [m_dataSource removeSection:SECTION_INDEX_ITEM2 forPage:m_page];
    if (NSNotFound != section1)
    {
        [indexSet addIndex:section1];
    }
    
    NSInteger section2 = [m_dataSource removeSection:SECTION_INDEX_ITEM1 forPage:m_page];
    if (NSNotFound != section2)
    {
        [indexSet addIndex:section2];
    }
    if (indexSet.count == 0)
    {
        return;
    }
    [UIView performWithoutAnimation:^{
        NSLog(@"UPD: %@", @"performBatchUpdates call");
        [self performBatchUpdates:^{
            NSLog(@"UPD: %@", @"performBatchUpdates block");
            // [self deleteSections:[NSIndexSet indexSetWithIndex:section]];
            [self deleteSections:indexSet];
            // [self deleteSections:indexSet];
            
            // [self insertSections:[NSIndexSet indexSetWithIndex:SECTION_INDEX_ITEM1]];
            NSLog(@"UPD: %@", @"performBatchUpdates block end");
        } completion:^(BOOL finished) {
            //
        }];
    }];
}

@end
