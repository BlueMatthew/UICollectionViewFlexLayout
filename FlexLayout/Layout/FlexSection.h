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

#import "FlexItem.h"

namespace nsflex
{

/*
// Required interfaces for TLayout!!!
class TLayoutImpl
{
public:

 bool isVertical() const;
 TInt m_layout->getNumberOfItemsInSection(TInt section) const;
 SizeT<TCoordinate> m_layout->getSizeForItem(TInt section, TInt item) const;
 InsetsT<TCoordinate> getInsetForSection(TInt section) const;
 TCoordinate getMinimumLineSpacingForSection(TInt section) const;
 TCoordinate getMinimumInteritemSpacingForSection(TInt section) const;
 SizeT<TCoordinate> getSizeForHeaderInSection(TInt section) const;
 inline SizeT<TCoordinate> getSizeForFooterInSection(TInt section) const;
 TInt getNumberOfColumnsForSection(TInt section) const;
 bool isFullSpanAtItem(TInt section, TInt item) const ;
 bool hasFixedSize(TInt section, SizeT<TCoordinate> *fixedSize);

};
*/

template<class TLayout, class TInt, class TCoordinate>
class FlexSectionT
{
public:
    typedef TCoordinate CoordinateType;
    typedef FlexItemT<TInt, TCoordinate> FlexItem;
    typedef PointT<TCoordinate> Point;
    typedef SizeT<TCoordinate> Size;
    typedef RectT<TCoordinate> Rect;
    typedef InsetsT<TCoordinate> Insets;
    
protected:
    TInt m_section;
    Rect m_frame; // // The origin is in the coordinate system of section, should convert to the coordinate system of UICollectionView

private:
    // FlexSectionT implements all required methods and sub-class doesn't need to access this member variable directly
    // So set it private
    TLayout *m_layout;

protected:
    struct {
        unsigned int sectionInvalidated : 1;    // The whole section is invalidated
        unsigned int headerInvalidated : 1;
        unsigned int itemsInvalidated : 1; // Some of items are invalidated
        unsigned int footerInvalidated : 1;
        unsigned int reserved : 4;
        unsigned int minimalInvalidatedItem : 24;   // If minimal invalidated item is greater than 2^28, just set sectionInvalidated to 1
    } m_invalidationContext;

    FlexItem m_header;
    std::vector<FlexItem *> m_items;
    FlexItem m_footer;

public:
    FlexSectionT(TLayout *layout, TInt section, const Rect& frame) : m_section(section), m_layout(layout), m_frame(frame), m_header(0), m_footer(0)
    {
        m_header.setHeader(true);
        m_footer.setFooter(true);
    }
    
    virtual ~FlexSectionT()
    {
        m_layout = NULL;
        clearItems();
    }
    
    inline void clearItems()
    {
        for(typename std::vector<FlexItem *>::iterator it = m_items.begin(); it != m_items.end(); delete *it, ++it);
        m_items.clear();
    }
    
    inline TInt getSection() const { return m_section; }
    inline const Rect getFrame() const { return m_frame; }
    inline Rect &getFrame() { return m_frame; }
    inline TInt getItemCount() const { return m_items.size(); }
    
    inline const Rect getItemFrameInView(TInt item) const
    {
#ifdef DEBUG
        assert(item < m_items.size());
#endif // DEBUG
        return getFrameInView(m_items[item]->getFrame());
    }
    
    inline const Rect getHeaderFrameInView() const
    {
        return getFrameInView(m_header.getFrame());
    }
    
    inline const Rect getFooterFrameInView() const
    {
        return getFrameInView(m_footer.getFrame());
    }
    
    inline const Rect getItemsFrame() const
    {
        return isVertical() ? Rect(m_frame.origin.x, m_frame.origin.y + m_header.getFrame().size.height, m_frame.size.width, m_frame.size.height - m_header.getFrame().size.height - m_footer.getFrame().size.height) : Rect(m_frame.origin.x + m_header.getFrame().origin.x, m_frame.origin.y, m_frame.size.width - m_header.getFrame().size.width - m_footer.getFrame().size.width, m_frame.size.height);
    }

    void prepareLayout(const Rect &bounds)
    {
        isVertical() ? prepareLayoutVertically(bounds) : prepareLayoutHorizontally(bounds);
    }
    
    bool filterInRect(std::vector<FlexItem *> &items, const Rect &rect)
    {
        bool matched = false;
        
        Rect rectInSection = Rect::intersectRects(m_frame, rect);
        if (rectInSection.empty())
        {
            return matched;
        }
        
        // Convert to coodinate of section
        rectInSection.offset( -m_frame.origin.x, -m_frame.origin.y);
        
        // Header
        if (!m_header.getFrame().empty() && rectInSection.intersects(m_header.getFrame()))
        {
            items.push_back(&m_header);
            matched = true;
        }
        
        // Items
        if (filterItemsInRect(items, rectInSection))
        {
            matched = true;
        }
        
        // Footer
        if (!m_footer.getFrame().size.empty() && m_footer.getFrame().intersects(rectInSection))
        {
            items.push_back(&m_footer);
            matched = true;
        }
        
        return matched;
    }
    
    void resetInvalidationContext()
    {
        unsigned int value = ~0;
        *((unsigned int *)&m_invalidationContext) = (value >> 8);
    }
    
    bool isSectionInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
    bool isHeaderInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
    bool hasInvalidatedItem() const { return m_invalidationContext.itemsInvalidated == 1; }
    bool isFooterInvalidated() const { return m_invalidationContext.sectionInvalidated == 1; }
    unsigned int getMinimalInvalidatedItem() const { return m_invalidationContext.minimalInvalidatedItem; }
    inline void invalidateSection()
    {
        m_invalidationContext.sectionInvalidated = 1;
    }
    
    inline void invalidateHeader()
    {
        m_invalidationContext.headerInvalidated = 1;
    }
    
    void invalidateFooter()
    {
        m_invalidationContext.footerInvalidated = 1;
    }
    
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
    
    inline void prepareLayoutVertically(const Rect &bounds)
    {
#define INTERNAL_VERTICAL_LAYOUT
        
        // Header
        m_header.getFrame().size = getSizeForHeader();
        
        // Initialize the section height with header height
#ifdef INTERNAL_VERTICAL_LAYOUT
        m_frame.size.height = m_header.getFrame().height();
#else
        m_frame.size.width = m_header.getFrame().width();
#endif // ifdef INTERNAL_VERTICAL_LAYOUT

        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Point pt = prepareLayoutWithItemsVertically(bounds);
#else
        Point pt = prepareLayoutWithItemsHorizontally(bounds);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT

        // Footer
        m_footer.getFrame().origin = pt;
        m_footer.getFrame().size = getSizeForFooter();

#ifdef INTERNAL_VERTICAL_LAYOUT
        m_frame.size.height = m_footer.getFrame().bottom();
#else
        m_frame.size.width = m_footer.getFrame().right();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT

#undef INTERNAL_VERTICAL_LAYOUT
        
    }
    
    /// DON"T EDIT THE CODE DIRECTLY
    /// Update the code in the function of "vertically" first and then sync the commented code of "horizontally" parts
    inline void prepareLayoutHorizontally(const Rect &bounds)
    {
#undef INTERNAL_VERTICAL_LAYOUT

        // Header
        m_header.getFrame().size = getSizeForHeader();
        
        // Initialize the section height with header height
#ifdef INTERNAL_VERTICAL_LAYOUT
        m_frame.size.height = m_header.getFrame().height();
#else
        m_frame.size.width = m_header.getFrame().width();
#endif // ifdef INTERNAL_VERTICAL_LAYOUT
        
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        Point pt = prepareLayoutWithItemsVertically(bounds);
#else
        Point pt = prepareLayoutWithItemsHorizontally(bounds);
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        
        // Footer
        m_footer.getFrame().origin = pt;
        m_footer.getFrame().size = getSizeForFooter();
        
#ifdef INTERNAL_VERTICAL_LAYOUT
        m_frame.size.height = m_footer.getFrame().bottom();
#else
        m_frame.size.width = m_footer.getFrame().right();
#endif // #ifdef INTERNAL_VERTICAL_LAYOUT
        

#undef INTERNAL_VERTICAL_LAYOUT
    }
    
    virtual Point prepareLayoutWithItemsVertically(const Rect &bounds) = 0;
    virtual Point prepareLayoutWithItemsHorizontally(const Rect &bounds) = 0;
    
    virtual bool filterItemsInRect(std::vector<FlexItem *> &items, const Rect &rectInSection) = 0;
    
    inline const Rect getFrameInView(const Rect& rect) const
    {
        Rect rectInView = rect;
        rectInView.offset(m_frame.origin.x, m_frame.origin.y);
        return rectInView;
    }
    
    
    // Layout Adapter Functions Begin
    inline bool isVertical() const { return m_layout->isVertical(); }
    
    inline TInt getNumberOfItems() const
    {
        return m_layout->getNumberOfItemsInSection(m_section);
    }
    
    inline Size getSizeForItem(TInt item) const
    {
        return m_layout->getSizeForItem(m_section, item);
    }
    
    inline Insets getInsets() const
    {
        return m_layout->getInsetForSection(m_section);
    }
    
    inline TCoordinate getMinimumLineSpacing() const
    {
        return m_layout->getMinimumLineSpacingForSection(m_section);
    }
    
    inline TCoordinate getMinimumInteritemSpacing() const
    {
        return m_layout->getMinimumInteritemSpacingForSection(m_section);
    }
    
    inline Size getSizeForHeader() const
    {
        return m_layout->getSizeForHeaderInSection(m_section);
    }
    
    inline Size getSizeForFooter() const
    {
        return m_layout->getSizeForFooterInSection(m_section);
    }
    
    inline TInt getNumberOfColumns() const
    {
        return m_layout->getNumberOfColumnsForSection(m_section);
    }
    
    inline bool isFullSpanAtItem(TInt item) const
    {
        return m_layout->isFullSpanAtItem(m_section, item);
    }
    
    inline bool hasFixedSize(TInt section, Size *fixedSize) const
    {
        return m_layout->hasFixedSize(m_section, fixedSize);
    }
    // Layout Adapter Functions End
    
};

} // namespace nsflex
    
#endif /* Section_h */
