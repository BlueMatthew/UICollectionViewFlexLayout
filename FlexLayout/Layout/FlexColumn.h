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

namespace nsflex
{

// Column Data for Waterfall Layout
template <typename TInt, typename TCoordinate>
class FlexColumnT
{
public:
    typedef TCoordinate CoordinateType;
    typedef FlexItemT<TInt, TCoordinate> FlexItem;
    typedef RectT<TCoordinate> Rect;
    
protected:
    std::vector<FlexItem *> m_items;
    
    Rect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    
public:
    inline const Rect getFrame() const { return m_frame; }
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

    inline void addItemVertically(FlexItem *item)
    {
        m_items.push_back(item);
        m_frame.size.height = item->getFrame().bottom();
        // m_frame.size.width = CGRectGetMaxX(item->getFrame());
    }
    
    inline void addItemHorizontally(FlexItem *item)
    {
        m_items.push_back(item);
        m_frame.size.width = item->getFrame().right();
    }
    
    inline std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> getVirticalItemsInRect(const Rect& rect)
    {
        return std::equal_range(m_items.begin(), m_items.end(), std::pair<TCoordinate, TCoordinate>(rect.top(), rect.bottom()), FlexVerticalCompareT<FlexItem>());
    }
    
    inline std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> getHorizontalItemsInRect(const Rect& rect)
    {
        return std::equal_range(m_items.begin(), m_items.end(), std::pair<TCoordinate, TCoordinate>(rect.left(), rect.right()), FlexHorizontalCompareT<FlexItem>());
    }
};


} // namespace nsflex

#endif /* FlexColumn_h */
