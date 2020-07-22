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
    struct VContainerBaseT
    {
        virtual ~VContainerBaseT()
        {

        }
    };

    template<typename TCoordinate, bool VERTICAL>
    struct NVContainerBaseT
    {
    };

    template<typename TCoordinate, bool VERTICAL>
    struct ContainerBaseT
    {
    public:
        using Point = PointT<TCoordinate>;
        using Size = SizeT<TCoordinate>;
        using Rect = RectT<TCoordinate>;
        using Insets = InsetsT<TCoordinate>;

        virtual ~ContainerBaseT()
        {
        }

        // Point
        inline Point makePoint(TCoordinate x, TCoordinate y) const
        {
            return makePoint(x, y, bool_trait<VERTICAL>());
        }
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
            return offset(point, x, y, bool_trait<VERTICAL>());
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
        inline Size makeSize(TCoordinate w, TCoordinate h) const
        {
            return makeSize(w, h, bool_trait<VERTICAL>());
        }

        inline TCoordinate width(const Size &size) const
        {
            return width(size, bool_trait<VERTICAL>());
        }
        inline void width(Size &size, TCoordinate w) const
        {
            width(size, w, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate height(const Size &size) const
        {
            return height(size, bool_trait<VERTICAL>());
        }
        inline void height(Size &size, TCoordinate h) const
        {
            height(size, h, bool_trait<VERTICAL>());
        }
        // Size Ends
        
        // Rect
        inline Rect makeRect(TCoordinate x, TCoordinate y, TCoordinate width, TCoordinate height) const
        {
            return makeRect(x, y, width, height, bool_trait<VERTICAL>());
        }

        inline TCoordinate left(const Rect &rect) const
        {
            return left(rect, bool_trait<VERTICAL>());
        }
        inline void left(Rect &rect, TCoordinate l) const
        {
            left(rect, l, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate top(const Rect &rect) const
        {
            return top(rect, bool_trait<VERTICAL>());
        }
        inline void top(Rect &rect, TCoordinate t) const
        {
            top(rect, t, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate right(const Rect &rect) const
        {
            return right(rect, bool_trait<VERTICAL>());
        }
        inline void right(Rect &rect, TCoordinate r) const
        {
            right(rect, r, bool_trait<VERTICAL>());
        }

        inline TCoordinate bottom(const Rect &rect) const
        {
            return bottom(rect, bool_trait<VERTICAL>());
        }
        inline void bottom(Rect &rect, TCoordinate b) const
        {
            bottom(rect, b, bool_trait<VERTICAL>());
        }
        
        inline Point leftBottom(const Rect &rect) const
        {
            return leftBottom(rect, bool_trait<VERTICAL>());
        }

        inline TCoordinate width(const Rect &rect) const
        {
            return width(rect, bool_trait<VERTICAL>());
        }
        inline void width(Rect &rect, TCoordinate w) const
        {
            width(rect, w, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate height(const Rect &rect) const
        {
            return height(rect, bool_trait<VERTICAL>());
        }
        inline void height(Rect &rect, TCoordinate h) const
        {
            height(rect, h, bool_trait<VERTICAL>());
        }
        
        inline Rect& offset(Rect &rect, TCoordinate x, TCoordinate y) const
        {
            return offset(rect, x, y, bool_trait<VERTICAL>());
        }
        inline Rect& offsetX(Rect &rect, TCoordinate x) const
        {
            return offsetX(rect, x, bool_trait<VERTICAL>());
        }
        inline Rect offsetY(Rect &rect, TCoordinate y) const
        {
            return offsetY(rect, y, bool_trait<VERTICAL>());
        }

        inline void incWidth(Rect &rect, TCoordinate dx) const
        {
            incWidth(rect, dx, bool_trait<VERTICAL>());
        }
        inline void incHeight(Rect &rect, TCoordinate dy) const
        {
            incHeight(rect, dy, bool_trait<VERTICAL>());
        }
        // Rect Ends
        
        // Insets
        inline TCoordinate left(const Insets &insets) const
        {
            return left(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate top(const Insets &insets) const
        {
            return top(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate right(const Insets &insets) const
        {
            return right(insets, bool_trait<VERTICAL>());
        }

        inline TCoordinate bottom(const Insets &insets) const
        {
            return bottom(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate hinsets(const Insets &insets) const
        {
            return hinsets(insets, bool_trait<VERTICAL>());
        }
        
        inline TCoordinate vinsets(const Insets &insets) const
        {
            return vinsets(insets, bool_trait<VERTICAL>());
        }
        // Insets Ends


    protected:
        
        // Point
        inline Point makePoint(TCoordinate x, TCoordinate y, bool_trait<true>) const
        {
            return Point(x, y);
        }
        inline Point makePoint(TCoordinate x, TCoordinate y, bool_trait<false>) const
        {
            return Point(y, x);
        }
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

        inline Point& offset(Point &point, TCoordinate x, TCoordinate y, bool_trait<true>) const
        {
            point.x += x;
            point.y += y;
            return point;
        }
        inline Point& offset(Point &point, TCoordinate x, TCoordinate y, bool_trait<false>) const
        {
            point.x += y;
            point.y += x;
            return point;
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

        inline Size makeSize(TCoordinate w, TCoordinate h, bool_trait<true>) const
        {
            return Size(w, h);
        }
        inline Size makeSize(TCoordinate w, TCoordinate h, bool_trait<false>) const
        {
            return Size(h, w);
        }

        inline TCoordinate width(const Size &size, bool_trait<true>) const
        {
            return size.width;
        }
        inline TCoordinate width(const Size &size, bool_trait<false>) const
        {
            return size.height;
        }
        inline void width(Size &size, TCoordinate w, bool_trait<true>) const
        {
            size.width = w;
        }
        inline void width(Size &size, TCoordinate w, bool_trait<false>) const
        {
            size.height = w;
        }
        
        inline TCoordinate height(const Size &size, bool_trait<true>) const
        {
            return size.height;
        }
        inline TCoordinate height(const Size &size, bool_trait<false>) const
        {
            return size.width;
        }
        inline void height(Size &size, TCoordinate h, bool_trait<true>) const
        {
            size.height = h;
        }
        inline void height(Size &size, TCoordinate h, bool_trait<false>) const
        {
            size.width = h;
        }
        // Size Ends
        
        
        // Rect
        inline Rect makeRect(TCoordinate x, TCoordinate y, TCoordinate width, TCoordinate height, bool_trait<true>) const
        {
            return Rect(x, y, width, height);
        }
        inline Rect makeRect(TCoordinate x, TCoordinate y, TCoordinate width, TCoordinate height, bool_trait<false>) const
        {
            return Rect(y, x, height, width);
        }
        
        inline TCoordinate left(const Rect &rect, bool_trait<true>) const
        {
            return rect.origin.x;
        }
        inline TCoordinate left(const Rect &rect, bool_trait<false>) const
        {
            return rect.origin.y;
        }
        inline void left(Rect &rect, TCoordinate l, bool_trait<true>) const
        {
            rect.origin.x = l;
        }
        inline void left(Rect &rect, TCoordinate l, bool_trait<false>) const
        {
            rect.origin.y = l;
        }
        
        inline TCoordinate top(const Rect &rect, bool_trait<true>) const
        {
            return rect.origin.y;
        }
        inline TCoordinate top(const Rect &rect, bool_trait<false>) const
        {
            return rect.origin.x;
        }
        inline void top(Rect &rect, TCoordinate t, bool_trait<true>) const
        {
            rect.origin.y = t;
        }
        inline void top(Rect &rect, TCoordinate t, bool_trait<false>) const
        {
            rect.origin.x = t;
        }
        
        inline TCoordinate right(const Rect &rect, bool_trait<true>) const
        {
            return rect.right();
        }
        inline TCoordinate right(const Rect &rect, bool_trait<false>) const
        {
            return rect.bottom();
        }
        inline void right(Rect &rect, TCoordinate r, bool_trait<true>) const
        {
            rect.size.width = r - rect.left();
        }
        inline void right(Rect &rect, TCoordinate r, bool_trait<false>) const
        {
            rect.size.height = r - rect.top();
        }

        inline TCoordinate bottom(const Rect &rect, bool_trait<true>) const
        {
            return rect.bottom();
        }
        inline TCoordinate bottom(const Rect &rect, bool_trait<false>) const
        {
            return rect.right();
        }
        inline void bottom(Rect &rect, TCoordinate b, bool_trait<true>) const
        {
            rect.size.height = b - rect.top();
        }
        inline void bottom(Rect &rect, TCoordinate b, bool_trait<false>) const
        {
            rect.size.width = b - rect.left();
        }
        
        inline Point leftBottom(const Rect &rect, bool_trait<true>) const
        {
            return Point(rect.left(), rect.bottom());
        }
        inline Point leftBottom(const Rect &rect, bool_trait<false>) const
        {
            return Point(rect.right(), rect.top());
        }
        
        inline TCoordinate width(const Rect &rect, bool_trait<true>) const
        {
            return rect.size.width;
        }
        inline TCoordinate width(const Rect &rect, bool_trait<false>) const
        {
            return rect.size.height;
        }
        inline void width(Rect &rect, TCoordinate w, bool_trait<true>) const
        {
            rect.size.width = w;
        }
        inline void width(Rect &rect, TCoordinate w, bool_trait<false>) const
        {
            rect.size.height = w;
        }
        
        inline TCoordinate height(const Rect &rect, bool_trait<true>) const
        {
            return rect.size.height;
        }
        inline TCoordinate height(const Rect &rect, bool_trait<false>) const
        {
            return rect.size.width;
        }
        inline void height(Rect &rect, TCoordinate h, bool_trait<true>) const
        {
            rect.size.height = h;
        }
        inline void height(Rect &rect, TCoordinate h, bool_trait<false>) const
        {
            rect.size.width = h;
        }
        inline Rect& offset(Rect &rect, TCoordinate x, TCoordinate y, bool_trait<true>) const
        {
            rect.origin.x += x;
            rect.origin.y += y;
            return rect;
        }
        inline Rect& offset(Rect &rect, TCoordinate x, TCoordinate y, bool_trait<false>) const
        {
            rect.origin.x += y;
            rect.origin.y += x;
            return rect;
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
        
        inline void incWidth(Rect &rect, TCoordinate dx, bool_trait<true>) const
        {
            rect.size.width += dx;
        }
        inline void incWidth(Rect &rect, TCoordinate dx, bool_trait<false>) const
        {
            rect.size.height += dx;
        }
        inline void incHeight(Rect &rect, TCoordinate dy, bool_trait<true>) const
        {
            rect.size.height += dy;
        }
        inline void incHeight(Rect &rect, TCoordinate dy, bool_trait<false>) const
        {
            rect.size.width += dy;
        }
        // Rect Ends
        
        
        // Insets
        
        inline TCoordinate left(const Insets &insets, bool_trait<true>) const
        {
            return insets.left;
        }
        inline TCoordinate left(const Insets &insets, bool_trait<false>) const
        {
            return insets.top;
        }

        inline TCoordinate top(const Insets &insets, bool_trait<true>) const
        {
            return insets.top;
        }
        inline TCoordinate top(const Insets &insets, bool_trait<false>) const
        {
            return insets.left;
        }
        
        inline TCoordinate right(const Insets &insets, bool_trait<true>) const
        {
            return insets.right;
        }
        inline TCoordinate right(const Insets &insets, bool_trait<false>) const
        {
            return insets.bottom;
        }
        
        inline TCoordinate bottom(const Insets &insets, bool_trait<true>) const
        {
            return insets.bottom;
        }
        inline TCoordinate bottom(const Insets &insets, bool_trait<false>) const
        {
            return insets.right;
        }
        
        inline TCoordinate hinsets(const Insets &insets, bool_trait<true>) const
        {
            return insets.hsize();
        }
        inline TCoordinate hinsets(const Insets &insets, bool_trait<false>) const
        {
            return insets.vsize();
        }
        
        inline TCoordinate vinsets(const Insets &insets, bool_trait<true>) const
        {
            return insets.vsize();
        }
        inline TCoordinate vinsets(const Insets &insets, bool_trait<false>) const
        {
            return insets.hszie();
        }
        // Insets Ends

        

    };



}

#endif //FLEXLAYOUT_ORIENTATIONHELPER_H
