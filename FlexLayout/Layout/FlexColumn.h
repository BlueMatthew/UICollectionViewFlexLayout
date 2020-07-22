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
#include <string>
#include <sstream>

#include "ContainerBase.h"

namespace nsflex
{

// Column Data for Waterfall Layout
template <typename TInt, typename TCoordinate, bool VERTICAL>
    class FlexColumnT : public ContainerBaseT<TCoordinate, VERTICAL>
{
public:
    using TBase = ContainerBaseT<TCoordinate, VERTICAL>;
    using CoordinateType = TCoordinate;
    using FlexItem = FlexItemT<TInt, TCoordinate>;
    using Rect = RectT<TCoordinate>;

    using ItemCompare = FlexCompareT<FlexItem, VERTICAL>;
    
    // using TBase::x;
    // using TBase::y;
    // using TBase::left;
    using TBase::top;
    // using TBase::right;
    using TBase::bottom;
    
    // using TBase::offset;
    // using TBase::offsetX;
    // using TBase::offsetY;
    // using TBase::incWidth;
    
    // using TBase::leftBottom;
    using TBase::height;
    // using TBase::width;
    
    // using TBase::hsize;
    // using TBase::vsize;
    
protected:
    std::vector<FlexItem *> m_items;
    
    Rect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    
public:
    inline Rect getFrame() const { return m_frame; }
    inline Rect& getFrame() { return m_frame; }
    
    FlexColumnT()
    {
    }
    
    FlexColumnT(TInt estimatedNumberOfItems)
    {
        m_items.reserve(estimatedNumberOfItems);
    }
    
    FlexColumnT(TInt estimatedNumberOfItems, const Rect &frame) : m_frame(frame)
    {
        m_items.reserve(estimatedNumberOfItems);
    }
    
    bool isEmpty() const
    {
        return m_items.empty();
    }

    inline void addItem(FlexItem *item)
    {
        m_items.push_back(item);
        height(m_frame, bottom(item->getFrame()));
    }

    inline std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> getItemsInRect(const Rect& rect)
    {
        return std::equal_range(m_items.begin(), m_items.end(), std::pair<TCoordinate, TCoordinate>(top(rect), bottom(rect)), ItemCompare());
    }

#ifndef NDEBUG
    std::string printDebugInfo(std::string prefix) const
    {
        std::ostringstream str;


        int idx = 1;
        for (typename std::vector<FlexItem *>::const_iterator it = m_items.begin(); it != m_items.end(); ++it)
        {
            str << prefix << "Item " << idx << "[" << (*it)->getFrame().left() << "," << (*it)->getFrame().top() << "-" << (*it)->getFrame().width() << "," << (*it)->getFrame().height() << "]\r\n";

            idx++;
        }

        return str.str();
    }
#endif // #ifndef NDEBUG

};


} // namespace nsflex

#endif /* FlexColumn_h */
