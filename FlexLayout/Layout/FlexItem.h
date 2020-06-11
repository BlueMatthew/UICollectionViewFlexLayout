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
public:
    NSInteger m_item;
    CGRect m_frame; // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
    UICollectionViewLayoutAttributes *m_la;
    
    UIFlexItem() : m_item(0), m_frame(CGRectZero), m_la(NULL)
    {
    }
    
    UIFlexItem(NSInteger item) : m_item(item), m_frame(CGRectZero), m_la(NULL)
    {
    }
    
    UIFlexItem(NSInteger item, CGPoint origin, CGSize size) : m_item(item), m_frame ({.origin = origin, .size = size})
    {
    }
    
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

struct UISectionItemCompare
{
    bool operator() ( const UIFlexItem* item, NSInteger itemToCompare) const
    {
        return item->m_item < itemToCompare;
    }
    bool operator() ( NSInteger itemToCompare, const UIFlexItem* item ) const
    {
        return itemToCompare < item->m_item;
    }
};

struct UISectionItemVerticalCompare
{
    bool operator() ( const UIFlexItem* item, const std::pair<CGFloat, CGFloat>& topBottom) const
    {
        return item->m_frame.origin.y + item->m_frame.size.height < topBottom.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& topBottom, const UIFlexItem* item ) const
    {
        return topBottom.second < item->m_frame.origin.y;
    }
};

struct UISectionItemHorizontalCompare
{
    bool operator() ( const UIFlexItem* item, const std::pair<CGFloat, CGFloat>& leftRight) const
    {
        return item->m_frame.origin.x + item->m_frame.size.width < leftRight.first;
    }
    bool operator() ( const std::pair<CGFloat, CGFloat>& leftRight, const UIFlexItem* item ) const
    {
        return leftRight.second < item->m_frame.origin.x;
    }
};

#endif /* SectionItem_h */
