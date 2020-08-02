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
    
    // Override
    void deleteItem(TInt itemIndex)
    {
        // Before the item is deleted, we have to remove the item pointer from columns first to avoid exception of bad address
        if (!TBaseSection::m_items.empty())
        {
            // In most cases, we removes items from end and it is enought to just check and remove the last item
            (TBaseSection::m_items.back()->getItem() == itemIndex) ? removeLastItemInColumnsIfEquals(itemIndex, false) : removeItemsInColumns(itemIndex, false);
        }
        TBaseSection::deleteItem(itemIndex);
    }
    
protected:
    
    inline void clearColumns()
    {
        for(FlexColumnIterator it = m_columns.begin(); it != m_columns.end(); delete *it, ++it);
        m_columns.clear();
        for(FlexItemIterator it = m_placeHolderItems.begin(); it != m_placeHolderItems.end(); delete *it, ++it);
        m_placeHolderItems.clear();
    }
    
    inline void removeItemsInColumns(TInt itemStart, bool includingPlaceHolder = true)
    {
        for (FlexColumnIterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            (*it)->removeItemsFrom(itemStart);
        }
        if (includingPlaceHolder)
        {
            removePlaceHolderItems(itemStart);
        }
    }
    
    inline void removePlaceHolderItems(TInt itemStart)
    {
        FlexItemIterator it = std::lower_bound(m_placeHolderItems.begin(), m_placeHolderItems.end(), itemStart, FlexItemLessCompareT<TInt, TCoordinate>());
        if (it != m_placeHolderItems.end())
        {
            for (; it != m_placeHolderItems.end(); ++it)
            {
                delete (*it);
            }
            m_placeHolderItems.erase(it, m_placeHolderItems.end());
        }
    }

    inline void removeLastItemInColumnsIfEquals(TInt itemIndex, bool includingPlaceHolder = true)
    {
        for (FlexColumnIterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            (*it)->removeLastItemIfEquals(itemIndex);
        }
        if (includingPlaceHolder)
        {
            removePlaceHolderItems(itemIndex);
        }
    }
    
    // Override
    void prepareItemsLayout(const TLayout *layout, const Size &size)
    {
        // Items
        // clearColumns();
        // TBaseSection::clearItems();

        TInt numberOfItems = TBaseSection::getNumberOfItems(layout);
        if (numberOfItems == 0)
        {
            clearColumns();
            TBaseSection::clearItems();
            return;
        }

        TBaseSection::prepareItems(numberOfItems);

        TInt minInvalidatedItem = TBaseSection::getMinimalInvalidatedItem();
        bool isFullSpan = false;
        TInt itemIndex = minInvalidatedItem;
        for (FlexItemIterator it = TBaseSection::m_items.begin() + minInvalidatedItem; it != TBaseSection::m_items.end(); ++it, ++itemIndex)
        {
            // isFullSpan = false;
            (*it)->setItem(itemIndex);
            (*it)->getFrame().size = TBaseSection::getSizeForItem(layout, itemIndex, &isFullSpan);
            (*it)->setFullSpan(isFullSpan);
        }

        // Get Number of Columns
        TInt numberOfColumns = TBaseSection::getNumberOfColumns(layout);
        if (numberOfColumns < 1)
        {
            numberOfColumns = 1;
        }
        Insets sectionInset = TBaseSection::getInsets(layout);
        
        if (numberOfColumns != m_columns.size() || minInvalidatedItem == 0 || TBaseSection::isSectionInvalidated())
        {
            // If NumberOfColumns changes, we need relayout all items
            minInvalidatedItem = 0;

            clearColumns();

            m_columns.reserve(numberOfColumns);
            TInt estimatedNumberOfItems = (TInt)ceil(numberOfItems / numberOfColumns);
            
            Rect frameOfColumn(sectionInset.left, sectionInset.top, 0, 0);
            
            TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing(layout);
            
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

                offsetX(frameOfColumn, sizeOfColumn + minimumInteritemSpacing);
            }
        }
        else
        {
            // Remove all layout-invalidated items from columns
            removeItemsInColumns(minInvalidatedItem);
        }
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing(layout);
        
        // Layout each item
        Rect frameOfItem;
        ColumnSizeCompare compare;
        FlexColumnIterator itOfTargetColumn = m_columns.begin();
        
        for (FlexItemIterator it = TBaseSection::m_items.begin() + minInvalidatedItem; it != TBaseSection::m_items.end(); ++it)
        {
            // Find the column with lowest hight
            itOfTargetColumn = (*it)->isFullSpan() ? max_element(m_columns.begin(), m_columns.end(), compare) : min_element(m_columns.begin(), m_columns.end(), compare);
            
            // Add spacing for the item which is not first one
            frameOfItem.origin = leftBottom((*itOfTargetColumn)->getFrame());
            offsetY(frameOfItem, (*itOfTargetColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing);
            
            (*it)->getFrame().origin = frameOfItem.origin;
            
            if ((*it)->isFullSpan())
            {
                // Add into the column
                for (FlexColumnIterator itColumn = m_columns.begin(); itColumn != m_columns.end(); ++itColumn)
                {
                    if (itOfTargetColumn != itColumn)
                    {
                        FlexItem *placeHolder = new FlexItem(*(*it));
                        placeHolder->setPlaceHolder(true);
                        m_placeHolderItems.push_back(placeHolder);
                        top(placeHolder->getFrame(), bottom((*itColumn)->getFrame()) + ((*itColumn)->isEmpty() ? (TCoordinate)0 : minimumLineSpacing));
                        (*itColumn)->addItem(placeHolder);
                    }
                }
            }
            
            (*itOfTargetColumn)->addItem(*it);
        }

        // Find the column with highest height
        FlexColumnConstIterator columnItOfMaximalSize = max_element(m_columns.begin(), m_columns.end(), compare);

        height(TBaseSection::m_itemsFrame, bottom((*columnItOfMaximalSize)->getFrame()) + bottom(sectionInset));
        height(TBaseSection::m_frame, bottom(TBaseSection::m_itemsFrame));
    }

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
