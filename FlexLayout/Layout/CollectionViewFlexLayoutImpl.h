//
//  CollectionViewFlexLayoutImpl.h
//  FlexLayout
//
//  Created by Matthew on 2020/8/12.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#include "FlexLayout.h"
#include "CollectionViewFlexAdapter.h"

#ifndef CollectionViewFlexLayoutImpl_h
#define CollectionViewFlexLayoutImpl_h

namespace nsflex
{
    // Redefinition with the Point/Size in MacTypes.h
    using Point = PointT<CGFloat>;
    using Size = SizeT<CGFloat>;
    using Rect = RectT<CGFloat>;
    using Insets = InsetsT<CGFloat>;
}

using HeaderSectionItem = SectionItemT<NSInteger, (char)(nsflex::FlexItemT<NSInteger, CGFloat>::ITEM_TYPE_HEADER)>;
using StickyItemState = StickyItemStateT<CGFloat>;

using StickyItem = StickyItemT<HeaderSectionItem, StickyItemState>;
using StickyItemCompare = StickyItemAndSectionItemCompareT<HeaderSectionItem, StickyItemState>;
using StickyItemAndSectionItemCompare = StickyItemAndSectionItemCompareT<HeaderSectionItem, StickyItemState>;
using StickyItemList = std::vector<StickyItem>;

using LayoutItem = LayoutItemT<NSInteger, CGFloat>;
template <bool VERTICAL>
using Section = nsflex::FlexSectionT<CollectionViewFlexLayoutAdapter, NSInteger, CGFloat, VERTICAL>;
template <bool VERTICAL>
using FlexLayout = FlexLayoutT<CollectionViewFlexLayoutAdapter, Section<VERTICAL>, VERTICAL>;

inline nsflex::Point FlexPointFromCGPoint(const CGPoint& point)
{
    return nsflex::Point(point.x, point.y);
}

inline CGPoint CGPointFromFlexPoint(const nsflex::Point& point)
{
    CGPoint pt;
    pt.x = point.x;
    pt.y = point.y;
    return pt;
}

inline CGSize CGSizeFromFlexSize(const nsflex::Size& size)
{
    CGSize cgSize;
    cgSize.width = size.width;
    cgSize.height = size.height;
    return cgSize;
}

inline nsflex::Size FlexSizeFromCGSize(const CGSize& size)
{
    return nsflex::Size(size.width, size.height);
}

inline nsflex::Rect FlexRectFromCGRect(const CGRect& rect)
{
    return nsflex::Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

inline CGRect CGRectFromFlexRect(const nsflex::Rect& rect)
{
    CGRect result;
    result.origin.x = rect.left();
    result.origin.y = rect.top();
    result.size.width = rect.width();
    result.size.height = rect.height();
    
    return result;
}

inline nsflex::Insets FlexInsetsFromUIEdgeInsets(const UIEdgeInsets& insets)
{
    return nsflex::Insets(insets.left, insets.top, insets.right, insets.bottom);
}

#endif /* CollectionViewFlexLayoutImpl_h */
