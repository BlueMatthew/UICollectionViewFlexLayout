//
//  CollectionViewFlexLayout.m
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "CollectionViewFlexLayout.h"
#import "CollectionViewFlexLayoutInvalidationContext.h"
#import "LayoutUtils.h"
#import "FlexItem.h"
#import "FlexRow.h"
#import "FlexColumn.h"
#import "FlexPage.h"
#import "FlowSection.h"
#import "WaterfallSection.h"
#include <vector>
#include <map>
#include <algorithm>

// #ifdef DEBUG

#define PERF_DEBUG

// #endif


typedef UISectionT<UICollectionViewFlexLayout> UISection;
typedef UIFlowSectionT<UICollectionViewFlexLayout> UIFlowSection;
typedef UIWaterfallSectionT<UICollectionViewFlexLayout> UIWaterfallSection;
typedef UISectionHorizontalCompareT<UICollectionViewFlexLayout> UISectionHorizontalCompare;
typedef UISectionVerticalCompareT<UICollectionViewFlexLayout> UISectionVerticalCompare;

@interface UICollectionViewFlexLayout ()

- (NSInteger)getNumberOfSections;
- (NSInteger)getNumberOfItemsInSection:(NSInteger)section;
- (CGSize)getSizeForItemAtIndexPath:(NSIndexPath *)indexPath;
- (UIEdgeInsets)getInsetForSectionAtIndex:(NSInteger)section;
- (CGFloat)getMinimumLineSpacingForSectionAtIndex:(NSInteger)section;
- (CGFloat)getMinimumInteritemSpacingForSectionAtIndex:(NSInteger)section;
- (CGSize)getSizeForHeaderInSection:(NSInteger)section;
- (CGSize)getSizeForFooterInSection:(NSInteger)section;
- (NSInteger)getNumberOfColumnsForSection:(NSInteger)section;
- (UICollectionViewFlexLayoutMode)getLayoutModeForSection:(NSInteger)section;

- (NSInteger)getPage;
- (NSInteger)getPageAtSection:(NSInteger)section;
- (NSInteger)getPageSize;
- (NSInteger)getNumberOfPagingSectionsForPage:(NSInteger)page;
- (BOOL)getContentOffset:(out CGPoint *)contentOffset forPage:(NSInteger)page;

@end

/*
class UICollectionViewFlexLayoutSectionContextAdapter
{
public:
    UICollectionViewFlexLayoutSectionContextAdapter(UICollectionViewFlexLayout *layout, NSInteger section) : m_layout(layout), m_section(section) { }
    inline bool isVertical() const { return m_layout.scrollDirection == UICollectionViewScrollDirectionVertical; }
    inline NSInteger numberOfItems() const { retrurn [m_layout getNumberOfItemsInSection:section]; }
    inline NSInteger numberOfColumns() const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    inline UIEdgeInsets insets() const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    inline CGSize sizeForHeader() const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    inline CGSize sizeForFooter() const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    inline CGSize sizeForItem(NSInteger item) const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    inline CGFloat minimumLineSpacing() const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    inline CGFloat minimumInteritemSpacing() const { retrurn [m_layout getNumberOfColumnsForSection:section]; }
    
protected:
    UICollectionViewFlexLayout *m_layout;
    NSInteger m_section;
}
*/

@interface UICollectionViewFlexLayout ()
{
    struct {
        void * delegatePointer;
        void * dataSourcePointer;
        union   // collectionView.dataSource
        {
            struct
            {
                unsigned int numberOfSections : 1;
                unsigned int numberOfItemsInSection : 1;
            };
            unsigned int dataSource;
        };
        union   // collectionView.delegate
        {
            struct
            {
                unsigned int sizeForItem : 1;
                unsigned int insetForSection : 1;
                unsigned int minimumLineSpacing : 1;
                unsigned int minimumInteritemSpacing : 1;
                unsigned int sizeForHeader : 1;
                unsigned int sizeForFooter : 1;
                unsigned int layoutModeForSection : 1;
                unsigned int numberOfColumns : 1;
                unsigned int enterStickyMode : 1;
                unsigned int exitStickyMode : 1;
                unsigned int page : 1;
                unsigned int pageAtSection : 1;
                unsigned int pageSize : 1;
                unsigned int numberOfPagingSections : 1;
                unsigned int contentOffsetForPage : 1;
            };
            unsigned int layoutDelegate;
        };
    } m_layoutDelegateFlags;
    
    std::vector<UISection *> m_sections;
    
    std::vector< std::vector<UISection *> > m_pages;
    // When m_pagingSection is validated and (m_pagingSection - 1) is in m_stickyHeaders, we will handle the page context saving/restoring
    std::map<NSInteger, CGPoint>    m_pageContexts;    // Page -> contentOffset, should be reset when the data is updated
    
    CGSize m_contentSize;
    
    NSMutableArray *m_updateIndexPaths;
    
    std::map<NSInteger, BOOL> m_stickyHeaders; // Section Index -> Sticy Status(YES/NO)
    
    BOOL        m_draggingMode;
    CGPoint     m_minPagingOffset;
    CGPoint     m_maxPagingOffset;
    BOOL    m_layoutInvalidated;
}

@end

@implementation UICollectionViewFlexLayout
@synthesize minimumLineSpacing = m_minimumLineSpacing;
@synthesize minimumInteritemSpacing = m_minimumInteritemSpacing;
@synthesize itemSize = m_itemSize;
@synthesize estimatedItemSize = m_estimatedItemSize;
@synthesize scrollDirection = m_scrollDirection;
@synthesize headerReferenceSize = m_headerReferenceSize;
@synthesize footerReferenceSize = m_footerReferenceSize;
@synthesize sectionInset = m_sectionInset;
@synthesize stackedStickyHeaders = m_stackedStickyHeaders;
@synthesize pagingOffset = m_pagingOffset;
@synthesize pagingSection = m_pagingSection;

#pragma mark -Init
- (instancetype)init
{
    if ( self = [super init] )
    {
        m_minimumLineSpacing = 0.0f;
        m_minimumInteritemSpacing = 0.0f;
        m_itemSize = CGSizeZero;
        m_estimatedItemSize = CGSizeZero;
        m_scrollDirection = UICollectionViewScrollDirectionVertical;
        m_headerReferenceSize = CGSizeZero;
        m_footerReferenceSize = CGSizeZero;
        m_sectionInset = UIEdgeInsetsZero;
        m_stackedStickyHeaders = YES;
        m_pagingSection = NSNotFound;
        m_pagingOffset = CGPointZero;
        
        m_draggingMode = NO;
        m_minPagingOffset = CGPointZero;
        m_maxPagingOffset = CGPointZero;
        m_layoutInvalidated = YES;
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    if ( self = [super initWithCoder:aDecoder] )
    {
        if ([aDecoder containsValueForKey:@"stickyHeaders"])
        {
            id obj = [aDecoder decodeObjectForKey:@"stickyHeaders"];
            if ([obj isKindOfClass:[NSDictionary<NSNumber *, NSNumber *> class]])
            {
                NSDictionary<NSNumber *, NSNumber *> *stickyHeaders = (NSDictionary<NSNumber *, NSNumber *> *)obj;
                for (NSNumber *section in stickyHeaders)
                {
                    m_stickyHeaders[[section integerValue]] = [[stickyHeaders objectForKey:section] boolValue];
                }
            }
        }
        m_pagingOffset = [aDecoder containsValueForKey:@"pagingOffset"] ? [aDecoder decodeCGPointForKey:@"pagingOffset"] : CGPointZero;
        m_pagingSection = [aDecoder containsValueForKey:@"pagingSection"] ? [aDecoder decodeIntegerForKey:@"pagingSection"] : NSNotFound;
        m_stackedStickyHeaders = [aDecoder containsValueForKey:@"stackedStickyHeaders"] ? [aDecoder decodeBoolForKey:@"stackedStickyHeaders"] : YES;
        m_sectionInset = [aDecoder containsValueForKey:@"sectionInset"] ? [aDecoder decodeUIEdgeInsetsForKey:@"sectionInset"] : UIEdgeInsetsZero;
        m_footerReferenceSize = [aDecoder containsValueForKey:@"footerReferenceSize"] ? [aDecoder decodeCGSizeForKey:@"footerReferenceSize"] : CGSizeZero;
        m_headerReferenceSize = [aDecoder containsValueForKey:@"headerReferenceSize"] ? [aDecoder decodeCGSizeForKey:@"headerReferenceSize"] : CGSizeZero;
        m_scrollDirection = [aDecoder containsValueForKey:@"scrollDirection"] ? (UICollectionViewScrollDirection)[aDecoder decodeIntegerForKey:@"scrollDirection"] : UICollectionViewScrollDirectionVertical;
        m_estimatedItemSize = [aDecoder containsValueForKey:@"estimatedItemSize"] ? [aDecoder decodeCGSizeForKey:@"estimatedItemSize"] : CGSizeZero;
        m_itemSize = [aDecoder containsValueForKey:@"itemSize"] ? [aDecoder decodeCGSizeForKey:@"itemSize"] : CGSizeZero;
        m_minimumInteritemSpacing = [aDecoder containsValueForKey:@"minimumInteritemSpacing"] ? [aDecoder decodeDoubleForKey:@"minimumInteritemSpacing"] : 0.0f;
        m_minimumLineSpacing = [aDecoder containsValueForKey:@"minimumLineSpacing"] ? [aDecoder decodeDoubleForKey:@"minimumLineSpacing"] : 0.0f;
        
        m_draggingMode = NO;
        m_minPagingOffset = CGPointZero;
        m_maxPagingOffset = CGPointZero;
        m_layoutInvalidated = YES;
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [super encodeWithCoder:aCoder];
    
    [aCoder encodeDouble:m_minimumLineSpacing forKey:@"minimumLineSpacing"];
    [aCoder encodeDouble:m_minimumInteritemSpacing forKey:@"minimumInteritemSpacing"];
    [aCoder encodeCGSize:m_itemSize forKey:@"itemSize"];
    [aCoder encodeCGSize:m_estimatedItemSize forKey:@"estimatedItemSize"];
    [aCoder encodeInteger:m_scrollDirection forKey:@"scrollDirection"];
    [aCoder encodeCGSize:m_headerReferenceSize forKey:@"headerReferenceSize"];
    [aCoder encodeCGSize:m_footerReferenceSize forKey:@"footerReferenceSize"];
    [aCoder encodeUIEdgeInsets:m_sectionInset forKey:@"sectionInset"];
    [aCoder encodeBool:m_stackedStickyHeaders forKey:@"stackedStickyHeaders"];
    [aCoder encodeInteger:m_pagingSection forKey:@"pagingSection"];
    [aCoder encodeCGPoint:m_pagingOffset forKey:@"pagingOffset"];
    
    if (!m_sections.empty())
    {
        NSMutableDictionary<NSNumber *, NSNumber *> *stickyHeaders = [[NSMutableDictionary<NSNumber *, NSNumber *> alloc] initWithCapacity:m_sections.size()];
        for (std::map<NSInteger, BOOL>::const_iterator it = m_stickyHeaders.begin(); it != m_stickyHeaders.end(); ++it)
        {
            [stickyHeaders setObject:[NSNumber numberWithBool:it->second] forKey:[NSNumber numberWithInteger:it->first]];
        }
        
        [aCoder encodeObject:stickyHeaders forKey:@"stickyHeaders"];
    }
}

- (void)dealloc
{
    for (std::vector<UISection *>::iterator it = m_sections.begin(); it != m_sections.end(); delete *it, ++it);
    m_sections.clear();
}

+ (Class)layoutAttributesClass
{
    return [UICollectionViewLayoutAttributes class];
}

+ (Class)invalidationContextClass
{
    return [UICollectionViewFlexLayoutInvalidationContext class];
}

- (void)setScrollDirection:(UICollectionViewScrollDirection)scrollDirection
{
    if (scrollDirection != m_scrollDirection)
    {
        m_scrollDirection = scrollDirection;
        [self invalidateLayout];
    }
}

- (void)addStickyHeader:(NSInteger)section
{
    if (m_stickyHeaders.find(section) == m_stickyHeaders.end())
    {
        m_stickyHeaders[section] = NO;
    }
    
    [self invalidateOffset];
}

- (void)removeStickyHeader:(NSInteger)section
{
    std::map<NSInteger, BOOL>::const_iterator it = m_stickyHeaders.find(section);
    if (it != m_stickyHeaders.end())
    {
        m_stickyHeaders.erase(it);
        [self invalidateOffset];
    }
}

- (void)removeAllStickyHeaders
{
    m_stickyHeaders.clear();
    
    [self invalidateOffset];
}

- (void)setStackedStickyHeaders:(BOOL)stackedStickyHeaders
{
    if (m_stackedStickyHeaders != stackedStickyHeaders)
    {
        m_stackedStickyHeaders = stackedStickyHeaders;
        
        [self invalidateOffset];
    }
}

- (void)invalidateOffset
{
    UICollectionViewFlexLayoutInvalidationContext *context = (UICollectionViewFlexLayoutInvalidationContext *)[[[UICollectionViewFlexLayout invalidationContextClass] alloc] init];
    context.invalidateOffset = YES;
    [self invalidateLayoutWithContext:context];
}

- (void)setPagingSection:(NSInteger)pagingSection
{
    if (pagingSection != m_pagingSection)
    {
        NSInteger minSection = MIN(m_pagingSection, pagingSection);
        m_pagingSection = pagingSection;
        
        [self recalcSectionFrameFrom:minSection + 1];
    }
}

- (void)setPagingOffset:(CGPoint)pagingOffset
{
    [self setPagingOffset:pagingOffset withDraggingMode:YES];
}

- (void)setPagingOffset:(CGPoint)pagingOffset withDraggingMode:(BOOL)draggingMode
{
    m_pagingOffset = pagingOffset;
    m_draggingMode = draggingMode;
    if (draggingMode)
    {
        if (m_minPagingOffset.x > pagingOffset.x) m_minPagingOffset.x = pagingOffset.x;
        if (m_minPagingOffset.y > pagingOffset.y) m_minPagingOffset.y = pagingOffset.y;
        if (m_maxPagingOffset.x < pagingOffset.x) m_maxPagingOffset.x = pagingOffset.x;
        if (m_maxPagingOffset.y < pagingOffset.y) m_maxPagingOffset.y = pagingOffset.y;
    }
    else
    {
        m_minPagingOffset = CGPointZero;
        m_maxPagingOffset = CGPointZero;
    }
    [self invalidateOffset];
}

- (void)setMinimumLineSpacing:(CGFloat)minimumLineSpacing
{
    m_minimumLineSpacing = minimumLineSpacing;
    // [self invalidateLayout]; // Need it?
}

- (void)insertSection:(NSInteger)section andRelayoutFollowingSections:(BOOL)relayout
{
    UICollectionViewFlexLayoutMode mode = [self getLayoutModeForSection:section];
    CGPoint origin = CGPointZero;
    if ((section - 1) > 0)
    {
        std::vector<UISection *>::iterator it = m_sections.begin() + (section - 1);  // Previous Section
        origin = (*it)->getFrame().origin;
        IS_CV_VERTICAL(self) ? origin.y += ((*it)->getFrame().size.height) : (origin.x += (*it)->getFrame().size.width);
    }
    
    UISection *context = (mode == UICollectionViewFlexLayoutModeFlow) ? ((UISection *)(new UIFlowSection(self, section, origin))) : ((UISection *)(new UIWaterfallSection(self, section, origin)));
    
    context->prepareLayout();
    
    m_sections.insert(m_sections.begin() + section, context);
    
    if (relayout) [self recalcSectionFrameFrom:section + 1];
}

- (void)removeSection:(NSInteger)section andRelayoutFollowingSections:(BOOL)relayout
{
    if (section < 0 || section >= m_sections.size())
    {
        return;
    }
    std::vector<UISection *>::iterator it = m_sections.begin() + section;
    delete *it;
    m_sections.erase(it);
    
    if (relayout) [self recalcSectionFrameFrom:section];
}

- (void)removeSections:(NSIndexSet *)sections andRelayoutFollowingSections:(BOOL)relayout
{
    __block NSInteger minSection = NSNotFound;
    [sections enumerateIndexesWithOptions:NSEnumerationReverse usingBlock:^(NSUInteger idx, BOOL * _Nonnull stop) {
        minSection = idx;
        if (idx >= self->m_sections.size())
        {
            return; // continue;
        }
        std::vector<UISection *>::iterator it = self->m_sections.begin() + idx;
        delete *it;
        self->m_sections.erase(it);
    }];
    
    if (relayout) [self recalcSectionFrameFrom:minSection];
}

- (void)recalcSectionFrameFrom:(NSInteger)section
{
    if (section >= m_sections.size())
    {
        return;
    }
    
    m_pages.clear();
    NSInteger pageSize = [self getPageSize];
    if (m_pagingSection != NSNotFound && pageSize > 1)
    {
        
        NSLog(@"PAGING: recalcSectionFrameFrom");
        m_pages.resize(pageSize);
        
        CGFloat position = 0.0;
        
        std::vector<UISection *>::iterator it = m_sections.begin();
        std::vector<UISection *>::iterator itEnd = it + MIN(m_pagingSection, m_sections.size());
        for (; it != itEnd; ++it)
        {
            IS_CV_VERTICAL(self) ? (*it)->getFrame().origin.y = position : (*it)->getFrame().origin.x = position;
            position += IS_CV_VERTICAL(self) ? ((*it)->getFrame().size.height) : ((*it)->getFrame().size.width);
        }
        
        for (std::vector<std::vector<UISection *> >::iterator itPage = m_pages.begin(); itPage != m_pages.end(); ++itPage)
        {
            itPage->insert(itPage->end(), m_sections.begin(), itEnd);
        }
        
        itEnd = m_sections.end();
        for (NSInteger page = 0; page < pageSize; page++)
        {
            std::vector<UISection *> &sections = m_pages[page];
            
            NSInteger pagingSections = [self getNumberOfPagingSectionsForPage:page];
            CGFloat pagingPosition = position;
            for (NSInteger sectionIndex = 0; sectionIndex < pagingSections && it != itEnd; sectionIndex++, ++it)
            {
                IS_CV_VERTICAL(self) ? (*it)->getFrame().origin.y = pagingPosition : (*it)->getFrame().origin.x = pagingPosition;
                pagingPosition += IS_CV_VERTICAL(self) ? ((*it)->getFrame().size.height) : ((*it)->getFrame().size.width);
                
                sections.push_back(*it);
            }
            
            if (it == itEnd) break;
        }
    }
    else
    {
        CGFloat position = 0.0;
        std::vector<UISection *>::iterator it = m_sections.begin() + section;
        
        if (section > 0)
        {
            std::vector<UISection *>::iterator prevIt = it - 1;
            position = IS_CV_VERTICAL(self) ? ((*prevIt)->getFrame().origin.y + (*prevIt)->getFrame().size.height) : ((*prevIt)->getFrame().origin.x + (*prevIt)->getFrame().size.width);
        }
        for (; it != m_sections.end(); ++it)
        {
            IS_CV_VERTICAL(self) ? (*it)->getFrame().origin.y = position : (*it)->getFrame().origin.x = position;
            position += IS_CV_VERTICAL(self) ? ((*it)->getFrame().size.height) : ((*it)->getFrame().size.width);
        }
    }
}

- (void)prepareLayout
{
    [super prepareLayout];

    if (m_layoutInvalidated)
    {
        for (std::vector<UISection *>::iterator it = m_sections.begin(); it != m_sections.end(); delete *it, ++it);
        m_sections.clear();
        
        NSInteger numberOfSections = [self getNumberOfSections];
        
        CGPoint origin = CGPointZero;
        for (NSInteger index = 0; index < numberOfSections; index++)
        {
            // Calc Section Frame
            UICollectionViewFlexLayoutMode mode = [self getLayoutModeForSection:index];
            UISection *section = (UICollectionViewFlexLayoutModeFlow == mode) ? ((UISection *)(new UIFlowSection(self, index, origin))) : ((UISection *)(new UIWaterfallSection(self, index, origin)));
            
            section->prepareLayout();
            
            IS_CV_VERTICAL(self) ? origin.y += section->getFrame().size.height : origin.x += section->getFrame().size.width;
            
            m_sections.push_back(section);
        }
        
        NSInteger pageSize = [self getPageSize];
        if (m_pagingSection != NSNotFound && pageSize > 1)
        {
            [self recalcSectionFrameFrom:0];
        }
        
        m_layoutInvalidated = NO;
    }
}

- (NSArray<UICollectionViewLayoutAttributes *> *)layoutAttributesForElementsInRect:(CGRect)rect
{
    if (nil == self.collectionView)
    {
        return nil;
    }
    
    UICollectionView * const cv = self.collectionView;
    CGSize contentSize = [self collectionViewContentSize];
    CGPoint contentOffset = cv.contentOffset;
    CGRect bounds = cv.bounds;
    
    if (!CGSizeEqualToSize(contentSize, cv.contentSize))
    {
        // If layout is updated but collectionview is not, we should adjust contentOffset
        CGFloat maxOffset = contentSize.width - bounds.size.width;
        if (contentOffset.x > maxOffset)
        {
            contentOffset.x = maxOffset;
        }
        maxOffset = contentSize.height - bounds.size.height;
        if (contentOffset.y > maxOffset)
        {
            contentOffset.y = maxOffset;
        }
    }
    
    CGRect visibleRect = (CGRect) { .origin = contentOffset, .size = self.collectionView.bounds.size };
    NSInteger page = [self getPage];
    NSInteger pageSize = [self getPageSize];
    
    UISectionsHorizontalFilterT<UICollectionViewFlexLayout> hFilter;
    UISectionsVerticalFilterT<UICollectionViewFlexLayout> vFilter;
    
    NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributesArray = [NSMutableArray array];
    
    std::vector<UISection *> const &sections = (m_pagingSection != NSNotFound && pageSize > 1) ? (m_pages[page]) : m_sections;
    std::pair<std::vector<UISection *>::const_iterator, std::vector<UISection *>::const_iterator> range = IS_CV_VERTICAL(self) ? vFilter(sections, visibleRect, layoutAttributesArray) : hFilter(sections, visibleRect, layoutAttributesArray);
    if (range.first == range.second)
    {
        // No Sections
        return nil;
    }
    
    NSMutableArray<UICollectionViewLayoutAttributes *> *newLayoutAttributesArray = nil;
    CGFloat totalHeaderSize = 0.0f; // When m_stackedStickyHeaders == YES
    
    if (!m_stickyHeaders.empty())
    {
        NSInteger maxSection = range.second - 1 - sections.begin();
        NSInteger minSection = range.first - sections.begin();
        std::map<NSInteger, UICollectionViewLayoutAttributes *> headerLayoutAttributesMap;
        
        for (UICollectionViewLayoutAttributes *layoutAttributes in layoutAttributesArray)
        {
            if (m_stickyHeaders.find(layoutAttributes.indexPath.section) != m_stickyHeaders.end())
            {
                if ([layoutAttributes.representedElementKind isEqualToString:UICollectionElementKindSectionHeader])
                {
                    headerLayoutAttributesMap[layoutAttributes.indexPath.section] = layoutAttributes;
                }
            }
        }
        
        UIEdgeInsets contentInset = self.collectionView.contentInset;
        
        
        for (std::map<NSInteger, BOOL>::iterator it = m_stickyHeaders.begin(); it != m_stickyHeaders.end(); ++it)
        {
            if (it->first > maxSection || (!m_stackedStickyHeaders && it->first < minSection))
            {
                if (it->second)
                {
                    it->second = NO;
                    [self exitStickyModeAt:it->first];
                }
                continue;
            }
            
            UICollectionViewLayoutAttributes *layoutAttributes = nil;
            
            std::map<NSInteger, UICollectionViewLayoutAttributes *>::const_iterator itHeaderLayoutAttributes = headerLayoutAttributesMap.find(it->first);
            if (itHeaderLayoutAttributes == headerLayoutAttributesMap.end())
            {
                NSIndexPath *indexPath = [NSIndexPath indexPathForItem:0 inSection:it->first];
                layoutAttributes = [self layoutAttributesForSupplementaryViewOfKind:UICollectionElementKindSectionHeader atIndexPath:indexPath];
                if (CGSizeEqualToSize(layoutAttributes.size, CGSizeZero))
                {
                    continue;
                }
                
                if (nil == newLayoutAttributesArray)
                {
                    newLayoutAttributesArray = [[NSMutableArray<UICollectionViewLayoutAttributes *> alloc] initWithCapacity:4];
                }
                [newLayoutAttributesArray addObject:layoutAttributes];
            }
            else
            {
                layoutAttributes = itHeaderLayoutAttributes->second;
            }
            
            CGFloat headerSize = CGRectGetHeight(layoutAttributes.frame);
            CGPoint origin = layoutAttributes.frame.origin;
            CGPoint oldOrigin = origin;
            
            if (m_stackedStickyHeaders)
            {
                origin.y = MAX(contentOffset.y + totalHeaderSize + contentInset.top, origin.y);
                
                layoutAttributes.frame = (CGRect){ .origin = CGPointMake(origin.x, origin.y), .size = layoutAttributes.frame.size };
            }
            else
            {
                CGRect frameItems = m_sections[it->first]->getItemsFrame();
                origin.y = MIN(
                               MAX(contentOffset.y + contentInset.top + contentInset.top, (frameItems.origin.y - headerSize)),
                               (CGRectGetMaxY(frameItems) - headerSize)
                               );
                
                layoutAttributes.frame = (CGRect){ .origin = CGPointMake(origin.x, origin.y), .size = layoutAttributes.frame.size };
            }
            
            // If original mode is sticky, we check contentOffset and if contentOffset.y is less than origin.y, it is exiting sticky mode
            // Otherwise, we check the top of sticky header
            BOOL stickyMode = it->second ? ((contentOffset.y + contentInset.top < oldOrigin.y) ? NO : YES) : ((layoutAttributes.frame.origin.y > oldOrigin.y) ? YES : NO);
            
            if (stickyMode != it->second)
            {
                // Notify caller if changed
                it->second = stickyMode;
                stickyMode ? [self enterStickyModeAt:it->first withOriginalPoint:oldOrigin] : [self exitStickyModeAt:it->first];
            }
            
            layoutAttributes.zIndex = 1024 + it->first;  //
            totalHeaderSize += headerSize;
        }
    }
    
    // Pagination
    if (m_pagingSection != NSNotFound && pageSize > 1)
    {
        if (!CGPointEqualToPoint(m_pagingOffset, CGPointZero))
        {
            // Offset all cells for current page
            for (UICollectionViewLayoutAttributes *layoutAttributes in layoutAttributesArray)
            {
                if (layoutAttributes.indexPath.section >= m_pagingSection)
                {
                    layoutAttributes.frame = CGRectOffset(layoutAttributes.frame, m_pagingOffset.x, m_pagingOffset.y);
                }
            }
            for (UICollectionViewLayoutAttributes *layoutAttributes in newLayoutAttributesArray)
            {
                if (layoutAttributes.indexPath.section >= m_pagingSection)
                {
                    layoutAttributes.frame = CGRectOffset(layoutAttributes.frame, m_pagingOffset.x, m_pagingOffset.y);
                }
            }
            
            // Other pages swiping, right now we just handle one page
            bool leftSwiping = (m_pagingOffset.x < 0);
            NSInteger swipingPage = leftSwiping ? (page + 1) : (page - 1);
            if (swipingPage >= 0 && swipingPage < pageSize)
            {
                CGPoint pagingContentOffset = contentOffset;
                if (![self getContentOffset:&pagingContentOffset forPage:swipingPage])
                {
                    // Calc
                    // Get bottom of last fixing section
                    CGFloat bottomOfLastFixedSection = 0;
                    if (m_pagingSection > 0)
                    {
                        bottomOfLastFixedSection = CGRectGetMaxY(m_sections[m_pagingSection - 1]->getFrame());
                    }
                    bottomOfLastFixedSection += totalHeaderSize;
                    pagingContentOffset.y = bottomOfLastFixedSection;
                }
                
                CGRect pagingRect = (CGRect) { .origin = pagingContentOffset, .size = self.collectionView.bounds.size };
                
                NSMutableArray<UICollectionViewLayoutAttributes *> *pagingLayoutAttributesArray = [NSMutableArray array];
                if (leftSwiping)
                {
                    CGFloat dx = swipingPage > 0 ? ((-m_pagingOffset.x) - (swipingPage - page - 1) * bounds.size.width) : (-m_pagingOffset.x);
                    pagingRect.origin.x = bounds.size.width - dx;
                    pagingRect.size.width = dx;
                }
                else
                {
                    pagingRect.size.width = m_pagingOffset.x - (swipingPage - page - 1) * bounds.size.width;
                }
                
                std::vector<UISection *> const &pageSections = *(m_pages.begin() + swipingPage);
                std::pair<std::vector<UISection *>::const_iterator, std::vector<UISection *>::const_iterator> rangePage = IS_CV_VERTICAL(self) ? vFilter(pageSections, pagingRect, pagingLayoutAttributesArray, m_pagingSection) : hFilter(pageSections, pagingRect, pagingLayoutAttributesArray, m_pagingSection);
                
                if (rangePage.first != rangePage.second && pagingLayoutAttributesArray.count > 0)
                {
                    CGFloat pagingOffsetX = (swipingPage - page) * bounds.size.width + m_pagingOffset.x;
                    
                    for (UICollectionViewLayoutAttributes *layoutAttributes in pagingLayoutAttributesArray)
                    {
                        layoutAttributes.frame = CGRectOffset(layoutAttributes.frame, pagingOffsetX, m_pagingOffset.y);
                    }
                    
                    [layoutAttributesArray addObjectsFromArray:pagingLayoutAttributesArray];
                    [pagingLayoutAttributesArray removeAllObjects];
                }
            }
            
            
        }
        
        

        /*
        if (m_draggingMode || !CGPointEqualToPoint(m_pagingOffset, CGPointZero))
        {
            bool negativeOffset = m_pagingOffset.x < 0.0f;
            CGFloat minOffset = negativeOffset ? m_pagingOffset.x : 0.0f;
            CGFloat maxOffset = negativeOffset ? 0.0f : m_pagingOffset.x;
            if (m_draggingMode)
            {
                minOffset = m_minPagingOffset.x;
                maxOffset = m_maxPagingOffset.x;
            }
            NSInteger minPageIndex = page - ((maxOffset >= 0.0f) ? ceil(maxOffset / bounds.size.width) : (-floor(maxOffset / bounds.size.width)));
            NSInteger maxPageIndex = page - ((minOffset < 0.0f) ? floor(minOffset / bounds.size.width) : (-ceil(minOffset / bounds.size.width)));
            if (minPageIndex < 0) minPageIndex = 0;
            if (maxPageIndex >= pageSize) maxPageIndex = pageSize - 1;
            NSMutableArray<UICollectionViewLayoutAttributes *> *pagingLayoutAttributesArray = [NSMutableArray array];
            
            for (NSInteger pageIndex = minPageIndex; pageIndex <= maxPageIndex; pageIndex++)
            {
                if (pageIndex == page) continue;

                CGRect pagingRect = visibleRect;
                if (pageIndex == minPageIndex)
                {
                    CGFloat dx = minPageIndex > 0 ? (abs(maxOffset) - (pageIndex - page - 1) * bounds.size.width) : abs(maxOffset);
                    pagingRect.origin.x = bounds.size.width - dx;
                    pagingRect.size.width = dx;
                }
                else if (pageIndex == maxPageIndex)
                {
                    pagingRect.size.width = abs(minOffset) - (pageIndex - page - 1) * bounds.size.width;
                }

                std::vector<UISection *> const &pageSections = (m_pages[pageIndex]);
                std::pair<std::vector<UISection *>::const_iterator, std::vector<UISection *>::const_iterator> rangePage = IS_CV_VERTICAL(self) ? vFilter(pageSections, pagingRect, pagingLayoutAttributesArray, m_pagingSection) : hFilter(pageSections, pagingRect, pagingLayoutAttributesArray, m_pagingSection);
                
                if (rangePage.first == rangePage.second || pagingLayoutAttributesArray.count == 0) continue;

                CGFloat pagingOffsetX = (pageIndex - page) * bounds.size.width + m_pagingOffset.x;
                
                for (UICollectionViewLayoutAttributes *layoutAttributes in pagingLayoutAttributesArray)
                {
                    layoutAttributes.frame = CGRectOffset(layoutAttributes.frame, pagingOffsetX, m_pagingOffset.y);
                }
                
                [layoutAttributesArray addObjectsFromArray:pagingLayoutAttributesArray];
                [pagingLayoutAttributesArray removeAllObjects];
            }
        }
         */
    }

    if (nil == newLayoutAttributesArray || 0 == newLayoutAttributesArray.count)
    {
        newLayoutAttributesArray = layoutAttributesArray;
    }
    else
    {
        [layoutAttributesArray addObjectsFromArray:newLayoutAttributesArray];
        newLayoutAttributesArray = layoutAttributesArray;
    }

    return newLayoutAttributesArray;
}

- (void)prepareForCollectionViewUpdates:(NSArray<UICollectionViewUpdateItem *> *)updateItems
{
    [super prepareForCollectionViewUpdates:updateItems];
    
    NSInteger minInvalidSection = NSIntegerMax;
    
    // Insert
    for (UICollectionViewUpdateItem *updateItem in updateItems)
    {
        if (UICollectionUpdateActionInsert == updateItem.updateAction)
        {
            if (nil == updateItem.indexPathAfterUpdate) continue;
            
            if (NSNotFound == updateItem.indexPathAfterUpdate.item) // The whole section
            {
                [self insertSection:updateItem.indexPathAfterUpdate.section andRelayoutFollowingSections:NO];
                if (updateItem.indexPathAfterUpdate.section + 1 < minInvalidSection)
                {
                    minInvalidSection = updateItem.indexPathAfterUpdate.section + 1;
                }
            }
            else
            {
                NSAssert(NO, @"Not implemented.");
            }
        }
    }
    
    // Reload
    for (UICollectionViewUpdateItem *updateItem in updateItems)
    {
        if (UICollectionUpdateActionReload == updateItem.updateAction)
        {
            if (nil == updateItem.indexPathBeforeUpdate) continue;
            
            if (NSNotFound == updateItem.indexPathBeforeUpdate.item) // The whole section
            {
                [self removeSection:updateItem.indexPathBeforeUpdate.section andRelayoutFollowingSections:NO];
                [self insertSection:updateItem.indexPathBeforeUpdate.section andRelayoutFollowingSections:NO];
                if (updateItem.indexPathBeforeUpdate.section + 1 < minInvalidSection)
                {
                    minInvalidSection = updateItem.indexPathBeforeUpdate.section + 1;
                }
            }
            else
            {
                NSAssert(NO, @"Not implemented.");
            }
        }
    }
    
    // Delete
    for (UICollectionViewUpdateItem *updateItem in updateItems)
    {
        if (UICollectionUpdateActionDelete == updateItem.updateAction)
        {
            if (nil == updateItem.indexPathBeforeUpdate) continue;
            
            if (NSNotFound == updateItem.indexPathBeforeUpdate.item) // The whole section
            {
                [self removeSection:updateItem.indexPathBeforeUpdate.section andRelayoutFollowingSections:NO];
                if (updateItem.indexPathBeforeUpdate.section < minInvalidSection)
                {
                    minInvalidSection = updateItem.indexPathBeforeUpdate.section;
                }
            }
            else
            {
                NSAssert(NO, @"Not implemented.");
            }
        }
    }
    
    if (minInvalidSection != NSIntegerMax)
    {
        [self recalcSectionFrameFrom:minInvalidSection];
    }
}

- (UICollectionViewLayoutAttributes*)initialLayoutAttributesForAppearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
{
    if ((nil != m_updateIndexPaths) && [m_updateIndexPaths containsObject:itemIndexPath])
    {
        UICollectionViewLayoutAttributes *layoutAttributes = [self layoutAttributesForItemAtIndexPath:itemIndexPath];
        
        layoutAttributes.transform = CGAffineTransformRotate(CGAffineTransformMakeScale(0.2, 0.2), M_PI);
        layoutAttributes.center = CGPointMake(CGRectGetMidX(self.collectionView.bounds), CGRectGetMaxY(self.collectionView.bounds));
        layoutAttributes.alpha = 1;
        
        [m_updateIndexPaths removeObject:itemIndexPath];
        return layoutAttributes;
    }
    
    return nil;
}

- (UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
{
    if ((nil != m_updateIndexPaths) && [m_updateIndexPaths containsObject:itemIndexPath])
    {
        UICollectionViewLayoutAttributes *layoutAttributes = [self layoutAttributesForItemAtIndexPath:itemIndexPath];
        
        layoutAttributes.transform = CGAffineTransformRotate(CGAffineTransformMakeScale(2, 2), 0);
        layoutAttributes.alpha = 0;
        [m_updateIndexPaths removeObject:itemIndexPath];
        return layoutAttributes;
    }
    
    return nil;
}

- (void)finalizeCollectionViewUpdates
{
    m_updateIndexPaths = nil;
}

- (void)prepareForTransitionFromLayout:(UICollectionViewLayout *)oldLayout;
{
    memset(&m_layoutDelegateFlags, 0, sizeof(m_layoutDelegateFlags));
}

- (void)prepareForTransitionToLayout:(UICollectionViewLayout *)newLayout
{
    memset(&m_layoutDelegateFlags, 0, sizeof(m_layoutDelegateFlags));
}

- (void)finalizeLayoutTransition
{
    [self prepareDelegate];
}


/*
 - (UICollectionViewLayoutInvalidationContext *)invalidationContextForBoundsChange:(CGRect)newBounds
 {
 CGRect oldBounds = self.collectionView.bounds;
 
 // guard _shouldDoCustomLayout,
 UICollectionViewLayoutInvalidationContext *invalidationContext = [super invalidationContextForBoundsChange:newBounds];
 
 CGPoint contentOffsetAdjustment = CGPointMake(newBounds.origin.x - oldBounds.origin.x, newBounds.origin.y - newBounds.origin.y);
 // CGPoint contentSizeAdjustment = CGPointMake(newBounds.origin.x - oldBounds.origin.x, newBounds.origin.y - newBounds.origin.y);
 
 invalidationContext.contentOffsetAdjustment = contentOffsetAdjustment;
 
 return invalidationContext;
 }
 */

/*
 - (UICollectionViewLayoutInvalidationContext *)invalidationContextForBoundsChange:(CGRect)newBounds
 {
 // guard _shouldDoCustomLayout,
 UICollectionViewLayoutInvalidationContext *invalidationContext = [super invalidationContextForBoundsChange:newBounds];
 if (nil == self.collectionView)
 {
 return invalidationContext;
 }
 
 CGRect oldBounds = self.collectionView.bounds;
 
 
 if (!CGSizeEqualToSize(oldBounds.size, newBounds.size))
 {
 // re-query the collection view delegate for metrics such as size information etc.
 // invalidationContext inval = YES;
 m_prepared = NO;
 }
 
 // Origin changes?
 if (!CGPointEqualToPoint(oldBounds.origin, newBounds.origin))
 {
 NSMutableIndexSet *sectionIds = [NSMutableIndexSet indexSet];
 [sectionIds addIndex:0];
 [sectionIds addIndex:2];
 // find and invalidate the sections that would fall into the new bounds
 // guard let sectionIdxPaths = sectionsHeadersIDxs(forRect: newBounds) else {return invalidationContext}
 
 // then invalidate
 NSMutableArray<NSIndexPath *> *indexPaths = [NSMutableArray<NSIndexPath *> arrayWithCapacity:sectionIds.count];
 [sectionIds enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL * _Nonnull stop) {
 [indexPaths addObject:[NSIndexPath indexPathForItem:0 inSection:idx]];
 }];
 [invalidationContext invalidateSupplementaryElementsOfKind:UICollectionElementKindSectionHeader atIndexPaths:indexPaths];
 }
 
 return invalidationContext;
 }
 */

/*
 - (BOOL)shouldInvalidateLayoutForPreferredLayoutAttributes:(UICollectionViewLayoutAttributes *)preferredAttributes withOriginalAttributes:(UICollectionViewLayoutAttributes *)originalAttributes
 {
 return YES;
 }
 */

- (UICollectionViewLayoutAttributes *)layoutAttributesForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UISection *section = NULL;
    if (indexPath.section >= m_sections.size() || indexPath.item >= (section = m_sections[indexPath.section])->m_items.size())
    {
        return nil;
    }
    
    UIFlexItem *item = section->m_items[indexPath.item];
    UICollectionViewLayoutAttributes *la = item->buildLayoutAttributesForCell([UICollectionViewFlexLayout layoutAttributesClass], indexPath, section->m_frame.origin);
    
    return [la copy];
}

- (UICollectionViewLayoutAttributes *)layoutAttributesForSupplementaryViewOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section >= m_sections.size())
    {
        return nil;
    }
    
    UISection *section = m_sections[indexPath.section];
    
    UIFlexItem *item = NULL;
    if ([elementKind isEqualToString:UICollectionElementKindSectionHeader])
    {
        item = &(section->m_header);
    }
    else if ([elementKind isEqualToString:UICollectionElementKindSectionFooter])
    {
        item = &(section->m_footer);
    }
    
    if (NULL == item)
    {
        return nil;
    }
    
    UICollectionViewLayoutAttributes *la = item->buildLayoutAttributesForSupplementaryView([UICollectionViewFlexLayout layoutAttributesClass], elementKind, indexPath, section->m_frame.origin);
    
    return [la copy];
    
    // UICollectionViewLayoutAttributes *layoutAttributes = [[super layoutAttributesForSupplementaryViewOfKind:elementKind atIndexPath:indexPath] copy];
    // UICollectionViewLayoutAttributes *layoutAttributes = [super layoutAttributesForSupplementaryViewOfKind:elementKind atIndexPath:indexPath];
    // UISection *section = m_sections[indexPath.section];
    // return section->adjustLayoutAttributes(layoutAttributes, indexPath);
}

- (UICollectionViewLayoutAttributes *)layoutAttributesForDecorationViewOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section >= m_sections.size())
    {
        return nil;
    }
    
    // UISection *section = m_sections[indexPath.section];
    
    NSAssert(NO, @"Not implemented yet.");
    
    return nil;
    // return item->buildLayoutAttributesForDecorationView([UICollectionViewFlexLayout layoutAttributesClass], elementKind, indexPath, section->m_frame.origin);
    
    // UICollectionViewLayoutAttributes *layoutAttributes = [[super layoutAttributesForDecorationViewOfKind:elementKind atIndexPath:indexPath] copy];
    // UICollectionViewLayoutAttributes *layoutAttributes = [super layoutAttributesForDecorationViewOfKind:elementKind atIndexPath:indexPath];
    // UISection *section = m_sections[indexPath.section];
    // TODO: Should adjust size???
    // return section->adjustLayoutAttributes(layoutAttributes, indexPath);
}

- (CGSize)collectionViewContentSize
{
    if (m_sections.empty())
    {
        return CGSizeZero;
    }
    
    UISection *section = NULL;
    
    NSInteger pageSize = [self getPageSize];
    if (m_pagingSection != NSNotFound && pageSize > 1)
    {
        NSInteger page = [self getPage];
        std::vector<UISection *> const &sections = m_pages[page];
        section = sections.empty() ? NULL : *(--(sections.end()));
    }
    else
    {
        section = *(--(m_sections.end()));
    }
    
    if (NULL == section)
    {
        return CGSizeZero;
    }
    
    // UISection *section = m_sections[m_sections.size() - 1];
    
    return IS_CV_VERTICAL(self) ? CGSizeMake(self.collectionView.bounds.size.width, section->m_frame.origin.y + section->m_frame.size.height) : CGSizeMake(section->m_frame.origin.x + section->m_frame.size.width, self.collectionView.bounds.size.width);
}

- (BOOL)shouldInvalidateLayoutForBoundsChange:(CGRect)newBounds
{
    if (!m_stickyHeaders.empty())
    {
        // Don't return YES because it will call invalidateLayout
        [self invalidateOffset];
    }
    
    return [super shouldInvalidateLayoutForBoundsChange:newBounds];
}

- (void)prepareDelegate
{
    id dataSource = self.collectionView.dataSource;
    if (NULL == m_layoutDelegateFlags.dataSourcePointer || m_layoutDelegateFlags.dataSourcePointer != (__bridge void *)dataSource)
    {
        m_layoutDelegateFlags.dataSourcePointer = (__bridge void *)dataSource;
        m_layoutDelegateFlags.dataSource = 0;
        if ([dataSource conformsToProtocol:@protocol(UICollectionViewDataSource)])
        {
            m_layoutDelegateFlags.numberOfSections = [dataSource respondsToSelector:@selector(numberOfSectionsInCollectionView:)] ? 1 : 0;
            m_layoutDelegateFlags.numberOfItemsInSection = [dataSource respondsToSelector:@selector(collectionView:numberOfItemsInSection:)] ? 1 : 0;
        }
    }
    
    id delegate = self.collectionView.delegate;
    if (NULL == m_layoutDelegateFlags.delegatePointer || m_layoutDelegateFlags.delegatePointer != (__bridge void *)delegate)
    {
        m_layoutDelegateFlags.delegatePointer = (__bridge void *)delegate;
        m_layoutDelegateFlags.layoutDelegate = 0;
        if ([delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)])
        {
            m_layoutDelegateFlags.sizeForItem = [delegate respondsToSelector:@selector(collectionView:layout:sizeForItemAtIndexPath:)] ? 1 : 0;
            m_layoutDelegateFlags.insetForSection = [delegate respondsToSelector:@selector(collectionView:layout:insetForSectionAtIndex:)] ? 1 : 0;
            m_layoutDelegateFlags.minimumLineSpacing = [delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)] ? 1 : 0;
            m_layoutDelegateFlags.minimumInteritemSpacing = [delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)] ? 1 : 0;
            m_layoutDelegateFlags.sizeForHeader = [delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForHeaderInSection:)] ? 1 : 0;
            m_layoutDelegateFlags.sizeForFooter = [delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForFooterInSection:)] ? 1 : 0;
            m_layoutDelegateFlags.numberOfColumns = [delegate respondsToSelector:@selector(collectionView:layout:numberOfColumnsInSection:)] ? 1 : 0;
            m_layoutDelegateFlags.layoutModeForSection = ([delegate respondsToSelector:@selector(collectionView:layout:layoutModeForSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.enterStickyMode = [delegate respondsToSelector:@selector(collectionView:layout:headerEnterStickyModeAtSection:withOriginalPoint:)] ? 1 : 0;
            m_layoutDelegateFlags.exitStickyMode = [delegate respondsToSelector:@selector(collectionView:layout:headerExitStickyModeAtSection:)] ? 1 : 0;
            m_layoutDelegateFlags.page = [delegate respondsToSelector:@selector(pageForCollectionView:layout:)] ? 1 : 0;
            m_layoutDelegateFlags.pageAtSection = [delegate respondsToSelector:@selector(pageForCollectionView:layout:atSection:)] ? 1 : 0;
            m_layoutDelegateFlags.pageSize = [delegate respondsToSelector:@selector(pageSizeForCollectionView:layout:)] ? 1 : 0;
            m_layoutDelegateFlags.numberOfPagingSections = [delegate respondsToSelector:@selector(collectionView:layout:numberOfPagingSectionsForPage:)] ? 1 : 0;
            m_layoutDelegateFlags.contentOffsetForPage = [delegate respondsToSelector:@selector(collectionView:layout:contentOffsetForPage:)] ? 1 : 0;
        }
    }
}

- (NSInteger)getNumberOfSections
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.numberOfSections == 1) ? [self.collectionView.dataSource numberOfSectionsInCollectionView:self.collectionView] : self.collectionView.numberOfSections;
}

- (NSInteger)getNumberOfItemsInSection:(NSInteger)section
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.numberOfSections == 1) ? [self.collectionView.dataSource collectionView:self.collectionView numberOfItemsInSection:section] : 0;
}

- (CGSize)getSizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.sizeForItem ? [((id<UICollectionViewDelegateFlowLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self sizeForItemAtIndexPath:indexPath] : self.itemSize;
}

- (UIEdgeInsets)getInsetForSectionAtIndex:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.insetForSection ? [((id<UICollectionViewDelegateFlowLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self insetForSectionAtIndex:section] : self.sectionInset;
}

- (CGFloat)getMinimumLineSpacingForSectionAtIndex:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.minimumLineSpacing ? [((id<UICollectionViewDelegateFlowLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self minimumLineSpacingForSectionAtIndex:section] : self.minimumLineSpacing;
}

- (CGFloat)getMinimumInteritemSpacingForSectionAtIndex:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.minimumInteritemSpacing ? [((id<UICollectionViewDelegateFlowLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self minimumInteritemSpacingForSectionAtIndex:section] : self.minimumInteritemSpacing;
}

- (CGSize)getSizeForHeaderInSection:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.sizeForHeader ? [((id<UICollectionViewDelegateFlowLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self referenceSizeForHeaderInSection:section] : self.headerReferenceSize;
}

- (CGSize)getSizeForFooterInSection:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.sizeForFooter ? [((id<UICollectionViewDelegateFlowLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self referenceSizeForFooterInSection:section] : self.footerReferenceSize;
}

- (NSInteger)getNumberOfColumnsForSection:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.numberOfColumns ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self numberOfColumnsInSection:section] : 1;
}

- (UICollectionViewFlexLayoutMode)getLayoutModeForSection:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.layoutModeForSection ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self layoutModeForSection:section] : UICollectionViewFlexLayoutModeFlow;
}

- (void)enterStickyModeAt:(NSInteger)section withOriginalPoint:(CGPoint)point
{
    [self prepareDelegate];
    if (m_layoutDelegateFlags.enterStickyMode)
    {
        [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self headerEnterStickyModeAtSection:section withOriginalPoint:point];
    }
}

- (void)exitStickyModeAt:(NSInteger)section
{
    [self prepareDelegate];
    if (m_layoutDelegateFlags.exitStickyMode)
    {
        [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self headerExitStickyModeAtSection:section];
    }
}

- (NSInteger)getPage
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.page) ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) pageForCollectionView:self.collectionView layout:self] : 0;
}

- (NSInteger)getPageAtSection:(NSInteger)section
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.pageAtSection) ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) pageForCollectionView:self.collectionView layout:self atSection:section] : 0;
}

- (NSInteger)getPageSize
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.pageSize) ? ([((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) pageSizeForCollectionView:self.collectionView layout:self]) : 1;
}

- (NSInteger)getNumberOfPagingSectionsForPage:(NSInteger)page
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.numberOfPagingSections) ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self numberOfPagingSectionsForPage:page] : 0;
}

- (BOOL)getContentOffset:(CGPoint *)contentOffset forPage:(NSInteger)page
{
    [self prepareDelegate];
    return (m_layoutDelegateFlags.contentOffsetForPage) ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self contentOffset:contentOffset forPage:page] : NO;
}

#ifdef DEBUG
- (NSString *)stringFromInvalidationContext:(UICollectionViewLayoutInvalidationContext *)context
{
    // CGPoint contentOffsetAdjustment = context.contentOffsetAdjustment;
    NSMutableString *description = [NSMutableString string];
    if (context.invalidateEverything)
    {
        [description appendString:@"invalidateEverything=YES;"];
    }
    if (context.invalidateDataSourceCounts)
    {
        [description appendString:@"invalidateDSCounts=YES;"];
    }
    if (!CGPointEqualToPoint(context.contentOffsetAdjustment, CGPointZero))
    {
        [description appendFormat:@"contentOffsetAdjustment=%@;", NSStringFromCGPoint(context.contentOffsetAdjustment)];
    }
    if (!CGSizeEqualToSize(context.contentSizeAdjustment, CGSizeZero))
    {
        [description appendFormat:@"contentSizeAdjustment=%@;", NSStringFromCGSize(context.contentSizeAdjustment)];
    }
    if (context.invalidatedItemIndexPaths.count > 0)
    {
        [description appendFormat:@"invalidatedItemIndexPaths=%ld;", context.invalidatedItemIndexPaths.count];
    }
    if (context.invalidatedSupplementaryIndexPaths.count > 0)
    {
        [description appendFormat:@"invalidatedSupplementaryIndexPaths=%ld;", context.invalidatedSupplementaryIndexPaths.count];
    }
    
    if (description.length == 0)
    {
        // CGPoint contentOffset = self.collectionView.contentOffset;
        // CGRect bounds = self.collectionView.bounds;
        // NSLog(@"No InvalidationContext%@", @"");
    }
    
    return description;
}
#endif // DEBUG


@end
