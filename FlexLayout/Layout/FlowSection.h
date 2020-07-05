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
    
    UIFlowSectionT(TLayout *layout, NSInteger section, const CGRect frame) : UISectionT<TLayout>(layout, section, frame)
    {
    }
    
    virtual ~UIFlowSectionT()
    {
        for(std::vector<UIFlexRow *>::iterator it = m_rows.begin(); it != m_rows.end(); delete *it, ++it);
        m_rows.clear();
    }

    void prepareLayoutVertically()
    {
        // Header
        UISectionT<TLayout>::m_header.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:(UISectionT<TLayout>::m_section)];
        
        // Initialize the section height with header height
        UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.getFrame().size.height;
        // UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.getFrame().size.width;
        
        // Items
        CGPoint originOfRow = UISectionT<TLayout>::m_header.m_frame.origin;
        
        originOfRow.y += UISectionT<TLayout>::m_header.getFrame().size.height;
        // originOfRow.x += UISectionT<TLayout>::m_header.getFrame().size.width;
        
        m_rows.clear();
        UISectionT<TLayout>::clearItems();
        
        NSInteger numberOfItems = [UISectionT<TLayout>::m_layout.collectionView numberOfItemsInSection:(UISectionT<TLayout>::m_section)];
        if (numberOfItems > 0)
        {
            UIEdgeInsets sectionInset = [UISectionT<TLayout>::m_layout getInsetForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            UISectionT<TLayout>::m_items.reserve(numberOfItems);
            
            // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
            NSInteger numberOfColumns = [UISectionT<TLayout>::m_layout getNumberOfColumnsForSection:(UISectionT<TLayout>::m_section)];
            m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
            
            CGFloat minimumLineSpacing = [UISectionT<TLayout>::m_layout getMinimumLineSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            CGFloat minimumInteritemSpacing = [UISectionT<TLayout>::m_layout getMinimumInteritemSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
            // CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;
            
            // Layout items
            UIFlexItem *sectionItem = NULL;
            UIFlexRow *row = NULL;
            
            CGPoint originOfItem = originOfRow;
            CGFloat availableSizeOfRow = 0.0f;
            CGSize sizeOfItem = CGSizeZero;
            CGFloat sizeOfItemInDirection = 0.0f;
            
            originOfRow.x += sectionInset.left;
            originOfRow.y += sectionInset.top;
            
            for (NSInteger itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
            {
                sizeOfItem = [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:itemIndex inSection:(UISectionT<TLayout>::m_section)]];
                
                sizeOfItemInDirection = sizeOfItem.width;
                // sizeOfItemInDirection = sizeOfItem.height;
                
                if (NULL != row)
                {
                    if (row->hasItems())
                    {
                        availableSizeOfRow = maximalSizeOfRow - row->m_frame.size.width - minimumInteritemSpacing;
                        // availableRowSize = maximalSizeOfRow - row->m_frame.size.height - minimumInteritemSpacing;
                        
                        if (availableSizeOfRow < sizeOfItemInDirection)
                        {
                            // New Line
                            m_rows.push_back(row);
                            originOfRow.y += minimumLineSpacing + row->m_frame.size.height;
                            // originOfRow.x += minimumLineSpacing + row->m_frame.size.width;
                            row = NULL;
                        }
                    }
                }
                
                if (NULL == row)
                {
                    row = new UIFlexRow();
                    row->m_frame.origin = originOfRow;
                    
                    originOfItem = originOfRow;
                }
                
                if (row->hasItems())
                {
                    originOfItem.x = CGRectGetMaxX(row->m_frame) + minimumInteritemSpacing;
                    // originOfItem.y = CGRectGetMaxY(row->m_frame) + minimumInteritemSpacing;
                }
                
                sectionItem = new UIFlexItem(itemIndex, originOfItem, sizeOfItem);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                row->addItemVertically(sectionItem);
                // row->addItemHorizontally(sectionItem);
            }
            
            // The last row
            if (NULL != row)
            {
                m_rows.push_back(row);
                
                originOfRow.y += row->getFrame().size.height;
                // originOfRow.x += row->getFrame().size.width;
            }
            
            originOfRow.y += sectionInset.bottom;
            // originOfRow.x += sectionInset.right;
        }
        
        // Footer
        UISectionT<TLayout>::m_footer.m_frame.origin.y = originOfRow.y;
        // UISectionT<TLayout>::m_footer.m_frame.origin.x = originOfRow.x;
        
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
        UISectionT<TLayout>::m_frame.size.height = CGRectGetMaxY(UISectionT<TLayout>::m_footer.m_frame);
        // UISectionT<TLayout>::m_frame.size.width = CGRectGetMaxX(UISectionT<TLayout>::m_footer.m_frame);
    }
    
    void prepareLayoutHorizontally()
    {
        // Header
        UISectionT<TLayout>::m_header.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:(UISectionT<TLayout>::m_section)];
        
        // Initialize the section height with header height
        // UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.getFrame().size.height;
        UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.getFrame().size.width;
        
        // Items
        CGPoint originOfRow = UISectionT<TLayout>::m_header.m_frame.origin;
        
        // originOfRow.y += UISectionT<TLayout>::m_header.getFrame().size.height;
        originOfRow.x += UISectionT<TLayout>::m_header.getFrame().size.width;
        
        m_rows.clear();
        UISectionT<TLayout>::clearItems();
        
        NSInteger numberOfItems = [UISectionT<TLayout>::m_layout.collectionView numberOfItemsInSection:(UISectionT<TLayout>::m_section)];
        if (numberOfItems > 0)
        {
            UIEdgeInsets sectionInset = [UISectionT<TLayout>::m_layout getInsetForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            UISectionT<TLayout>::m_items.reserve(numberOfItems);
            
            // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
            NSInteger numberOfColumns = [UISectionT<TLayout>::m_layout getNumberOfColumnsForSection:(UISectionT<TLayout>::m_section)];
            m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
            
            CGFloat minimumLineSpacing = [UISectionT<TLayout>::m_layout getMinimumLineSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            CGFloat minimumInteritemSpacing = [UISectionT<TLayout>::m_layout getMinimumInteritemSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            // CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
            CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;
            
            // Layout items
            UIFlexItem *sectionItem = NULL;
            UIFlexRow *row = NULL;
            
            CGPoint originOfItem = originOfRow;
            CGFloat availableSizeOfRow = 0.0f;
            CGSize sizeOfItem = CGSizeZero;
            CGFloat sizeOfItemInDirection = 0.0f;
            
            originOfRow.x += sectionInset.left;
            originOfRow.y += sectionInset.top;
            
            for (NSInteger itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
            {
                sizeOfItem = [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:itemIndex inSection:(UISectionT<TLayout>::m_section)]];
                
                // sizeOfItemInDirection = sizeOfItem.width;
                sizeOfItemInDirection = sizeOfItem.height;
                
                if (NULL != row)
                {
                    if (row->hasItems())
                    {
                        // availableSizeOfRow = maximalSizeOfRow - row->m_frame.size.width - minimumInteritemSpacing;
                        availableSizeOfRow = maximalSizeOfRow - row->m_frame.size.height - minimumInteritemSpacing;
                        
                        if (availableSizeOfRow < sizeOfItemInDirection)
                        {
                            // New Line
                            m_rows.push_back(row);
                            // originOfRow.y += minimumLineSpacing + row->m_frame.size.height;
                            originOfRow.x += minimumLineSpacing + row->m_frame.size.width;
                            row = NULL;
                        }
                    }
                }
                
                if (NULL == row)
                {
                    row = new UIFlexRow();
                    row->m_frame.origin = originOfRow;
                    
                    originOfItem = originOfRow;
                }
                
                if (row->hasItems())
                {
                    // originOfItem.x = CGRectGetMaxX(row->m_frame) + minimumInteritemSpacing;
                    originOfItem.y = CGRectGetMaxY(row->m_frame) + minimumInteritemSpacing;
                }
                
                sectionItem = new UIFlexItem(itemIndex, originOfItem, sizeOfItem);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                // row->addItemVertically(sectionItem);
                row->addItemHorizontally(sectionItem);
            }
            
            // The last row
            if (NULL != row)
            {
                m_rows.push_back(row);
                
                // originOfRow.y += row->getFrame().size.height;
                originOfRow.x += row->getFrame().size.width;
            }
            
            // originOfRow.y += sectionInset.bottom;
            originOfRow.x += sectionInset.right;
        }
        
        // Footer
        // UISectionT<TLayout>::m_footer.m_frame.origin.y = originOfRow.y;
        UISectionT<TLayout>::m_footer.m_frame.origin.x = originOfRow.x;
        
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
        // UISectionT<TLayout>::m_frame.size.height = CGRectGetMaxY(UISectionT<TLayout>::m_footer.m_frame);
        UISectionT<TLayout>::m_frame.size.width = CGRectGetMaxX(UISectionT<TLayout>::m_footer.m_frame);
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
        return std::equal_range(m_rows.begin(), m_rows.end(), std::pair<CGFloat, CGFloat>(rect.origin.y, rect.origin.y + rect.size.height), UIFlexRowVerticalCompare());
    }
    
    inline std::pair<std::vector<UIFlexRow *>::iterator, std::vector<UIFlexRow *>::iterator> getHorizontalRowsInRect(const CGRect& rect)
    {
        return std::equal_range(m_rows.begin(), m_rows.end(), std::pair<CGFloat, CGFloat>(rect.origin.x, rect.origin.x + rect.size.width), UIFlexRowHorizontalCompare());
    }
    
    
};



#endif /* FlowSection_h */
