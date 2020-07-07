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
protected:
    std::vector<UIFlexItem *> m_items;
    
    CGRect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    
public:
    UIFlexRow() : m_frame(CGRectZero)
    {
    }
    
    inline const CGRect& getFrame() const { return m_frame; }
    inline CGRect& getFrame() { return m_frame; }
    
    bool hasItems() const
    {
        return !m_items.empty();
    }
    
    inline void addItem(UIFlexItem *item, bool vertical)
    {
        m_items.push_back(item);
        if (vertical)
        {
            m_frame.size.width += item->getFrame().size.width;
            if (m_frame.size.height < item->getFrame().size.height) m_frame.size.height = item->getFrame().size.height;
        }
        else
        {
            m_frame.size.height += item->getFrame().size.height;
            if (m_frame.size.width < item->getFrame().size.width) m_frame.size.width = item->getFrame().size.width;
        }
    }
    
    inline void addItemVertically(UIFlexItem *item)
    {
        m_items.push_back(item);
        
        m_frame.size.width += item->getFrame().size.width;
        if (m_frame.size.height < item->getFrame().size.height) m_frame.size.height = item->getFrame().size.height;
    }
    
    inline void addItemHorizontally(UIFlexItem *item)
    {
        m_items.push_back(item);
        
        m_frame.size.height += item->getFrame().size.height;
        if (m_frame.size.width < item->getFrame().size.width) m_frame.size.width = item->getFrame().size.width;
    }
    
    inline std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator> getItemIterator()
    {
        return std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator>(m_items.begin(), m_items.end());
    }
    
    inline void getItemsInRect(std::vector<UIFlexItem *> &items, const CGRect& rect)
    {
        for (std::vector<UIFlexItem *>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            if (CGRectIntersectsRect((*it)->getFrame(), rect))
            {
                items.push_back(*it);
            }
        }
    }
    
};

/*
struct UIFlexRowItemCompare
{
    bool operator() ( const UIFlexRow* row, NSInteger item) const
    {
        return row->m_items[row->m_items.size() - 1]->getItem() < item;
    }
    bool operator() ( NSInteger item, const UIFlexRow* row ) const
    {
        return item < row->m_items[0]->getItem();
    }
};
 */


struct UIFlexRowVerticalCompare
{
    bool operator() ( const UIFlexRow* row, const std::pair<CGFloat, CGFloat>& topBottom) const
    {
        return row->getFrame().origin.y + row->getFrame().size.height < topBottom.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& topBottom, const UIFlexRow* row ) const
    {
        return topBottom.second < row->getFrame().origin.y;
    }
};

struct UIFlexRowHorizontalCompare
{
    bool operator() ( const UIFlexRow* row, const std::pair<CGFloat, CGFloat>& leftRight) const
    {
        return row->getFrame().origin.x + row->getFrame().size.width < leftRight.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& leftRight, const UIFlexRow* row ) const
    {
        return leftRight.second < row->getFrame().origin.x;
    }
};



#endif /* FlexRow_h */
