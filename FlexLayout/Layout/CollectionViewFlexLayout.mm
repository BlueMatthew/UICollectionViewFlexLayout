//
//  CollectionViewFlexLayout.mm
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import <Foundation/Foundation.h>
#define HAVING_HEADER_AND_FOOTER

#import "CollectionViewFlexLayout.h"
#import "CollectionViewFlexLayoutInvalidationContext.h"
#import "LayoutUtils.h"
#import "FlexItem.h"
#import "FlexRow.h"
#import "FlexColumn.h"
#import "FlexPage.h"
#import "FlexFlowSection.h"
#import "FlexWaterfallSection.h"
#include "ContainerBase.h"
#include "FlexLayout.h"
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

namespace nsflex
{
    class LayoutCallbackAdapter;
    
    // Redefinition with the Point/Size in MacTypes.h
    using Point = PointT<CGFloat>;
    using Size = SizeT<CGFloat>;
    using Rect = RectT<CGFloat>;
    using Insets = InsetsT<CGFloat>;
}

using SectionItem = SectionItemT<NSInteger>;
using StickyItemState = StickyItemStateT<CGFloat>;
using StickyItem = StickyItemT<NSInteger, CGFloat>;
using LayoutItem = LayoutItemT<NSInteger, CGFloat>;
using StickyItemList = std::vector<StickyItem>;
template <bool VERTICAL>
using FlexLayout = FlexLayoutT<nsflex::LayoutCallbackAdapter, NSInteger, CGFloat, VERTICAL>;
using StickyItemAndSectionItemCompare = StickyItemAndSectionItemCompareT<NSInteger, CGFloat>;

#ifndef NDEBUG
#define PERF_DEBUG
#endif

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
- (BOOL)isFullSpanAtItem:(NSInteger)item forSection:(NSInteger)section;
- (BOOL)hasFixedSize:(CGSize *)fixedSize forSection:(NSInteger)section;

@end

inline CGRect CGRectFromFlexRect(const nsflex::Rect& rect)
{
    CGRect result;
    result.origin.x = rect.left();
    result.origin.y = rect.top();
    result.size.width = rect.width();
    result.size.height = rect.height();
    
    return result;
}

inline nsflex::Point FlexPointFromCGPoint(const CGPoint& point)
{
    return nsflex::Point(point.x, point.y);
}

inline CGPoint CGPointFromFlexPoint(const nsflex::Point& point)
{
    CGPoint pt;
    pt.x = point.x;
    pt.y = point.y;
    return pt;
}

inline CGSize CGSizeFromFlexSize(const nsflex::Size& size)
{
    CGSize cgSize;
    cgSize.width = size.width;
    cgSize.height = size.height;
    return cgSize;
}

inline nsflex::Size FlexSizeFromCGSize(const CGSize& size)
{
    return nsflex::Size(size.width, size.height);
}

inline nsflex::Insets FlexInsetsFromUIEdgeInsets(const UIEdgeInsets& insets)
{
    return nsflex::Insets(insets.left, insets.top, insets.right, insets.bottom);
}

namespace nsflex
{
    class LayoutCallbackAdapter
    {
    protected:
        UICollectionViewFlexLayout *m_layout;
    public:
        
        LayoutCallbackAdapter(UICollectionViewFlexLayout *layout) : m_layout(layout) {}
        ~LayoutCallbackAdapter() { m_layout = NULL; }
        
        inline int getLayoutModeForSection(NSInteger section) const
        {
            return [m_layout getLayoutModeForSection:section];
        }
        
        inline NSInteger getNumberOfSections() const
        {
            return [m_layout getNumberOfSections];
        }
        
        inline NSInteger getNumberOfItemsInSection(NSInteger section) const
        {
            return [m_layout getNumberOfItemsInSection:section];
        }
        
        inline Size getSizeForItem(NSInteger section, NSInteger item, bool *isFullSpan) const
        {
            CGSize size = [m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:section]];
            if (isFullSpan != NULL)
            {
                *isFullSpan = [m_layout isFullSpanAtItem:item forSection:section];
            }
            return FlexSizeFromCGSize(size);
        }
        
        inline Insets getInsetForSection(NSInteger section) const
        {
            UIEdgeInsets insets = [m_layout getInsetForSectionAtIndex:section];
            return Insets(insets.left, insets.top, insets.right, insets.bottom);
        }
        
        inline CGFloat getMinimumLineSpacingForSection(NSInteger section) const
        {
            return [m_layout getMinimumLineSpacingForSectionAtIndex:section];
        }
        
        inline CGFloat getMinimumInteritemSpacingForSection(NSInteger section) const
        {
            return [m_layout getMinimumInteritemSpacingForSectionAtIndex:section];
        }
        
        inline Size getSizeForHeaderInSection(NSInteger section) const
        {
            CGSize size = [m_layout getSizeForHeaderInSection:section];
            return FlexSizeFromCGSize(size);
        }
        
        inline Size getSizeForFooterInSection(NSInteger section) const
        {
            CGSize size = [m_layout getSizeForFooterInSection:section];
            return FlexSizeFromCGSize(size);
        }
        
        inline NSInteger getNumberOfColumnsForSection(NSInteger section) const
        {
            return [m_layout getNumberOfColumnsForSection:section];
        }
        
        inline bool hasFixedItemSize(NSInteger section, Size *fixedItemSize) const
        {
            CGSize size = CGSizeZero;
            BOOL hasFixedItemSize = [m_layout hasFixedSize:&size forSection:section];
            if (NULL != fixedItemSize)
            {
                *fixedItemSize = FlexSizeFromCGSize(size);
            }
            
            return hasFixedItemSize;
        }
    };
}

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
                unsigned int hasFixedSize : 1;
                unsigned int isFullSpan : 1;
                unsigned int numberOfColumns : 1;
                unsigned int enterStickyMode : 1;
                unsigned int exitStickyMode : 1;
            };
            unsigned int layoutDelegate;
        };
    } m_layoutDelegateFlags;
    
    FlexLayout<true>    *m_verticalLayout;
    FlexLayout<false>   *m_horizontalLayout;
    
    // NSMutableArray *m_updateIndexPaths;
    
    StickyItemList m_stickyHeaders; // Section Index -> Sticy Status(YES/NO)
    
    CGSize m_contentSize;
    BOOL m_layoutInvalidated;
    
    NSMutableDictionary<NSIndexPath *, UICollectionViewLayoutAttributes *> *m_itemLayoutAttributes;
    NSMutableDictionary<NSIndexPath *, UICollectionViewLayoutAttributes *> *m_headerLayoutAttributes;
    NSMutableDictionary<NSIndexPath *, UICollectionViewLayoutAttributes *> *m_footerLayoutAttributes;
}

@end

@implementation UICollectionViewFlexLayout
@synthesize minimumLineSpacing = m_minimumLineSpacing;
@synthesize minimumInteritemSpacing = m_minimumInteritemSpacing;
@synthesize itemSize = m_itemSize;
// @synthesize estimatedItemSize = m_estimatedItemSize;
@synthesize scrollDirection = m_scrollDirection;
@synthesize headerReferenceSize = m_headerReferenceSize;
@synthesize footerReferenceSize = m_footerReferenceSize;
@synthesize sectionInset = m_sectionInset;
@synthesize stackedStickyHeaders = m_stackedStickyHeaders;
@synthesize pagingOffset = m_pagingOffset;
@synthesize pagingSection = m_pagingSection;

#pragma mark - Initialization Functions
- (instancetype)init
{
    if ( self = [super init] )
    {
        m_minimumLineSpacing = 0.0f;
        m_minimumInteritemSpacing = 0.0f;
        m_itemSize = CGSizeZero;
        // m_estimatedItemSize = CGSizeZero;
        m_scrollDirection = UICollectionViewScrollDirectionVertical;
        m_headerReferenceSize = CGSizeZero;
        m_footerReferenceSize = CGSizeZero;
        m_sectionInset = UIEdgeInsetsZero;
        m_stackedStickyHeaders = YES;
        m_pagingSection = NSNotFound;
        m_pagingOffset = CGPointZero;
        
        m_verticalLayout = NULL;
        m_horizontalLayout = NULL;
        
        m_contentSize = CGSizeZero;
        m_layoutInvalidated = YES;
        
        m_itemLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:8];
        m_headerLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:4];
        m_footerLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:2];
        
        [self initializeLayout:m_scrollDirection];
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
                    m_stickyHeaders.push_back(std::make_pair(SectionItem([section integerValue], 0), StickyItemState([[stickyHeaders objectForKey:section] boolValue] ? true : false)));
                }
                std::sort(m_stickyHeaders.begin(), m_stickyHeaders.end());
            }
        }
        m_pagingOffset = [aDecoder containsValueForKey:@"pagingOffset"] ? [aDecoder decodeCGPointForKey:@"pagingOffset"] : CGPointZero;
        m_pagingSection = [aDecoder containsValueForKey:@"pagingSection"] ? [aDecoder decodeIntegerForKey:@"pagingSection"] : NSNotFound;
        m_stackedStickyHeaders = [aDecoder containsValueForKey:@"stackedStickyHeaders"] ? [aDecoder decodeBoolForKey:@"stackedStickyHeaders"] : YES;
        m_sectionInset = [aDecoder containsValueForKey:@"sectionInset"] ? [aDecoder decodeUIEdgeInsetsForKey:@"sectionInset"] : UIEdgeInsetsZero;
        m_footerReferenceSize = [aDecoder containsValueForKey:@"footerReferenceSize"] ? [aDecoder decodeCGSizeForKey:@"footerReferenceSize"] : CGSizeZero;
        m_headerReferenceSize = [aDecoder containsValueForKey:@"headerReferenceSize"] ? [aDecoder decodeCGSizeForKey:@"headerReferenceSize"] : CGSizeZero;
        m_scrollDirection = [aDecoder containsValueForKey:@"scrollDirection"] ? (UICollectionViewScrollDirection)[aDecoder decodeIntegerForKey:@"scrollDirection"] : UICollectionViewScrollDirectionVertical;
        // m_estimatedItemSize = [aDecoder containsValueForKey:@"estimatedItemSize"] ? [aDecoder decodeCGSizeForKey:@"estimatedItemSize"] : CGSizeZero;
        m_itemSize = [aDecoder containsValueForKey:@"itemSize"] ? [aDecoder decodeCGSizeForKey:@"itemSize"] : CGSizeZero;
        m_minimumInteritemSpacing = [aDecoder containsValueForKey:@"minimumInteritemSpacing"] ? [aDecoder decodeDoubleForKey:@"minimumInteritemSpacing"] : 0.0f;
        m_minimumLineSpacing = [aDecoder containsValueForKey:@"minimumLineSpacing"] ? [aDecoder decodeDoubleForKey:@"minimumLineSpacing"] : 0.0f;
        
        m_verticalLayout = NULL;
        m_horizontalLayout = NULL;
        
        m_contentSize = CGSizeZero;
        m_layoutInvalidated = YES;
        
        m_itemLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:8];
        m_headerLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:4];
        m_footerLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:2];
        
        [self initializeLayout:m_scrollDirection];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [super encodeWithCoder:aCoder];
    
    [aCoder encodeDouble:m_minimumLineSpacing forKey:@"minimumLineSpacing"];
    [aCoder encodeDouble:m_minimumInteritemSpacing forKey:@"minimumInteritemSpacing"];
    [aCoder encodeCGSize:m_itemSize forKey:@"itemSize"];
    // [aCoder encodeCGSize:m_estimatedItemSize forKey:@"estimatedItemSize"];
    [aCoder encodeInteger:m_scrollDirection forKey:@"scrollDirection"];
    [aCoder encodeCGSize:m_headerReferenceSize forKey:@"headerReferenceSize"];
    [aCoder encodeCGSize:m_footerReferenceSize forKey:@"footerReferenceSize"];
    [aCoder encodeUIEdgeInsets:m_sectionInset forKey:@"sectionInset"];
    [aCoder encodeBool:m_stackedStickyHeaders forKey:@"stackedStickyHeaders"];
    [aCoder encodeInteger:m_pagingSection forKey:@"pagingSection"];
    [aCoder encodeCGPoint:m_pagingOffset forKey:@"pagingOffset"];
    
    if (!m_stickyHeaders.empty())
    {
        NSMutableDictionary<NSNumber *, NSNumber *> *stickyHeaders = [[NSMutableDictionary<NSNumber *, NSNumber *> alloc] initWithCapacity:m_stickyHeaders.size()];
        for (StickyItemList::const_iterator it = m_stickyHeaders.begin(); it != m_stickyHeaders.end(); ++it)
        {
            [stickyHeaders setObject:[NSNumber numberWithBool:it->second.isInSticky()] forKey:[NSNumber numberWithInteger:it->first.getSection()]];
        }
        
        [aCoder encodeObject:stickyHeaders forKey:@"stickyHeaders"];
    }
}

- (void)dealloc
{
}

+ (Class)layoutAttributesClass
{
    return [UICollectionViewLayoutAttributes class];
}

+ (Class)invalidationContextClass
{
    return [UICollectionViewFlexLayoutInvalidationContext class];
}

#pragma mark - Properties

- (void)setMinimumLineSpacing:(CGFloat)minimumLineSpacing
{
    if (m_minimumLineSpacing != minimumLineSpacing)
    {
        m_minimumLineSpacing = minimumLineSpacing;
        
        [self invalidateLayout];
    }
}

- (void)setMinimumInteritemSpacing:(CGFloat)minimumInteritemSpacing
{
    if (m_minimumInteritemSpacing != minimumInteritemSpacing)
    {
        m_minimumInteritemSpacing = minimumInteritemSpacing;
        
        [self invalidateLayout];
    }
}

- (void)setItemSize:(CGSize)itemSize
{
    if (!CGSizeEqualToSize(m_itemSize, itemSize))
    {
        m_itemSize = itemSize;
        
        [self invalidateLayout];
    }
}

- (void)setScrollDirection:(UICollectionViewScrollDirection)scrollDirection
{
    if (scrollDirection != m_scrollDirection)
    {
        m_scrollDirection = scrollDirection;
        [self initializeLayout:m_scrollDirection];
        [self invalidateLayout];
    }
}

- (void)setHeaderReferenceSize:(CGSize)headerReferenceSize
{
    if (!CGSizeEqualToSize(m_headerReferenceSize, headerReferenceSize))
    {
        m_headerReferenceSize = headerReferenceSize;
        
        [self invalidateLayout];
    }
}

- (void)setFooterReferenceSize:(CGSize)footerReferenceSize
{
    if (!CGSizeEqualToSize(m_footerReferenceSize, footerReferenceSize))
    {
        m_footerReferenceSize = footerReferenceSize;
        
        [self invalidateLayout];
    }
}

- (void)setSectionInset:(UIEdgeInsets)sectionInset
{
    if (!UIEdgeInsetsEqualToEdgeInsets(m_sectionInset, sectionInset))
    {
        m_sectionInset = sectionInset;
        
        [self invalidateLayout];
    }
}

- (void)addStickyHeader:(NSInteger)section
{
    SectionItem sectionItem(section, 0);
    StickyItemList::iterator it = std::lower_bound(m_stickyHeaders.begin(), m_stickyHeaders.end(), sectionItem, StickyItemAndSectionItemCompare());
    if (it == m_stickyHeaders.end() || it->first != sectionItem)
    {
        m_stickyHeaders.insert(it, std::make_pair(sectionItem, StickyItemState()));
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

- (void)setPagingOffset:(CGPoint)pagingOffset
{
    m_pagingOffset = pagingOffset;
    [self invalidateOffset];
}

#pragma mark - Layout Functions

- (void)invalidateOffset
{
    UICollectionViewFlexLayoutInvalidationContext *context = (UICollectionViewFlexLayoutInvalidationContext *)[[[UICollectionViewFlexLayout invalidationContextClass] alloc] init];
    context.invalidatedOffset = YES;
    [self invalidateLayoutWithContext:context];
}

- (void)initializeLayout:(UICollectionViewScrollDirection)scrollDirection
{
    if (UICollectionViewScrollDirectionVertical == scrollDirection)
    {
        if (NULL == m_verticalLayout)
        {
            m_verticalLayout = new FlexLayout<true>();
        }
        if (NULL != m_horizontalLayout)
        {
            delete m_horizontalLayout;
            m_horizontalLayout = NULL;
        }
    }
    else
    {
        if (NULL == m_horizontalLayout)
        {
            m_horizontalLayout = new FlexLayout<false>();
        }
        if (NULL != m_verticalLayout)
        {
            delete m_verticalLayout;
            m_verticalLayout = NULL;
        }
    }
}

#pragma mark - Overrides

- (void)prepareLayout
{
    [super prepareLayout];
    
    if (m_layoutInvalidated)
    {
#ifdef PERF_DEBUG
        double time = 0.0f;
        double prevTime = 0.0;
        
        prevTime = [[NSDate date] timeIntervalSince1970] * 1000;
        
#endif

        UICollectionView *cv = self.collectionView;
        nsflex::LayoutCallbackAdapter layoutAdapter(self);
        
        if (UICollectionViewScrollDirectionVertical == m_scrollDirection)
        {
            m_verticalLayout->prepareLayout(layoutAdapter, FlexSizeFromCGSize(cv.bounds.size), FlexInsetsFromUIEdgeInsets(cv.contentInset));
        }
        else
        {
            m_horizontalLayout->prepareLayout(layoutAdapter, FlexSizeFromCGSize(cv.bounds.size), FlexInsetsFromUIEdgeInsets(cv.contentInset));
        }
        
#ifdef PERF_DEBUG
        time = [[NSDate date] timeIntervalSince1970] * 1000;
        NSLog(@"PERF prepareLayout takes %0.2f ms", time - prevTime);
#endif
    }
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

- (void)invalidateLayout
{
    m_layoutInvalidated = YES;
    [super invalidateLayout];
}

- (void)invalidateLayoutWithContext:(UICollectionViewLayoutInvalidationContext *)context
{
    [super invalidateLayoutWithContext:context];
    
    if ([context isKindOfClass:[UICollectionViewFlexLayout invalidationContextClass]])
    {
        UICollectionViewFlexLayoutInvalidationContext *flexInvalidationContext = (UICollectionViewFlexLayoutInvalidationContext *)context;
        if (!flexInvalidationContext.invalidatedOffset)
        {
            // It is not caused by internal offset change, should call prepareLayout
            if (!(flexInvalidationContext.invalidateDataSourceCounts && !flexInvalidationContext.invalidateEverything))
            {
                m_layoutInvalidated = YES;
            }
            
        }
    }
    else
    {
        // It is not caused by offset change, should call prepareLayout
        m_layoutInvalidated = YES;
    }
}

- (NSArray<UICollectionViewLayoutAttributes *> *)layoutAttributesForElementsInRect:(CGRect)rect
{
#ifdef PERF_DEBUG
    double time = 0.0f;
    double prevTime = 0.0;
    
    prevTime = [[NSDate date] timeIntervalSince1970] * 1000;
#endif

    if (nil == self.collectionView)
    {
        return nil;
    }
    
    std::vector<LayoutItem> layoutItems;
    StickyItemList changingStickyList;
    UICollectionView * const cv = self.collectionView;
    BOOL vertical = (UICollectionViewScrollDirectionVertical == self.scrollDirection);
    
    nsflex::Size layoutContentSize = vertical ? m_verticalLayout->getContentSize() : m_horizontalLayout->getContentSize();

    nsflex::Size contentSize = FlexSizeFromCGSize(self.collectionView.contentSize);
    nsflex::Point contentOffset = FlexPointFromCGPoint(cv.contentOffset);
    nsflex::Size boundsSize = FlexSizeFromCGSize(cv.bounds.size);
    // Sometimes contentSize on UICollectionView doesn't euqal contentSize on Layout
    // We have to correct visible Rect
    
    nsflex::Rect visibleRect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
    if (layoutContentSize != contentSize)
    {
        // If layout is updated but collectionview is not, we should adjust contentOffset
        CGFloat maxOffset = layoutContentSize.width - boundsSize.width;
        if (contentOffset.x > maxOffset)
        {
            contentOffset.x = maxOffset;
        }
        maxOffset = layoutContentSize.height - boundsSize.height;
        if (contentOffset.y > maxOffset)
        {
            contentOffset.y = maxOffset;
        }
        
        visibleRect.set(contentOffset.x, contentOffset.y, contentOffset.x + boundsSize.width, contentOffset.y + boundsSize.height);
    }
    
    if (vertical)
    {
        m_verticalLayout->getItemsInRect(layoutItems, changingStickyList, m_stickyHeaders, m_stackedStickyHeaders, visibleRect, boundsSize, contentSize, FlexInsetsFromUIEdgeInsets(cv.contentInset), contentOffset);
    }
    else
    {
        m_horizontalLayout->getItemsInRect(layoutItems, changingStickyList, m_stickyHeaders, m_stackedStickyHeaders, visibleRect, boundsSize, contentSize, FlexInsetsFromUIEdgeInsets(cv.contentInset), contentOffset);
    }
    
    if (layoutItems.empty())
    {
        // No Items
        return nil;
    }
    
    NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributesArray = [NSMutableArray arrayWithCapacity:layoutItems.size()];
    UICollectionViewLayoutAttributes *la = nil;
    BOOL hasOffset = ((m_pagingSection != NSNotFound) && !CGPointEqualToPoint(m_pagingOffset, CGPointZero));
    BOOL isCopy = NO;
    for (std::vector<LayoutItem>::const_iterator it = layoutItems.begin(); it != layoutItems.end(); ++it)
    {
        isCopy = NO;
        if (it->isHeader())
        {
            la = [self layoutAttributesForSupplementaryViewOfKind:UICollectionElementKindSectionHeader atIndexPath:[NSIndexPath indexPathForItem:0 inSection:it->getSection()]];
        }
        else if (it->isFooter())
        {
            la = [self layoutAttributesForSupplementaryViewOfKind:UICollectionElementKindSectionFooter atIndexPath:[NSIndexPath indexPathForItem:0 inSection:it->getSection()]];
        }
        else if (it->isDecoration())
        {
            
        }
        else // Item
        {
            la = [self layoutAttributesForItemAtIndexPath:[NSIndexPath indexPathForItem:it->getItem() inSection:it->getSection()]];
        }
        
        // Reset all attributes
        la.zIndex = 0;
        la.frame = CGRectFromFlexRect(it->getFrame());
        if (it->isInSticky())
        {
            // If the sticky header's origin is changed, we have to put it higher in z-coodinate
            if (it->isOriginChanged())
            {
                la.zIndex = 1024 + it->getSection();
            }
        }
        
        // PagingOffset
        if (hasOffset && it->getSection() >= m_pagingSection)
        {
            la.frame = CGRectOffset(la.frame, m_pagingOffset.x, m_pagingOffset.y);
        }
        
        [layoutAttributesArray addObject:la];
    }
    
    // If sticky header state is changing, notify caller
    for (StickyItemList::const_iterator it = changingStickyList.begin(); it != changingStickyList.end(); ++it)
    {
        it->second.isInSticky() ? [self enterStickyModeAt:(it->first.getSection()) withOriginalPoint:CGPointFromFlexPoint(it->second.getFrame().origin)] : [self exitStickyModeAt:it->first.getSection()];
    }

#ifdef PERF_DEBUG
    time = [[NSDate date] timeIntervalSince1970] * 1000;
    NSLog(@"PERF elementsInRect takes %0.2f ms", time - prevTime);
    static int inrect = 0;
    inrect ++;
    
    for (UICollectionViewLayoutAttributes *la in layoutAttributesArray)
    {
        NSLog(@"InRect-%d LA=%@", inrect, [la debugDescription]);
    }
#endif
    
    return layoutAttributesArray;
}

- (void)prepareForCollectionViewUpdates:(NSArray<UICollectionViewUpdateItem *> *)updateItems
{
    [super prepareForCollectionViewUpdates:updateItems];
    
    NSInteger minInvalidSection = NSIntegerMax;
    
    UICollectionView *cv = self.collectionView;
    nsflex::Size size = FlexSizeFromCGSize(cv.bounds.size);
    nsflex::Insets padding = FlexInsetsFromUIEdgeInsets(cv.contentInset);
    nsflex::LayoutCallbackAdapter layoutAdapter(self);
    
    // Insert
    for (UICollectionViewUpdateItem *updateItem in updateItems)
    {
        if (UICollectionUpdateActionInsert == updateItem.updateAction)
        {
            if (nil == updateItem.indexPathAfterUpdate) continue;
            
            if (NSNotFound == updateItem.indexPathAfterUpdate.item) // The whole section
            {
                if (UICollectionViewScrollDirectionVertical == m_scrollDirection)
                {
                    m_verticalLayout->insertSection(layoutAdapter, size, padding, updateItem.indexPathAfterUpdate.section, false);
                }
                else
                {
                    m_horizontalLayout->insertSection(layoutAdapter, size, padding, updateItem.indexPathAfterUpdate.section, false);
                }
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
                if (UICollectionViewScrollDirectionVertical == m_scrollDirection)
                {
                    m_verticalLayout->removeSection(layoutAdapter, size, padding, updateItem.indexPathBeforeUpdate.section, false);
                    m_verticalLayout->insertSection(layoutAdapter, size, padding, updateItem.indexPathBeforeUpdate.section, false);
                }
                else
                {
                    m_horizontalLayout->removeSection(layoutAdapter, size, padding, updateItem.indexPathBeforeUpdate.section, false);
                    m_horizontalLayout->insertSection(layoutAdapter, size, padding, updateItem.indexPathBeforeUpdate.section, false);
                }
                // [self removeSection:updateItem.indexPathBeforeUpdate.section andRelayout:NO];
                // [self insertSection:updateItem.indexPathBeforeUpdate.section andRelayout:NO];
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
                if (UICollectionViewScrollDirectionVertical == m_scrollDirection)
                {
                    m_verticalLayout->removeSection(layoutAdapter, size, padding, updateItem.indexPathBeforeUpdate.section, false);
                }
                else
                {
                    m_horizontalLayout->removeSection(layoutAdapter, size, padding, updateItem.indexPathBeforeUpdate.section, false);
                }
                // [self removeSection:updateItem.indexPathBeforeUpdate.section andRelayout:NO];
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
        if (UICollectionViewScrollDirectionVertical == m_scrollDirection)
        {
            m_verticalLayout->refreshSectionFrom(layoutAdapter, size, padding, minInvalidSection);
        }
        else
        {
            m_horizontalLayout->refreshSectionFrom(layoutAdapter, size, padding, minInvalidSection);
        }
    }
}

/*
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
*/

- (UICollectionViewLayoutAttributes *)layoutAttributesForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewLayoutAttributes *layoutAttributes = [m_itemLayoutAttributes objectForKey:indexPath];
    if (nil == layoutAttributes)
    {
        layoutAttributes = [[UICollectionViewFlexLayout layoutAttributesClass] layoutAttributesForCellWithIndexPath:indexPath];
        [m_itemLayoutAttributes setObject:layoutAttributes forKey:indexPath];
    }
    
    nsflex::Rect frame;
    bool existed = (UICollectionViewScrollDirectionVertical == m_scrollDirection) ? m_verticalLayout->getItemFrame(indexPath.section, indexPath.item, frame) : m_horizontalLayout->getItemFrame(indexPath.section, indexPath.item, frame);
    if (existed)
    {
        // frame.offset(-self.collectionView.contentInset.left, -self.collectionView.contentInset.top);
        layoutAttributes.frame = CGRectFromFlexRect(frame);
        return layoutAttributes;
    }
    
    return nil;
}

- (UICollectionViewLayoutAttributes *)layoutAttributesForSupplementaryViewOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewLayoutAttributes *layoutAttributes = nil;
    if ([UICollectionElementKindSectionHeader isEqualToString:elementKind])
    {
        layoutAttributes = [m_headerLayoutAttributes objectForKey:indexPath];
        if (nil == layoutAttributes)
        {
            layoutAttributes = [[UICollectionViewFlexLayout layoutAttributesClass] layoutAttributesForSupplementaryViewOfKind:elementKind withIndexPath:indexPath];
            [m_headerLayoutAttributes setObject:layoutAttributes forKey:indexPath];
        }
        
        nsflex::Rect frame;
        bool existed = (UICollectionViewScrollDirectionVertical == m_scrollDirection) ? m_verticalLayout->getHeaderFrame(indexPath.section, frame) : m_horizontalLayout->getHeaderFrame(indexPath.section, frame);
        if (existed)
        {
            // frame.offset(-self.collectionView.contentInset.left, -self.collectionView.contentInset.top);
            layoutAttributes.frame = CGRectFromFlexRect(frame);
            return layoutAttributes;
        }
    }
    else if ([UICollectionElementKindSectionFooter isEqualToString:elementKind])
    {
        layoutAttributes = [m_footerLayoutAttributes objectForKey:indexPath];
        if (nil == layoutAttributes)
        {
            layoutAttributes = [[UICollectionViewFlexLayout layoutAttributesClass] layoutAttributesForSupplementaryViewOfKind:elementKind withIndexPath:indexPath];
            [m_footerLayoutAttributes setObject:layoutAttributes forKey:indexPath];
        }
        nsflex::Rect frame;
        bool existed = (UICollectionViewScrollDirectionVertical == m_scrollDirection) ? m_verticalLayout->getFooterFrame(indexPath.section, frame) : m_horizontalLayout->getFooterFrame(indexPath.section, frame);
        if (existed)
        {
            // frame.offset(-self.collectionView.contentInset.left, -self.collectionView.contentInset.top);
            layoutAttributes.frame = CGRectFromFlexRect(frame);
            return layoutAttributes;
        }
    }
    
    return layoutAttributes;
}

- (UICollectionViewLayoutAttributes *)layoutAttributesForDecorationViewOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    return nil;
    /*
     if (indexPath.section >= m_sections.size())
    {
        return nil;
    }
     */
    
    // return section->buildLayoutAttributesForDecorationView([UICollectionViewFlexLayout layoutAttributesClass], elementKind, indexPath);
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

- (CGSize)collectionViewContentSize
{
    return CGSizeFromFlexSize((UICollectionViewScrollDirectionVertical == m_scrollDirection) ? m_verticalLayout->getContentSize() : m_horizontalLayout->getContentSize());
}

#pragma mark - Utility Functions

- (void)prepareDelegate
{
    id dataSource = self.collectionView.dataSource;
    if (NULL == m_layoutDelegateFlags.dataSourcePointer || m_layoutDelegateFlags.dataSourcePointer != (__bridge void *)dataSource)
    {
        m_layoutDelegateFlags.dataSourcePointer = (__bridge void *)dataSource;
        m_layoutDelegateFlags.dataSource = 0;
        if ([dataSource conformsToProtocol:@protocol(UICollectionViewDataSource)])
        {
            m_layoutDelegateFlags.numberOfSections = ([dataSource respondsToSelector:@selector(numberOfSectionsInCollectionView:)]) ? 1 : 0;
            m_layoutDelegateFlags.numberOfItemsInSection = ([dataSource respondsToSelector:@selector(collectionView:numberOfItemsInSection:)]) ? 1 : 0;
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
            m_layoutDelegateFlags.insetForSection = ([delegate respondsToSelector:@selector(collectionView:layout:insetForSectionAtIndex:)]) ? 1 : 0;
            m_layoutDelegateFlags.minimumLineSpacing = ([delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)]) ? 1 : 0;
            m_layoutDelegateFlags.minimumInteritemSpacing = ([delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)]) ? 1 : 0;
            m_layoutDelegateFlags.sizeForHeader = ([delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForHeaderInSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.sizeForFooter = ([delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForFooterInSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.numberOfColumns = ([delegate respondsToSelector:@selector(collectionView:layout:numberOfColumnsInSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.layoutModeForSection = ([delegate respondsToSelector:@selector(collectionView:layout:layoutModeForSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.hasFixedSize = ([delegate respondsToSelector:@selector(collectionView:layout:hasFixedSize:forSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.isFullSpan = ([delegate respondsToSelector:@selector(collectionView:layout:isFullSpanAtItem:forSection:)]) ? 1 : 0;
            m_layoutDelegateFlags.enterStickyMode = [delegate respondsToSelector:@selector(collectionView:layout:headerEnterStickyModeAtSection:withOriginalPoint:)];
            m_layoutDelegateFlags.exitStickyMode = [delegate respondsToSelector:@selector(collectionView:layout:headerExitStickyModeAtSection:)];
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

- (BOOL)hasFixedSize:(CGSize *)fixedSize forSection:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.hasFixedSize ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self hasFixedSize:fixedSize forSection:section] : NO;
}

- (BOOL)isFullSpanAtItem:(NSInteger)item forSection:(NSInteger)section
{
    [self prepareDelegate];
    return m_layoutDelegateFlags.isFullSpan ? [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self isFullSpanAtItem:item forSection:section] : NO;
}

- (void)enterStickyModeAt:(NSInteger)section withOriginalPoint:(CGPoint)point
{
    [self prepareDelegate];
    if (m_layoutDelegateFlags.enterStickyMode)
    {
        
    }
    if ([self.collectionView.delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)] && [self.collectionView.delegate respondsToSelector:@selector(collectionView:layout:headerEnterStickyModeAtSection:withOriginalPoint:)])
    {
        [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self headerEnterStickyModeAtSection:section withOriginalPoint:point];
    }
}

- (void)exitStickyModeAt:(NSInteger)section
{
    // [self prepareDelegate];
    if ([self.collectionView.delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)] && [self.collectionView.delegate respondsToSelector:@selector(collectionView:layout:headerExitStickyModeAtSection:)])
    {
        [((id<UICollectionViewDelegateFlexLayout>)self.collectionView.delegate) collectionView:self.collectionView layout:self headerExitStickyModeAtSection:section];
    }
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
