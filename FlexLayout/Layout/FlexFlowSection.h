//
//  FlowSection.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlowSection_h
#define FlowSection_h

#include "FlexSection.h"
#include "FlexRow.h"
#include <utility>
#include <memory>

namespace nsflex
{
    
template<class TBaseSection, bool VERTICAL>
class FlexFlowSectionT : public TBaseSection
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
    using FlexItemIterator = typename std::vector<FlexItem *>::iterator;
    using FlexRow = FlexRowT<TInt, TCoordinate, VERTICAL>;
    using FlexRowIterator = typename std::vector<FlexRow *>::iterator;
    using FlexRowConstIterator = typename std::vector<FlexRow *>::const_iterator;

    using RowCompare = FlexCompareT<FlexRow, VERTICAL>;
    using RowItemLessCompare = FlexRowItemLessCompareT<TInt, TCoordinate, VERTICAL>;

    using TBase::x;
    using TBase::top;
    using TBase::right;
    using TBase::bottom;

    using TBase::offset;
    using TBase::offsetY;
    
    using TBase::height;
    using TBase::width;

    using TBase::hinsets;

protected:
    std::vector<FlexRow *> m_rows;
    
public:
    FlexFlowSectionT(TInt section, const Rect &frame) : TBaseSection(section, frame)
    {
    }
    
    virtual ~FlexFlowSectionT()
    {
        clearRows();
    }
    
    // Override
    void deleteItem(TInt itemIndex)
    {
        // Before the item is deleted, we have to remove the item pointer from columns first to avoid exception of bad address
        if (!TBaseSection::m_items.empty())
        {
            // In most cases, we removes items from end and it is enought to just check and remove the last item
            if (TBaseSection::m_items.back()->getItem() != itemIndex || !removeLastItemInRowsIfEquals(itemIndex))
            {
                removeItemsInRows(itemIndex);
            }
        }
        
        TBaseSection::deleteItem(itemIndex);
    }
    
protected:
    inline void clearRows()
    {
        for(typename std::vector<FlexRow *>::iterator it = m_rows.begin(); it != m_rows.end(); delete *it, ++it);
        m_rows.clear();
    }
    
    inline void removeItemsInRows(TInt itemStart)
    {
        FlexRowIterator it = lower_bound(m_rows.begin(), m_rows.end(), itemStart, RowItemLessCompare());
        if (it != m_rows.end())
        {
            (*it)->removeItemsFrom(itemStart);
            if ((*it)->hasItems())
            {
                ++it;
            }
            
            for (FlexRowIterator it2 = it; it2 != m_rows.end(); delete (*it2), ++it2);
            m_rows.erase(it, m_rows.end());
        }
    }
    
    inline bool removeLastItemInRowsIfEquals(TInt itemIndex)
    {
        if (m_rows.empty())
        {
            return false;
        }
        
        FlexRowIterator it = m_rows.end() - 1;
        bool existed = (*it)->removeLastItemIfEquals(itemIndex);

        // check if needs remove the last row
        if (!((*it)->hasItems()))
        {
            delete (*it);
            m_rows.erase(it);
        }
        
        return existed;
    }

    void prepareItemsLayout(const TLayout *layout, const Size &size)
    {
        TInt numberOfItems = TBaseSection::getNumberOfItems(layout);
        if (numberOfItems == 0)
        {
            clearRows();
            TBaseSection::clearItems();
            return;
        }

        TBaseSection::prepareItems(numberOfItems);

        TInt minInvalidatedItem = TBaseSection::getMinimalInvalidatedItem();
        if (minInvalidatedItem == 0)
        {
            clearRows();
        }
        else
        {
            // Remove all layout-invalidated items from columns
            removeItemsInRows(minInvalidatedItem);
        }

        TInt itemIndex = minInvalidatedItem;
        for (FlexItemIterator it = TBaseSection::m_items.begin() + minInvalidatedItem; it != TBaseSection::m_items.end(); ++it, ++itemIndex)
        {
            (*it)->getFrame().size = TBaseSection::getSizeForItem(layout, itemIndex, NULL);
            (*it)->setItem(itemIndex);
        }

        if (minInvalidatedItem < TBaseSection::m_items.size())
        {
            Insets sectionInset = TBaseSection::getInsets(layout);
            // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
            TInt numberOfColumns = TBaseSection::getNumberOfColumns(layout);
            m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
            
            TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing(layout);
            TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing(layout);
            
            TCoordinate maximalSizeOfRow = width(size) - hinsets(sectionInset);

            // Layout items
            FlexRow *row = m_rows.empty() ? NULL : m_rows.back();
            
            Point originOfRow(sectionInset.left, sectionInset.top);
            Point originOfItem = originOfRow;
            TCoordinate availableSizeOfRow = 0;
            
            // for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
            for (FlexItemIterator it = TBaseSection::m_items.begin() + minInvalidatedItem; it != TBaseSection::m_items.end(); ++it)
            {
                // frameOfItem.size = TBaseSection::getSizeForItem(layout, itemIndex, NULL);
                if (NULL != row && row->hasItems())
                {
                    // If there is an available row and it has items already
                    // Check if the available width is enough to put the new item
                    // If it is not enough, will create new row
                    availableSizeOfRow = maximalSizeOfRow - width(row->getFrame()) - minimumInteritemSpacing;
                    if (availableSizeOfRow < width((*it)->getFrame()))
                    {
                        // New Line
                        m_rows.push_back(row);
                        offsetY(originOfRow, minimumLineSpacing + height(row->getFrame()));
                        row = NULL;
                    }
                }
                
                if (NULL == row)
                {
                    row = new FlexRow();
                    row->getFrame().origin = originOfRow;
                    
                    originOfItem = originOfRow;
                }
                if (row->hasItems())
                {
                    x(originOfItem, right(row->getFrame()) + minimumInteritemSpacing);
                }
                
                (*it)->getFrame().origin = originOfItem;
                // FlexItem *item = new FlexItem(itemIndex, frameOfItem);
                // TBaseSection::m_items.push_back(item);
                row->addItem(*it);
            }
            
            // The last row MUST go here. If row is NULL, there is no items
            if (NULL != row)
            {
                m_rows.push_back(row);
            }
        }

        height(TBaseSection::m_itemsFrame, bottom(m_rows.back()->getFrame()));
        height(TBaseSection::m_frame, bottom(TBaseSection::m_itemsFrame));
    }
    
    bool filterItemsInRect(const Rect &rectInSection, std::vector<const FlexItem *> &items) const
    {
        // As iterator will be changed once array is re-allocated on inserting, we use size to check if there is change
        typename std::vector<const FlexItem *>::size_type orgSize = items.size();
        
        std::pair<FlexRowConstIterator, FlexRowConstIterator> range = std::equal_range(m_rows.begin(), m_rows.end(), std::pair<TCoordinate, TCoordinate>(top(rectInSection), bottom(rectInSection)), RowCompare());
        
        FlexRowConstIterator lastRow = range.second - 1;
        for (FlexRowConstIterator it = range.first; it != range.second; ++it)
        {
            std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> itemRange = (*it)->getItemIterator();
            if ((it != range.first && it != lastRow))
            {
                items.insert(items.end(), itemRange.first, itemRange.second);
                continue;
            }
            for (typename std::vector<FlexItem *>::iterator itItem = itemRange.first; itItem != itemRange.second; ++itItem)
            {
                if ((*itItem)->getFrame().intersects(rectInSection))
                {
                    items.push_back(*itItem);
                }
            }
        }
        
        return orgSize != items.size();
    }

    
};

} // namespace nsflex

#endif /* FlowSection_h */
