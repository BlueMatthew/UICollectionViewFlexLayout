//
// Created by Matthew Shi on 2020-07-18.
//

#include "Graphics.h"
#include "BoolTraits.h"

#ifndef FLEXLAYOUT_ORIENTATIONHELPER_H
#define FLEXLAYOUT_ORIENTATIONHELPER_H

namespace nsflex
{

    template<typename TCoordinate, bool VERTICAL>
    struct ContainerBaseT
    {
    public:
        using Point = PointT<TCoordinate>;
        using Size = SizeT<TCoordinate>;
        using Rect = RectT<TCoordinate>;
        using Insets = InsetsT<TCoordinate>;
        
        
        // Point
        inline TCoordinate x(const Point &point) const
        {
            return x(point, bool_trait<VERTICAL>());
        }
        inline void x(Point &point, TCoordinate v) const
        {
            x(point, v, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate y(const Point &point) const
        {
            return y(point, bool_trait<VERTICAL>());
        }
        inline void y(Point &point, TCoordinate v) const
        {
            y(point, v, bool_trait<VERTICAL>());
        }
        
        inline Point& offset(Point &point, TCoordinate x, TCoordinate y) const
        {
            point.x += x;
            point.y += y;
            return point;
        }
        inline Point& offsetX(Point &point, TCoordinate x) const
        {
            return offsetX(point, x, bool_trait<VERTICAL>());
        }
        inline Point& offsetY(Point &point, TCoordinate y) const
        {
            return offsetY(point, y, bool_trait<VERTICAL>());
        }
        
        // Point Ends
        
        // Size
        inline TCoordinate width(const Size &size)
        {
            return width(size, bool_trait<VERTICAL>());
        }
        inline void width(Size &size, TCoordinate w)
        {
            width(size, w, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate height(const Size &size)
        {
            return height(size, bool_trait<VERTICAL>());
        }
        inline void height(Size &size, TCoordinate h)
        {
            height(size, h, bool_trait<VERTICAL>());
        }
        // Size Ends
        
        // Rect
        inline TCoordinate left(const Rect &rect)
        {
            return left(rect, bool_trait<VERTICAL>());
        }
        inline void left(Rect &rect, TCoordinate l)
        {
            left(rect, l, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate top(const Rect &rect)
        {
            return top(rect, bool_trait<VERTICAL>());
        }
        inline void top(Rect &rect, TCoordinate t)
        {
            top(rect, t, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate right(const Rect &rect)
        {
            return right(rect, bool_trait<VERTICAL>());
        }
        inline void right(Rect &rect, TCoordinate r)
        {
            right(rect, r, bool_trait<VERTICAL>());
        }

        inline TCoordinate bottom(const Rect &rect)
        {
            return bottom(rect, bool_trait<VERTICAL>());
        }
        inline void bottom(Rect &rect, TCoordinate b)
        {
            bottom(rect, b, bool_trait<VERTICAL>());
        }
        
        inline Point leftBottom(const Rect &rect)
        {
            return leftBottom(rect, bool_trait<VERTICAL>());
        }

        inline TCoordinate width(const Rect &rect)
        {
            return width(rect, bool_trait<VERTICAL>());
        }
        inline void width(Rect &rect, TCoordinate w)
        {
            width(rect, w, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate height(const Rect &rect)
        {
            return height(rect, bool_trait<VERTICAL>());
        }
        inline void height(Rect &rect, TCoordinate h)
        {
            height(rect, h, bool_trait<VERTICAL>());
        }
        
        inline Rect& offset(Rect &rect, TCoordinate x, TCoordinate y) const
        {
            rect.offset(x, y);
            return rect;
        }
        inline Rect& offsetX(Rect &rect, TCoordinate x) const
        {
            return offsetX(rect, x, bool_trait<VERTICAL>());
        }
        inline Rect offsetY(Rect &rect, TCoordinate y) const
        {
            return offsetY(rect, y, bool_trait<VERTICAL>());
        }
        /*
        inline void addSize(Rect &rect, TCoordinate dx, TCoordinate dy)
        {
            resize(rect, dx, dy, bool_trait<VERTICAL>());
        }
         */
        inline void incWidth(Rect &rect, TCoordinate dx)
        {
            incWidth(rect, dx, bool_trait<VERTICAL>());
        }
        inline void incHeight(Rect &rect, TCoordinate dy)
        {
            incHeight(rect, dy, bool_trait<VERTICAL>());
        }
        // Rect Ends
        
        // Insets
        inline TCoordinate left(const Insets &insets)
        {
            return left(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate top(const Insets &insets)
        {
            return top(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate right(const Insets &insets)
        {
            return right(insets, bool_trait<VERTICAL>());
        }

        inline TCoordinate bottom(const Insets &insets)
        {
            return bottom(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate leftRight(const Insets &insets)
        {
            return leftRight(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate topBottom(const Insets &insets)
        {
            return topBottom(insets, bool_trait<VERTICAL>());
        }
        // Insets Ends


    protected:
        
        // Point
        inline TCoordinate x(const Point &point, bool_trait<true>) const
        {
            return point.x;
        }
        inline TCoordinate x(const Point &point, bool_trait<false>) const
        {
            return point.y;
        }
        inline void x(const Point &point, TCoordinate v, bool_trait<true>) const
        {
            point.x = v;
        }
        inline void x(const Point &point, TCoordinate v, bool_trait<false>) const
        {
            point.y = v;
        }
        
        inline TCoordinate y(const Point &point, bool_trait<true>) const
        {
            return point.y;
        }
        inline TCoordinate y(const Point &point, bool_trait<false>) const
        {
            return point.x;
        }
        inline void y(Point &point, TCoordinate v, bool_trait<true>) const
        {
            point.y = v;
        }
        inline void y(Point &point, TCoordinate v, bool_trait<false>) const
        {
            point.x = v;
        }
        
        inline Point& offsetX(Point &point, TCoordinate x, bool_trait<true>) const
        {
            point.x += x;
            return point;
        }
        inline Point& offsetX(Point &point, TCoordinate x, bool_trait<false>) const
        {
            point.y += x;
            return point;
        }
        inline Point& offsetY(Point &point, TCoordinate y, bool_trait<true>) const
        {
            point.y += y;
            return point;
        }
        inline Point& offsetY(Point &point, TCoordinate y, bool_trait<false>) const
        {
            point.x += y;
            return point;
        }
        
       
        
        // Point Ends
        
        // Size
        inline TCoordinate width(const Size &size, bool_trait<true>)
        {
            return size.width;
        }
        inline TCoordinate width(const Size &size, bool_trait<false>)
        {
            return size.height;
        }
        inline void width(Size &size, TCoordinate w, bool_trait<true>)
        {
            size.width = w;
        }
        inline void width(Size &size, TCoordinate w, bool_trait<false>)
        {
            size.height = w;
        }
        
        inline TCoordinate height(const Size &size, bool_trait<true>)
        {
            return size.height;
        }
        inline TCoordinate height(const Size &size, bool_trait<false>)
        {
            return size.width;
        }
        inline void height(Size &size, TCoordinate h, bool_trait<true>)
        {
            size.height = h;
        }
        inline void height(Size &size, TCoordinate h, bool_trait<false>)
        {
            size.width = h;
        }
        // Size Ends
        
        
        // Rect
        inline Rect makeRect(TCoordinate x, TCoordinate y, TCoordinate width, TCoordinate height, bool_trait<true>)
        {
            return Rect(x, y, width, height);
        }
        
        inline Rect makeRect(TCoordinate x, TCoordinate y, TCoordinate width, TCoordinate height, bool_trait<false>)
        {
            return Rect(y, x, height, width);
        }
        
        inline TCoordinate left(const Rect &rect, bool_trait<true>)
        {
            return rect.origin.x;
        }
        inline TCoordinate left(const Rect &rect, bool_trait<false>)
        {
            return rect.origin.y;
        }
        inline void left(Rect &rect, TCoordinate l, bool_trait<true>)
        {
            rect.origin.x = l;
        }
        inline void left(Rect &rect, TCoordinate l, bool_trait<false>)
        {
            rect.origin.y = l;
        }
        
        inline TCoordinate top(const Rect &rect, bool_trait<true>)
        {
            return rect.origin.y;
        }
        inline TCoordinate top(const Rect &rect, bool_trait<false>)
        {
            return rect.origin.x;
        }
        inline void top(Rect &rect, TCoordinate t, bool_trait<true>)
        {
            rect.origin.y = t;
        }
        inline void top(Rect &rect, TCoordinate t, bool_trait<false>)
        {
            rect.origin.x = t;
        }
        
        inline TCoordinate right(const Rect &rect, bool_trait<true>)
        {
            return rect.right();
        }
        inline TCoordinate right(const Rect &rect, bool_trait<false>)
        {
            return rect.bottom();
        }
        inline void right(Rect &rect, TCoordinate r, bool_trait<true>)
        {
            rect.size.width = r - rect.left();
        }
        inline void right(Rect &rect, TCoordinate r, bool_trait<false>)
        {
            rect.size.height = r - rect.top();
        }

        inline TCoordinate bottom(const Rect &rect, bool_trait<true>)
        {
            return rect.bottom();
        }
        inline TCoordinate bottom(const Rect &rect, bool_trait<false>)
        {
            return rect.right();
        }
        inline void bottom(Rect &rect, TCoordinate b, bool_trait<true>)
        {
            rect.size.height = b - rect.top();
        }
        inline void bottom(Rect &rect, TCoordinate b, bool_trait<false>)
        {
            rect.size.width = b - rect.left();
        }
        
        inline Point leftBottom(const Rect &rect, bool_trait<true>)
        {
            return Point(rect.left(), rect.bottom());
        }
        inline Point leftBottom(const Rect &rect, bool_trait<false>)
        {
            return Point(rect.right(), rect.top());
        }
        
        inline TCoordinate width(const Rect &rect, bool_trait<true>)
        {
            return rect.size.width;
        }
        inline TCoordinate width(const Rect &rect, bool_trait<false>)
        {
            return rect.size.height;
        }
        inline void width(Rect &rect, TCoordinate w, bool_trait<true>)
        {
            rect.size.width = w;
        }
        inline void width(Rect &rect, TCoordinate w, bool_trait<false>)
        {
            rect.size.height = w;
        }
        
        inline TCoordinate height(const Rect &rect, bool_trait<true>)
        {
            return rect.size.height;
        }
        inline TCoordinate height(const Rect &rect, bool_trait<false>)
        {
            return rect.size.width;
        }
        inline void height(Rect &rect, TCoordinate h, bool_trait<true>)
        {
            rect.size.height = h;
        }
        inline void height(Rect &rect, TCoordinate h, bool_trait<false>)
        {
            rect.size.width = h;
        }
        
        inline Rect& offsetX(Rect &rect, TCoordinate x, bool_trait<true>) const
        {
            rect.origin.x += x;
            return rect;
        }
        inline Rect& offsetX(Rect &rect, TCoordinate x, bool_trait<false>) const
        {
            rect.origin.y += x;
            return rect;
        }
        inline Rect& offsetY(Rect &rect, TCoordinate y, bool_trait<true>) const
        {
            rect.origin.y += y;
            return rect;
        }
        inline Rect& offsetY(Rect &rect, TCoordinate y, bool_trait<false>) const
        {
            rect.origin.x += y;
            return rect;
        }
        
        inline void incWidth(Rect &rect, TCoordinate dx, bool_trait<true>)
        {
            rect.size.width += dx;
        }
        inline void incWidth(Rect &rect, TCoordinate dx, bool_trait<false>)
        {
            rect.size.height += dx;
        }
        inline void incHeight(Rect &rect, TCoordinate dy, bool_trait<true>)
        {
            rect.size.height += dy;
        }
        inline void incHeight(Rect &rect, TCoordinate dy, bool_trait<false>)
        {
            rect.size.width += dy;
        }
        // Rect Ends
        
        
        // Insets
        
        inline TCoordinate left(const Insets &insets, bool_trait<true>)
        {
            return insets.left;
        }
        inline TCoordinate left(const Insets &insets, bool_trait<false>)
        {
            return insets.top;
        }

        inline TCoordinate top(const Insets &insets, bool_trait<true>)
        {
            return insets.top;
        }
        inline TCoordinate top(const Insets &insets, bool_trait<false>)
        {
            return insets.left;
        }
        
        inline TCoordinate right(const Insets &insets, bool_trait<true>)
        {
            return insets.right;
        }
        inline TCoordinate right(const Insets &insets, bool_trait<false>)
        {
            return insets.bottom;
        }
        
        inline TCoordinate bottom(const Insets &insets, bool_trait<true>)
        {
            return insets.bottom;
        }
        inline TCoordinate bottom(const Insets &insets, bool_trait<false>)
        {
            return insets.right;
        }
        
        inline TCoordinate leftRight(const Insets &insets, bool_trait<true>)
        {
            return insets.left + insets.right;
        }
        inline TCoordinate leftRight(const Insets &insets, bool_trait<false>)
        {
            return insets.top + insets.bottom;
        }
        
        inline TCoordinate topBottom(const Insets &insets, bool_trait<true>)
        {
            return insets.top + insets.bottom;
        }
        inline TCoordinate topBottom(const Insets &insets, bool_trait<false>)
        {
            return insets.left + insets.right;
        }
        // Insets Ends

        

    };



}

#endif //FLEXLAYOUT_ORIENTATIONHELPER_H
