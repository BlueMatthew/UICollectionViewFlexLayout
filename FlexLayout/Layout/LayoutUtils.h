//
//  LayoutUtils.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef LayoutUtils_h
#define LayoutUtils_h

namespace nsflex
{

class DirectionalTransformor
{
public:
    virtual ~DirectionalTransformor() {}
    
    virtual CGFloat x(const CGPoint &point) = 0;
    virtual CGFloat left(const CGRect &rect) = 0;
    virtual CGFloat width(const CGSize &sz) = 0;
    virtual CGFloat height(const CGSize &sz) = 0;
    virtual void width(CGSize &sz, CGFloat dv) = 0;
    virtual CGFloat width(const CGRect &rect) = 0;
    virtual CGFloat height(const CGRect &rect) = 0;
    virtual void width(CGRect &rect, CGFloat dv) = 0;
    virtual CGFloat right(const CGRect &rect) = 0;
    virtual void offset(CGPoint &point, CGFloat dv) = 0;
    virtual void offset(CGRect &rect, CGFloat dv) = 0;
    virtual void resize(CGSize &size, CGFloat dv) = 0;
    virtual void resize(CGRect &rect, CGFloat dv) = 0;
    virtual void inset(CGRect &rect, CGFloat dv) = 0;
    virtual void inset(CGRect &rect, CGFloat dv1, CGFloat dv2) = 0;
    virtual CGSize makeSize(CGFloat width, CGFloat height);
};

struct HorizontalTransformor : public DirectionalTransformor
{
public:
    ~HorizontalTransformor() {}
    
    CGFloat x(const CGPoint &point) { return point.x; }
    CGFloat left(const CGRect &rect) { return rect.origin.x; }
    CGFloat width(const CGSize &sz) { return sz.width; }
    CGFloat height(const CGSize &sz) { return sz.height; }
    void width(CGSize &sz, CGFloat dv) { sz.width = dv; }
    CGFloat width(const CGRect &rect) { return rect.size.width; }
    CGFloat height(const CGRect &rect) { return rect.size.height; }
    void width(CGRect &rect, CGFloat dv) { rect.size.width = dv; }
    CGFloat right(const CGRect &rect) { return rect.origin.x + rect.size.height; }
    void offset(CGPoint &point, CGFloat dv) { point.x += dv; }
    void offset(CGRect &rect, CGFloat dv) { rect.origin.x += dv; }
    void resize(CGSize &size, CGFloat dv) { size.width += dv; }
    void resize(CGRect &rect, CGFloat dv) { rect.size.width += dv; }
    void inset(CGRect &rect, CGFloat dv) { rect.origin.x += dv; rect.size.width -= (dv + dv); }
    void inset(CGRect &rect, CGFloat dv1, CGFloat dv2) { rect.origin.x += dv1; rect.size.width -= (dv1 + dv2); }
    
    CGSize makeSize(CGFloat width, CGFloat height) { return CGSizeMake(width, height); }
};

struct VerticalTransformor : public DirectionalTransformor
{
public:
    ~VerticalTransformor() {}
    
    CGFloat x(const CGPoint &point) { return point.y; }
    CGFloat left(const CGRect &rect) { return rect.origin.y; }
    CGFloat width(const CGSize &sz) { return sz.height; }
    CGFloat height(const CGSize &sz) { return sz.width; }
    void width(CGSize &sz, CGFloat dv) { sz.height = dv; }
    CGFloat width(const CGRect &rect) { return rect.size.height; }
    CGFloat height(const CGRect &rect) { return rect.size.width; }
    void width(CGRect &rect, CGFloat dv) { rect.size.height = dv; }
    CGFloat right(const CGRect &rect) { return rect.origin.y + rect.size.height; }
    void offset(CGPoint &point, CGFloat dv) { point.y += dv; }
    void offset(CGRect &rect, CGFloat dv) { rect.origin.y += dv; }
    void resize(CGSize &size, CGFloat dv) { size.height += dv; }
    void resize(CGRect &rect, CGFloat dv) { rect.size.height += dv; }
    void inset(CGRect &rect, CGFloat dv) { rect.origin.y += dv; rect.size.height -= (dv + dv); }
    void inset(CGRect &rect, CGFloat dv1, CGFloat dv2) { rect.origin.y += dv1; rect.size.height -= (dv1 + dv2); }
    CGSize makeSize(CGFloat width, CGFloat height) { return CGSizeMake(height, width); }
};

#define IS_CV_VERTICAL(layout) (layout.scrollDirection == UICollectionViewScrollDirectionVertical)
#define CV_SIZE(layout, size) (IS_CV_VERTICAL(layout) ? size.width : size.height)
#define CV_ORIGIN(layout, point) (IS_CV_VERTICAL(layout) ? point.y : point.x)

} // namespace nsflex
    
#endif /* LayoutUtils_h */
