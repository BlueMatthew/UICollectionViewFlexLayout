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
    using FlexRow = FlexRowT<TInt, TCoordinate, VERTICAL>;
    using FlexRowConstIterator = typename std::vector<FlexRow *>::const_iterator;

    using RowCompare = FlexCompareT<FlexRow, VERTICAL>;

    using TBase::x;
    using TBase::y;
    using TBase::left;
    using TBase::top;
    using TBase::right;
    using TBase::bottom;

    using TBase::offset;
    using TBase::offsetX;
    using TBase::offsetY;
    using TBase::incWidth;

    using TBase::leftBottom;
    using TBase::height;
    using TBase::width;

    using TBase::leftRight;
    using TBase::topBottom;
    using TBase::makePoint;


public:
    std::vector<FlexRow *> m_rows;
    
    FlexFlowSectionT(TInt section, const Rect &frame) : TBaseSection(section, frame)
    {
    }
    
    virtual ~FlexFlowSectionT()
    {
        clearRows();
    }
    
protected:
    inline void clearRows()
    {
        for(typename std::vector<FlexRow *>::iterator it = m_rows.begin(); it != m_rows.end(); delete *it, ++it);
        m_rows.clear();
    }

    Point prepareLayoutWithItems(const TLayout *layout, const Rect &bounds)
    {
        // Items
        clearRows();
        TBaseSection::clearItems();
        
        Point pt = makePoint(0, height(TBaseSection::m_frame));
        
        TInt numberOfItems = TBaseSection::getNumberOfItems(layout);
        if (numberOfItems == 0)
        {
            return pt;
        }
        TBaseSection::m_items.reserve(numberOfItems);
        
        Insets sectionInset = TBaseSection::getInsets(layout);
        
        // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
        TInt numberOfColumns = TBaseSection::getNumberOfColumns(layout);
        m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing(layout);
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing(layout);
        
        TCoordinate maximalSizeOfRow = width(bounds) - leftRight(sectionInset);

        // Layout items
        FlexItem *sectionItem = NULL;
        FlexRow *row = NULL;
        
        Point originOfRow(pt);
        offset(originOfRow, left(sectionInset), top(sectionInset));
        Rect frameOfItem(originOfRow.x, originOfRow.y, 0, 0);
        TCoordinate availableSizeOfRow = 0;
        TCoordinate sizeOfItemInDirection = 0;
        
        for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
        {
            frameOfItem.size = TBaseSection::getSizeForItem(layout, itemIndex, NULL);
            
            sizeOfItemInDirection = width(frameOfItem);

            if (NULL != row)
            {
                if (row->hasItems())
                {
                    availableSizeOfRow = maximalSizeOfRow - width(row->getFrame()) - minimumInteritemSpacing;

                    if (availableSizeOfRow < sizeOfItemInDirection)
                    {
                        // New Line
                        m_rows.push_back(row);
                        offsetY(originOfRow, minimumLineSpacing + height(row->getFrame()));
                        row = NULL;
                    }
                }
            }
            
            if (NULL == row)
            {
                row = new FlexRow();
                row->getFrame().origin = originOfRow;
                
                frameOfItem.origin = originOfRow;
            }
            
            if (row->hasItems())
            {
                left(frameOfItem, right(row->getFrame()) + minimumInteritemSpacing);
            }
            
            sectionItem = new FlexItem(itemIndex, frameOfItem);
            
            TBaseSection::m_items.push_back(sectionItem);
            row->addItem(sectionItem);
        }
        
        // The last row
        if (NULL != row)
        {
            m_rows.push_back(row);
            
            offsetY(originOfRow, height(row->getFrame()));
        }
        
        offsetY(originOfRow, bottom(sectionInset));
        y(pt, y(originOfRow));

        return pt;
    }
    

    bool filterItemsInRect(const Rect &rectInSection, std::vector<const FlexItem *> &items) const
    {
        bool matched = false;
        
        std::pair<FlexRowConstIterator, FlexRowConstIterator> range = std::equal_range(m_rows.begin(), m_rows.end(), std::pair<TCoordinate, TCoordinate>(top(rectInSection), bottom(rectInSection)), RowCompare());
        
        FlexRowConstIterator lastRow = range.second - 1;
        for (FlexRowConstIterator it = range.first; it != range.second; ++it)
        {
            std::pair<typename std::vector<FlexItem *>::iterator, typename std::vector<FlexItem *>::iterator> itemRange = (*it)->getItemIterator();
            for (typename std::vector<FlexItem *>::iterator itItem = itemRange.first; itItem != itemRange.second; ++itItem)
            {
                if ((it != range.first && it != lastRow) || (*itItem)->getFrame().intersects(rectInSection))
                {
                    items.push_back(*itItem);
                    matched = true;
                }
            }
        }
        
        return matched;
    }

    
};

} // namespace nsflex

#endif /* FlowSection_h */
