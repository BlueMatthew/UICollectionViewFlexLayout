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

    typedef typename TBaseSection::LayoutType TLayout;
    typedef typename TBaseSection::IntType TInt;
    typedef typename TBaseSection::CoordinateType TCoordinate;
    
    typedef typename TBaseSection::FlexItem FlexItem;
    typedef FlexRowT<TInt, TCoordinate, VERTICAL> FlexRow;
    typedef typename std::vector<FlexRow *>::const_iterator FlexRowConstIterator;
    typedef FlexVerticalCompareT<FlexRow> UIFlexRowVerticalCompare;
    typedef FlexHorizontalCompareT<FlexRow> UIFlexRowHorizontalCompare;
    
    typedef PointT<TCoordinate> Point;
    typedef SizeT<TCoordinate> Size;
    typedef RectT<TCoordinate> Rect;
    typedef InsetsT<TCoordinate> Insets;
    
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

    Point prepareLayoutWithItemsVertically(const TLayout *layout, const Rect &bounds)
    {
#define INTERNAL_VERTICAL_LAYOUT

        // Items
        clearRows();
        TBaseSection::clearItems();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Point originOfRow = leftBottom(TBaseSection::m_header.getFrame());
#else
        Point originOfRow(TBaseSection::m_header.getFrame().right(), TBaseSection::m_header.getFrame().top());
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        TInt numberOfItems = TBaseSection::getNumberOfItems(layout);
        if (numberOfItems == 0)
        {
            return originOfRow;
        }
        
        Insets sectionInset = TBaseSection::getInsets(layout);
        
        TBaseSection::m_items.reserve(numberOfItems);
        
        // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
        TInt numberOfColumns = TBaseSection::getNumberOfColumns(layout);
        m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);
        
        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing(layout);
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing(layout);
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        TCoordinate maximalSizeOfRow = width(TBaseSection::m_frame) - leftRight(sectionInset);
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
            frameOfItem.size = TBaseSection::getSizeForItem(layout, itemIndex, NULL);
            
#ifdef INTERNAL_VERTICAL_LAYOUT
            sizeOfItemInDirection = width(frameOfItem);
#else
            sizeOfItemInDirection = frameOfItem.height();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
            
            if (NULL != row)
            {
                if (row->hasItems())
                {
#ifdef INTERNAL_VERTICAL_LAYOUT
                    availableSizeOfRow = maximalSizeOfRow - width(row->getFrame()) - minimumInteritemSpacing;
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
                left(frameOfItem, right(row->getFrame()) + minimumInteritemSpacing);
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
            offsetY(originOfRow, height(row->getFrame()));
#else
            originOfRow.x += row->getFrame().width();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        }
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        offsetY(originOfRow, bottom(sectionInset));
#else
        originOfRow.x += sectionInset.right;
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        return originOfRow;

#undef INTERNAL_VERTICAL_LAYOUT
    }
    
    Point prepareLayoutWithItemsHorizontally(const TLayout *layout, const Rect &bounds)
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

        TInt numberOfItems = TBaseSection::getNumberOfItems(layout);
        if (numberOfItems == 0)
        {
            return originOfRow;
        }

        Insets sectionInset = TBaseSection::getInsets(layout);

        TBaseSection::m_items.reserve(numberOfItems);

        // For FlowLayout, there is no column property but we still try to get the number of columns, and use it to estimate the number of rows
        TInt numberOfColumns = TBaseSection::getNumberOfColumns(layout);
        m_rows.reserve(numberOfColumns > 0 ? ceil(numberOfItems / numberOfColumns) : numberOfItems);

        TCoordinate minimumLineSpacing = TBaseSection::getMinimumLineSpacing(layout);
        TCoordinate minimumInteritemSpacing = TBaseSection::getMinimumInteritemSpacing(layout);

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
            frameOfItem.size = TBaseSection::getSizeForItem(layout, itemIndex, NULL);

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
    
    bool filterItemsInRect(bool vertical, const Rect &rectInSection, std::vector<const FlexItem *> &items) const
    {
        bool matched = false;
        
        std::pair<FlexRowConstIterator, FlexRowConstIterator> range = vertical ? getVirticalRowsInRect(rectInSection) : getHorizontalRowsInRect(rectInSection);
        
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
