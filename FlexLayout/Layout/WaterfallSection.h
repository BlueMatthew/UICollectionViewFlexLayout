//
//  WaterfallSection.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef WaterfallSection_h
#define WaterfallSection_h

#import "Section.h"
#import "FlexColumn.h"

template<class TLayout>
class UIWaterfallSectionT : public UISectionT<TLayout>
{
public:
    std::vector<UIFlexColumn *> m_columns;
    
    UIWaterfallSectionT(UICollectionViewFlexLayout *layout, NSInteger section, CGPoint origin) : UISectionT<TLayout>(layout, section, origin)
    {
    }
    
    ~UIWaterfallSectionT()
    {
        for(std::vector<UIFlexColumn *>::iterator it = m_columns.begin(); it != m_columns.end(); delete *it, ++it);
        m_columns.clear();
    }
    
    void prepareLayout()
    {
        UISectionT<TLayout>::m_frame.size = CGSizeZero;
        
        IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (UISectionT<TLayout>::m_frame.size.width = UISectionT<TLayout>::m_layout.collectionView.bounds.size.width) : (UISectionT<TLayout>::m_frame.size.height = UISectionT<TLayout>::m_layout.collectionView.bounds.size.height);
        
        // header
        UISectionT<TLayout>::m_header.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForHeaderInSection:UISectionT<TLayout>::m_section];
        IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_header.m_frame.size.height) : (UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_header.m_frame.size.width);
        
        NSInteger numberOfItems = [UISectionT<TLayout>::m_layout.collectionView numberOfItemsInSection:(UISectionT<TLayout>::m_section)];
        if (numberOfItems > 0)
        {
            
            CGSize *pSize = (CGSize *)malloc(sizeof(CGSize) * numberOfItems);
            for (NSInteger item = 0; item < numberOfItems; item++)
            {
                *(pSize + item) = [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]];
            }
            
            UIEdgeInsets sectionInset = [UISectionT<TLayout>::m_layout getInsetForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            UISectionT<TLayout>::m_items.reserve(numberOfItems);
            
            CGFloat minimumLineSpacing = [UISectionT<TLayout>::m_layout getMinimumLineSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            CGFloat minimumInteritemSpacing = [UISectionT<TLayout>::m_layout getMinimumInteritemSpacingForSectionAtIndex:(UISectionT<TLayout>::m_section)];
            
            // 获取列数
            NSInteger numberOfColumns = [UISectionT<TLayout>::m_layout getNumberOfColumnsForSection:(UISectionT<TLayout>::m_section)];
            
            m_columns.reserve(numberOfColumns);
            CGFloat sizeOfColumn = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (UISectionT<TLayout>::m_layout.collectionView.frame.size.width - sectionInset.left - sectionInset.right) : (UISectionT<TLayout>::m_layout.collectionView.frame.size.height - sectionInset.top - sectionInset.bottom);
            if (numberOfColumns > 1)
            {
                sizeOfColumn = (sizeOfColumn - (numberOfColumns - 1) * minimumInteritemSpacing) / numberOfColumns;
            }
            
            CGPoint columnOrigin = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? CGPointMake(sectionInset.left, sectionInset.top + UISectionT<TLayout>::m_header.m_frame.size.height) : CGPointMake(sectionInset.left + UISectionT<TLayout>::m_header.m_frame.size.width, sectionInset.top);
            CGSize columnSize = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? CGSizeMake(sizeOfColumn, 0) : CGSizeMake(0, sizeOfColumn);
            NSInteger itemCapacity = numberOfItems / numberOfColumns;
            
            for (int column = 0; column < numberOfColumns; column++)
            {
                UIFlexColumn *sectionColumn = new UIFlexColumn(itemCapacity);
                sectionColumn->m_frame.origin = columnOrigin;
                sectionColumn->m_frame.size = columnSize;
                m_columns.push_back(sectionColumn);
                
                IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (columnOrigin.x += sizeOfColumn + minimumInteritemSpacing) : (columnOrigin.y += sizeOfColumn + minimumInteritemSpacing);
            }
            
            // 对每一个Item进行布局，分配到相应的列中
            std::vector<UIFlexColumn *>::iterator columnItOfMinimalSize = m_columns.begin();
            NSInteger itemIndexInColumn = 0;
            // UISectionItem *sectionItem = new UISectionItem[numberOfItems];
            
            // 比较器实例，因为成本很小，所以两个实例全部创建，避免后续频繁分配小对象
            UISectionColumnVerticalCompare vComp;
            UISectionColumnHorizontalCompare hComp;
            
            CGSize *pCurSize = pSize;
            for (NSInteger item = 0; item < numberOfItems; item++, pCurSize++)
            {
                // 找到size最小的column
                columnItOfMinimalSize = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? min_element(m_columns.begin(), m_columns.end(), vComp) : min_element(m_columns.begin(), m_columns.end(), hComp);
                
                // 获取item在column中的位置，以处理spacing，非第一column需要在左边添加spacing
                itemIndexInColumn = (*columnItOfMinimalSize)->m_items.size();
                
                CGPoint originOfItem = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? CGPointMake((*columnItOfMinimalSize)->m_frame.origin.x, (*columnItOfMinimalSize)->m_frame.origin.y + (*columnItOfMinimalSize)->m_frame.size.height + (itemIndexInColumn == 0 ? 0.0f : minimumLineSpacing)) : CGPointMake((*columnItOfMinimalSize)->m_frame.origin.x + (*columnItOfMinimalSize)->m_frame.size.width + (itemIndexInColumn == 0 ? 0.0f : minimumLineSpacing), (*columnItOfMinimalSize)->m_frame.origin.y);
                
                // sectionItem->m_item = item;
                // sectionItem->m_frame = {.origin = originOfItem, .size = [m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:m_section]]};
                // sectionItem->m_frame = {.origin = originOfItem, .size = *pCurSize };
                
                UIFlexItem *sectionItem = new UIFlexItem(item, originOfItem, [UISectionT<TLayout>::m_layout getSizeForItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:(UISectionT<TLayout>::m_section)]]);
                
                UISectionT<TLayout>::m_items.push_back(sectionItem);
                // 加到最小Size的列中
                (*columnItOfMinimalSize)->addItem(sectionItem, IS_CV_VERTICAL(UISectionT<TLayout>::m_layout));
            }
            
            free(pSize);
            
            // 找到size最大的column
            std::vector<UIFlexColumn *>::iterator columnItOfMaximalSize = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? max_element(m_columns.begin(), m_columns.end(), vComp) : max_element(m_columns.begin(), m_columns.end(), hComp);
            
            IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? UISectionT<TLayout>::m_frame.size.height += (*columnItOfMaximalSize)->m_frame.size.height + sectionInset.top + sectionInset.bottom : UISectionT<TLayout>::m_frame.size.width += (*columnItOfMaximalSize)->m_frame.size.width + sectionInset.left + sectionInset.right;
        }
        
        // footer
        UISectionT<TLayout>::m_footer.m_frame.size = [UISectionT<TLayout>::m_layout getSizeForFooterInSection:(UISectionT<TLayout>::m_section)];
        
        IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (UISectionT<TLayout>::m_frame.size.height += UISectionT<TLayout>::m_footer.m_frame.size.height) : (UISectionT<TLayout>::m_frame.size.width += UISectionT<TLayout>::m_footer.m_frame.size.width);
    }
    
    bool getLayoutAttributesForItemsInRect(NSMutableArray<UICollectionViewLayoutAttributes *> *layoutAttributes, const CGRect &rectInSection)
    {
        bool merged = false;
        
        // Items
        for (std::vector<UIFlexColumn *>::iterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            std::pair<std::vector<UIFlexItem *>::iterator, std::vector<UIFlexItem *>::iterator> range = IS_CV_VERTICAL(UISectionT<TLayout>::m_layout) ? (*it)->getVirticalItemsInRect(rectInSection) : (*it)->getHorizontalItemsInRect(rectInSection);
            
            for (std::vector<UIFlexItem *>::iterator itItem = range.first; itItem != range.second; ++itItem)
            {
                // layoutAttributes
                NSIndexPath *indexPath = [NSIndexPath indexPathForItem:(*itItem)->m_item inSection:(UISectionT<TLayout>::m_section)];
                UICollectionViewLayoutAttributes *la = [UISectionT<TLayout>::m_layout layoutAttributesForItemAtIndexPath:indexPath];
                
                la.frame = CGRectMake((*itItem)->getFrame().origin.x + UISectionT<TLayout>::m_frame.origin.x, (*itItem)->getFrame().origin.y + UISectionT<TLayout>::m_frame.origin.y, (*itItem)->getFrame().size.width, (*itItem)->getFrame().size.height);
                
                [layoutAttributes addObject:la];
                merged = true;
            }
        }
        
        return merged;
    }
    
};

#endif /* WaterfallSection_h */
