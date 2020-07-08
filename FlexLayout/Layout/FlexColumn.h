//
//  FlexColumn.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlexColumn_h
#define FlexColumn_h

#include <vector>
#include <algorithm>

// Column Data for Waterfall Layout
class UIFlexColumn
{
protected:
    std::vector<UIFlexItem *> m_items;
    
    CGRect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView

    bool m_isEmpty;
    
public:
    inline const CGRect getFrame() const { return m_frame; }
    inline CGRect& getFrame() { return m_frame; }
    
    
    UIFlexColumn() : m_frame(CGRectZero), m_isEmpty(true)
    {
    }
    
    UIFlexColumn(NSInteger estimatedNumberOfItems) : m_frame(CGRectZero), m_isEmpty(true)
    {
        m_items.reserve(estimatedNumberOfItems);
    }
    
    UIFlexColumn(NSInteger estimatedNumberOfItems, const CGRect &frame) : m_frame(frame), m_isEmpty(true)
    {
        m_items.reserve(estimatedNumberOfItems);
    }
    
    bool isEmpty() const
    {
        return m_isEmpty;
    }
    
    /*
    inline void addItem(UIFlexItem *item, bool vertical)
    {
        m_items.push_back(item);
        
        // vertical ? (m_size.height += item->m_size.height) : m_size.width += item->m_size.width;
        vertical ? (m_frame.size.height = item->m_frame.origin.y + item->m_frame.size.height - (*(m_items.begin()))->m_frame.origin.y) : (m_frame.size.width = item->m_frame.origin.x + item->m_frame.size.width - (*(m_items.begin()))->m_frame.origin.x);
    }
     */
    
    inline void addItemVertically(UIFlexItem *item, bool fake = false)
    {
        if (!fake)
        {
            m_items.push_back(item);
        }
        m_frame.size.height = CGRectGetMaxY(item->getFrame());
        // m_frame.size.width = CGRectGetMaxX(item->getFrame());
        m_isEmpty = false;
    }
    
    inline void addItemHorizontally(UIFlexItem *item, bool fake = false)
    {
        if (!fake)
        {
            m_items.push_back(item);
        }
        m_frame.size.width = CGRectGetMaxX(item->getFrame());
        m_isEmpty = false;
    }
    
    inline std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator> getVirticalItemsInRect(const CGRect& rect)
    {
        return std::equal_range(m_items.begin(), m_items.end(), std::pair<CGFloat, CGFloat>(rect.origin.y, rect.origin.y + rect.size.height), UIFlexItemVerticalCompare());
    }
    
    inline std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator> getHorizontalItemsInRect(const CGRect& rect)
    {
        return std::equal_range(m_items.begin(), m_items.end(), std::pair<CGFloat, CGFloat>(rect.origin.x, rect.origin.x + rect.size.width), UIFlexItemHorizontalCompare());
    }
};

struct UIFlexColumnHorizontalSizeCompare
{
    bool operator() ( const UIFlexColumn* lhs, const UIFlexColumn* rhs) const
    {
        return lhs->getFrame().size.width < rhs->getFrame().size.width;
    }
};

struct UIFlexColumnVerticalSizeCompare
{
    bool operator() ( const UIFlexColumn* lhs, const UIFlexColumn* rhs) const
    {
        return lhs->getFrame().size.height < rhs->getFrame().size.height;
    }
};

#endif /* FlexColumn_h */
