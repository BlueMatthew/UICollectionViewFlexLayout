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
#define VERTICAL_LAYOUT
        
        // Header
        UISectionT<TLayout>::m_header.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:(UISectionT<TLayout>::m_section)];
        
        // Initialize the section height with header height
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.getFrame().size.height;
#else
        UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.getFrame().size.width;
#endif // ifdef VERTICAL_LAYOUT
        
        // Items
        CGPoint originOfRow = UISectionT<TLayout>::m_header.getFrame().origin;
        
#ifdef VERTICAL_LAYOUT
        originOfRow.y += UISectionT<TLayout>::m_header.getFrame().size.height;
#else
        originOfRow.x += UISectionT<TLayout>::m_header.getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
        
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
          
#ifdef VERTICAL_LAYOUT
            CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
#else
            CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;
#endif // #ifdef VERTICAL_LAYOUT
            
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
                
#ifdef VERTICAL_LAYOUT
                sizeOfItemInDirection = sizeOfItem.width;
#else
                sizeOfItemInDirection = sizeOfItem.height;
#endif // #ifdef VERTICAL_LAYOUT
                
                if (NULL != row)
                {
                    if (row->hasItems())
                    {
#ifdef VERTICAL_LAYOUT
                        availableSizeOfRow = maximalSizeOfRow - row->getFrame().size.width - minimumInteritemSpacing;
#else
                        availableSizeOfRow = maximalSizeOfRow - row->getFrame().size.height - minimumInteritemSpacing;
#endif // #ifdef VERTICAL_LAYOUT
                        
                        if (availableSizeOfRow < sizeOfItemInDirection)
                        {
                            // New Line
                            m_rows.push_back(row);
#ifdef VERTICAL_LAYOUT
                            originOfRow.y += minimumLineSpacing + row->getFrame().size.height;
#else
                            originOfRow.x += minimumLineSpacing + row->getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
                            row = NULL;
                        }
                    }
                }
                
                if (NULL == row)
                {
                    row = new UIFlexRow();
                    row->getFrame().origin = originOfRow;
                    
                    originOfItem = originOfRow;
                }
                
                if (row->hasItems())
                {
#ifdef VERTICAL_LAYOUT
                    originOfItem.x = CGRectGetMaxX(row->getFrame()) + minimumInteritemSpacing;
#else
                    originOfItem.y = CGRectGetMaxY(row->getFrame()) + minimumInteritemSpacing;
#endif // #ifdef VERTICAL_LAYOUT
                }
                
                sectionItem = new UIFlexItem(itemIndex, originOfItem, sizeOfItem);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
#ifdef VERTICAL_LAYOUT
                row->addItemVertically(sectionItem);
#else
                row->addItemHorizontally(sectionItem);
#endif // #ifdef VERTICAL_LAYOUT
            }
            
            // The last row
            if (NULL != row)
            {
                m_rows.push_back(row);
                
#ifdef VERTICAL_LAYOUT
                originOfRow.y += row->getFrame().size.height;
#else
                originOfRow.x += row->getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
            }
            
#ifdef VERTICAL_LAYOUT
            originOfRow.y += sectionInset.bottom;
#else
            originOfRow.x += sectionInset.right;
#endif // #ifdef VERTICAL_LAYOUT
        }
        
        // Footer
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_footer.getFrame().origin.y = originOfRow.y;
#else
        UISectionT<TLayout>::m_footer.getFrame().origin.x = originOfRow.x;
#endif // #ifdef VERTICAL_LAYOUT
        
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height = CGRectGetMaxY(UISectionT<TLayout>::m_footer.getFrame());
#else
        UISectionT<TLayout>::m_frame.size.width = CGRectGetMaxX(UISectionT<TLayout>::m_footer.getFrame());
#endif // #ifdef VERTICAL_LAYOUT
        
#undef VERTICAL_LAYOUT
    }
    
    void prepareLayoutHorizontally()
    {
#undef VERTICAL_LAYOUT
        
        // Header
        UISectionT<TLayout>::m_header.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:(UISectionT<TLayout>::m_section)];
        
        // Initialize the section height with header height
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.getFrame().size.height;
#else
        UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.getFrame().size.width;
#endif // ifdef VERTICAL_LAYOUT
        
        // Items
        CGPoint originOfRow = UISectionT<TLayout>::m_header.getFrame().origin;
        
#ifdef VERTICAL_LAYOUT
        originOfRow.y += UISectionT<TLayout>::m_header.getFrame().size.height;
#else
        originOfRow.x += UISectionT<TLayout>::m_header.getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
        
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
            
#ifdef VERTICAL_LAYOUT
            CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
#else
            CGFloat maximalSizeOfRow = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;
#endif // #ifdef VERTICAL_LAYOUT
            
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
                
#ifdef VERTICAL_LAYOUT
                sizeOfItemInDirection = sizeOfItem.width;
#else
                sizeOfItemInDirection = sizeOfItem.height;
#endif // #ifdef VERTICAL_LAYOUT
                
                if (NULL != row)
                {
                    if (row->hasItems())
                    {
#ifdef VERTICAL_LAYOUT
                        availableSizeOfRow = maximalSizeOfRow - row->getFrame().size.width - minimumInteritemSpacing;
#else
                        availableSizeOfRow = maximalSizeOfRow - row->getFrame().size.height - minimumInteritemSpacing;
#endif // #ifdef VERTICAL_LAYOUT
                        
                        if (availableSizeOfRow < sizeOfItemInDirection)
                        {
                            // New Line
                            m_rows.push_back(row);
#ifdef VERTICAL_LAYOUT
                            originOfRow.y += minimumLineSpacing + row->getFrame().size.height;
#else
                            originOfRow.x += minimumLineSpacing + row->getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
                            row = NULL;
                        }
                    }
                }
                
                if (NULL == row)
                {
                    row = new UIFlexRow();
                    row->getFrame().origin = originOfRow;
                    
                    originOfItem = originOfRow;
                }
                
                if (row->hasItems())
                {
#ifdef VERTICAL_LAYOUT
                    originOfItem.x = CGRectGetMaxX(row->getFrame()) + minimumInteritemSpacing;
#else
                    originOfItem.y = CGRectGetMaxY(row->getFrame()) + minimumInteritemSpacing;
#endif // #ifdef VERTICAL_LAYOUT
                }
                
                sectionItem = new UIFlexItem(itemIndex, originOfItem, sizeOfItem);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
#ifdef VERTICAL_LAYOUT
                row->addItemVertically(sectionItem);
#else
                row->addItemHorizontally(sectionItem);
#endif // #ifdef VERTICAL_LAYOUT
            }
            
            // The last row
            if (NULL != row)
            {
                m_rows.push_back(row);
                
#ifdef VERTICAL_LAYOUT
                originOfRow.y += row->getFrame().size.height;
#else
                originOfRow.x += row->getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
            }
            
#ifdef VERTICAL_LAYOUT
            originOfRow.y += sectionInset.bottom;
#else
            originOfRow.x += sectionInset.right;
#endif // #ifdef VERTICAL_LAYOUT
        }
        
        // Footer
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_footer.getFrame().origin.y = originOfRow.y;
#else
        UISectionT<TLayout>::m_footer.getFrame().origin.x = originOfRow.x;
#endif // #ifdef VERTICAL_LAYOUT
        
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height = CGRectGetMaxY(UISectionT<TLayout>::m_footer.getFrame());
#else
        UISectionT<TLayout>::m_frame.size.width = CGRectGetMaxX(UISectionT<TLayout>::m_footer.getFrame());
#endif // #ifdef VERTICAL_LAYOUT
        
#undef VERTICAL_LAYOUT
    }
    
    virtual bool getLayoutAttributesForItemsInRect(NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributes, const CGRect &rectInSection)
    {
        bool matched = false;
        
        std::pair<std::vector<UIFlexRow *>::iterator, std::vector<UIFlexRow *>::iterator> range = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? getVirticalRowsInRect(rectInSection) : getHorizontalRowsInRect(rectInSection);
        
        
        std::vector<UIFlexRow *>::iterator lastRow = range.second - 1;
        for (std::vector<UIFlexRow *>::iterator it = range.first; it != range.second; ++it)
        {
            std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator> itemRange = (*it)->getItemIterator();
            for (std::vector<UIFlexItem *>::iterator itItem = itemRange.first; itItem != itemRange.second; ++itItem)
            {
                if ((it != range.first && it != lastRow) || CGRectIntersectsRect((*itItem)->getFrame(), rectInSection))
                {
                    // layoutAttributes
                    NSIndexPath *indexPath = [NSIndexPath indexPathForItem:(*itItem)->getItem() inSection:(UISectionT<TLayout>::m_section)];
                    UICollectionViewLayoutAttributes *la = [UISectionT<TLayout>::m_layout layoutAttributesForItemAtIndexPath:indexPath];
                    
                    la.frame = CGRectMake((*itItem)->getFrame().origin.x + UISectionT<TLayout>::m_frame.origin.x, (*itItem)->getFrame().origin.y + UISectionT<TLayout>::m_frame.origin.y, (*itItem)->getFrame().size.width, (*itItem)->getFrame().size.height);
                    
                    [layoutAttributes addObject:la];
                    matched = true;
                }
            }
        }
        
        return matched;
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
