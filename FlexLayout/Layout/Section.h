//
//  Section.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef Section_h
#define Section_h

#import <UIKit/UIKit.h>
#include <vector>
#include <map>
#include <algorithm>

#import "FlexItem.h"
#import "LayoutUtils.h"

/*
class SectionContextAdapter
{
public:

    bool isVertical() const;
    NSInteger numberOfItems() const;
    NSInteger numberOfColumns() const;
    UIEdgeInsets insets() const;
    CGSize sizeForHeader() const;
    CGSize sizeForFooter() const;
    CGSize sizeForItem(NSInteger item);
    CGFloat minimumLineSpacing() const;
    CGFloat minimumInteritemSpacing() const;
};
*/

template<class TLayout>
class UISectionT
{
public:
    NSInteger m_section;
    TLayout *m_layout;
    
    CGRect m_frame; // 基于当前Section区域的位置，使用时需要转为CollectionView坐标
    
protected:
    struct {
        unsigned int sectionInvalidated : 1;    // The whole section is invalidated
        unsigned int headerInvalidated : 1;
        unsigned int itemsInvalidated : 1; // Some of items are invalidated
        unsigned int footerInvalidated : 1;
        unsigned int reserved : 4;
        unsigned int minimalInvalidatedItem : 24;   // If minimal invalidated item is greater than 2^28, just set sectionInvalidated to 1
    } m_invalidationContext;
    
public:
    UIFlexItem m_header;
    std::vector<UIFlexItem *> m_items;
    UIFlexItem m_footer;
    
    UISectionT(UICollectionViewFlexLayout *layout, NSInteger section, CGPoint origin) : m_section(section), m_layout(layout), m_frame({.origin = origin, .size = CGSizeZero}), m_header(0), m_footer(0)
    {
    }
    
    virtual ~UISectionT()
    {
        m_layout = nil;
        for(std::vector<UIFlexItem *>::iterator it = m_items.begin(); it != m_items.end(); delete *it, ++it);
        m_items.clear();
    }
    
    inline const CGRect getFrame() const { return m_frame; }
    inline CGRect &getFrame() { return m_frame; }
    
    inline const CGRect getItemsFrame() const
    {
        return IS_CV_VERTICAL(m_layout) ? CGRectMake(m_frame.origin.x, m_frame.origin.y + m_header.getFrame().size.height, m_frame.size.width, m_frame.size.height - m_header.getFrame().size.height - m_footer.getFrame().size.height) : CGRectMake(m_frame.origin.x + m_header.getFrame().origin.x, m_frame.origin.y, m_frame.size.width - m_header.getFrame().size.width - m_footer.getFrame().size.width, m_frame.size.height);
    }

    virtual void prepareLayout() = 0;
    virtual BOOL getLayoutAttributesForItemsInRect(NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributes, const CGRect &rectInSection) = 0;
    
    bool getLayoutAttributesInRect(NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributes, const CGRect &rect)
    {
        bool merged = false;
        
        CGRect rectInSection = CGRectIntersection(*((CGRect *)&m_frame.origin), rect);
        if (CGRectIsEmpty(rectInSection))
        {
            return merged;
        }
        
        // 转成section坐标系
        rectInSection.origin.x -= m_frame.origin.x;
        rectInSection.origin.y -= m_frame.origin.y;
        
        // Header
        if (!CGSizeEqualToSize(m_header.m_frame.size, CGSizeZero) && CGRectIntersectsRect(m_header.getFrame(), rectInSection))
        {
            UICollectionViewLayoutAttributes *la = [m_layout layoutAttributesForSupplementaryViewOfKind:UICollectionElementKindSectionHeader atIndexPath:[NSIndexPath indexPathForItem:0 inSection:m_section]];
            
            [layoutAttributes addObject:la];
            merged = true;
            // la.frame = CGRectMake(la.frame.origin.x, la.frame.origin.y, la.frame.size.width, la.frame.size.height);
        }
        
        // Items
        if (getLayoutAttributesForItemsInRect(layoutAttributes, rectInSection))
        {
            merged = true;
        }
        
        // Footer
        if (!CGSizeEqualToSize(m_footer.m_frame.size, CGSizeZero) && CGRectIntersectsRect(m_footer.getFrame(), rectInSection))
        {
            UICollectionViewLayoutAttributes *la = [m_layout layoutAttributesForSupplementaryViewOfKind:UICollectionElementKindSectionFooter atIndexPath:[NSIndexPath indexPathForItem:0 inSection:m_section]];
            
            [layoutAttributes addObject:la];
            merged = true;
        }
        
        return merged;
    }
    
    void resetInvalidationContext()
    {
        unsigned int value = ~0;
        *((unsigned int *)&m_invalidationContext) = (value >> 8);
    }
    
    bool isSectionInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
    bool isHeaderInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
    bool hasInvalidatedItem() const { return m_invalidationContext.itemsInvalidated == 1; }
    bool isFooterInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
    unsigned int getMinimalInvalidatedItem() const { return m_invalidationContext.minimalInvalidatedItem; }
    inline void invalidateSection()
    {
        m_invalidationContext.sectionInvalidated = 1;
    }
    
    inline void invalidateHeader()
    {
        m_invalidationContext.headerInvalidated = 1;
    }
    
    void invalidateFooter()
    {
        m_invalidationContext.footerInvalidated = 1;
    }
    
    void invalidateItem(NSInteger item)
    {
        m_invalidationContext.sectionInvalidated = 1;
        unsigned int maximum = ~0;
        maximum = (maximum >> 8);
        if (item > (NSInteger)maximum)
        {
            m_invalidationContext.sectionInvalidated = 1;
            m_invalidationContext.minimalInvalidatedItem = maximum;
        }
        else
        {
            m_invalidationContext.minimalInvalidatedItem = (unsigned int)item;
        }
    }
    
    void clearLayoutAttributes(NSInteger item = 0)
    {
        if (item >= m_items.size())
        {
            return;
        }
        
        std::vector<UIFlexItem *>::iterator it = m_items.begin() + item;
        for (; it != m_items.end(); ++it)
        {
            (*it)->clearLayoutAttributes();
        }
    }
};

template<class TLayout>
struct UISectionVerticalCompareT
{
    bool operator() (const UISectionT<TLayout>* section, const std::pair<CGFloat, CGFloat>& topBottom) const
    {
        return section->m_frame.origin.y + section->m_frame.size.height < topBottom.first;
    }
    bool operator() (const std::pair<CGFloat, CGFloat>& topBottom, const UISectionT<TLayout>* section) const
    {
        return topBottom.second < section->m_frame.origin.y;
    }
};

template<class TLayout>
struct UISectionHorizontalCompareT
{
    bool operator() (const UISectionT<TLayout>* section, const std::pair<CGFloat, CGFloat>& leftRight) const
    {
        return section->m_frame.origin.x + section->m_frame.size.width < leftRight.first;
    }
    bool operator() (const std::pair<CGFloat, CGFloat>& leftRight, const UISectionT<TLayout>* section) const
    {
        return leftRight.second < section->m_frame.origin.x;
    }
};

#endif /* Section_h */
