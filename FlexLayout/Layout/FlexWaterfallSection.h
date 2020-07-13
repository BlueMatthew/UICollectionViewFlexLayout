//
//  WaterfallSection.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef WaterfallSection_h
#define WaterfallSection_h

#import "FlexSection.h"
#import "FlexColumn.h"

namespace nsflex
{

template<typename TBaseSection>
class FlexWaterfallSectionT : public TBaseSection
{
protected:
    typedef typename TBaseSection::LayoutType TLayout;
    typedef typename TBaseSection::IntType TInt;
    typedef typename TBaseSection::CoordinateType TCoordinate;
    typedef typename TBaseSection::Point Point;
    typedef typename TBaseSection::Size Size;
    typedef typename TBaseSection::Rect Rect;
    typedef typename TBaseSection::Insets Insets;
    typedef typename TBaseSection::FlexItem FlexItem;
    typedef FlexColumnT<TInt, TCoordinate> FlexColumn;
    typedef typename std::vector<FlexColumn *>::const_iterator FlexColumnConstIterator;

public:
    std::vector<FlexItem *> m_placeHolderItems;
    std::vector<FlexColumn *> m_columns;
    
    FlexWaterfallSectionT(TLayout *layout, TInt section, const Rect &frame) : TBaseSection(layout, section, frame)
    {
    }
    
    ~FlexWaterfallSectionT()
    {
        clearColumns();
    }
    
protected:
    
    inline void clearColumns()
    {
        for(typename std::vector<FlexColumn *>::iterator it = m_columns.begin(); it != m_columns.end(); delete *it, ++it);
        m_columns.clear();
        for(typename std::vector<FlexItem *>::iterator it = m_placeHolderItems.begin(); it != m_placeHolderItems.end(); delete *it, ++it);
    }
    
    /// Keep the commented code of "horizontally" parts
    Point prepareLayoutWithItemsVertically(const Rect &bounds)
    {
#define INTERNAL_VERTICAL_LAYOUT

        // Items
        clearColumns();
        TBaseSection::clearItems();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Rect frameOfColumn(bounds.left(), TBaseSection::m_header.getFrame().bottom(), 0, 0);
#else
        Rect frameOfColumn(TBaseSection::m_header.getFrame().right(), bounds.top(), 0, 0);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT

        TInt numberOfItems = TBaseSection::getNumberOfItems();
        if (numberOfItems == 0)
        {
            return frameOfColumn.origin;
        }
        
        TBaseSection::m_items.reserve(numberOfItems);
            
        Insets sectionInset = TBaseSection::getInsets();
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing();
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing();
        
        // Get Number of Columns
        TInt numberOfColumns = TBaseSection::getNumberOfColumns();
        if (numberOfColumns < 1)
        {
            numberOfColumns = 1;
        }
        
        m_columns.reserve(numberOfColumns);
        TInt estimatedNumberOfItems = (TInt)ceil(numberOfItems / numberOfColumns);
        TCoordinate sizeOfColumn = 0.0;
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        TCoordinate availableSizeOfColumn = TBaseSection::m_frame.size.width - sectionInset.left - sectionInset.right;
        frameOfColumn.origin.y += sectionInset.top;
#else
        TCoordinate availableSizeOfColumn = TBaseSection::m_frame.size.height - sectionInset.top - sectionInset.bottom;
        frameOfColumn.origin.x += sectionInset.left;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT

        for (TInt columnIndex = 0; columnIndex < numberOfColumns; columnIndex++)
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
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            frameOfColumn.size.width = sizeOfColumn;
#else
            frameOfColumn.size.height = sizeOfColumn;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            FlexColumn *column = new FlexColumn(estimatedNumberOfItems, frameOfColumn);
            m_columns.push_back(column);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            frameOfColumn.origin.x += sizeOfColumn + minimumInteritemSpacing;
#else
            frameOfColumn.origin.y += sizeOfColumn + minimumInteritemSpacing;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
        // Layout each item
        typename std::vector<FlexColumn *>::iterator itOfTargetColumn = m_columns.begin();

#ifdef INTERNAL_VERTICAL_LAYOUT
        FlexVerticalSizeCompare<FlexColumn> compare;
#else
        FlexHorizontalSizeCompare<FlexColumn> compare;
#endif
        
        Rect frameOfItem;
        bool isFullSpan = false;
        for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
        {
            isFullSpan = false;
            frameOfItem.size = TBaseSection::getSizeForItem(itemIndex, &isFullSpan);
            
            // Find the column with lowest hight
            itOfTargetColumn = isFullSpan ? max_element(m_columns.begin(), m_columns.end(), compare) : min_element(m_columns.begin(), m_columns.end(), compare);

            // Add spacing for the item which is not first one
#ifdef INTERNAL_VERTICAL_LAYOUT
            frameOfItem.origin.x = (*itOfTargetColumn)->getFrame().left();
            frameOfItem.origin.y = (*itOfTargetColumn)->getFrame().bottom() + ((*itOfTargetColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
#else
            frameOfItem.origin.x = (*itOfTargetColumn)->getFrame().right() + ((*itOfTargetColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
            frameOfItem.origin.y = (*itOfTargetColumn)->getFrame().top();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT

            FlexItem *item = new FlexItem(itemIndex, frameOfItem);

            if (isFullSpan)
            {
                item->setFullSpan(true);
                
                // Add into the column
                for (typename std::vector<FlexColumn *>::iterator itColumn = m_columns.begin(); itColumn != m_columns.end(); ++itColumn)
                {
                    if (itOfTargetColumn != itColumn)
                    {
                        FlexItem *placeHolder = new FlexItem(*item);
                        placeHolder->setPlaceHolder(true);
                        m_placeHolderItems.push_back(placeHolder);
#ifdef INTERNAL_VERTICAL_LAYOUT
                        placeHolder->getFrame().origin.y = (*itColumn)->getFrame().bottom() + ((*itColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
                        (*itColumn)->addItemVertically(placeHolder);
#else
                        placeHolder->getFrame().origin.x = (*itColumn)->getFrame().right() + ((*itColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
                        (*itColumn)->addItemHorizontally(placeHolder);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
                    }
                }
            }

#ifdef INTERNAL_VERTICAL_LAYOUT
            (*itOfTargetColumn)->addItemVertically(item);
#else
            (*itOfTargetColumn)->addItemHorizontally(item);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT

            TBaseSection::m_items.push_back(item);
        }
        
        // Find the column with highest height
        typename std::vector<FlexColumn *>::iterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        return Point(bounds.left(), (*columnItOfMaximalSize)->getFrame().bottom() + sectionInset.bottom);
#else
        return Point((*columnItOfMaximalSize)->getFrame().right() + sectionInset.right, bounds.top());
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT


#undef INTERNAL_VERTICAL_LAYOUT
    }
    
    /// DON"T EDIT THE CODE DIRECTLY
    /// Update the code in the function of "vertically" first and then sync the commented code of "horizontally" parts
    Point prepareLayoutWithItemsHorizontally(const Rect &bounds)
    {
#undef INTERNAL_VERTICAL_LAYOUT

        // Items
        clearColumns();
        TBaseSection::clearItems();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Rect frameOfColumn(bounds.left(), TBaseSection::m_header.getFrame().bottom(), 0, 0);
#else
        Rect frameOfColumn(TBaseSection::m_header.getFrame().right(), bounds.top(), 0, 0);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        TInt numberOfItems = TBaseSection::getNumberOfItems();
        if (numberOfItems == 0)
        {
            return frameOfColumn.origin;
        }
        
        TBaseSection::m_items.reserve(numberOfItems);
        
        Insets sectionInset = TBaseSection::getInsets();
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing();
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing();
        
        // Get Number of Columns
        TInt numberOfColumns = TBaseSection::getNumberOfColumns();
        if (numberOfColumns < 1)
        {
            numberOfColumns = 1;
        }
        
        m_columns.reserve(numberOfColumns);
        TInt estimatedNumberOfItems = (TInt)ceil(numberOfItems / numberOfColumns);
        TCoordinate sizeOfColumn = 0.0;
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        TCoordinate availableSizeOfColumn = TBaseSection::m_frame.size.width - sectionInset.left - sectionInset.right;
        frameOfColumn.origin.y += sectionInset.top;
#else
        TCoordinate availableSizeOfColumn = TBaseSection::m_frame.size.height - sectionInset.top - sectionInset.bottom;
        frameOfColumn.origin.x += sectionInset.left;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        for (TInt columnIndex = 0; columnIndex < numberOfColumns; columnIndex++)
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
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            frameOfColumn.size.width = sizeOfColumn;
#else
            frameOfColumn.size.height = sizeOfColumn;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            FlexColumn *column = new FlexColumn(estimatedNumberOfItems, frameOfColumn);
            m_columns.push_back(column);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            frameOfColumn.origin.x += sizeOfColumn + minimumInteritemSpacing;
#else
            frameOfColumn.origin.y += sizeOfColumn + minimumInteritemSpacing;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
        // Layout each item
        typename std::vector<FlexColumn *>::iterator itOfTargetColumn = m_columns.begin();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        FlexVerticalSizeCompare<FlexColumn> compare;
#else
        FlexHorizontalSizeCompare<FlexColumn> compare;
#endif
        
        Rect frameOfItem;
        bool isFullSpan = false;
        for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
        {
            isFullSpan = false;
            frameOfItem.size = TBaseSection::getSizeForItem(itemIndex, &isFullSpan);
            
            // Find the column with lowest hight
            itOfTargetColumn = isFullSpan ? max_element(m_columns.begin(), m_columns.end(), compare) : min_element(m_columns.begin(), m_columns.end(), compare);
            
            // Add spacing for the item which is not first one
#ifdef INTERNAL_VERTICAL_LAYOUT
            frameOfItem.origin.x = (*itOfTargetColumn)->getFrame().left();
            frameOfItem.origin.y = (*itOfTargetColumn)->getFrame().bottom() + ((*itOfTargetColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
#else
            frameOfItem.origin.x = (*itOfTargetColumn)->getFrame().right() + ((*itOfTargetColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
            frameOfItem.origin.y = (*itOfTargetColumn)->getFrame().top();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            FlexItem *item = new FlexItem(itemIndex, frameOfItem);
            
            if (isFullSpan)
            {
                item->setFullSpan(true);
                
                // Add into the column
                for (typename std::vector<FlexColumn *>::iterator itColumn = m_columns.begin(); itColumn != m_columns.end(); ++itColumn)
                {
                    if (itOfTargetColumn != itColumn)
                    {
                        FlexItem *placeHolder = new FlexItem(*item);
                        placeHolder->setPlaceHolder(true);
                        m_placeHolderItems.push_back(placeHolder);
#ifdef INTERNAL_VERTICAL_LAYOUT
                        placeHolder->getFrame().origin.y = (*itColumn)->getFrame().bottom() + ((*itColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
                        (*itColumn)->addItemVertically(placeHolder);
#else
                        placeHolder->getFrame().origin.x = (*itColumn)->getFrame().right() + ((*itColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
                        (*itColumn)->addItemHorizontally(placeHolder);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
                    }
                }
            }
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            (*itOfTargetColumn)->addItemVertically(item);
#else
            (*itOfTargetColumn)->addItemHorizontally(item);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            TBaseSection::m_items.push_back(item);
        }
        
        // Find the column with highest height
        typename std::vector<FlexColumn *>::iterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        return Point(bounds.left(), (*columnItOfMaximalSize)->getFrame().bottom() + sectionInset.bottom);
#else
        return Point((*columnItOfMaximalSize)->getFrame().right() + sectionInset.right, bounds.top());
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
       
#undef INTERNAL_VERTICAL_LAYOUT
    }
    
    bool filterItemsInRect(std::vector<const FlexItem *> &items, const Rect &rectInSection) const
    {
        bool matched = false;
        
        // Items
        for (FlexColumnConstIterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> range = TBaseSection::isVertical() ? (*it)->getVirticalItemsInRect(rectInSection) : (*it)->getHorizontalItemsInRect(rectInSection);
            
            for (typename std::vector<FlexItem *>::iterator itItem = range.first; itItem != range.second; ++itItem)
            {
                items.push_back(*itItem);
                matched = true;
            }
        }
        
        return matched;
    }
    
};
    
} // namespace nsflex

#endif /* WaterfallSection_h */
