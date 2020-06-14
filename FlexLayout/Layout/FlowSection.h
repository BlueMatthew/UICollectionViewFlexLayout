//
//  FlowSection.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlowSection_h
#define FlowSection_h

#include "Section.h"
#include "FlexRow.h"
#include <utility>
#include <memory>


template<class TLayout>
class UIFlowSectionT : public UISectionT<TLayout>
{
public:
    std::vector<UIFlexRow *> m_rows;
    
    UIFlowSectionT(UICollectionViewFlexLayout *layout, NSInteger section, CGPoint origin) : UISectionT<TLayout>(layout, section, origin)
    {
    }
    
    virtual ~UIFlowSectionT()
    {
        for(std::vector<UIFlexRow *>::iterator it = m_rows.begin(); it != m_rows.end(); delete *it, ++it);
        m_rows.clear();
    }
    
    virtual void prepareLayout()
    {
        bool isVertical = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout);
        std::auto_ptr<DirectionalTransformor> tr(isVertical ? (DirectionalTransformor *)(new VerticalTransformor()) : (DirectionalTransformor *)(new HorizontalTransformor()));
        
        CGSize boundsSize = UISectionT<TLayout>::m_layout.collectionView.bounds.size;
        // Set size::width first
        UISectionT<TLayout>::m_frame.size = boundsSize;
        // Set size::height to 0
        tr->size(UISectionT<TLayout>::m_frame, 0.0f);
        
        // Header
        UISectionT<TLayout>::m_header.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:(UISectionT<TLayout>::m_section)];
        
        // isVertical ? (UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_header.getFrame().size.height) : (UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_header.getFrame().size.width);
        // Add Header Height
        tr->resize(UISectionT<TLayout>::m_frame, tr->size(UISectionT<TLayout>::m_header.getFrame()));
        
        NSInteger numberOfItems = [UISectionT<TLayout>::m_layout.collectionView numberOfItemsInSection:(UISectionT<TLayout>::m_section)];
        if (numberOfItems > 0)
        {
            UIEdgeInsets sectionInset = [UISectionT<TLayout>::m_layout getInsetForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            UISectionT<TLayout>::m_items.reserve(numberOfItems);
            
            // 获取列数
            NSInteger numberOfColumns = [UISectionT<TLayout>::m_layout getNumberOfColumnsForSection:(UISectionT<TLayout>::m_section)];
            
            m_rows.reserve(numberOfItems / numberOfColumns);
            
            CGFloat minimumLineSpacing = [UISectionT<TLayout>::m_layout getMinimumLineSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            CGFloat minimumInteritemSpacing = [UISectionT<TLayout>::m_layout getMinimumInteritemSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            // CGFloat maximalSizeOfRow = isVertical ? (boundsSize.width - sectionInset.left - sectionInset.right) : (boundsSize.height - sectionInset.top - sectionInset.bottom);
            
            CGFloat maximalSizeOfRow = tr->rsize(boundsSize) - tr->rinset(sectionInset);
            
            // 对每一个Item进行布局，分配到相应的列中
            UIFlexItem *sectionItem = NULL;
            UIFlexRow *row = NULL;
            
            // CGPoint originForRow = isVertical ? CGPointMake(sectionInset.left, sectionInset.top + UISectionT<TLayout>::m_header.getFrame().size.height) : CGPointMake(sectionInset.left + UISectionT<TLayout>::m_header.getFrame().size.width, sectionInset.top);
            
            CGPoint originForRow = CGPointMake(sectionInset.left, sectionInset.top);
            tr->origin(originForRow, tr->size(UISectionT<TLayout>::m_header.getFrame()));
            
            for (NSInteger item = 0; item < numberOfItems; item++)
            {
                CGSize sizeOfItem = [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]];
                CGPoint originOfItem = CGPointZero;
                
                if (NULL != row)
                {
                    if (row->m_items.size() > 0)
                    {
                        // CGFloat availableSize = maximalSizeOfRow - (isVertical ? row->m_frame.size.width : row->m_frame.size.height) - minimumInteritemSpacing;
                        CGFloat availableSize = maximalSizeOfRow - tr->rsize(row->getFrame()) - minimumInteritemSpacing;
                        // if (availableSize < (isVertical ? sizeOfItem.width : sizeOfItem.height))
                        if (availableSize < tr->rsize(sizeOfItem))
                        {
                            // New Line
                            m_rows.push_back(row);
                            row = NULL;
                        }
                        else
                        {
                            // originOfItem.x = (isVertical ? (row->m_frame.origin.x + row->m_frame.size.width) : (row->m_frame.origin.y + row->m_frame.size.height)) + minimumInteritemSpacing;
                            // originOfItem.y = row->m_frame.origin.y;
                            tr->rorigin(originOfItem, tr->rupperOrigin(row->getFrame()) + minimumInteritemSpacing);
                            tr->origin(originOfItem, tr->origin(row->getFrame()));
                        }
                    }
                }
                
                if (NULL == row)
                {
                    if (m_rows.size() > 0)
                    {
                        // isVertical ? originForRow.y += minimumLineSpacing + m_rows[m_rows.size() - 1]->m_frame.size.height : originForRow.x += minimumLineSpacing + m_rows[m_rows.size() - 1]->m_frame.size.width;
                        
                        tr->offset(originForRow, minimumLineSpacing + tr->size(m_rows[m_rows.size() - 1]->getFrame()));
                    }
                    
                    row = new UIFlexRow();
                    row->getFrame().origin = originForRow;
                    
                    originOfItem = originForRow;
                }
                
                sectionItem = new UIFlexItem(item, originOfItem, sizeOfItem);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                row->addItem(sectionItem, isVertical);
            }
            
            if (NULL != row)
            {
                // Last row
                m_rows.push_back(row);
                
                
                // isVertical ? (UISectionT<TLayout>::m_frame.size.height += row->m_frame.origin.y + row->m_frame.size.height - m_rows[0]->m_frame.origin.y + sectionInset.top + sectionInset.bottom) : (UISectionT<TLayout>::m_frame.size.width += row->m_frame.origin.x + row->m_frame.size.width - m_rows[0]->m_frame.origin.x + sectionInset.left + sectionInset.right);

                tr->resize(UISectionT<TLayout>::m_frame.size, tr->upperOrigin(row->getFrame()) - tr->origin(m_rows[0]->getFrame()) + tr->inset(sectionInset));
            }
        }
        
        // Footer
        // UISectionT<TLayout>::m_footer.m_frame.origin = isVertical ? CGPointMake(0, UISectionT<TLayout>::m_frame.size.height) : CGPointMake(UISectionT<TLayout>::m_frame.size.width, 0);
        UISectionT<TLayout>::m_footer.getFrame().origin = CGPointZero;
        tr->origin(UISectionT<TLayout>::m_footer.getFrame().origin, tr->size(UISectionT<TLayout>::getFrame()));
        
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
        // isVertical ? (UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_footer.m_frame.size.height) : (UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_footer.getFrame().size.width);
        
        tr->resize(UISectionT<TLayout>::m_frame, tr->size(UISectionT<TLayout>::m_footer.getFrame()));
    }
    
    virtual bool getLayoutAttributesForItemsInRect(NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributes, const CGRect &rectInSection)
    {
        bool merged = true;
        
        std::pair<std::vector<UIFlexRow *>::iterator, std::vector<UIFlexRow *>::iterator> range = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? getVirticalRowsInRect(rectInSection) : getHorizontalRowsInRect(rectInSection);
        
        for (std::vector<UIFlexRow *>::iterator it = range.first; it != range.second; ++it)
        {
            for (std::vector<UIFlexItem *>::iterator itItem = (*it)->m_items.begin(); itItem != (*it)->m_items.end(); ++itItem)
            {
                if (CGRectIntersectsRect((*itItem)->getFrame(), rectInSection))
                {
                    // layoutAttributes
                    NSIndexPath *indexPath = [NSIndexPath indexPathForItem:(*itItem)->m_item inSection:(UISectionT<TLayout>::m_section)];
                    UICollectionViewLayoutAttributes *la = [UISectionT<TLayout>::m_layout layoutAttributesForItemAtIndexPath:indexPath];
                    
                    la.frame = CGRectMake((*itItem)->m_frame.origin.x + UISectionT<TLayout>::m_frame.origin.x, (*itItem)->m_frame.origin.y + UISectionT<TLayout>::m_frame.origin.y, (*itItem)->m_frame.size.width, (*itItem)->m_frame.size.height);
                    
                    [layoutAttributes addObject:la];
                    merged = false;
                }
            }
        }
        
        return merged;
    }
    
    inline std::pair<std::vector<UIFlexRow *>::iterator, std::vector<UIFlexRow *>::iterator> getVirticalRowsInRect(const CGRect& rect)
    {
        return std::equal_range(m_rows.begin(), m_rows.end(), std::pair<CGFloat, CGFloat>(rect.origin.y, rect.origin.y + rect.size.height), UISectionRowVerticalCompare());
    }
    
    inline std::pair<std::vector<UIFlexRow *>::iterator, std::vector<UIFlexRow *>::iterator> getHorizontalRowsInRect(const CGRect& rect)
    {
        return std::equal_range(m_rows.begin(), m_rows.end(), std::pair<CGFloat, CGFloat>(rect.origin.x, rect.origin.x + rect.size.width), UISectionRowHorizontalCompare());
    }
    
    
};



#endif /* FlowSection_h */
