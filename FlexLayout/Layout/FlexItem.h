//
//  FlexItem.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlexItem_h
#define FlexItem_h

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#include <map>

@class UICollectionViewLayoutAttributes;
@class NSIndexPath;

class UIFlexItem
{
private:
    NSInteger m_item;
    CGRect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    UICollectionViewLayoutAttributes *m_la;
   
public:
    UIFlexItem() : m_item(0), m_frame(CGRectZero), m_la(NULL)
    {
    }
    
    UIFlexItem(NSInteger item) : m_item(item), m_frame(CGRectZero), m_la(NULL)
    {
    }
    
    UIFlexItem(NSInteger item, CGPoint origin, CGSize size) : m_item(item), m_frame ({.origin = origin, .size = size})
    {
    }
    
    inline NSInteger getItem() const { return m_item; }
    inline CGRect &getFrame() { return m_frame; }
    inline const CGRect getFrame() const { return m_frame; }
    
    void clearLayoutAttributes()
    {
        m_la = nil;
    }
    
    UICollectionViewLayoutAttributes* buildLayoutAttributesForCell(Class layoutAttributesClass, NSIndexPath *indexPath, CGPoint sectionOrigin);
    UICollectionViewLayoutAttributes* buildLayoutAttributesForSupplementaryView(Class layoutAttributesClass, NSString *representedElementKind, NSIndexPath *indexPath, CGPoint sectionOrigin);
    UICollectionViewLayoutAttributes* buildLayoutAttributesForDecorationView(Class layoutAttributesClass, NSString *representedElementKind, NSIndexPath *indexPath, CGPoint sectionOrigin);
};

template<typename T>
struct UIVerticalCompare
{
    bool operator() ( const T* item, const std::pair<CGFloat, CGFloat>& topBottom) const
    {
        return item->getFrame().origin.y + item->getFrame().size.height < topBottom.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& topBottom, const T* item ) const
    {
        return topBottom.second < item->getFrame().origin.y;
    }
};

template<typename T>
struct UIHorizontalCompare
{
    bool operator() ( const T* item, const std::pair<CGFloat, CGFloat>& leftRight) const
    {
        return item->getFrame().origin.x + item->getFrame().size.width < leftRight.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& leftRight, const T* item ) const
    {
        return leftRight.second < item->getFrame().origin.x;
    }
};

typedef UIVerticalCompare<UIFlexItem> UIFlexItemVerticalCompare;
typedef UIHorizontalCompare<UIFlexItem> UIFlexItemHorizontalCompare;

#endif /* FlexItem_h */
