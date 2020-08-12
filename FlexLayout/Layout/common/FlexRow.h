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
#include "ContainerBase.h"
#include <vector>
#include <utility>

namespace nsflex
{

template <typename TInt, typename TCoordinate, bool VERTICAL>
class FlexRowT : public ContainerBaseT<TCoordinate, VERTICAL>
{
public:
    using TBase = ContainerBaseT<TCoordinate, VERTICAL>;
    using CoordinateType = TCoordinate;
    using FlexItem = FlexItemT<TInt, TCoordinate>;
    using FlexItemIterator = typename std::vector<FlexItem *>::iterator;
    using FlexItemConstIterator = typename std::vector<FlexItem *>::const_iterator;
    
    using Rect = RectT<TCoordinate>;
    
    using TBase::x;
    using TBase::y;
    using TBase::left;
    using TBase::top;
    using TBase::right;
    using TBase::bottom;
    
    using TBase::offset;
    using TBase::offsetX;
    using TBase::offsetY;
    using TBase::incWidth;
    
    using TBase::leftBottom;
    using TBase::height;
    using TBase::width;
    
    using TBase::hinsets;
    using TBase::vinsets;

protected:
    std::vector<FlexItem *> m_items;
    
    Rect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    
public:
    FlexRowT()
    {
    }
    
    inline Rect getFrame() const { return m_frame; }
    inline Rect& getFrame() { return m_frame; }
    
    bool hasItems() const
    {
        return !m_items.empty();
    }
    
    bool lessThan(TInt item) const
    {
        // empty() ???
        return m_items.back()->getItem() < item;
    }
    
    bool greaterThan(TInt item) const
    {
        // empty() ???
        return m_items.front()->getItem() > item;
    }

    inline void addItem(FlexItem *item)
    {
        m_items.push_back(item);
        
        incWidth(m_frame, width(item->getFrame()));
        if (height(m_frame) < height(item->getFrame())) height(m_frame, height(item->getFrame()));
    }
    
    inline void adjustHeight()
    {
        TCoordinate newHeight = 0;
        
        for (FlexItemConstIterator it = m_items.cbegin(); it != m_items.cend(); ++it)
        {
            if (newHeight < height((*it)->getFrame()))
            {
                newHeight = height((*it)->getFrame());
            }
        }
        height(m_frame, newHeight);
    }
    
    inline bool removeItemsFrom(TInt item)
    {
        FlexItemIterator it = std::lower_bound(m_items.begin(), m_items.end(), item, FlexItemLessCompareT<TInt, TCoordinate>());
        if (it != m_items.end())
        {
            bool heightChanged = height((*it)->getFrame()) >= height(m_frame);
            m_items.erase(it, m_items.end());
            if (heightChanged)
            {
                adjustHeight();
            }
            return true;
        }
        return false;
    }
    
    inline bool removeLastItemIfEquals(TInt itemIndex)
    {
        if (!m_items.empty())
        {
            FlexItemIterator it = m_items.end() - 1;
            if ((*it)->getItem() == itemIndex)
            {
                bool heightChanged = height(m_items.back()->getFrame()) >= height(m_frame);
                m_items.erase(it);
                if (heightChanged)
                {
                    adjustHeight();
                }
                return true;
            }
        }
        return false;
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

template <typename TInt, typename TCoordinate, bool VERTICAL>
struct FlexRowItemLessCompareT
{
    using FlexRow = FlexRowT<TInt, TCoordinate, VERTICAL>;

    inline bool operator() ( const FlexRow* lhs, TInt item) const
    {
        return lhs->lessThan(item);
    }
    
};

template <typename TInt, typename TCoordinate, bool VERTICAL>
struct FlexRowItemGreaterCompareT
{
    using FlexRow = FlexRowT<TInt, TCoordinate, VERTICAL>;

    inline bool operator() ( const FlexRow* lhs, TInt item) const
    {
        return lhs->greaterThan(item);
    }
    
};

} // namespace nsflex

#endif /* FlexRow_h */
