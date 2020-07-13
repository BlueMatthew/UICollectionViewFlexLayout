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

namespace nsflex
{

template <typename TInt, typename TCoordinate>
class FlexRowT
{
public:
    typedef TCoordinate CoordinateType;
    typedef FlexItemT<TInt, TCoordinate> FlexItem;
    typedef RectT<TCoordinate> Rect;

protected:
    std::vector<FlexItem *> m_items;
    
    Rect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    
public:
    FlexRowT()
    {
    }
    
    inline const Rect getFrame() const { return m_frame; }
    inline Rect& getFrame() { return m_frame; }
    
    bool hasItems() const
    {
        return !m_items.empty();
    }
    
    inline void addItem(FlexItem *item, bool vertical)
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
    
    inline void addItemVertically(FlexItem *item)
    {
        m_items.push_back(item);
        
        m_frame.size.width += item->getFrame().size.width;
        if (m_frame.size.height < item->getFrame().size.height) m_frame.size.height = item->getFrame().size.height;
    }
    
    inline void addItemHorizontally(FlexItem *item)
    {
        m_items.push_back(item);
        
        m_frame.size.height += item->getFrame().size.height;
        if (m_frame.size.width < item->getFrame().size.width) m_frame.size.width = item->getFrame().size.width;
    }
    
    inline std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> getItemIterator()
    {
        return std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator>(m_items.begin(), m_items.end());
    }
    
    inline void getItemsInRect(std::vector<FlexItem *> &items, const Rect& rect)
    {
        for (typename std::vector<FlexItem *>::iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            if ((*it)->getFrame().intersects(rect))
            {
                items.push_back(*it);
            }
        }
    }
    
};

} // namespace nsflex

#endif /* FlexRow_h */
