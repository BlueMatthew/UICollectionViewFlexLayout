//
//  CollectionViewFlexLayout.mm
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import <Foundation/Foundation.h>
#define HAVING_HEADER_AND_FOOTER
#ifndef NDEBUG
#define PERF_DEBUG
#endif

#import "CollectionViewFlexLayout.h"
#import "CollectionViewFlexLayoutInvalidationContext.h"
#include "FlexLayout.h"
#import "CollectionViewFlexAdapter.h"

typedef NS_ENUM(NSUInteger, UICollectionViewFlexInvalidationFlags) {
    UICollectionViewFlexInvalidationFlagNone = 0,
    UICollectionViewFlexInvalidationFlagEverything = 1,
    UICollectionViewFlexInvalidationFlagDataSourceChanged = 2,
};

namespace nsflex
{
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
using FlexLayout = FlexLayoutT<CollectionViewFlexLayoutAdapter, NSInteger, CGFloat, VERTICAL>;
using StickyItemAndSectionItemCompare = StickyItemAndSectionItemCompareT<NSInteger, CGFloat>;

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

inline nsflex::Rect FlexRectFromCGRect(const CGRect& rect)
{
    return nsflex::Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

inline CGRect CGRectFromFlexRect(const nsflex::Rect& rect)
{
    CGRect result;
    result.origin.x = rect.left();
    result.origin.y = rect.top();
    result.size.width = rect.width();
    result.size.height = rect.height();
    
    return result;
}

inline nsflex::Insets FlexInsetsFromUIEdgeInsets(const UIEdgeInsets& insets)
{
    return nsflex::Insets(insets.left, insets.top, insets.right, insets.bottom);
}

@interface UICollectionViewFlexLayout ()
{
    FlexLayout<true>    *m_verticalLayout;
    FlexLayout<false>   *m_horizontalLayout;
    
    StickyItemList m_stickyHeaders; // Section Index -> Sticy Status(YES/NO)

    NSInteger m_layoutInvalidated;
#if defined(USING_INTERNAL_UPDATE_ITEMS_FOR_BATCH_UPDATES)
    NSArray<UICollectionViewUpdateItem *> *m_updateItems;
#else
    NSArray<UIFlexUpdateItem *> *m_updateItems;
#endif

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

        m_layoutInvalidated = UICollectionViewFlexInvalidationFlagNone;
        
        m_itemLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:8];
        m_headerLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:4];
        m_footerLayoutAttributes = [NSMutableDictionary dictionaryWithCapacity:2];
        
        m_updateItems = nil;
        
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

        m_layoutInvalidated = UICollectionViewFlexInvalidationFlagNone;
        m_updateItems = nil;
        
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
    if (NULL != m_verticalLayout)
    {
        delete m_verticalLayout;
        m_verticalLayout = NULL;
    }
    if (NULL != m_horizontalLayout)
    {
        delete m_horizontalLayout;
        m_horizontalLayout = NULL;
    }
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

- (void)commitBatchUpdates:(NSArray<UIFlexUpdateItem *> *)updateItems
{
    m_updateItems = updateItems;
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
    if (m_layoutInvalidated == UICollectionViewFlexInvalidationFlagNone)
    {
        return;
    }

#ifdef PERF_DEBUG
    double time = 0.0f;
    double prevTime = 0.0;
    
    prevTime = [[NSDate date] timeIntervalSince1970] * 1000;
#endif
    UICollectionView *cv = self.collectionView;
    CollectionViewFlexLayoutAdapter layoutAdapter(self);
    nsflex::Insets padding = FlexInsetsFromUIEdgeInsets(cv.contentInset);
    nsflex::Size boundSize = FlexSizeFromCGSize(cv.bounds.size);
    boundSize.width -= padding.hsize();
    boundSize.height -= padding.vsize();
    const BOOL vertical = (UICollectionViewScrollDirectionVertical == m_scrollDirection);
    
    if ((UICollectionViewFlexInvalidationFlagEverything & m_layoutInvalidated) != 0)
    {
        vertical ? m_verticalLayout->prepareLayout(layoutAdapter, boundSize, padding) : m_horizontalLayout->prepareLayout(layoutAdapter, boundSize, padding);
    }
    else if ((UICollectionViewFlexInvalidationFlagDataSourceChanged & m_layoutInvalidated) != 0)
    {
        NSInteger minInvalidSection = NSIntegerMax;
        NSInteger sectionDeleted = 0;
        
        NSIndexPath *indexPath = nil;
#if defined(USING_INTERNAL_UPDATE_ITEMS_FOR_BATCH_UPDATES)
        for (UICollectionViewUpdateItem *updateItem in m_updateItems)
#else
        for (UIFlexUpdateItem *updateItem in m_updateItems)
#endif
        {
            UICollectionUpdateAction updateAction = updateItem.updateAction;
            switch(updateAction)
            {
                case UICollectionUpdateActionInsert:
                    indexPath = updateItem.indexPathAfterUpdate;
                    break;
                case UICollectionUpdateActionReload:
                    indexPath = updateItem.indexPathBeforeUpdate;
                    break;
                case UICollectionUpdateActionDelete:
                    indexPath = updateItem.indexPathBeforeUpdate;
                    break;
                default:
                    continue;
                    break;
            }
            
            if (nil == indexPath)
            {
                continue;
            }
            
            if (NSNotFound == indexPath.item)
            {
                switch(updateAction)
                {
                    case UICollectionUpdateActionInsert:
                        vertical ? m_verticalLayout->insertSection(layoutAdapter, boundSize, padding, indexPath.section) : m_horizontalLayout->insertSection(layoutAdapter, boundSize, padding, indexPath.section);
                        break;
                    case UICollectionUpdateActionReload:
                        vertical ? m_verticalLayout->reloadSection(layoutAdapter, boundSize, padding, indexPath.section) : m_horizontalLayout->reloadSection(layoutAdapter, boundSize, padding, indexPath.section);
                        break;
                    case UICollectionUpdateActionDelete:
                        vertical ? m_verticalLayout->deleteSection(layoutAdapter, boundSize, padding, indexPath.section - sectionDeleted) : m_horizontalLayout->deleteSection(layoutAdapter, boundSize, padding, indexPath.section - sectionDeleted);
                        sectionDeleted++;
                        break;
                    default:
                        continue;
                        break;
                }
            }
            else // (NSNotFound != indexPath.item)
            {
                switch(updateAction)
                {
                    case UICollectionUpdateActionInsert:
                        vertical ? m_verticalLayout->insertItem(layoutAdapter, boundSize, padding, indexPath.section, indexPath.item) : m_horizontalLayout->insertItem(layoutAdapter, boundSize, padding, indexPath.section, indexPath.item);
                        break;
                    case UICollectionUpdateActionReload:
                        vertical ? m_verticalLayout->reloadItem(layoutAdapter, boundSize, padding, indexPath.section, indexPath.item) : m_horizontalLayout->reloadItem(layoutAdapter, boundSize, padding, indexPath.section, indexPath.item);
                        break;
                    case UICollectionUpdateActionDelete:
                        vertical ? m_verticalLayout->deleteItem(layoutAdapter, boundSize, padding, indexPath.section, indexPath.item) : m_horizontalLayout->deleteItem(layoutAdapter, boundSize, padding, indexPath.section, indexPath.item);
                        break;
                    default:
                        break;
                }
            }
            
            if (indexPath.section < minInvalidSection)
            {
                minInvalidSection = indexPath.section;
            }
        }

        if (minInvalidSection != NSIntegerMax)
        {
            vertical ? m_verticalLayout->prepareLayoutIncrementally(layoutAdapter, boundSize, padding, minInvalidSection) : m_horizontalLayout->prepareLayoutIncrementally(layoutAdapter, boundSize, padding, minInvalidSection);
        }
    }
    
    [m_itemLayoutAttributes removeAllObjects];
    [m_headerLayoutAttributes removeAllObjects];
    [m_footerLayoutAttributes removeAllObjects];
    
    m_updateItems = nil;
    m_layoutInvalidated = UICollectionViewFlexInvalidationFlagNone;
    
#ifdef PERF_DEBUG
    time = [[NSDate date] timeIntervalSince1970] * 1000;
    NSLog(@"PERF: prepareLayout takes %0.2f ms.", time - prevTime);
#endif
}

- (BOOL)shouldInvalidateLayoutForBoundsChange:(CGRect)newBounds
{
    CGSize oldSize = self.collectionView.bounds.size;
    if (!CGSizeEqualToSize(oldSize, newBounds.size))
    {
        // Bound size changes, should re-layout
        return YES;
    }
    
    if (!m_stickyHeaders.empty())
    {
        // If size it not changed, offset must be changed
        // Trigger a offset invalidation and it will be blocked by invalidateLayoutWithContext
        // And then layoutAttributesForElementsInRect will be called
        [self invalidateOffset];
    }

    // Return NO for offset change by default
    return [super shouldInvalidateLayoutForBoundsChange:newBounds];
}

- (void)invalidateLayoutWithContext:(UICollectionViewLayoutInvalidationContext *)context
{
    if (nil == context || ![context isKindOfClass:[UICollectionViewFlexLayout invalidationContextClass]])
    {
        // If it is something we don't know, just make layout happen
        m_layoutInvalidated |= UICollectionViewFlexInvalidationFlagEverything;
        [super invalidateLayoutWithContext:context];
        return;
    }
    
    UICollectionViewFlexLayoutInvalidationContext *invalidationContext = (UICollectionViewFlexLayoutInvalidationContext *)context;
    // if invalidatedOffset is YES, doesn't need to do layout
    if (!invalidationContext.invalidatedOffset)
    {
        // It is not caused by internal offset change, should call prepareLayout
        if ((invalidationContext.invalidateDataSourceCounts && invalidationContext.invalidateEverything == NO))
        {
            // From performBatchUpdate
            // m_layoutInvalidated |= UICollectionViewFlexInvalidationFlagDataSourceChanged;
            // Wait for next invalidateLayoutWithContext of updateItems
#if defined(USING_MANUAL_UPDATE_ITEMS_FOR_BATCH_UPDATES)
            m_layoutInvalidated |= UICollectionViewFlexInvalidationFlagDataSourceChanged;
#elif defined(USING_INTERNAL_UPDATE_ITEMS_FOR_BATCH_UPDATES)
            m_updateItems = [invalidationContext objectForKey:@"_updateItems"];
            m_layoutInvalidated |= UICollectionViewFlexInvalidationFlagDataSourceChanged;
#else
            m_layoutInvalidated |= UICollectionViewFlexInvalidationFlagEverything;
#endif
        }
        else
        {
            m_layoutInvalidated |= UICollectionViewFlexInvalidationFlagEverything;
        }
    }
    [super invalidateLayoutWithContext:context];
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
    nsflex::Rect visibleRect = FlexRectFromCGRect(rect);
    
    /*
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
    */
    
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
    for (std::vector<LayoutItem>::const_iterator it = layoutItems.begin(); it != layoutItems.end(); ++it)
    {
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
        
        if (nil == la)
        {
            
            NSLog(@"nil la: %ld, %ld", (long)(it->getSection()), it->getItem());
        }
        
        // Reset all attributes
        la.zIndex = 0;
        la.frame = CGRectFromFlexRect(it->getFrame());
        if (it->isInSticky())
        {
            // If the sticky header's origin is changed, we have to put it higher in z-coodinate
            if (it->isOriginChanged())
            {
                la.zIndex = layoutItems.size() + 1024 + it->getSection();
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
    NSLog(@"PERF ElementsInRect takes %0.2f ms rect=%@", time - prevTime, NSStringFromCGRect(rect));
    
    /*
    static int inrect = 0;
    inrect ++;
    for (UICollectionViewLayoutAttributes *la in layoutAttributesArray)
    {
        NSLog(@"InRect-%d LA=[%ld-%ld], frame=[%d-%f]-[%d-%f] zIndex=%ld", inrect, la.indexPath.section, la.indexPath.item, (int)CGRectGetMinX(la.frame), CGRectGetMinY(la.frame), (int)CGRectGetWidth(la.frame), CGRectGetHeight(la.frame), la.zIndex);
    }
     */
#endif
    
    return layoutAttributesArray;
}

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
/*
- (CGPoint)targetContentOffsetForProposedContentOffset:(CGPoint)proposedContentOffset
{
    CGPoint targetContentOffset = [super targetContentOffsetForProposedContentOffset:proposedContentOffset];
    

    UICollectionView *cv = self.collectionView;
    CGSize boundsSize = cv.bounds.size;
    CGSize layoutContentSize = [self collectionViewContentSize];
    
    if (!CGSizeEqualToSize(layoutContentSize, cv.contentSize))
    {
        // If layout is updated but collectionview is not, we should adjust contentOffset
        CGFloat maxOffset = layoutContentSize.width - boundsSize.width;
        if (targetContentOffset.x > maxOffset)
        {
            targetContentOffset.x = maxOffset;
        }
        maxOffset = layoutContentSize.height - boundsSize.height;
        if (targetContentOffset.y > maxOffset)
        {
            targetContentOffset.y = maxOffset;
        }
    }
    
    return targetContentOffset;
}
 */

- (CGSize)collectionViewContentSize
{
    return CGSizeFromFlexSize((UICollectionViewScrollDirectionVertical == m_scrollDirection) ? m_verticalLayout->getContentSize() : m_horizontalLayout->getContentSize());
}

#pragma mark - Utility Functions

- (void)enterStickyModeAt:(NSInteger)section withOriginalPoint:(CGPoint)point
{
    id<UICollectionViewDelegate> delegate = self.collectionView.delegate;
    if ([delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)] && [delegate respondsToSelector:@selector(collectionView:layout:headerEnterStickyModeAtSection:withOriginalPoint:)])
    {
        [((id<UICollectionViewDelegateFlexLayout>)delegate) collectionView:self.collectionView layout:self headerEnterStickyModeAtSection:section withOriginalPoint:point];
    }
}

- (void)exitStickyModeAt:(NSInteger)section
{
    id<UICollectionViewDelegate> delegate = self.collectionView.delegate;
    if ([delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)] && [delegate respondsToSelector:@selector(collectionView:layout:headerExitStickyModeAtSection:)])
    {
        [((id<UICollectionViewDelegateFlexLayout>)delegate) collectionView:self.collectionView layout:self headerExitStickyModeAtSection:section];
    }
}

@end
