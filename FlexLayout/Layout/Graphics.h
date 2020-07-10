//
// Created by Matthew on 2020-07-03.
//

#ifndef FLEXLAYOUT_GRAPHICS_H
#define FLEXLAYOUT_GRAPHICS_H

#include <algorithm>

namespace nsflex
{
    
// Here we use template for int and float
template<typename T>
struct PointT {
    T x;
    T y;

    PointT() : x(0), y(0) {}
    PointT(T x1, T y1) : x(x1), y(y1) {}
    
    inline bool operator==(const PointT<T> &other)
    {
        if (this == &other) return true;
        return x == other.x && y == other.y;
    }
};

template<typename T>
struct SizeT {

    T width;
    T height;

    SizeT() : width(0), height(0) {}
    SizeT(T w, T h) : width(w), height(h) {}
    inline bool empty() const { return width == 0 || height == 0; }
    
    inline bool operator==(const SizeT<T> &other)
    {
        if (this == &other) return true;
        return width == other.width && height == other.height;
    }
};

template<typename T>
struct RectT {
    typedef PointT<T> Point;
    typedef SizeT<T> Size;

    Point origin;
    Size size;

    RectT() {}
    RectT(T x, T y, T width, T height) : origin(x, y), size(width, height) {}
    RectT(const Point &p, const Size &s) : origin(p), size(s) {}
    RectT(const RectT<T> &rect) : origin(rect.origin), size(rect.size) {}

    inline T left() const { return origin.x; }
    inline T top() const { return origin.y; }
    inline T right() const { return origin.x + size.width; }
    inline T bottom() const { return origin.y + size.height; }
    
    inline T width() const { return size.width; }
    inline T height() const { return size.height; }
    
    inline bool operator==(const RectT<T> &other)
    {
        if (this == &other) return true;
        return origin.x == other.origin.x && origin.y == other.origin.y && size.width == other.size.width && size.height == other.size.height;
    }
    
    inline void offset(T dx, T dy) { origin.x += dx; origin.y += dy; }

    inline bool empty() const { return size.width <= 0 || size.height <= 0; }
    inline bool intersects(const RectT<T> &rect) const {
        return (origin.x < rect.right() && right() > rect.origin.x &&
            origin.y < rect.bottom() && bottom() > rect.origin.y);
    }
    
    inline void intersectRect(const RectT<T> &other)
    {
        T r = right();
        T b = bottom();
        
        origin.x = std::max(left(), other.left());
        origin.y = std::max(top(), other.top());
        
        size.width = std::min(r, other.right()) - left();
        size.height = std::min(b, other.bottom()) - top();
    }
    
    static RectT<T> intersectRects(const RectT<T> &first, const RectT<T> &second)
    {
        RectT<T> rect = first;
        rect.intersectRect(second);
        return rect;
    }

};

template<typename T>
struct InsetsT
{
    T left;
    T top;
    T right;
    T bottom;

    InsetsT() : left(0), top(0), right(0), bottom(0) {}
    InsetsT(T l, T t, T r, T b) : left(l), top(t), right(r), bottom(b) {}
};


#if defined(__APPLE__) && defined(__MACH__)

// Apple OSX and iOS (Darwin)
    
/*
// Refer the definition of CGRect, CGSize, CGPoint
#if defined(__LP64__) && __LP64__

typedef PointT<double> Point;
typedef SizeT<double> Size;
typedef RectT<double> Rect;
typedef InsetsT<double> Insets;

#else

typedef PointT<float> Point;
typedef SizeT<float> Size;
typedef RectT<float> Rect;
typedef InsetsT<float> Insets;


#endif // #if defined(__LP64__) && __LP64__

#else

typedef PointT<int> Point;
typedef SizeT<int> Size;
typedef RectT<int> Rect;
typedef InsetsT<int> Insets;
*/
    
#endif // #if defined(__APPLE__) && defined(__MACH__)

    
} // namespace nsflex

#endif //FLEXLAYOUT_GRAPHICS_H
