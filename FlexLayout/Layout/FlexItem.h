//
//  FlexItem.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlexItem_h
#define FlexItem_h

#include <map>
#include "Graphics.h"
#include "BoolTraits.h"

namespace nsflex
{

template <typename TInt, typename TCoordinate>
class FlexItemT
{
public:
    typedef TCoordinate CoordinateType;
    typedef RectT<TCoordinate> Rect;

private:
    TInt m_item;
    Rect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    union {
        struct
        {
            unsigned int m_header : 1;
            unsigned int m_footer : 1;
            unsigned int m_decoration : 1;
            unsigned int m_placeHolder : 1;
            unsigned int m_fullSpan : 1;
        };
        unsigned int m_flags;
    };

public:
    FlexItemT() : m_item(0), m_frame(), m_flags(0)
    {
    }
    
    FlexItemT(TInt item) : m_item(item), m_frame(), m_flags(0)
    {
    }
    
    FlexItemT(TInt item, const Rect &frame) : m_item(item), m_frame(frame), m_flags(0)
    {
    }
    
    bool isHeader() const { return m_header; }
    void setHeader(bool header) { m_header = header ? 1 : 0; }
    
    bool isFooter() const { return m_footer; }
    void setFooter(bool footer) { m_footer = footer ? 1 : 0; }
    
    bool isDecoration() const { return m_decoration; }
    void setDecoration(bool decoration) { m_decoration = decoration ? 1 : 0; }
    
    bool isItem() const { return !(m_header || m_footer || m_decoration); }
    
    bool isFullSpan() const { return m_fullSpan; }
    void setFullSpan(bool fullSpan) { m_fullSpan = fullSpan ? 1 : 0; }
    
    bool isPlaceHolder() const { return m_placeHolder; }
    void setPlaceHolder(bool placeHolder) { m_placeHolder = placeHolder ? 1 : 0; }

    inline TInt getItem() const { return m_item; }
    inline Rect &getFrame() { return m_frame; }
    inline const Rect getFrame() const { return m_frame; }

};
    
template<typename T, bool VERTICAL>
struct FlexCompareT
{
    bool operator() ( const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom) const
    {
        return less(item, topBottom, bool_trait<VERTICAL>());
    }
    bool operator() ( const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item ) const
    {
        return less(topBottom, item, bool_trait<VERTICAL>());
    }
    
protected:
    bool less(const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, bool_trait<true>) const
    {
        return item->getFrame().bottom() < topBottom.first;
    }
    bool less(const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, bool_trait<false>) const
    {
        return item->getFrame().right() < topBottom.first;
    }
    
    bool less(const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item, bool_trait<true>) const
    {
        return topBottom.second < item->getFrame().top();
    }
    bool less(const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item, bool_trait<false>) const
    {
        return topBottom.second < item->getFrame().left();
    }
};

template<typename T>
struct FlexVerticalCompareT
{
    bool operator() ( const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom) const
    {
        return item->getFrame().bottom() < topBottom.first;
    }
    bool operator() ( const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item ) const
    {
        return topBottom.second < item->getFrame().top();
    }
};

template<typename T>
struct FlexHorizontalCompareT
{
    bool operator() ( const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& leftRight) const
    {
        return item->getFrame().right() < leftRight.first;
    }
    bool operator() ( const std::pair<typename T::CoordinateType, typename T::CoordinateType>& leftRight, const T* item ) const
    {
        return leftRight.second < item->getFrame().left();
    }
};
  
template<typename T>
struct FlexHorizontalSizeCompare
{
    bool operator() ( const T* lhs, const T* rhs) const
    {
        return lhs->getFrame().width() < rhs->getFrame().width();
    }
};

template<typename T>
struct FlexVerticalSizeCompare
{
    bool operator() ( const T* lhs, const T* rhs) const
    {
        return lhs->getFrame().height() < rhs->getFrame().height();
    }
};

    
} // namespace nsflex

#endif /* FlexItem_h */
