//
//  LayoutUtils.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef LayoutUtils_h
#define LayoutUtils_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

class DirectionalTransformor
{
public:
    DirectionalTransformor() {}
    virtual ~DirectionalTransformor();
    
    virtual CGFloat origin(const CGPoint &point) = 0;
    virtual CGFloat rorigin(const CGPoint &point) = 0;
    virtual void origin(CGPoint &point, CGFloat v) = 0;
    virtual void rorigin(CGPoint &point, CGFloat v) = 0;
    virtual CGFloat origin(const CGRect &rect) = 0;
    virtual CGFloat rorigin(const CGRect &rect) = 0;
    virtual void origin(CGRect &rect, CGFloat v) = 0;
    virtual void rorigin(CGRect &rect, CGFloat v) = 0;
    virtual CGFloat size(const CGSize &sz) = 0;
    virtual CGFloat rsize(const CGSize &sz) = 0;
    virtual void size(CGSize &sz, CGFloat dv) = 0;
    virtual CGFloat size(const CGRect &rect) = 0;
    virtual CGFloat rsize(const CGRect &rect) = 0;
    virtual void size(CGRect &rect, CGFloat dv) = 0;
    virtual CGFloat upperOrigin(const CGRect &rect) = 0;
    virtual CGFloat rupperOrigin(const CGRect &rect) = 0;
    virtual void offset(CGPoint &point, CGFloat dv) = 0;
    virtual void offset(CGRect &rect, CGFloat dv) = 0;
    virtual void resize(CGSize &size, CGFloat dv) = 0;
    virtual void resize(CGRect &rect, CGFloat dv) = 0;
    virtual void inset(CGRect &rect, CGFloat dv) = 0;
    virtual void inset(CGRect &rect, CGFloat dv1, CGFloat dv2) = 0;
    virtual CGFloat inset(const UIEdgeInsets &insets) = 0;
    virtual CGFloat rinset(const UIEdgeInsets &insets) = 0;
    
};

struct HorizontalTransformor : public DirectionalTransformor
{
public:
    HorizontalTransformor() : DirectionalTransformor() {}
    ~HorizontalTransformor();
    
    CGFloat origin(const CGPoint &point);
    CGFloat rorigin(const CGPoint &point);
    void origin(CGPoint &point, CGFloat v);
    void rorigin(CGPoint &point, CGFloat v);
    CGFloat origin(const CGRect &rect);
    CGFloat rorigin(const CGRect &rect);
    void origin(CGRect &rect, CGFloat v);
    void rorigin(CGRect &rect, CGFloat v);
    CGFloat size(const CGSize &sz);
    CGFloat rsize(const CGSize &sz);
    void size(CGSize &sz, CGFloat dv);
    CGFloat size(const CGRect &rect);
    CGFloat rsize(const CGRect &rect);
    void size(CGRect &rect, CGFloat dv);
    CGFloat upperOrigin(const CGRect &rect);
    CGFloat rupperOrigin(const CGRect &rect);
    void offset(CGPoint &point, CGFloat dv);
    void offset(CGRect &rect, CGFloat dv);
    void resize(CGSize &size, CGFloat dv);
    void resize(CGRect &rect, CGFloat dv);
    void inset(CGRect &rect, CGFloat dv);
    void inset(CGRect &rect, CGFloat dv1, CGFloat dv2);
    CGFloat inset(const UIEdgeInsets &insets);
    CGFloat rinset(const UIEdgeInsets &insets);
    
};

struct VerticalTransformor : public DirectionalTransformor
{
public:
    VerticalTransformor() : DirectionalTransformor() {}
    ~VerticalTransformor();
    
    CGFloat origin(const CGPoint &point);
    CGFloat rorigin(const CGPoint &point);
    void origin(CGPoint &point, CGFloat v);
    void rorigin(CGPoint &point, CGFloat v);
    CGFloat origin(const CGRect &rect);
    CGFloat rorigin(const CGRect &rect);
    void origin(CGRect &rect, CGFloat v);
    void rorigin(CGRect &rect, CGFloat v);
    CGFloat size(const CGSize &sz);
    CGFloat rsize(const CGSize &sz);
    void size(CGSize &sz, CGFloat dv);
    CGFloat size(const CGRect &rect);
    CGFloat rsize(const CGRect &rect);
    void size(CGRect &rect, CGFloat dv);
    CGFloat upperOrigin(const CGRect &rect);
    CGFloat rupperOrigin(const CGRect &rect);
    void offset(CGPoint &point, CGFloat dv);
    void offset(CGRect &rect, CGFloat dv);
    void resize(CGSize &size, CGFloat dv);
    void resize(CGRect &rect, CGFloat dv);
    void inset(CGRect &rect, CGFloat dv);
    void inset(CGRect &rect, CGFloat dv1, CGFloat dv2);
    CGFloat inset(const UIEdgeInsets &insets);
    CGFloat rinset(const UIEdgeInsets &insets);
    
};

#define IS_CV_VERTICAL(layout) (layout.scrollDirection == UICollectionViewScrollDirectionVertical)
#define CV_SIZE(layout, size) (IS_CV_VERTICAL(layout) ? size.width : size.height)
#define CV_ORIGIN(layout, point) (IS_CV_VERTICAL(layout) ? point.y : point.x)

#endif /* LayoutUtils_h */
