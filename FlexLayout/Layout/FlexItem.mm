//
//  FlexItem.m
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "FlexItem.h"
#import <UIKit/UIKit.h>

UICollectionViewLayoutAttributes* UIFlexItem::buildLayoutAttributesForCell(Class layoutAttributesClass, NSIndexPath *indexPath, CGPoint sectionOrigin)
{
    if (nil == m_la || m_la.indexPath.section != indexPath.item || m_la.indexPath.section != indexPath.item)
    {
        m_la = [layoutAttributesClass layoutAttributesForCellWithIndexPath:indexPath];
    }
    m_la.frame = CGRectMake(m_frame.origin.x + sectionOrigin.x, m_frame.origin.y + sectionOrigin.y, m_frame.size.width, m_frame.size.height);
    
    return m_la;
}

UICollectionViewLayoutAttributes* UIFlexItem::buildLayoutAttributesForSupplementaryView(Class layoutAttributesClass, NSString *representedElementKind, NSIndexPath *indexPath, CGPoint sectionOrigin)
{
    if (nil == m_la || m_la.indexPath.section != indexPath.item || m_la.indexPath.section != indexPath.item)
    {
        m_la = [layoutAttributesClass layoutAttributesForSupplementaryViewOfKind:representedElementKind withIndexPath:indexPath];
    }
    m_la.frame = CGRectMake(m_frame.origin.x + sectionOrigin.x, m_frame.origin.y + sectionOrigin.y, m_frame.size.width, m_frame.size.height);
    
    return m_la;
}

UICollectionViewLayoutAttributes* UIFlexItem::buildLayoutAttributesForDecorationView(Class layoutAttributesClass, NSString *representedElementKind, NSIndexPath *indexPath, CGPoint sectionOrigin)
{
    if (nil == m_la || m_la.indexPath.section != indexPath.item || m_la.indexPath.section != indexPath.item)
    {
        m_la = [layoutAttributesClass layoutAttributesForDecorationViewOfKind:representedElementKind withIndexPath:indexPath];
    }
    m_la.frame = CGRectMake(m_frame.origin.x + sectionOrigin.x, m_frame.origin.y + sectionOrigin.y, m_frame.size.width, m_frame.size.height);
    
    return m_la;
}
