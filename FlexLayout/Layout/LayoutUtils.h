//
//  LayoutUtils.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef LayoutUtils_h
#define LayoutUtils_h

class DirectionalTransformor
{
public:
    virtual ~DirectionalTransformor() {}
    
    virtual CGFloat origin(const CGPoint &point) = 0;
    virtual CGFloat origin(const CGRect &rect) = 0;
    virtual CGFloat size(const CGSize &sz) = 0;
    virtual CGFloat rsize(const CGSize &sz) = 0;
    virtual void size(CGSize &sz, CGFloat dv) = 0;
    virtual CGFloat size(const CGRect &rect) = 0;
    virtual CGFloat rsize(const CGRect &rect) = 0;
    virtual void size(CGRect &rect, CGFloat dv) = 0;
    virtual CGFloat upperOrigin(const CGRect &rect) = 0;
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
    
    CGFloat origin(const CGPoint &point) { return point.x; }
    CGFloat origin(const CGRect &rect) { return rect.origin.x; }
    CGFloat size(const CGSize &sz) { return sz.width; }
    CGFloat rsize(const CGSize &sz) { return sz.height; }
    void size(CGSize &sz, CGFloat dv) { sz.width = dv; }
    CGFloat size(const CGRect &rect) { return rect.size.width; }
    CGFloat rsize(const CGRect &rect) { return rect.size.height; }
    void size(CGRect &rect, CGFloat dv) { rect.size.width = dv; }
    CGFloat upperOrigin(const CGRect &rect) { return rect.origin.x + rect.size.height; }
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
    
    CGFloat origin(const CGPoint &point) { return point.y; }
    CGFloat origin(const CGRect &rect) { return rect.origin.y; }
    CGFloat size(const CGSize &sz) { return sz.height; }
    CGFloat rsize(const CGSize &sz) { return sz.width; }
    void size(CGSize &sz, CGFloat dv) { sz.height = dv; }
    CGFloat size(const CGRect &rect) { return rect.size.height; }
    CGFloat rsize(const CGRect &rect) { return rect.size.width; }
    void size(CGRect &rect, CGFloat dv) { rect.size.height = dv; }
    CGFloat upperOrigin(const CGRect &rect) { return rect.origin.y + rect.size.height; }
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

#endif /* LayoutUtils_h */
