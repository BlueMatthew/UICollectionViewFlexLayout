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
    static const unsigned char ITEM_TYPE_DECORATION = ~0;
    static const unsigned char ITEM_TYPE_HEADER = (unsigned char)-1;
    static const unsigned char ITEM_TYPE_ITEM = 0;
    static const unsigned char ITEM_TYPE_FOOTER = 1;
    
    template <typename TInt, typename TCoordinate>
    class FlexItemT
    {
    public:
        using IntType = TInt;
        using CoordinateType = TCoordinate;
        using Rect = RectT<TCoordinate>;

    private:
        TInt m_item;
        Rect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
        union {
            struct
            {
                unsigned int m_type : 8;
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
        
        FlexItemT(const FlexItemT &src) : m_item(src.m_item), m_frame(src.m_frame), m_flags(src.m_flags)
        {
        }
        
        inline FlexItemT& operator=(const FlexItemT &rhs)
        {
            if (this != &rhs)
            {
                m_item = rhs.m_item;
                m_frame = rhs.m_frame;
                m_flags = rhs.m_flags;
            }
            return *this;
        }
        
        inline bool operator<(const FlexItemT &rhs) const
        {
            return ((char)m_type) < (rhs.getType()) || ((m_type) == (rhs.getType()) && (m_item == rhs.getItem()));
        }
        
        inline char getType() const { return (char)m_type; }
        
        inline bool isHeader() const { return m_type == ITEM_TYPE_HEADER; }
        inline void setHeader(bool header) { m_type = header ? ITEM_TYPE_HEADER : ITEM_TYPE_ITEM; }

        inline bool isFooter() const { return m_type == ITEM_TYPE_FOOTER; }
        inline void setFooter(bool footer) { m_type = footer ? ITEM_TYPE_FOOTER : ITEM_TYPE_ITEM; }

        inline bool isDecoration() const { return m_type == ITEM_TYPE_FOOTER; }
        inline void setDecoration(bool decoration) { m_type = decoration ? ITEM_TYPE_DECORATION : ITEM_TYPE_ITEM; }

        inline bool isItem() const { return m_type == ITEM_TYPE_ITEM; }

        inline bool isFullSpan() const { return m_fullSpan; }
        inline void setFullSpan(bool fullSpan) { m_fullSpan = fullSpan ? 1 : 0; }

        inline bool isPlaceHolder() const { return m_placeHolder; }
        inline void setPlaceHolder(bool placeHolder) { m_placeHolder = placeHolder ? 1 : 0; }

        inline TInt getItem() const { return m_item; }
        inline Rect &getFrame() { return m_frame; }
        inline Rect getFrame() const { return m_frame; }

    };

    template<typename T, bool VERTICAL>
    struct FlexCompareT
    {
        inline bool operator() ( const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom) const
        {
            return less(item, topBottom, bool_trait<VERTICAL>());
        }
        inline bool operator() ( const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item ) const
        {
            return less(topBottom, item, bool_trait<VERTICAL>());
        }

    protected:
        inline bool less(const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, bool_trait<true>) const
        {
            return item->getFrame().bottom() < topBottom.first;
        }
        inline bool less(const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, bool_trait<false>) const
        {
            return item->getFrame().right() < topBottom.first;
        }

        inline bool less(const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item, bool_trait<true>) const
        {
            return topBottom.second < item->getFrame().top();
        }
        inline bool less(const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item, bool_trait<false>) const
        {
            return topBottom.second < item->getFrame().left();
        }
    };

    template<typename T>
    struct FlexVerticalCompareT
    {
        inline bool operator() ( const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom) const
        {
            return item->getFrame().bottom() < topBottom.first;
        }
        inline bool operator() ( const std::pair<typename T::CoordinateType, typename T::CoordinateType>& topBottom, const T* item ) const
        {
            return topBottom.second < item->getFrame().top();
        }
    };

    template<typename T>
    struct FlexHorizontalCompareT
    {
        inline bool operator() ( const T* item, const std::pair<typename T::CoordinateType, typename T::CoordinateType>& leftRight) const
        {
            return item->getFrame().right() < leftRight.first;
        }
        inline bool operator() ( const std::pair<typename T::CoordinateType, typename T::CoordinateType>& leftRight, const T* item ) const
        {
            return leftRight.second < item->getFrame().left();
        }
    };

    template<typename T>
    struct FlexHorizontalSizeCompare
    {
        inline bool operator() ( const T* lhs, const T* rhs) const
        {
            return lhs->getFrame().width() < rhs->getFrame().width();
        }
    };

    template<typename T>
    struct FlexVerticalSizeCompare
    {
        inline bool operator() ( const T* lhs, const T* rhs) const
        {
            return lhs->getFrame().height() < rhs->getFrame().height();
        }
    };


    template<typename T, bool VERTICAL>
    struct FlexSizeCompare
    {

        inline bool operator() ( const T* lhs, const T* rhs) const
        {
            return less(lhs, rhs, bool_trait<VERTICAL>());
        }
    protected:
        inline bool less(const T* item, const T* rhs, bool_trait<true>) const
        {
            return item->getFrame().height() < rhs->getFrame().height();
        }
        inline bool less(const T* item, const T* rhs, bool_trait<false>) const
        {
            return item->getFrame().width() < rhs->getFrame().width();
        }
    };



} // namespace nsflex

#endif /* FlexItem_h */
