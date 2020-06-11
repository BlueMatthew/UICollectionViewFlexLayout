//
//  FlexRow.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlexRow_h
#define FlexRow_h

#include "FlexItem.h"
#include <vector>
#include <utility>

class UIFlexRow
{
public:
    std::vector<UIFlexItem *> m_items;
    
    CGRect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    
    UIFlexRow() : m_frame(CGRectZero)
    {
    }
    
    inline const CGRect& getFrame() const { return m_frame; }
    
    inline void addItem(UIFlexItem *item, bool vertical)
    {
        m_items.push_back(item);
        if (vertical)
        {
            m_frame.size.width += item->m_frame.size.width;
            if (m_frame.size.height < item->m_frame.size.height) m_frame.size.height = item->m_frame.size.height;
        }
        else
        {
            m_frame.size.height += item->m_frame.size.height;
            if (m_frame.size.width < item->m_frame.size.width) m_frame.size.width = item->m_frame.size.width;
        }
    }
    
    /*
     inline pair<vector<UISectionItem *>::iterator, vector<UISectionItem *>::iterator> getVirticalItemsInRect(const CGRect& rect)
     {
     return equal_range(items.begin(), items.end(), pair<CGFloat, CGFloat>(rect.origin.y, rect.origin.y + rect.size.height), UISectionItemVerticalCompare());
     }
     
     inline pair<vector<UISectionItem *>::iterator, vector<UISectionItem *>::iterator> getHorizontalItemsInRect(const CGRect& rect)
     {
     return equal_range(items.begin(), items.end(), pair<CGFloat, CGFloat>(rect.origin.x, rect.origin.x + rect.size.width), UISectionItemHorizontalCompare());
     }
     */
};

struct UISectionRowItemCompare
{
    bool operator() ( const UIFlexRow* row, NSInteger item) const
    {
        return row->m_items[row->m_items.size() - 1]->m_item < item;
    }
    bool operator() ( NSInteger item, const UIFlexRow* row ) const
    {
        return item < row->m_items[0]->m_item;
    }
};

struct UISectionRowVerticalCompare
{
    bool operator() ( const UIFlexRow* row, const std::pair<CGFloat, CGFloat>& topBottom) const
    {
        return row->m_frame.origin.y + row->m_frame.size.height < topBottom.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& topBottom, const UIFlexRow* row ) const
    {
        return topBottom.second < row->m_frame.origin.y;
    }
};

struct UISectionRowHorizontalCompare
{
    bool operator() ( const UIFlexRow* row, const std::pair<CGFloat, CGFloat>& leftRight) const
    {
        return row->m_frame.origin.x + row->m_frame.size.width < leftRight.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& leftRight, const UIFlexRow* row ) const
    {
        return leftRight.second < row->m_frame.origin.x;
    }
};



#endif /* SectionRow_h */
