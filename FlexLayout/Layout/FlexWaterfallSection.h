//
//  WaterfallSection.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//
#include "FlexSection.h"
#include "FlexColumn.h"

#ifndef NDEBUG
#include <string>
#include <sstream>
#endif // NDEBUG

#ifndef WaterfallSection_h
#define WaterfallSection_h

namespace nsflex
{

template<class TBaseSection, bool VERTICAL>
class FlexWaterfallSectionT : public TBaseSection
{
protected:
    using TBase = TBaseSection;
    
    using TLayout = typename TBaseSection::LayoutType;
    using TInt = typename TBaseSection::IntType;
    using TCoordinate = typename TBaseSection::CoordinateType;
    using Point = typename TBaseSection::Point;
    using Size = typename TBaseSection::Size;
    using Rect = typename TBaseSection::Rect;
    using Insets = typename TBaseSection::Insets;
    using FlexItem = typename TBaseSection::FlexItem;
    using FlexItemLessCompare = FlexItemLessCompareT<TInt, TCoordinate>;
    using FlexColumn = FlexColumnT<TInt, TCoordinate, VERTICAL>;
    using FlexItemIterator = typename std::vector<FlexItem *>::iterator;
    using FlexColumnIterator = typename std::vector<FlexColumn *>::iterator;
    using FlexColumnConstIterator = typename std::vector<FlexColumn *>::const_iterator;
    using ColumnSizeCompare = FlexSizeCompare<FlexColumn, VERTICAL>;
    
    using TBase::x;
    // using TBase::y;
    using TBase::left;
    using TBase::top;
    // using TBase::right;
    using TBase::bottom;
    
    using TBase::offset;
    using TBase::offsetX;
    using TBase::offsetY;
    // using TBase::incWidth;
    
    using TBase::leftBottom;
    using TBase::height;
    using TBase::width;
    
    using TBase::hinsets;
    // using TBase::vinsets;

    using TBase::makePoint;
    

protected:
    std::vector<FlexItem *> m_placeHolderItems;
    std::vector<FlexColumn *> m_columns;

public:
    FlexWaterfallSectionT(TInt section, const Rect &frame) : TBaseSection(section, frame)
    {
    }
    
    ~FlexWaterfallSectionT()
    {
        clearColumns();
    }
    
protected:
    
    inline void clearColumns()
    {
        for(FlexColumnIterator it = m_columns.begin(); it != m_columns.end(); delete *it, ++it);
        m_columns.clear();
        for(FlexItemIterator it = m_placeHolderItems.begin(); it != m_placeHolderItems.end(); delete *it, ++it);
        m_placeHolderItems.clear();
    }
    
    /// Keep the commented code of "horizontally" parts
    void prepareItemsLayout(const TLayout *layout, const Size &size)
    {
        // Items
        clearColumns();
        TBaseSection::clearItems();

        TInt numberOfItems = TBaseSection::getNumberOfItems(layout);
        if (numberOfItems == 0)
        {
            return;
        }
        
        Insets sectionInset = TBaseSection::getInsets(layout);
        Rect frameOfColumn(sectionInset.left, sectionInset.top, 0, 0);
        
        TBaseSection::m_items.reserve(numberOfItems);

        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing(layout);
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing(layout);
        
        // Get Number of Columns
        TInt numberOfColumns = TBaseSection::getNumberOfColumns(layout);
        if (numberOfColumns < 1)
        {
            numberOfColumns = 1;
        }
        
        m_columns.reserve(numberOfColumns);
        TInt estimatedNumberOfItems = (TInt)ceil(numberOfItems / numberOfColumns);
        TCoordinate sizeOfColumn = 0.0;
        
        TCoordinate availableSizeOfColumn = width(size) - hinsets(sectionInset);

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
            
            width(frameOfColumn, sizeOfColumn);

            FlexColumn *column = new FlexColumn(estimatedNumberOfItems, frameOfColumn);
            m_columns.push_back(column);

            Rect rect = m_columns[m_columns.size() - 1]->getFrame();

            
            offsetX(frameOfColumn, sizeOfColumn + minimumInteritemSpacing);
        }
        
        // Layout each item
        typename std::vector<FlexColumn *>::iterator itOfTargetColumn = m_columns.begin();

        ColumnSizeCompare compare;

        Rect frameOfItem;
        bool isFullSpan = false;
        for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
        {
            isFullSpan = false;
            frameOfItem.size = TBaseSection::getSizeForItem(layout, itemIndex, &isFullSpan);
            
            // Find the column with lowest hight
            itOfTargetColumn = isFullSpan ? max_element(m_columns.begin(), m_columns.end(), compare) : min_element(m_columns.begin(), m_columns.end(), compare);

            // Add spacing for the item which is not first one
            frameOfItem.origin = leftBottom((*itOfTargetColumn)->getFrame());
            offsetY(frameOfItem, (*itOfTargetColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);

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
                        top(placeHolder->getFrame(), bottom((*itColumn)->getFrame()) + ((*itColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing));
                        (*itColumn)->addItem(placeHolder);
                    }
                }
            }

            (*itOfTargetColumn)->addItem(item);

            TBaseSection::m_items.push_back(item);
        }
        
        // Find the column with highest height
        FlexColumnConstIterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);
        
        height(TBaseSection::m_itemsFrame, bottom((*columnItOfMaximalSize)->getFrame()) + bottom(sectionInset));
        height(TBaseSection::m_frame, bottom(TBaseSection::m_itemsFrame));
    }

#ifndef NDEBUG
    std::string printDebugInfo() const
    {
        std::ostringstream str;
        int idx = 1;
        for (typename std::vector<FlexColumn *>::const_iterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            str << "Column:" << idx << "\r\n";
            str << (*it)->printDebugInfo("    ");

            idx++;
        }

        return str.str();
    }
#endif // #ifndef NDEBUG

    bool filterItemsInRect(const Rect &rectInSection, std::vector<const FlexItem *> &items) const
    {
        typename std::vector<const FlexItem *>::size_type orgSize = items.size();
        bool firstColumn = true;
        FlexItemLessCompare comp;
        
        // Items
        for (FlexColumnConstIterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> range = (*it)->getItemsInRect(rectInSection);
            
            if (range.first == range.second)
            {
                continue;
            }
            
            items.reserve(items.size() + (range.second - range.first) * m_columns.size());
            
            typename std::vector<const FlexItem *>::iterator itToInsert = items.begin() + orgSize;
            
            for (typename std::vector<FlexItem *>::iterator itItem = range.first; itItem != range.second; ++itItem)
            {
                if ((*itItem)->isPlaceHolder())
                {
                    continue;
                }
                
                if (firstColumn)
                {
                    items.push_back(*itItem);
                }
                else
                {
                    // Keep sorted when we insert
                    if (itToInsert != items.end())
                    {
                        itToInsert = std::lower_bound(itToInsert, items.end(), (*itItem), comp);
                    }
                    items.insert(itToInsert, *itItem);
                    ++itToInsert;
                }
            }
            if (firstColumn)
            {
                firstColumn = false;
                
            }
        }
        
        return orgSize != items.size();;
    }
    
};
    
} // namespace nsflex

#endif /* WaterfallSection_h */
