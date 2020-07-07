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
#define VERTICAL_LAYOUT
        
        // Header
        UISectionT<TLayout>::m_header.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:UISectionT<TLayout>::m_section];
        
        // Items
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.getFrame().size.height;
#else
        UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
        
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
#ifdef VERTICAL_LAYOUT
            CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
#else
            CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;
#endif // #ifdef VERTICAL_LAYOUT

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
                
#ifdef VERTICAL_LAYOUT
                frameOfColumn.size.width = sizeOfColumn;
#else
                frameOfColumn.size.height = sizeOfColumn;
#endif // #ifdef VERTICAL_LAYOUT
                
                UIFlexColumn *column = new UIFlexColumn(estimatedNumberOfItems, frameOfColumn);
                m_columns.push_back(column);
                
#ifdef VERTICAL_LAYOUT
                frameOfColumn.origin.x += sizeOfColumn + minimumInteritemSpacing;
#else
                frameOfColumn.origin.y += sizeOfColumn + minimumInteritemSpacing;
#endif // #ifdef VERTICAL_LAYOUT
            }
            
            // Layout each item
            std::vector<UIFlexColumn *>::iterator itOfTargetColumn = m_columns.begin();
            
            // Comparator object is small, create for both orientations
#ifdef VERTICAL_LAYOUT
            UIFlexColumnVerticalCompare compare;
#else
            UIFlexColumnHorizontalCompare compare;
#endif
            
            CGPoint originOfItem = CGPointZero;
            for (NSInteger item = 0; item < numberOfItems; item++)
            {
                BOOL isFullSpan = [UISectionT<TLayout>::m_layout isFullSpanAtItem:item forSection:(UISectionT<TLayout>::m_section)];
                
                // Find the column with lowest hight
                itOfTargetColumn = isFullSpan ? max_element(m_columns.begin(), m_columns.end(), compare) : min_element(m_columns.begin(), m_columns.end(), compare);

                // Add spacing for the item which is not first one
#ifdef VERTICAL_LAYOUT
                originOfItem.x = (*itOfTargetColumn)->getFrame().origin.x;
                originOfItem.y = CGRectGetMaxY((*itOfTargetColumn)->getFrame()) + ((*itOfTargetColumn)->isEmpty() ? 0.0f : minimumLineSpacing);
#else
                originOfItem.x = CGRectGetMaxX((*itOfTargetColumn)->getFrame()) + ((*itOfTargetColumn)->isEmpty() ? 0.0f : minimumLineSpacing);
                originOfItem.y = (*itOfTargetColumn)->getFrame().origin.y;
#endif // #ifdef VERTICAL_LAYOUT
                
                UIFlexItem *sectionItem = new UIFlexItem(item, originOfItem, [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]]);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                
                if (isFullSpan)
                {
                    // Add into the column
                    for (std::vector<UIFlexColumn *>::iterator itColumn = m_columns.begin(); itColumn != m_columns.end(); ++itColumn)
                    {
#ifdef VERTICAL_LAYOUT
                        (*itColumn)->addItemVertically(sectionItem, (itOfTargetColumn != itColumn));
#else
                        (*itColumn)->addItemHorizontally(sectionItem, (itOfTargetColumn != itColumn));
#endif // #ifdef VERTICAL_LAYOUT
                    }
                }
                else
                {
#ifdef VERTICAL_LAYOUT
                    (*itOfTargetColumn)->addItemVertically(sectionItem);
#else
                    (*itOfTargetColumn)->addItemHorizontally(sectionItem);
#endif // #ifdef VERTICAL_LAYOUT
                }
            }
            
            // Find the column with highest height
            std::vector<UIFlexColumn *>::iterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
            
#ifdef VERTICAL_LAYOUT
            UISectionT<TLayout>::getFrame().size.height += (*columnItOfMaximalSize)->getFrame().size.height + sectionInset.top + sectionInset.bottom;
#else
            UISectionT<TLayout>::getFrame().size.width += (*columnItOfMaximalSize)->getFrame().size.width + sectionInset.left + sectionInset.right;
#endif // #ifdef VERTICAL_LAYOUT
        }
        
        // Footer
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
     
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_footer.getFrame().size.height;
#else
        UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_footer.getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT

#undef VERTICAL_LAYOUT
    }
    
    /// DON"T EDIT THE CODE DIRECTLY
    /// Update the code in the function of "vertically" first and then sync the commented code of "horizontally" parts
    void prepareLayoutHorizontally()
    {
#undef VERTICAL_LAYOUT
        
        // Header
        UISectionT<TLayout>::m_header.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:UISectionT<TLayout>::m_section];
        
        // Items
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_header.getFrame().size.height;
#else
        UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_header.getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT
        
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
#ifdef VERTICAL_LAYOUT
            CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.width - sectionInset.left - sectionInset.right;
#else
            CGFloat availableSizeOfColumn = UISectionT<TLayout>::m_frame.size.height - sectionInset.top - sectionInset.bottom;
#endif // #ifdef VERTICAL_LAYOUT
            
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
                
#ifdef VERTICAL_LAYOUT
                frameOfColumn.size.width = sizeOfColumn;
#else
                frameOfColumn.size.height = sizeOfColumn;
#endif // #ifdef VERTICAL_LAYOUT
                
                UIFlexColumn *column = new UIFlexColumn(estimatedNumberOfItems, frameOfColumn);
                m_columns.push_back(column);
                
#ifdef VERTICAL_LAYOUT
                frameOfColumn.origin.x += sizeOfColumn + minimumInteritemSpacing;
#else
                frameOfColumn.origin.y += sizeOfColumn + minimumInteritemSpacing;
#endif // #ifdef VERTICAL_LAYOUT
            }
            
            // Layout each item
            std::vector<UIFlexColumn *>::iterator itOfTargetColumn = m_columns.begin();
            
            // Comparator object is small, create for both orientations
#ifdef VERTICAL_LAYOUT
            UIFlexColumnVerticalCompare compare;
#else
            UIFlexColumnHorizontalCompare compare;
#endif
            
            CGPoint originOfItem = CGPointZero;
            for (NSInteger item = 0; item < numberOfItems; item++)
            {
                BOOL isFullSpan = [UISectionT<TLayout>::m_layout isFullSpanAtItem:item forSection:(UISectionT<TLayout>::m_section)];
                
                // Find the column with lowest hight
                itOfTargetColumn = isFullSpan ? max_element(m_columns.begin(), m_columns.end(), compare) : min_element(m_columns.begin(), m_columns.end(), compare);
                
                // Add spacing for the item which is not first one
#ifdef VERTICAL_LAYOUT
                originOfItem.x = (*itOfTargetColumn)->getFrame().origin.x;
                originOfItem.y = CGRectGetMaxY((*itOfTargetColumn)->getFrame()) + ((*itOfTargetColumn)->isEmpty() ? 0.0f : minimumLineSpacing);
#else
                originOfItem.x = CGRectGetMaxX((*itOfTargetColumn)->getFrame()) + ((*itOfTargetColumn)->isEmpty() ? 0.0f : minimumLineSpacing);
                originOfItem.y = (*itOfTargetColumn)->getFrame().origin.y;
#endif // #ifdef VERTICAL_LAYOUT
                
                UIFlexItem *sectionItem = new UIFlexItem(item, originOfItem, [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]]);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                
                if (isFullSpan)
                {
                    // Add into the column
                    for (std::vector<UIFlexColumn *>::iterator itColumn = m_columns.begin(); itColumn != m_columns.end(); ++itColumn)
                    {
#ifdef VERTICAL_LAYOUT
                        (*itColumn)->addItemVertically(sectionItem, (itOfTargetColumn != itColumn));
#else
                        (*itColumn)->addItemHorizontally(sectionItem, (itOfTargetColumn != itColumn));
#endif // #ifdef VERTICAL_LAYOUT
                    }
                }
                else
                {
#ifdef VERTICAL_LAYOUT
                    (*itOfTargetColumn)->addItemVertically(sectionItem);
#else
                    (*itOfTargetColumn)->addItemHorizontally(sectionItem);
#endif // #ifdef VERTICAL_LAYOUT
                }
            }
            
            // Find the column with highest height
            std::vector<UIFlexColumn *>::iterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
            
#ifdef VERTICAL_LAYOUT
            UISectionT<TLayout>::getFrame().size.height += (*columnItOfMaximalSize)->getFrame().size.height + sectionInset.top + sectionInset.bottom;
#else
            UISectionT<TLayout>::getFrame().size.width += (*columnItOfMaximalSize)->getFrame().size.width + sectionInset.left + sectionInset.right;
#endif // #ifdef VERTICAL_LAYOUT
        }
        
        // Footer
        UISectionT<TLayout>::m_footer.getFrame().size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
#ifdef VERTICAL_LAYOUT
        UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_footer.getFrame().size.height;
#else
        UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_footer.getFrame().size.width;
#endif // #ifdef VERTICAL_LAYOUT

#undef VERTICAL_LAYOUT
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
                NSIndexPath *indexPath = [NSIndexPath indexPathForItem:(*itItem)->getItem() inSection:(UISectionT<TLayout>::m_section)];
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
