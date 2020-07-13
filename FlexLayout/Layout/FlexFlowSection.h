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
    
template<class TBaseSection>
class FlexFlowSectionT : public TBaseSection
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
    typedef FlexRowT<TInt, TCoordinate> FlexRow;
    typedef typename std::vector<FlexRow *>::const_iterator FlexRowConstIterator;
    typedef FlexVerticalCompareT<FlexRow> UIFlexRowVerticalCompare;
    typedef FlexHorizontalCompareT<FlexRow> UIFlexRowHorizontalCompare;

public:
    std::vector<FlexRow *> m_rows;
    
    FlexFlowSectionT(TLayout *layout, TInt section, const Rect &frame) : TBaseSection(layout, section, frame)
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

    Point prepareLayoutWithItemsVertically(const Rect &bounds)
    {
#define INTERNAL_VERTICAL_LAYOUT

        // Items
        clearRows();
        TBaseSection::clearItems();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Point originOfRow(TBaseSection::m_header.getFrame().left(), TBaseSection::m_header.getFrame().bottom());
#else
        Point originOfRow(TBaseSection::m_header.getFrame().right(), TBaseSection::m_header.getFrame().top());
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        TInt numberOfItems = TBaseSection::getNumberOfItems();
        if (numberOfItems == 0)
        {
            return originOfRow;
        }
        
        Insets sectionInset = TBaseSection::getInsets();
        
        TBaseSection::m_items.reserve(numberOfItems);
        
        // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
        TInt numberOfColumns = TBaseSection::getNumberOfColumns();
        m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing();
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        TCoordinate maximalSizeOfRow = TBaseSection::m_frame.width() - sectionInset.left - sectionInset.right;
#else
        TCoordinate maximalSizeOfRow = TBaseSection::m_frame.height() - sectionInset.top - sectionInset.bottom;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        // Layout items
        FlexItem *sectionItem = NULL;
        FlexRow *row = NULL;
        
        Rect frameOfItem(originOfRow.x, originOfRow.y, 0, 0);
        TCoordinate availableSizeOfRow = 0.0f;
        TCoordinate sizeOfItemInDirection = 0.0f;
        
        originOfRow.x += sectionInset.left;
        originOfRow.y += sectionInset.top;
        
        for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
        {
            frameOfItem.size = TBaseSection::getSizeForItem(itemIndex, NULL);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            sizeOfItemInDirection = frameOfItem.width();
#else
            sizeOfItemInDirection = frameOfItem.height();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            if (NULL != row)
            {
                if (row->hasItems())
                {
#ifdef INTERNAL_VERTICAL_LAYOUT
                    availableSizeOfRow = maximalSizeOfRow - row->getFrame().width() - minimumInteritemSpacing;
#else
                    availableSizeOfRow = maximalSizeOfRow - row->getFrame().height() - minimumInteritemSpacing;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
                    
                    if (availableSizeOfRow < sizeOfItemInDirection)
                    {
                        // New Line
                        m_rows.push_back(row);
#ifdef INTERNAL_VERTICAL_LAYOUT
                        originOfRow.y += minimumLineSpacing + row->getFrame().height();
#else
                        originOfRow.x += minimumLineSpacing + row->getFrame().width();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
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
#ifdef INTERNAL_VERTICAL_LAYOUT
                frameOfItem.origin.x = row->getFrame().right() + minimumInteritemSpacing;
#else
                frameOfItem.origin.y = row->getFrame().bottom() + minimumInteritemSpacing;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            }
            
            sectionItem = new FlexItem(itemIndex, frameOfItem);
            
            TBaseSection::m_items.push_back(sectionItem);
#ifdef INTERNAL_VERTICAL_LAYOUT
            row->addItemVertically(sectionItem);
#else
            row->addItemHorizontally(sectionItem);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
        // The last row
        if (NULL != row)
        {
            m_rows.push_back(row);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            originOfRow.y += row->getFrame().height();
#else
            originOfRow.x += row->getFrame().width();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        originOfRow.y += sectionInset.bottom;
#else
        originOfRow.x += sectionInset.right;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        return originOfRow;

#undef INTERNAL_VERTICAL_LAYOUT
    }
    
    Point prepareLayoutWithItemsHorizontally(const Rect &bounds)
    {
#undef INTERNAL_VERTICAL_LAYOUT

        // Items
        clearRows();
        TBaseSection::clearItems();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Point originOfRow(TBaseSection::m_header.getFrame().left(), TBaseSection::m_header.getFrame().bottom());
#else
        Point originOfRow(TBaseSection::m_header.getFrame().right(), TBaseSection::m_header.getFrame().top());
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        TInt numberOfItems = TBaseSection::getNumberOfItems();
        if (numberOfItems == 0)
        {
            return originOfRow;
        }
        
        Insets sectionInset = TBaseSection::getInsets();
        
        TBaseSection::m_items.reserve(numberOfItems);
        
        // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
        TInt numberOfColumns = TBaseSection::getNumberOfColumns();
        m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing();
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        TCoordinate maximalSizeOfRow = TBaseSection::m_frame.width() - sectionInset.left - sectionInset.right;
#else
        TCoordinate maximalSizeOfRow = TBaseSection::m_frame.height() - sectionInset.top - sectionInset.bottom;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        // Layout items
        FlexItem *sectionItem = NULL;
        FlexRow *row = NULL;
        
        Rect frameOfItem(originOfRow.x, originOfRow.y, 0, 0);
        TCoordinate availableSizeOfRow = 0.0f;
        TCoordinate sizeOfItemInDirection = 0.0f;
        
        originOfRow.x += sectionInset.left;
        originOfRow.y += sectionInset.top;
        
        for (TInt itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
        {
            frameOfItem.size = TBaseSection::getSizeForItem(itemIndex, NULL);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            sizeOfItemInDirection = frameOfItem.width();
#else
            sizeOfItemInDirection = frameOfItem.height();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            if (NULL != row)
            {
                if (row->hasItems())
                {
#ifdef INTERNAL_VERTICAL_LAYOUT
                    availableSizeOfRow = maximalSizeOfRow - row->getFrame().width() - minimumInteritemSpacing;
#else
                    availableSizeOfRow = maximalSizeOfRow - row->getFrame().height() - minimumInteritemSpacing;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
                    
                    if (availableSizeOfRow < sizeOfItemInDirection)
                    {
                        // New Line
                        m_rows.push_back(row);
#ifdef INTERNAL_VERTICAL_LAYOUT
                        originOfRow.y += minimumLineSpacing + row->getFrame().height();
#else
                        originOfRow.x += minimumLineSpacing + row->getFrame().width();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
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
#ifdef INTERNAL_VERTICAL_LAYOUT
                frameOfItem.origin.x = row->getFrame().right() + minimumInteritemSpacing;
#else
                frameOfItem.origin.y = row->getFrame().bottom() + minimumInteritemSpacing;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            }
            
            sectionItem = new FlexItem(itemIndex, frameOfItem);
            
            TBaseSection::m_items.push_back(sectionItem);
#ifdef INTERNAL_VERTICAL_LAYOUT
            row->addItemVertically(sectionItem);
#else
            row->addItemHorizontally(sectionItem);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
        // The last row
        if (NULL != row)
        {
            m_rows.push_back(row);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            originOfRow.y += row->getFrame().height();
#else
            originOfRow.x += row->getFrame().width();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        originOfRow.y += sectionInset.bottom;
#else
        originOfRow.x += sectionInset.right;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        return originOfRow;

#undef INTERNAL_VERTICAL_LAYOUT
    }
    
    bool filterItemsInRect(std::vector<const FlexItem *> &items, const Rect &rectInSection) const
    {
        bool matched = false;
        
        std::pair<FlexRowConstIterator, FlexRowConstIterator> range = TBaseSection::isVertical() ? getVirticalRowsInRect(rectInSection) : getHorizontalRowsInRect(rectInSection);
        
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
    
    inline std::pair<FlexRowConstIterator, FlexRowConstIterator> getVirticalRowsInRect(const Rect& rect) const
    {
        return std::equal_range(m_rows.begin(), m_rows.end(), std::pair<TCoordinate, TCoordinate>(rect.top(), rect.bottom()), UIFlexRowVerticalCompare());
    }
    
    inline std::pair<FlexRowConstIterator, FlexRowConstIterator> getHorizontalRowsInRect(const Rect& rect) const
    {
        return std::equal_range(m_rows.begin(), m_rows.end(), std::pair<TCoordinate, TCoordinate>(rect.left(), rect.right()), UIFlexRowHorizontalCompare());
    }
    
    
};

} // namespace nsflex

#endif /* FlowSection_h */
