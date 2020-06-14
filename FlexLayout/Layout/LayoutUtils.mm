//
//  LayoutUtils.m
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/12.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "LayoutUtils.h"

#pragma mark DirectionalTransformor

DirectionalTransformor::~DirectionalTransformor() {}

#pragma mark HorizontalTransformor
HorizontalTransformor::~HorizontalTransformor() {}

CGFloat HorizontalTransformor::origin(const CGPoint &point) { return point.x; }
CGFloat HorizontalTransformor::rorigin(const CGPoint &point) { return point.y; }
void HorizontalTransformor::origin(CGPoint &point, CGFloat v) { point.x = v; }
void HorizontalTransformor::rorigin(CGPoint &point, CGFloat v) { point.y = v; }
CGFloat HorizontalTransformor::origin(const CGRect &rect) { return rect.origin.x; }
CGFloat HorizontalTransformor::rorigin(const CGRect &rect) { return rect.origin.y; }
void HorizontalTransformor::origin(CGRect &rect, CGFloat v) { rect.origin.x = v; }
void HorizontalTransformor::rorigin(CGRect &rect, CGFloat v) { rect.origin.y = v; }
CGFloat HorizontalTransformor::size(const CGSize &sz) { return sz.width; }
CGFloat HorizontalTransformor::rsize(const CGSize &sz) { return sz.height; }
void HorizontalTransformor::size(CGSize &sz, CGFloat dv) { sz.width = dv; }
CGFloat HorizontalTransformor::size(const CGRect &rect) { return rect.size.width; }
CGFloat HorizontalTransformor::rsize(const CGRect &rect) { return rect.size.height; }
void HorizontalTransformor::size(CGRect &rect, CGFloat dv) { rect.size.width = dv; }
CGFloat HorizontalTransformor::upperOrigin(const CGRect &rect) { return rect.origin.x + rect.size.width; }
CGFloat HorizontalTransformor::rupperOrigin(const CGRect &rect) { return rect.origin.y + rect.size.height; }
void HorizontalTransformor::offset(CGPoint &point, CGFloat dv) { point.x += dv; }
void HorizontalTransformor::offset(CGRect &rect, CGFloat dv) { rect.origin.x += dv; }
void HorizontalTransformor::resize(CGSize &size, CGFloat dv) { size.width += dv; }
void HorizontalTransformor::resize(CGRect &rect, CGFloat dv) { rect.size.width += dv; }
void HorizontalTransformor::inset(CGRect &rect, CGFloat dv) { rect.origin.x += dv; rect.size.width -= (dv + dv); }
void HorizontalTransformor::inset(CGRect &rect, CGFloat dv1, CGFloat dv2) { rect.origin.x += dv1; rect.size.width -= (dv1 + dv2); }
CGFloat HorizontalTransformor::inset(const UIEdgeInsets &insets) { return insets.left + insets.right; }
CGFloat HorizontalTransformor::rinset(const UIEdgeInsets &insets) { return insets.top + insets.left; }
// CGSize HorizontalTransformor::makeSize(CGFloat width, CGFloat height) { return CGSizeMake(width, height); }

#pragma mark VerticalTransformor

VerticalTransformor::~VerticalTransformor() {}

CGFloat VerticalTransformor::origin(const CGPoint &point) { return point.y; }
CGFloat VerticalTransformor::rorigin(const CGPoint &point) { return point.x; }
void VerticalTransformor::origin(CGPoint &point, CGFloat v) { point.y = v; }
void VerticalTransformor::rorigin(CGPoint &point, CGFloat v) { point.x = v; }
CGFloat VerticalTransformor::origin(const CGRect &rect) { return rect.origin.y; }
CGFloat VerticalTransformor::rorigin(const CGRect &rect) { return rect.origin.x; }
void VerticalTransformor::origin(CGRect &rect, CGFloat v) { rect.origin.y = v; }
void VerticalTransformor::rorigin(CGRect &rect, CGFloat v) { rect.origin.x = v; }
CGFloat VerticalTransformor::size(const CGSize &sz) { return sz.height; }
CGFloat VerticalTransformor::rsize(const CGSize &sz) { return sz.width; }
void VerticalTransformor::size(CGSize &sz, CGFloat dv) { sz.height = dv; }
CGFloat VerticalTransformor::size(const CGRect &rect) { return rect.size.height; }
CGFloat VerticalTransformor::rsize(const CGRect &rect) { return rect.size.width; }
void VerticalTransformor::size(CGRect &rect, CGFloat dv) { rect.size.height = dv; }
CGFloat VerticalTransformor::upperOrigin(const CGRect &rect) { return rect.origin.y + rect.size.height; }
CGFloat VerticalTransformor::rupperOrigin(const CGRect &rect) { return rect.origin.x + rect.size.width; }
void VerticalTransformor::offset(CGPoint &point, CGFloat dv) { point.y += dv; }
void VerticalTransformor::offset(CGRect &rect, CGFloat dv) { rect.origin.y += dv; }
void VerticalTransformor::resize(CGSize &size, CGFloat dv) { size.height += dv; }
void VerticalTransformor::resize(CGRect &rect, CGFloat dv) { rect.size.height += dv; }
void VerticalTransformor::inset(CGRect &rect, CGFloat dv) { rect.origin.y += dv; rect.size.height -= (dv + dv); }
void VerticalTransformor::inset(CGRect &rect, CGFloat dv1, CGFloat dv2) { rect.origin.y += dv1; rect.size.height -= (dv1 + dv2); }
CGFloat VerticalTransformor::inset(const UIEdgeInsets &insets) { return insets.top + insets.bottom; }
CGFloat VerticalTransformor::rinset(const UIEdgeInsets &insets) { return insets.left + insets.right; }
// CGSize VerticalTransformor::makeSize(CGFloat width, CGFloat height) { return CGSizeMake(height, width); }


