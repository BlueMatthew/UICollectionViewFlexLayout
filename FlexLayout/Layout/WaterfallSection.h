//
//  WaterfallSection.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef WaterfallSection_h
#define WaterfallSection_h

#import "Section.h"
#import "FlexColumn.h"

template<class TLayout>
class UIWaterfallSectionT : public UISectionT<TLayout>
{
public:
    std::vector<UIFlexColumn *> m_columns;
    
    UIWaterfallSectionT(TLayout *layout, NSInteger section, const CGRect &frame) : UISectionT<TLayout>(layout, section, frame)
    {
    }
    
    ~UIWaterfallSectionT()
    {
        for(std::vector<UIFlexColumn *>::iterator it = m_columns.begin(); it != m_columns.end(); delete *it, ++it);
        m_columns.clear();
    }
    
    /// Keep the commented code of "horizontally" parts
    void prepareLayoutVertically()
    {
        // Header
        UISectionT<TLayout>::m_header.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:UISectionT<TLayout>::m_section];
        
        // Items
        UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.m_frame.size.height;
        // UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.m_frame.size.width;
        
        m_columns.clear();
        UISectionT<TLayout>::clearItems();
        
        NSInteger numberOfItems = [UISectionT<TLayout>::m_layout.collectionView numberOfItemsInSection:(UISectionT<TLayout>::m_section)];
        if (numberOfItems > 0)
        {
            UISectionT<TLayout>::m_items.reserve(numberOfItems);
            
            UIEdgeInsets sectionInset = [UISectionT<TLayout>::m_layout getInsetForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            CGFloat minimumLineSpacing = [UISectionT<TLayout>::m_layout getMinimumLineSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            CGFloat minimumInteritemSpacing = [UISectionT<TLayout>::m_layout getMinimumInteritemSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            // Get Number of Columns
            NSInteger numberOfColumns = [UISectionT<TLayout>::m_layout getNumberOfColumnsForSection:(UISectionT<TLayout>::m_section)];
            if (numberOfColumns < 1)
            {
                numberOfColumns = 1;
            }
            
            m_columns.reserve(numberOfColumns);
            
            CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
            // CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;

            NSInteger estimatedNumberOfItems = (NSInteger)ceil(numberOfItems / numberOfColumns);
            
            CGFloat sizeOfColumn = 0.0;
            CGRect frameOfColumn = CGRectMake(sectionInset.left, sectionInset.top, sizeOfColumn, sizeOfColumn);
            for (int columnIndex = 0; columnIndex < numberOfColumns; columnIndex++)
            {
                if (columnIndex == numberOfColumns - 1)
                {
                    sizeOfColumn = availableSizeOfColumn;
                    // availableSizeOfColumn = 0.0;
                }
                else
                {
                    sizeOfColumn = round((availableSizeOfColumn - (numberOfColumns - columnIndex - 1) * minimumInteritemSpacing) / (numberOfColumns - columnIndex));
                    availableSizeOfColumn -= sizeOfColumn + minimumInteritemSpacing;
                }
                
                frameOfColumn.size.width = sizeOfColumn;
                // frameOfColumn.size.height = sizeOfColumn;
                
                UIFlexColumn *column = new UIFlexColumn(estimatedNumberOfItems, frameOfColumn);
                m_columns.push_back(column);
                
                frameOfColumn.origin.x += sizeOfColumn + minimumInteritemSpacing;
                // frameOfColumn.origin.y += sizeOfColumn + minimumInteritemSpacing;
            }
            
            // Layout each item
            std::vector<UIFlexColumn *>::iterator columnItOfMinimalSize = m_columns.begin();
            
            // Comparator object is small, create for both orientations
            UIFlexColumnVerticalCompare compare;
            // UIFlexColumnHorizontalCompare compare;
            
            CGPoint originOfItem = CGPointZero;
            for (NSInteger item = 0; item < numberOfItems; item++)
            {
                // Find the column with lowest hight
                columnItOfMinimalSize = min_element(m_columns.begin(), m_columns.end(), compare);

                // Add spacing for the item which is not first one
                originOfItem.x = (*columnItOfMinimalSize)->m_frame.origin.x;
                originOfItem.y = CGRectGetMaxY((*columnItOfMinimalSize)->m_frame) + ((*columnItOfMinimalSize)->hasItems() ? minimumLineSpacing : 0.0f);
                // originOfItem.x = CGRectGetMaxX((*columnItOfMinimalSize)->m_frame) + ((*columnItOfMinimalSize)->hasItems() ? minimumLineSpacing : 0.0f);
                // originOfItem.y = (*columnItOfMinimalSize)->m_frame.origin.y;
                
                UIFlexItem *sectionItem = new UIFlexItem(item, originOfItem, [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]]);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                
                // Add into the column with lowest hight
                (*columnItOfMinimalSize)->addItemVertically(sectionItem);
                // (*columnItOfMinimalSize)->addItemHorizontally(sectionItem);
            }
            
            // Find the column with highest height
            std::vector<UIFlexColumn *>::iterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
            
            UISectionT<TLayout>::m_frame.size.height += (*columnItOfMaximalSize)->m_frame.size.height + sectionInset.top + sectionInset.bottom;
            // UISectionT<TLayout>::m_frame.size.width += (*columnItOfMaximalSize)->m_frame.size.width + sectionInset.left + sectionInset.right;
        }
        
        // Footer
        UISectionT<TLayout>::m_footer.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
        UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_footer.m_frame.size.height;
        // UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_footer.m_frame.size.width;
    }
    
    /// DON"T EDIT THE CODE DIRECTLY
    /// Update the code in the function of "vertically" first and then sync the commented code of "horizontally" parts
    void prepareLayoutHorizontally()
    {
        // Header
        UISectionT<TLayout>::m_header.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:UISectionT<TLayout>::m_section];
        
        // Items
        // UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.m_frame.size.height;
        UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.m_frame.size.width;
        
        m_columns.clear();
        UISectionT<TLayout>::clearItems();
        
        NSInteger numberOfItems = [UISectionT<TLayout>::m_layout.collectionView numberOfItemsInSection:(UISectionT<TLayout>::m_section)];
        if (numberOfItems > 0)
        {
            UISectionT<TLayout>::m_items.reserve(numberOfItems);
            
            UIEdgeInsets sectionInset = [UISectionT<TLayout>::m_layout getInsetForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            CGFloat minimumLineSpacing = [UISectionT<TLayout>::m_layout getMinimumLineSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            CGFloat minimumInteritemSpacing = [UISectionT<TLayout>::m_layout getMinimumInteritemSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            // Get Number of Columns
            NSInteger numberOfColumns = [UISectionT<TLayout>::m_layout getNumberOfColumnsForSection:(UISectionT<TLayout>::m_section)];
            if (numberOfColumns < 1)
            {
                numberOfColumns = 1;
            }
            
            m_columns.reserve(numberOfColumns);
            
            // CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
            CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;

            NSInteger estimatedNumberOfItems = (NSInteger)ceil(numberOfItems / numberOfColumns);
            
            CGFloat sizeOfColumn = 0.0;
            CGRect frameOfColumn = CGRectMake(sectionInset.left, sectionInset.top, sizeOfColumn, sizeOfColumn);
            for (int columnIndex = 0; columnIndex < numberOfColumns; columnIndex++)
            {
                if (columnIndex == numberOfColumns - 1)
                {
                    sizeOfColumn = availableSizeOfColumn;
                    // availableSizeOfColumn = 0.0;
                }
                else
                {
                    sizeOfColumn = round((availableSizeOfColumn - (numberOfColumns - columnIndex - 1) * minimumInteritemSpacing) / (numberOfColumns - columnIndex));
                    availableSizeOfColumn -= sizeOfColumn + minimumInteritemSpacing;
                }
                
                // frameOfColumn.size.width = sizeOfColumn;
                frameOfColumn.size.height = sizeOfColumn;
                
                UIFlexColumn *column = new UIFlexColumn(estimatedNumberOfItems, frameOfColumn);
                m_columns.push_back(column);
                
                // frameOfColumn.origin.x += sizeOfColumn + minimumInteritemSpacing;
                frameOfColumn.origin.y += sizeOfColumn + minimumInteritemSpacing;
            }
            
            // Layout each item
            std::vector<UIFlexColumn *>::iterator columnItOfMinimalSize = m_columns.begin();
            
            // Comparator object is small, create for both orientations
            // UIFlexColumnVerticalCompare compare;
            UIFlexColumnHorizontalCompare compare;
            
            CGPoint originOfItem = CGPointZero;
            for (NSInteger item = 0; item < numberOfItems; item++)
            {
                // Find the column with lowest hight
                columnItOfMinimalSize = min_element(m_columns.begin(), m_columns.end(), compare);
                
                // Add spacing for the item which is not first one
                // originOfItem.x = (*columnItOfMinimalSize)->m_frame.origin.x;
                // originOfItem.y = CGRectGetMaxY((*columnItOfMinimalSize)->m_frame) + ((*columnItOfMinimalSize)->hasItems() ? minimumLineSpacing : 0.0f);
                originOfItem.x = CGRectGetMaxX((*columnItOfMinimalSize)->m_frame) + ((*columnItOfMinimalSize)->hasItems() ? minimumLineSpacing : 0.0f);
                originOfItem.y = (*columnItOfMinimalSize)->m_frame.origin.y;
                
                UIFlexItem *sectionItem = new UIFlexItem(item, originOfItem, [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]]);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                
                // Add into the column with lowest hight
                // (*columnItOfMinimalSize)->addItemVertically(sectionItem);
                (*columnItOfMinimalSize)->addItemHorizontally(sectionItem);
            }
            
            // Find the column with highest height
            std::vector<UIFlexColumn *>::iterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
            
            UISectionT<TLayout>::m_frame.size.height += (*columnItOfMaximalSize)->m_frame.size.height + sectionInset.top + sectionInset.bottom;
            // UISectionT<TLayout>::m_frame.size.width += (*columnItOfMaximalSize)->m_frame.size.width + sectionInset.left + sectionInset.right;
        }
        
        // Footer
        UISectionT<TLayout>::m_footer.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
        UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_footer.m_frame.size.height;
        // UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_footer.m_frame.size.width;
    }
    
    bool getLayoutAttributesForItemsInRect(NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributes, const CGRect &rectInSection)
    {
        bool merged = false;
        
        // Items
        for (std::vector<UIFlexColumn *>::iterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator> range = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (*it)->getVirticalItemsInRect(rectInSection) : (*it)->getHorizontalItemsInRect(rectInSection);
            
            for (std::vector<UIFlexItem *>::iterator itItem = range.first; itItem != range.second; ++itItem)
            {
                // layoutAttributes
                NSIndexPath *indexPath = [NSIndexPath indexPathForItem:(*itItem)->m_item inSection:(UISectionT<TLayout>::m_section)];
                UICollectionViewLayoutAttributes *la = [UISectionT<TLayout>::m_layout layoutAttributesForItemAtIndexPath:indexPath];
                
                la.frame = CGRectMake((*itItem)->getFrame().origin.x + UISectionT<TLayout>::m_frame.origin.x, (*itItem)->getFrame().origin.y + UISectionT<TLayout>::m_frame.origin.y, (*itItem)->getFrame().size.width, (*itItem)->getFrame().size.height);
                
                [layoutAttributes addObject:la];
                merged = true;
            }
        }
        
        return merged;
    }
    
};

#endif /* WaterfallSection_h */
