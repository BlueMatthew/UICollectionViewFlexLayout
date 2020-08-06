//
//  CollectionViewFlexLayoutAdapter.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/7/27.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef CollectionViewFlexAdapter_h
#define CollectionViewFlexAdapter_h

class CollectionViewFlexLayoutAdapter
{
public:
    using Point = nsflex::PointT<CGFloat>;
    using Size = nsflex::SizeT<CGFloat>;
    using Rect = nsflex::RectT<CGFloat>;
    using Insets = nsflex::InsetsT<CGFloat>;
    
protected:
    struct LayoutDelegateFlags
    {
        union   // collectionView.dataSource
        {
            struct
            {
                unsigned int numberOfSections : 1;
                unsigned int numberOfItemsInSection : 1;
            };
            unsigned int dataSource;
        };
        union   // collectionView.delegate
        {
            struct
            {
                unsigned int sizeForItem : 1;
                unsigned int insetForSection : 1;
                unsigned int minimumLineSpacing : 1;
                unsigned int minimumInteritemSpacing : 1;
                unsigned int sizeForHeader : 1;
                unsigned int sizeForFooter : 1;
                unsigned int layoutModeForSection : 1;
                unsigned int hasFixedSize : 1;
                unsigned int isFullSpan : 1;
                unsigned int numberOfColumns : 1;
#ifdef SUPPORT_PAGINATION
                /*
                 unsigned int page : 1;
                unsigned int pageSize : 1;
                unsigned int numberOfFixedSections : 1;
                unsigned int numberOfSectionsForPage : 1;
                unsigned int contentOffsetForPage : 1;
                 */
#endif // #ifdef SUPPORT_PAGINATION
            };
            unsigned int layoutDelegate;
        };
        
        LayoutDelegateFlags() : dataSource(0), layoutDelegate(0)
        {
        }
    };
    
protected:
    LayoutDelegateFlags                     m_layoutDelegateFlags;
    UICollectionViewFlexLayout              *m_layout;
    UICollectionView                        *m_collectionView;
    id<UICollectionViewDataSource>          m_dataSource;
    id<UICollectionViewDelegateFlexLayout>  m_delegate;
    SEL                                     m_itemSizeSel;
    IMP                                     m_itemSizeImp;
    SEL                                     m_fullSpanSel;
    IMP                                     m_fullSpanImp;
    
public:
    
    CollectionViewFlexLayoutAdapter(UICollectionViewFlexLayout *layout) : m_layoutDelegateFlags(), m_layout(layout), m_collectionView(layout.collectionView), m_dataSource(NULL), m_delegate(NULL)
    {
        m_dataSource = m_collectionView.dataSource;
        m_delegate = (id<UICollectionViewDelegateFlexLayout>)(m_collectionView.delegate);

        prepareDelegate();
    }
    
    ~CollectionViewFlexLayoutAdapter()
    {
        m_delegate = NULL;
        m_dataSource = NULL;
        m_collectionView = NULL;
        m_layout = NULL;
    }
    
    inline int getLayoutModeForSection(NSInteger section) const
    {
        return m_layoutDelegateFlags.layoutModeForSection ? (int)[m_delegate collectionView:m_collectionView layout:m_layout layoutModeForSection:section] : (int)UICollectionViewFlexLayoutModeFlow;
    }
    
    inline NSInteger getNumberOfSections() const
    {
        return (m_layoutDelegateFlags.numberOfSections == 1) ? [m_dataSource numberOfSectionsInCollectionView:m_collectionView] : m_collectionView.numberOfSections;
    }
    
    inline NSInteger getNumberOfItemsInSection(NSInteger section) const
    {
        return (m_layoutDelegateFlags.numberOfSections == 1) ? [m_dataSource collectionView:m_collectionView numberOfItemsInSection:section] : 0;
    }
    
    inline Size getSizeForItem(NSInteger section, NSInteger item, bool *isFullSpan) const
    {
        NSIndexPath *indexPath = [NSIndexPath indexPathForItem:item inSection:section];
        
        // id result =(id)((id (*)(id, SEL, NSString*))(impletation)(delegate, @selector(selector), arg1));
        
        CGSize size = m_layoutDelegateFlags.sizeForItem ? ((CGSize (*)(id, SEL, UICollectionView *, UICollectionViewLayout *, NSIndexPath *))m_itemSizeImp)(m_delegate, m_itemSizeSel, m_collectionView, m_layout, indexPath) : m_layout.itemSize;
        
        // CGSize size = m_layoutDelegateFlags.sizeForItem ? [m_delegate collectionView:m_collectionView layout:m_layout sizeForItemAtIndexPath:indexPath] : m_layout.itemSize;
        
        if (isFullSpan != NULL)
        {
            *isFullSpan = m_layoutDelegateFlags.isFullSpan ? (((BOOL (*)(id, SEL, UICollectionView *, UICollectionViewFlexLayout *, NSInteger, NSInteger))m_fullSpanImp)(m_delegate, m_fullSpanSel, m_collectionView, m_layout, item, section) == YES) : false;
            
            // *isFullSpan = m_layoutDelegateFlags.isFullSpan ? ([m_delegate collectionView:m_collectionView layout:m_layout isFullSpanAtItem:item forSection:section] == YES) : false;
        }
        return Size(size.width, size.height);
    }
    
    inline Insets getInsetForSection(NSInteger section) const
    {
        UIEdgeInsets inset = m_layoutDelegateFlags.insetForSection ? [m_delegate collectionView:m_collectionView layout:m_layout insetForSectionAtIndex:section] : m_layout.sectionInset;
        return Insets(inset.left, inset.top, inset.right, inset.bottom);
    }
    
    inline CGFloat getMinimumLineSpacingForSection(NSInteger section) const
    {
        return m_layoutDelegateFlags.minimumLineSpacing ? [m_delegate collectionView:m_collectionView layout:m_layout minimumLineSpacingForSectionAtIndex:section] : m_layout.minimumLineSpacing;
    }
    
    inline CGFloat getMinimumInteritemSpacingForSection(NSInteger section) const
    {
        return m_layoutDelegateFlags.minimumInteritemSpacing ? [m_delegate collectionView:m_collectionView layout:m_layout minimumInteritemSpacingForSectionAtIndex:section] : m_layout.minimumInteritemSpacing;
    }
    
    inline Size getSizeForHeaderInSection(NSInteger section) const
    {
        CGSize size = m_layoutDelegateFlags.sizeForHeader ? [m_delegate collectionView:m_collectionView layout:m_layout referenceSizeForHeaderInSection:section] : m_layout.headerReferenceSize;
        
        return Size(size.width, size.height);
    }
    
    inline Size getSizeForFooterInSection(NSInteger section) const
    {
        return Size();
        // CGSize size = [m_layout getSizeForFooterInSection:section];
        // return FlexSizeFromCGSize(size);
    }
    
    inline NSInteger getNumberOfColumnsForSection(NSInteger section) const
    {
        return m_layoutDelegateFlags.numberOfColumns ? [m_delegate collectionView:m_collectionView layout:m_layout numberOfColumnsInSection:section] : 1;
    }
    
    inline bool hasFixedItemSize(NSInteger section, Size *fixedItemSize) const
    {
        CGSize size = CGSizeZero;
        BOOL hasFixedItemSize = m_layoutDelegateFlags.hasFixedSize ? [m_delegate collectionView:m_collectionView layout:m_layout hasFixedSize:&size forSection:section] : NO;
        if (NULL != fixedItemSize)
        {
            fixedItemSize->width = size.width;
            fixedItemSize->height = size.height;
        }
        
        return hasFixedItemSize == YES;
    }
    
#ifdef SUPPORT_PAGINATION
    /*
    inline NSInteger getPage()
    {
        return [m_layout getPage];
    }
    
    inline NSInteger getPageSize()
    {
        return [m_layout getPageSize];
    }
    
    inline NSInteger getNumberOfFixedSections(int page)
    {
        return [m_layout getNumberOfFixedSections];
    }
    
    inline NSInteger getNumberOfSectionsForPage(NSInteger page)
    {
        return [m_layout getNumberOfSectionsForPage:page];
    }
    
    inline bool getContentOffset(nsflex::Point *pContentOffset, NSInteger page)
    {
        CGPoint contentOffset = CGPointZero;
        BOOL result = [m_layout getContentOffset:&contentOffset forPage:page];
        if (NULL != pContentOffset)
        {
            *pContentOffset = FlexPointFromCGPoint(contentOffset);
        }
        return result == YES ? true : false;
    }
     */
#endif // #ifdef SUPPORT_PAGINATION
    
protected:
    
    void prepareDelegate()
    {
        // if (NULL == m_layoutDelegateFlags.dataSourcePointer || m_layoutDelegateFlags.dataSourcePointer != (__bridge void *)dataSource)
        {
            // m_layoutDelegateFlags.dataSourcePointer = (__bridge void *)dataSource;
            m_layoutDelegateFlags.dataSource = 0;
            if ([m_dataSource conformsToProtocol:@protocol(UICollectionViewDataSource)])
            {
                m_layoutDelegateFlags.numberOfSections = ([m_dataSource respondsToSelector:@selector(numberOfSectionsInCollectionView:)]) ? 1 : 0;
                m_layoutDelegateFlags.numberOfItemsInSection = ([m_dataSource respondsToSelector:@selector(collectionView:numberOfItemsInSection:)]) ? 1 : 0;
            }
        }
        
        id delegate = m_delegate;
        // if (NULL == m_layoutDelegateFlags.delegatePointer || m_layoutDelegateFlags.delegatePointer != (__bridge void *)delegate)
        {
            // m_layoutDelegateFlags.delegatePointer = (__bridge void *)delegate;
            m_layoutDelegateFlags.layoutDelegate = 0;
            if ([delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)])
            {
                m_itemSizeSel = @selector(collectionView:layout:sizeForItemAtIndexPath:);
                m_itemSizeImp = NULL;
                if ([delegate respondsToSelector:m_itemSizeSel])
                {
                    m_itemSizeImp = [delegate methodForSelector:m_itemSizeSel];
                }
                m_layoutDelegateFlags.sizeForItem = ([delegate respondsToSelector:m_itemSizeSel]) ? 1 : 0;
                m_layoutDelegateFlags.insetForSection = ([delegate respondsToSelector:@selector(collectionView:layout:insetForSectionAtIndex:)]) ? 1 : 0;
                m_layoutDelegateFlags.minimumLineSpacing = ([delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)]) ? 1 : 0;
                m_layoutDelegateFlags.minimumInteritemSpacing = ([delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)]) ? 1 : 0;
                m_layoutDelegateFlags.sizeForHeader = ([delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForHeaderInSection:)]) ? 1 : 0;
                m_layoutDelegateFlags.sizeForFooter = ([delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForFooterInSection:)]) ? 1 : 0;
                m_layoutDelegateFlags.numberOfColumns = ([delegate respondsToSelector:@selector(collectionView:layout:numberOfColumnsInSection:)]) ? 1 : 0;
                m_layoutDelegateFlags.layoutModeForSection = ([delegate respondsToSelector:@selector(collectionView:layout:layoutModeForSection:)]) ? 1 : 0;
                m_layoutDelegateFlags.hasFixedSize = ([delegate respondsToSelector:@selector(collectionView:layout:hasFixedSize:forSection:)]) ? 1 : 0;
                
                m_fullSpanSel = @selector(collectionView:layout:isFullSpanAtItem:forSection:);
                if ([delegate respondsToSelector:m_fullSpanSel])
                {
                    m_fullSpanImp = [delegate methodForSelector:m_fullSpanSel];
                }
                m_layoutDelegateFlags.isFullSpan = ([delegate respondsToSelector:m_fullSpanSel]) ? 1 : 0;
                
            }
        }
    }
};

class CollectionViewFlexDisplayAdapter
{
public:
    using Point = nsflex::PointT<CGFloat>;
    using Size = nsflex::SizeT<CGFloat>;
    using Rect = nsflex::RectT<CGFloat>;
    using Insets = nsflex::InsetsT<CGFloat>;
    
protected:
    
    
    
protected:
    union    // collectionView.delegate
    {
        struct
        {
            unsigned int m_enterStickyMode : 1;
            unsigned int m_exitStickyMode : 1;
        };
        unsigned int m_layoutDelegateFlags;
    };
    UICollectionViewFlexLayout              *m_layout;
    UICollectionView                        *m_collectionView;
    id<UICollectionViewDelegateFlexLayout>  m_delegate;
    
public:
    
    CollectionViewFlexDisplayAdapter(UICollectionViewFlexLayout *layout) : m_layoutDelegateFlags(0), m_layout(layout), m_collectionView(layout.collectionView), m_delegate(NULL)
    {
        m_delegate = (id<UICollectionViewDelegateFlexLayout>)(m_collectionView.delegate);
        
        prepareDelegate();
    }
    
    ~CollectionViewFlexDisplayAdapter()
    {
        m_delegate = NULL;
        m_collectionView = NULL;
        m_layout = NULL;
    }
    
    void enterStickyMode(NSInteger section, const Point &point)
    {
        if ([m_delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)] && [m_delegate respondsToSelector:@selector(collectionView:layout:headerEnterStickyModeAtSection:withOriginalPoint:)])
        {
            CGPoint pt = {point.x, point.y};
            [m_delegate collectionView:m_collectionView layout:m_layout headerEnterStickyModeAtSection:section withOriginalPoint:pt];
        }
    }
    
    void exitStickyMode(NSInteger section)
    {
        if ([m_delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)] && [m_delegate respondsToSelector:@selector(collectionView:layout:headerExitStickyModeAtSection:)])
        {
            [m_delegate collectionView:m_collectionView layout:m_layout headerExitStickyModeAtSection:section];
        }
    }

    
protected:
    
    void prepareDelegate()
    {
        id delegate = m_delegate;
        // if (NULL == m_layoutDelegateFlags.delegatePointer || m_layoutDelegateFlags.delegatePointer != (__bridge void *)delegate)
        {
            // m_layoutDelegateFlags.delegatePointer = (__bridge void *)delegate;
            m_layoutDelegateFlags = 0;
            if ([delegate conformsToProtocol:@protocol(UICollectionViewDelegateFlexLayout)])
            {
                m_enterStickyMode = [delegate respondsToSelector:@selector(collectionView:layout:headerEnterStickyModeAtSection:withOriginalPoint:)];
                m_exitStickyMode = [delegate respondsToSelector:@selector(collectionView:layout:headerExitStickyModeAtSection:)];
            }
        }
    }
};


#endif /* CollectionViewFlexAdapter_h */
