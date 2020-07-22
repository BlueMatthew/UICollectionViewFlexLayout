//
//  Section.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef Section_h
#define Section_h

#include <vector>
#include <map>
#include <algorithm>

#include "FlexItem.h"
#include "ContainerBase.h"

/*
// Required interfaces for TLayout!!!
class TLayoutImpl
{
public:

 TInt getNumberOfItemsInSection(TInt section) const;
 SizeT<TCoordinate> getSizeForItem(TInt section, TInt item, bool *isFullSpan) const;
 InsetsT<TCoordinate> getInsetForSection(TInt section) const;
 TCoordinate getMinimumLineSpacingForSection(TInt section) const;
 TCoordinate getMinimumInteritemSpacingForSection(TInt section) const;
 SizeT<TCoordinate> getSizeForHeaderInSection(TInt section) const;
 SizeT<TCoordinate> getSizeForFooterInSection(TInt section) const;
 TInt getNumberOfColumnsForSection(TInt section) const;
 bool hasFixedItemSize(TInt section, SizeT<TCoordinate> *fixedItemSize) const;

};
*/

namespace nsflex
{
    template<class TLayout, class TInt, class TCoordinate, bool VERTICAL>
    class FlexSectionT : public ContainerBaseT<TCoordinate, VERTICAL>
    {
    public:

        using TBase = ContainerBaseT<TCoordinate, VERTICAL>;
        using LayoutType = TLayout;
        using IntType = TInt;
        using CoordinateType = TCoordinate;
        using FlexItem = FlexItemT<TInt, TCoordinate>;

        using Point = PointT<TCoordinate>;
        using Size = SizeT<TCoordinate>;
        using Rect = RectT<TCoordinate>;
        using Insets = InsetsT<TCoordinate>;

        // using TBase::x;
        using TBase::y;
        // using TBase::left;
        // using TBase::top;
        // using TBase::right;
        using TBase::bottom;

        // using TBase::offset;
        // using TBase::offsetX;
        // using TBase::offsetY;
        // using TBase::incWidth;

        using TBase::leftBottom;
        using TBase::height;
        using TBase::width;

        // using TBase::hinsets;
        // using TBase::vinsets;

    protected:
        TInt m_section;
        Rect m_frame; // // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView
        Rect m_itemsFrame;

    private:
        // FlexSectionT implements all required methods and sub-class doesn't need to access this member variable directly
        // So set it private


    protected:
        struct {
            TInt sectionInvalidated : 1;    // The whole section is invalidated
            TInt headerInvalidated : 1;
            TInt itemsInvalidated : 1; // Some of items are invalidated
            TInt footerInvalidated : 1;
            TInt reserved : 4;
            TInt minimalInvalidatedItem : sizeof(TInt) * 8 - 8;   // If minimal invalidated item is greater than 2^24, just set sectionInvalidated to 1
        } m_invalidationContext;

#ifdef HAVING_HEADER_AND_FOOTER
        FlexItem m_header;
#endif // #ifdef HAVING_HEADER_AND_FOOTER
        std::vector<FlexItem *> m_items;
#ifdef HAVING_HEADER_AND_FOOTER
        FlexItem m_footer;
#endif // #ifdef HAVING_HEADER_AND_FOOTER

    public:
        FlexSectionT(TInt section, const Rect& frame) : ContainerBaseT<TCoordinate, VERTICAL>(), m_section(section), m_frame(frame)
        {
#ifdef HAVING_HEADER_AND_FOOTER
            m_header.setHeader(true);
            m_footer.setFooter(true);
#endif // #ifdef HAVING_HEADER_AND_FOOTER
        }

        virtual ~FlexSectionT()
        {
            clearItems();
        }

        FlexItem *getItem(TInt itemIndex) const
        {
            return (itemIndex < m_items.size()) ? m_items[itemIndex] : NULL;
        }

        inline void clearItems()
        {
            for(typename std::vector<FlexItem *>::iterator it = m_items.begin(); it != m_items.end(); delete *it, ++it);
            m_items.clear();
        }

        inline TInt getSection() const { return m_section; }
        inline void setSection(TInt section) { m_section = section; }
        inline Rect getFrame() const { return m_frame; }
        inline Rect &getFrame() { return m_frame; }
        inline TInt getItemCount() const { return m_items.size(); }

        inline Rect getItemFrameInView(TInt item) const
        {
#ifdef DEBUG
            assert(item < m_items.size());
#endif // DEBUG
            return getFrameInView(m_items[item]->getFrame());
        }

#ifdef HAVING_HEADER_AND_FOOTER
        inline Rect getHeaderFrameInView() const
        {
            return getFrameInView(m_header.getFrame());
        }

        inline Rect getFooterFrameInView() const
        {
            return getFrameInView(m_footer.getFrame());
        }
#endif // #ifdef HAVING_HEADER_AND_FOOTER

        inline Rect getItemsFrame() const
        {
            return m_itemsFrame;
        }
        
        inline Rect getItemsFrameInView() const
        {
            return getFrameInView(m_itemsFrame);
        }

        inline const Rect getItemsFrameInViewAfterItem(int itemIndex) const
        {
            FlexItem *item = m_items[itemIndex];
            FlexItem *itemLast = m_items.back();

            Rect rect(item->getFrame().left(), item->getFrame().bottom(), (*itemLast).getFrame().width(), (*itemLast).getFrame().bottom() - item->getFrame().bottom());
            rect.offset(m_frame.left(), m_frame.top());
            return rect;
        }

        void prepareLayout(const TLayout *layout, const Rect &bounds)
        {
#ifdef HAVING_HEADER_AND_FOOTER
            // Header
            m_header.getFrame().size = getSizeForHeader(layout);

            // Initialize the section height with header height
            height(m_frame, height(m_header.getFrame()));
            m_itemsFrame.origin = leftBottom(m_frame);
            width(m_itemsFrame, width(m_frame));
#endif // #ifdef HAVING_HEADER_AND_FOOTER

            Point pt = prepareLayoutWithItems(layout, bounds);
            height(m_itemsFrame, y(pt) - height(m_frame));
            height(m_frame, y(pt));


#ifdef HAVING_HEADER_AND_FOOTER
            // Footer
            m_footer.getFrame().origin = pt;
            m_footer.getFrame().size = getSizeForFooter(layout);

            height(m_frame, bottom(m_footer.getFrame()));
            height(m_itemsFrame, height(m_frame) - (height(m_header.getFrame()) + height(m_footer.getFrame())));
#endif // #ifdef HAVING_HEADER_AND_FOOTER
        }

        bool filterInRect(std::vector<const FlexItem *> &items, const Rect &rect) const
        {
            bool matched = false;

            Rect rectInSection = Rect::intersectRects(m_frame, rect);
            if (rectInSection.empty())
            {
                return matched;
            }

            // Convert to coodinate of section
            rectInSection.offset( -m_frame.origin.x, -m_frame.origin.y);

#ifdef HAVING_HEADER_AND_FOOTER
            // Header
            if (!m_header.getFrame().empty() && rectInSection.intersects(m_header.getFrame()))
            {
                items.push_back(&m_header);
                matched = true;
            }
#endif // #ifdef HAVING_HEADER_AND_FOOTER

            // Items
            if (filterItemsInRect(rectInSection, items))
            {
                matched = true;
            }

#ifdef HAVING_HEADER_AND_FOOTER
            // Footer
            if (!m_footer.getFrame().size.empty() && m_footer.getFrame().intersects(rectInSection))
            {
                items.push_back(&m_footer);
                matched = true;
            }
#endif // #ifdef HAVING_HEADER_AND_FOOTER

            return matched;
        }

        void resetInvalidationContext()
        {
            unsigned int value = ~0;
            *((unsigned int *)&m_invalidationContext) = (value >> 8);
        }

        bool isSectionInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
#ifdef HAVING_HEADER_AND_FOOTER
        bool isHeaderInvalidated() const { return m_invalidationContext.headerInvalidated == 1; }
        bool isFooterInvalidated() const { return m_invalidationContext.footerInvalidated == 1; }
#endif // #ifdef HAVING_HEADER_AND_FOOTER
        bool hasInvalidatedItem() const { return m_invalidationContext.itemsInvalidated == 1; }

        unsigned int getMinimalInvalidatedItem() const { return m_invalidationContext.minimalInvalidatedItem; }
        inline void invalidateSection()
        {
            m_invalidationContext.sectionInvalidated = 1;
        }

#ifdef HAVING_HEADER_AND_FOOTER
        inline void invalidateHeader()
        {
            m_invalidationContext.headerInvalidated = 1;
        }

        void invalidateFooter()
        {
            m_invalidationContext.footerInvalidated = 1;
        }
#endif // #ifdef HAVING_HEADER_AND_FOOTER

        void invalidateItem(TInt item)
        {
            m_invalidationContext.sectionInvalidated = 1;
            unsigned int maximum = ~0;
            maximum = (maximum >> 8);
            if (item > (TInt)maximum)
            {
                m_invalidationContext.sectionInvalidated = 1;
                m_invalidationContext.minimalInvalidatedItem = maximum;
            }
            else
            {
                m_invalidationContext.minimalInvalidatedItem = (unsigned int)item;
            }
        }

    protected:

        virtual Point prepareLayoutWithItems(const TLayout *layout, const Rect &bounds) = 0;
        virtual bool filterItemsInRect(const Rect &rectInSection, std::vector<const FlexItem *> &items) const = 0;

        inline Rect getFrameInView(const Rect& rect) const
        {
            Rect rectInView(rect);
            rectInView.offset(m_frame.origin.x, m_frame.origin.y);
            return rectInView;
        }

        // Layout Adapter Functions Begin
        inline TInt getNumberOfItems(const TLayout *layout) const
        {
            return layout->getNumberOfItemsInSection(m_section);
        }

        inline Size getSizeForItem(const TLayout *layout, TInt item, bool *isFullSpan) const
        {
            return layout->getSizeForItem(m_section, item, isFullSpan);
        }

        inline Insets getInsets(const TLayout *layout) const
        {
            return layout->getInsetForSection(m_section);
        }

        inline TCoordinate getMinimumLineSpacing(const TLayout *layout) const
        {
            return layout->getMinimumLineSpacingForSection(m_section);
        }

        inline TCoordinate getMinimumInteritemSpacing(const TLayout *layout) const
        {
            return layout->getMinimumInteritemSpacingForSection(m_section);
        }

#ifdef HAVING_HEADER_AND_FOOTER
        inline Size getSizeForHeader(const TLayout *layout) const
        {
            return layout->getSizeForHeaderInSection(m_section);
        }

        inline Size getSizeForFooter(const TLayout *layout) const
        {
            return layout->getSizeForFooterInSection(m_section);
        }
#endif // #ifdef HAVING_HEADER_AND_FOOTER

        inline TInt getNumberOfColumns(const TLayout *layout) const
        {
            return layout->getNumberOfColumnsForSection(m_section);
        }

        inline bool isFullSpanAtItem(const TLayout *layout, TInt item) const
        {
            return layout->isFullSpanAtItem(m_section, item);
        }

        inline bool hasFixedSize(const TLayout *layout, TInt section, Size *fixedSize) const
        {
            return layout->hasFixedSize(m_section, fixedSize);
        }
        // Layout Adapter Functions End

    };

} // namespace nsflex
    
#endif /* Section_h */
