//
// Created by Matthew on 2020-07-03.
//

#ifndef FLEXLAYOUTMANAGER_FLEXLAYOUT_H
#define FLEXLAYOUTMANAGER_FLEXLAYOUT_H

#include <algorithm>
#include <cmath>
// #include "FlexLayoutObjects.h"

#include "FlexSection.h"
#include "FlexFlowSection.h"
#include "FlexWaterfallSection.h"
//# include "LayoutCallbackAdapter.h"

class LayoutCallbackAdapter;

#define INVALID_OFFSET INT_MIN

template<class TInt>
class SectionItemT
{
protected:
    TInt m_section;
    TInt m_item;
    
public:
    SectionItemT(TInt section, TInt item) : m_section(section), m_item(item)
    {
    }
    
    TInt getSection() const { return m_section; }
    TInt getItem() const { return m_item; }
    char getType() const { return nsflex::ITEM_TYPE_HEADER; }
    
    bool operator==(const SectionItemT &other) const
    {
        return (m_section == other.getSection() && m_item == other.getItem());
    }
    bool operator!=(const SectionItemT &other) const
    {
        return !(*this == other);
    }
    
    bool operator<(const SectionItemT &other) const
    {
        return m_section < other.getSection() || (m_section == other.getSection() && m_item < other.getItem());
    }
    
    bool operator>(const SectionItemT &other) const
    {
        return m_section > other.getSection() || (m_section == other.getSection() && m_item > other.getItem());
    }
    
};


template<class TCoordinate>
class StickyItemStateT
{
public:
    using Rect = nsflex::RectT<TCoordinate>;

protected:
    bool m_inSticky;
    bool m_originChanged;
    Rect m_itemsFrame;
    Rect m_frame;
    
public:
    StickyItemStateT() : m_inSticky(false), m_originChanged(false)
    {
    }
    
    StickyItemStateT(bool inSticky) : m_inSticky(inSticky), m_originChanged(false)
    {
    }
    
    inline bool isInSticky() const
    {
        return m_inSticky;
    }
    
    inline void setInSticky(bool inSticky = true)
    {
        m_inSticky = inSticky;
    }
    
    inline bool isOriginChanged() const
    {
        return m_originChanged;
    }
    
    inline void setOriginChanged(bool originChanged = true)
    {
        m_originChanged = originChanged;
    }
    
    inline Rect getFrame() const
    {
        return m_frame;
    }
    
    inline void setFrame(const Rect &frame)
    {
        m_frame = frame;
    }
    
    inline Rect getItemsFrame() const
    {
        return m_itemsFrame;
    }
    
    inline void setItemsFrame(const Rect &frame)
    {
        m_itemsFrame = frame;
    }
    
};

template<class TInt, class TCoordinate>
using StickyItemT = std::pair<SectionItemT<TInt>, StickyItemStateT<TCoordinate>>;

template<class TInt, class TCoordinate>
inline bool operator<(const StickyItemT<TInt, TCoordinate> &lhs, const StickyItemT<TInt, TCoordinate> &rhs)
{
    return lhs.first < rhs.first;
}

template<class TInt, class TCoordinate>
struct StickyItemAndSectionItemCompareT
{
    inline bool operator() (const StickyItemT<TInt, TCoordinate> &lhs, const SectionItemT<TInt> &rhs) const
    {
        return lhs.first < rhs;
        
    }
    inline bool operator() (const SectionItemT<TInt> &lhs, const StickyItemT<TInt, TCoordinate> &rhs) const
    {
        return lhs < rhs.first;
    }
};

template<class TInt, class TCoordinate>
class LayoutItemT : public nsflex::FlexItemT<TInt, TCoordinate>
{
public:
    using FlexItem = nsflex::FlexItemT<TInt, TCoordinate>;
    using Rect = nsflex::RectT<TCoordinate>;
    using FlexItem::getItem;
    
protected:
    TInt m_section;
    bool m_inSticky;
    bool m_originChanged;
    
public:
    LayoutItemT() : FlexItem(), m_section(0), m_inSticky(false), m_originChanged(false) {}
    LayoutItemT(TInt s, TInt i) : FlexItem(i), m_section(s), m_inSticky(false), m_originChanged(false) {}
    LayoutItemT(TInt s, TInt i,const Rect &f) : FlexItem(i, f), m_section(s), m_inSticky(false), m_originChanged(false) {}
    LayoutItemT(TInt section, const FlexItem &src) : FlexItem(src), m_section(section), m_inSticky(false), m_originChanged(false) {}
    LayoutItemT(const LayoutItemT &src) : FlexItem((const FlexItem &)src), m_section(src.m_section), m_inSticky(src.m_inSticky), m_originChanged(src.m_originChanged) {}
    LayoutItemT(const LayoutItemT *src) : LayoutItemT(*src) {}
    
    bool operator==(const LayoutItemT &other) const
    {
        if (this == &other) return true;
        // !!! Here MUST use FlexItem::getItem() to compare
        return m_section == other.getSection() && FlexItem::getItem() == other.FlexItem::getItem();
    }
    
    bool operator==(const LayoutItemT *other) const
    {
        return *this == *other;
    }
    
    bool operator!=(const LayoutItemT &other) const
    {
        return !(*this == other);
    }
    
    bool operator!=(const LayoutItemT *other) const
    {
        return !(*this == *other);
    }
    
    bool operator<(const LayoutItemT &other) const
    {
        // !!! Here MUST use FlexItem::getItem() to compare
        return (m_section < other.getSection()) || ((m_section == other.getSection()) && (FlexItem::getItem() < other.FlexItem::getItem()));
    }
    
    TInt getSection() const
    {
        return m_section;
    }
    
    TInt getItem() const
    {
        return FlexItem::isItem() ? FlexItem::getItem() : 0;
    }

    void setInSticky(bool inSticky)
    {
        m_inSticky = inSticky;
    }
    
    bool isInSticky() const
    {
        return m_inSticky;
    }
    
    void setOriginChanged(bool originChanged)
    {
        m_originChanged = originChanged;
    }
    
    bool isOriginChanged() const
    {
        return m_originChanged;
    }
};

template<class TInt, class TCoordinate>
struct LayoutStickyItemCompareT
{
    bool operator() ( const LayoutItemT<TInt, TCoordinate> &lhs, const StickyItemT<TInt, TCoordinate> &rhs) const
    {
        return (lhs.getSection() < rhs.first.getSection()) || ((lhs.getSection() == rhs.first.getSection()) && (lhs.getType() < lhs.getType())) || ((lhs.getSection() == rhs.first.getSection()) && (lhs.getType() == rhs.first.getType()) && (lhs.getItem() < rhs.first.getItem()));
    }
};

template<class TLayoutCallbackAdapter, class TInt, class TCoordinate, bool VERTICAL>
class FlexLayoutT : public nsflex::ContainerBaseT<TCoordinate, VERTICAL>
{
public:
    using TBase = nsflex::ContainerBaseT<TCoordinate, VERTICAL>;
    
    using StickyItem = StickyItemT<TInt, TCoordinate>;
    using LayoutItem = LayoutItemT<TInt, TCoordinate>;
    using StickyItemList = std::vector<StickyItem>;
    using LayoutStickyItemCompare = LayoutStickyItemCompareT<TInt, TCoordinate>;

    using Point = nsflex::PointT<TCoordinate>;
    using Size = nsflex::SizeT<TCoordinate>;
    using Rect = nsflex::RectT<TCoordinate>;
    using Insets = nsflex::InsetsT<TCoordinate>;
    
    using FlexItem = nsflex::FlexItemT<TInt, TCoordinate>;
    using Section = nsflex::FlexSectionT<TLayoutCallbackAdapter, TInt, TCoordinate, VERTICAL>;
    using FlowSection = typename nsflex::FlexFlowSectionT<Section, VERTICAL>;
    using WaterfallSection = typename nsflex::FlexWaterfallSectionT<Section, VERTICAL>;
    using SectionCompare = nsflex::FlexCompareT<Section, VERTICAL>;
    using SectionIterator = typename std::vector<Section *>::iterator;
    using SectionConstIterator = typename std::vector<Section *>::const_iterator;
    using SectionConstIteratorPair = std::pair<SectionConstIterator, SectionConstIterator>;
    // using SectionPositionCompare = FlexSectionPositionCompare<Section>;
    using ItemConstIterator = typename std::vector<const FlexItem *>::const_iterator;

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

    using TBase::hinsets;
    using TBase::vinsets;
    using TBase::makeSize;
    using TBase::makeRect;


protected:

    std::vector<Section *> m_sections;
    Size m_contentSize;

public:
    
    FlexLayoutT()
    {

    }
    ~FlexLayoutT()
    {
        clearSections();
    }

    void insertItem(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt sectionIndex, TInt itemIndex)
    {
        if (sectionIndex < 0 || sectionIndex >= m_sections.size())
        {
            return;
        }
        
        SectionIterator it = m_sections.begin() + sectionIndex;  // Previous Section
        (*it)->insertItem(itemIndex);
    }
    
    void insertSection(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt sectionIndex)
    {
        int mode = layoutCallbackAdapter.getLayoutModeForSection(sectionIndex);
        
        Rect frame;
        width(frame, width(boundSize));
        // Get the leftBottom(topRight for horizontal direction) of the previous section
        if (sectionIndex >= 1)
        {
            SectionConstIterator it = m_sections.cbegin() + (sectionIndex - 1);  // Previous Section
            top(frame, bottom((*it)->getFrame()));
        }
        
        Section *section = (mode == UICollectionViewFlexLayoutModeFlow) ? ((Section *)(new FlowSection(sectionIndex, frame))) : ((Section *)(new WaterfallSection(sectionIndex, frame)));
        
        m_sections.insert(m_sections.begin() + sectionIndex, section);
    }
    
    void deleteItem(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt sectionIndex, TInt itemIndex)
    {
        if (sectionIndex < 0 || sectionIndex >= m_sections.size())
        {
            return;
        }
        SectionIterator it = m_sections.begin() + sectionIndex;
        (*it)->deleteItem(itemIndex);
    }
    
    // Parameter "relayout" indicates wheather we should relayout the following sections immidiately
    // If we have multiple updates, it is better to layout after the last update.
    void deleteSection(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt sectionIndex)
    {
        if (sectionIndex < 0 || sectionIndex >= m_sections.size())
        {
            return;
        }
        typename std::vector<Section *>::iterator it = m_sections.begin() + sectionIndex;
        delete *it;
        m_sections.erase(it);
    }
    
    void reloadItem(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt sectionIndex, TInt itemIndex)
    {
        if (sectionIndex < 0 || sectionIndex >= m_sections.size())
        {
            return;
        }
        SectionIterator it = m_sections.begin() + sectionIndex;
        (*it)->reloadItem(itemIndex);
    }
    
    // Parameter "relayout" indicates wheather we should relayout the following sections immidiately
    // If we have multiple updates, it is better to layout after the last update.
    void reloadSection(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt sectionIndex)
    {
        if (sectionIndex < 0 || sectionIndex >= m_sections.size())
        {
            return;
        }
        SectionIterator it = m_sections.begin() + sectionIndex;
        (*it)->reloadSection();
    }

    void prepareLayout(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding);
    void prepareLayoutIncrementally(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt minInvalidatedSection);
    
    inline Size getContentSize() const
    {
        return m_contentSize;
    }

    void updateItems(TInt action, TInt itemStart, TInt itemCount)
    {
    }

    // LayoutItem::data == 1, indicates that the item is sticky
    void getItemsInRect(std::vector<LayoutItem> &items, StickyItemList &changingStickyItems, StickyItemList &stickyItems, bool stackedStickyItems, const Rect &rect, const Size &size,  const Size &contentSize, const Insets &padding, const Point &contentOffset) const;
    
    bool getItemFrame(TInt sectionIndex, TInt itemIndex, Rect &frame) const
    {
        if (sectionIndex >= m_sections.size())
        {
            return false;
        }
        Section *section = m_sections[sectionIndex];
        if (itemIndex >= section->getItemCount())
        {
            return false;
        }
        
        // layoutItem = *it;
        frame = section->getItemFrameInView(itemIndex);
        return true;
    }

    bool getHeaderFrame(TInt sectionIndex, Rect &frame) const
    {
        if (sectionIndex >= m_sections.size())
        {
            return false;
        }
        
        // layoutItem = *it;
        frame = m_sections[sectionIndex]->getHeaderFrameInView();
        return true;
    }
    
    bool getFooterFrame(TInt sectionIndex, Rect &frame) const
    {
        if (sectionIndex >= m_sections.size())
        {
            return false;
        }
        
        // layoutItem = *it;
        frame = m_sections[sectionIndex]->getFooterFrameInView();
        return true;
    }

    
protected:

    inline void clearSections()
    {
        for (typename std::vector<Section *>::iterator it = m_sections.begin(); it != m_sections.end(); delete *it, ++it);
        m_sections.clear();
    }

    LayoutItem *makeLayoutItem(TInt sectionIndex, TInt itemIndex) const
    {
        if (sectionIndex < m_sections.size())
        {
            Section *section = m_sections[sectionIndex];

            FlexItem *item = section->getItem(itemIndex);

            if (NULL != item)
            {
                Rect rect = item->getFrame();
                rect.offset(section->getFrame().left(), section->getFrame().top());

                return new LayoutItem(sectionIndex, itemIndex, rect);
            }
        }
        return NULL;
    }

#ifndef NDEBUG
    std::string printDebugInfo(std::string prefix) const
    {
        std::ostringstream str;


        int idx = 1;
        for (typename std::vector<Section *>::const_iterator it = m_sections.begin(); it != m_sections.end(); ++it)
        {
            str << prefix << "Section " << idx << "[" << (*it)->getFrame().left() << "," << (*it)->getFrame().top() << "-" << (*it)->getFrame().width() << "," << (*it)->getFrame().height() << "]\r\n";

            idx++;
        }

        return str.str();
    }
#endif

};

template<class TLayoutCallbackAdapter, class TInt, class TCoordinate, bool VERTICAL>
void FlexLayoutT<TLayoutCallbackAdapter, TInt, TCoordinate, VERTICAL>::prepareLayout(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding)
{
    // Clear all sections, maybe optimize later
    clearSections();
    
    TInt sectionCount = layoutCallbackAdapter.getNumberOfSections();
    if (sectionCount <= 0)
    {
        // Set contentSize to bound size
        m_contentSize = boundSize;;
        return;
    }
    
    // Initialize width and set height to 0, layout will calculate new height
    Rect rectOfSection = makeRect(0, 0, width(boundSize), 0);
    for (TInt sectionIndex = 0; sectionIndex < sectionCount; sectionIndex++)
    {
        int layoutMode = layoutCallbackAdapter.getLayoutModeForSection(sectionIndex);
        Section *section = layoutMode == 1 ?
        static_cast<Section *>(new WaterfallSection(sectionIndex, rectOfSection)) :
        static_cast<Section *>(new FlowSection(sectionIndex, rectOfSection));

        section->prepareLayout(&layoutCallbackAdapter, boundSize);
        
        m_sections.push_back(section);

        offsetY(rectOfSection, height(section->getFrame()));
    }
    
    m_contentSize = makeSize(width(boundSize), bottom(rectOfSection));
}

template<class TLayoutCallbackAdapter, class TInt, class TCoordinate, bool VERTICAL>
void FlexLayoutT<TLayoutCallbackAdapter, TInt, TCoordinate, VERTICAL>::prepareLayoutIncrementally(const TLayoutCallbackAdapter& layoutCallbackAdapter, const Size &boundSize, const Insets &padding, TInt minInvalidatedSection)
{
    TInt sectionCount = layoutCallbackAdapter.getNumberOfSections();
    if (sectionCount <= 0)
    {
        // Set contentSize to bound size
        m_contentSize = boundSize;;
        return;
    }
    
    if (minInvalidatedSection >= sectionCount)
    {
        return;
    }
    
    // Initialize width and set height to 0, layout will calculate new height
    
    typename std::vector<Section *>::iterator it = m_sections.begin() + minInvalidatedSection;
    Rect rectOfSection = (*it)->getFrame();
    height(rectOfSection, 0);
    TInt sectionIndex = minInvalidatedSection;
    for (; it != m_sections.end(); ++it, ++sectionIndex)
    {
        top((*it)->getFrame(), bottom(rectOfSection));
        (*it)->setSection(sectionIndex);
        (*it)->prepareLayout(&layoutCallbackAdapter, boundSize);
        offsetY(rectOfSection, height((*it)->getFrame()));
    }
    
    m_contentSize = makeSize(width(boundSize), bottom(rectOfSection));
}

// LayoutItem::data == 1, indicates that the item is sticky
template<class TLayoutCallbackAdapter, class TInt, class TCoordinate, bool VERTICAL>
void FlexLayoutT<TLayoutCallbackAdapter, TInt, TCoordinate, VERTICAL>::getItemsInRect(std::vector<LayoutItem> &items, StickyItemList &changingStickyItems, StickyItemList &stickyItems, bool stackedStickyItems, const Rect &rect, const Size &size,  const Size &contentSize, const Insets &padding, const Point &contentOffset) const
{
    SectionConstIteratorPair range = std::equal_range(m_sections.begin(), m_sections.end(), std::pair<TCoordinate, TCoordinate>(top(rect), bottom(rect)), SectionCompare());
    
    if (range.first == range.second)
    {
        // No Sections
        // If there is no matched section in rect, there must not be sticky items
        return;
    }
    
    std::vector<const FlexItem *> flexItems;
    for (SectionConstIterator it = range.first; it != range.second; ++it)
    {
        (*it)->filterInRect(flexItems, rect);
        if (flexItems.empty())
        {
            continue;
        }
        
        for (ItemConstIterator itItem = flexItems.begin(); itItem != flexItems.end(); ++itItem)
        {
            LayoutItem item((*it)->getSection(), *(*itItem));
            item.getFrame() = (*it)->getItemFrameInView(*itItem);
            
            items.push_back(item);
        }
        
        flexItems.clear();
    }

    if (!stickyItems.empty())
    {
        TInt maxSection = range.second - 1 - m_sections.begin();
        TInt minSection = range.first - m_sections.begin();
        
        TCoordinate totalStickyItemSize = 0; // When m_stackedStickyItems == YES
        
        LayoutStickyItemCompare comp;
        Rect rect;
        Point origin;
        
        for (typename StickyItemList::iterator it = stickyItems.begin(); it != stickyItems.end(); ++it)
        {
            if (it->first.getSection() > maxSection || (!stackedStickyItems && (it->first.getSection() < minSection)))
            {
                if (it->second.isInSticky())
                {
                    it->second.setInSticky(false);
                    // Pass the change info to caller
                    changingStickyItems.push_back(std::make_pair(it->first, it->second));
                    // notifyItemLeavingStickyMode((*it)->section, (*it)->item, (*it)->position);
                }
                continue;
            }
            
            Section *section = m_sections[it->first.getSection()];
            rect = section->getHeaderFrameInView();
            if (rect.size.empty())
            {
                continue;
            }
            origin = rect.origin;
            
            TCoordinate stickyItemSize = height(rect);
            
            if (stackedStickyItems)
            {
                top(rect, round(std::max(y(contentOffset) + totalStickyItemSize - top(padding), y(origin))));
            }
            else
            {
                Rect frameItems = m_sections[it->first.getSection()]->getItemsFrameInView();
                top(rect, std::min(std::max(top(padding) + y(contentOffset), (top(frameItems) - stickyItemSize)),
                                   (bottom(frameItems) - stickyItemSize)));
            }
            
            
            // If original mode is sticky, we check contentOffset and if contentOffset.y is less than origin.y, it is exiting sticky mode
            // Otherwise, we check the top of sticky header
            bool stickyMode = top(rect) >= y(origin);
            bool originChanged = top(rect) > y(origin);
            // bool stickyMode = (rect.origin.y >= origin.y);
            if (stickyMode != it->second.isInSticky())
            {
                // Pass the change info to caller
                it->second.setInSticky(stickyMode);
                changingStickyItems.push_back(std::make_pair(it->first, it->second));
            }
            
            if (stickyMode)
            {
                typename std::vector<LayoutItem>::iterator itVisibleItem = std::lower_bound(items.begin(), items.end(), *it, comp);
                if (itVisibleItem == items.end() || (itVisibleItem->getSection() != it->first.getSection() || itVisibleItem->getItem() != it->first.getItem()))
                {
                    // Create new LayoutItem and put it into visibleItems
                    LayoutItem layoutItem(it->first.getSection(), it->first.getItem(), rect);
                    layoutItem.setHeader(true);
                    layoutItem.setInSticky(true);
                    layoutItem.setOriginChanged(originChanged);
                    items.insert(itVisibleItem, layoutItem);
                }
                else
                {
                    // Update in place
                    itVisibleItem->getFrame() = rect;
                    itVisibleItem->setInSticky(true);
                    itVisibleItem->setOriginChanged(originChanged);
                }
                
                totalStickyItemSize += stickyItemSize;
            }
        }
    }
    
}


#endif //FLEXLAYOUTMANAGER_FLEXLAYOUT_H
