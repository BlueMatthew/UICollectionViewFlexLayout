//
//  FlexListView.mm
//  FlexLayout
//
//  Created by Matthew Shi on 2020/6/6.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "FlexListView.h"
#import "CollectionViewFlexLayout.h"
#import "CategoryBarViewCell.h"
#import "ItemViewCell.h"
#import "InfoViewCell.h"
#import "ImageItemViewCell.h"
#import "CategoryBar.h"
#import "UIUtility.h"
#include <map>

#define SIZE_MAIN_IMAGE_WIDTH       282
#define SIZE_MAIN_IMAGE_HEIGHT      282

#define REUSE_ID_NAVBAR             "navbar"
#define REUSE_ID_ENTRY              "entry"
#define REUSE_ID_CATBAR             "catbar"
#define REUSE_ID_LOADMORE           "loadMore"
#define REUSE_ID_ITEM1              "item1"
#define REUSE_ID_ITEM2              "item2"

#define REUSE_ID_INFO_HEADER        "info_header"
#define REUSE_ID_INFO_FOOTER        "info_footer"

#define ITEM_TEXT_NAVBAR            "Navigation Bar"
#define ITEM_TEXT_LOADMORE          "Loading More Data..."
#define ITEM_TEXT_CATBAR_ITEM       "Cat %ld"
#define ITEM_TEXT_ITEM1             "Cat:%lu Item:%lu"
#define ITEM_TEXT_ITEM2             "Cat:%lu Item:%lu"

// #define SECTION_INDEX_NAVBAR            0
#define SECTION_INDEX_ENTRY             0
#define SECTION_INDEX_TEST1             1
#define SECTION_INDEX_TEST2             2
#define SECTION_INDEX_CATBAR            3
#define SECTION_INDEX_ITEM1             4
#define SECTION_INDEX_ITEM2             5

#define SECTION_INDEX_ITEM_PAGING1       0
#define SECTION_INDEX_ITEM_PAGING2       1

#define NUM_OF_ITEMS_IN_CATEGORY_BAR    8

#define NUM_OF_ITEMS_IN_SECTION_ENTRY   1
#define NUM_OF_ITEMS_IN_SECTION_TEST1   1
#define NUM_OF_ITEMS_IN_SECTION_TEST2   1
#define NUM_OF_ITEMS_IN_SECTION_ITEM1   12
#define NUM_OF_ITEMS_IN_SECTION_ITEM2   2

#define ITEM_HEIGHT_NAVBAR              100
#define ITEM_HEIGHT_ENTRY               120
#define ITEM_HEIGHT_TEST                40
#define ITEM_HEIGHT_INFO                80
#define ITEM_HEIGHT_LOADMORE            110
#define ITEM_HEIGHT_CATBAR              40
#define ITEM_HEIGHT_ITEM1               160
#define ITEM_HEIGHT_ITEM2               160

#define ITEM_SPACING_ITEM                0
#define LINE_SPACING_ITEM                0

#define SECTION_INSET_ITEM_LEFT          0
#define SECTION_INSET_ITEM_TOP           0
#define SECTION_INSET_ITEM_RIGHT         0
#define SECTION_INSET_ITEM_BOTTOM        0

#define SECTION_INSET_TEST1_PADDING      10

#define CONTENT_INSET                    10

#define ITEM_COLUMNS                     2

@interface SUIFlexListView() <UICollectionViewDelegateFlexLayout, UICollectionViewDataSource, SUICategoryBarDelegate>
{
    NSInteger                       m_page;
    SUICategoryBar                  *m_categoryBarView;
    
    BOOL                            m_isCategoryBarSticky;
    CGFloat                         m_minPagingTop;
    
    // DataSource
    NSMutableDictionary<NSNumber *, NSMutableArray< NSNumber * > *>    *m_sections;     // Category Page -> Sections
    NSMutableArray<UIColor *>       *m_entryColors;
    NSNumber                        *m_navbarColor;
    NSMutableArray<UIBarItem *>     *m_barItems;
    UIColor                         *m_catColor;
    NSNumber                        *m_loadMoreColor;
    
    NSMutableArray<NSMutableDictionary *>                       *m_entries;
    NSMutableArray<NSMutableDictionary *>                       *m_test1;
    NSMutableArray<NSMutableDictionary *>                       *m_test2;
    
    NSMutableArray< NSMutableArray<NSMutableDictionary *> *>    *m_items1;
    NSMutableArray< NSMutableArray<NSMutableDictionary *> *>    *m_items2;
    
    std::map<NSInteger, CGPoint>    m_pageContexts;    // Category Page -> contentOffset, should be reset when the data is updated
}

@end

@implementation SUIFlexListView

#pragma mark Construction Functions

- (nonnull instancetype)initWithFrame:(CGRect)frame;
{
    UICollectionViewFlexLayout *layout = [[UICollectionViewFlexLayout alloc] init];
    
    [layout setScrollDirection:UICollectionViewScrollDirectionVertical];
    
    layout.minimumInteritemSpacing = 0.0;
    layout.minimumLineSpacing = 0.0;
    // layout.pagingSection = SECTION_INDEX_ITEM1;
    // layout.sectionHeadersPinToVisibleBounds = YES;
    [layout addStickyHeader:SECTION_INDEX_ENTRY];
    [layout addStickyHeader:SECTION_INDEX_CATBAR];
    [layout setStackedStickyHeaders:YES];
    
    if (self = [super initWithFrame:frame collectionViewLayout:layout])
    {
        if (@available(iOS 11.0, *))
        {
            self.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
        
        self.dataSource = self;
        self.delegate = self;
        // self.pagingDelegate = self;
        
        // [self enablePagingWithDirection:UICollectionViewScrollDirectionHorizontal];
        
        if (@available(iOS 10.0, *))
        {
            self.prefetchingEnabled = NO;   // avoid crashing ?
        }
        self.backgroundColor = [UIColor colorWithRed:245.0 / 255.0 green:245.0 / 255.0 blue:245.0 / 255.0 alpha:1.0];
        self.showsVerticalScrollIndicator = NO;
        self.contentInset = UIEdgeInsetsMake(CONTENT_INSET, CONTENT_INSET, CONTENT_INSET, CONTENT_INSET);

        [self registerClass:[SUIItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_NAVBAR];
        [self registerClass:[SUIItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ENTRY];
        [self registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM1];
        [self registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM2];
        
        [self registerClass:[SUIItemViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_NAVBAR];
        [self registerClass:[SUICategoryBarViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_CATBAR];
        [self registerClass:[SUIItemViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionFooter withReuseIdentifier:@REUSE_ID_LOADMORE];
        [self registerClass:[SUIInfoViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_INFO_HEADER];
        [self registerClass:[SUIInfoViewCell class] forSupplementaryViewOfKind:UICollectionElementKindSectionFooter withReuseIdentifier:@REUSE_ID_INFO_FOOTER];
        
        m_isCategoryBarSticky = NO;
        m_minPagingTop = CGFLOAT_MAX;
        
        [self initializeDataSource];
        
        [self performSelector:@selector(reloadData) withObject:nil afterDelay:0];
    }
    
    return self;
}

#pragma mark DataSource Functions

- (void)initializeDataSource
{
    m_sections = [NSMutableDictionary<NSNumber *, NSMutableArray< NSNumber * > *> dictionaryWithCapacity:NUM_OF_ITEMS_IN_CATEGORY_BAR];
    for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_CATEGORY_BAR; idx++)
    {
        NSMutableArray<NSNumber *> *sections = [(@[/*@SECTION_INDEX_NAVBAR, */@SECTION_INDEX_ENTRY, @SECTION_INDEX_TEST1, @SECTION_INDEX_TEST2, @SECTION_INDEX_CATBAR, @SECTION_INDEX_ITEM1, @SECTION_INDEX_ITEM2]) mutableCopy];
        [m_sections setObject:sections forKey:@(idx)];
    }
    
    m_navbarColor = [NSNumber numberWithUnsignedLong:0xFF7F50];   // coral
    m_catColor = UIColorFromRGB(0xFEA460);   // sandybrown
    m_loadMoreColor = [NSNumber numberWithUnsignedLong:0xFFDCB4];
    
    int bgColorIndex = 0;
    unsigned int entryColors[] = {0x7CFC00, 0x32CD32, 0x006400, 0x9ACD32, 0x00FA9A, 0x98FB98, 0x808000, 0x6B8E23};
    m_entries = [[NSMutableArray<NSMutableDictionary *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_ENTRY];
    for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_SECTION_ENTRY; idx++, bgColorIndex++)
    {
        NSMutableDictionary *item = [[NSMutableDictionary alloc] initWithCapacity:4];
        NSNumber *bgColor = [NSNumber numberWithUnsignedLong:entryColors[bgColorIndex % (sizeof(entryColors) / sizeof(unsigned int))]];
        [item setObject:bgColor forKey:@"bgColor"];
        [item setObject:[NSString stringWithFormat:@"Entry %ld", idx] forKey:@"text"];
        [m_entries addObject:item];
    }
    
    m_test1 = [[NSMutableArray<NSMutableDictionary *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_TEST1];
    bgColorIndex++;
    for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_SECTION_TEST1; idx++, bgColorIndex++)
    {
        NSMutableDictionary *item = [[NSMutableDictionary alloc] initWithCapacity:4];
        NSNumber *bgColor = [NSNumber numberWithUnsignedLong:entryColors[bgColorIndex % (sizeof(entryColors) / sizeof(unsigned int))]];
        [item setObject:bgColor forKey:@"bgColor"];
        [item setObject:[NSString stringWithFormat:@"Test1 %ld", idx] forKey:@"text"];
        [m_test1 addObject:item];
    }
    
    m_test2 = [[NSMutableArray<NSMutableDictionary *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_TEST2];
    bgColorIndex++;
    for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_SECTION_TEST2; idx++, bgColorIndex++)
    {
        NSMutableDictionary *item = [[NSMutableDictionary alloc] initWithCapacity:4];
        NSNumber *bgColor = [NSNumber numberWithUnsignedLong:entryColors[bgColorIndex % (sizeof(entryColors) / sizeof(unsigned int))]];
        [item setObject:bgColor forKey:@"bgColor"];
        [item setObject:[NSString stringWithFormat:@"Test2 %ld", idx] forKey:@"text"];
        [m_test2 addObject:item];
    }
    
    unsigned int itemColors[] = {0xB0E0E6, 0x87CEFA, 0x87CEEB, 0x00BFFF, 0x1E90FF, 0x6495ED, 0x4169E1, 0x0000FF, 0xB0E0E6, 0x87CEFA, 0x87CEEB, 0x00BFFF, 0x1E90FF, 0x6495ED, 0x4169E1, 0x0000FF};
    
    unsigned int imageColors[] = {0x800000, 0x8B0000, 0xA52A2A, 0xB22222, 0xDC143C, 0xFF0000, 0xFF6347, 0xFF7F50, 0xCD5C5C, 0xF08080, 0xE9967A, 0xFA8072, 0xFFA07A, 0xFF4500, 0xFF8C00, 0xFFA500, 0xFFD700, 0xB8860B, 0xDAA520, 0xEEE8AA, 0xBDB76B, 0xF0E68C, 0x808000, 0xFFFF00, 0x9ACD32, 0x556B2F, 0x6B8E23, 0x7CFC00, 0x7FFF00, 0xADFF2F, 0x006400, 0x008000, 0x228B22, 0x00FF00, 0x32CD32, 0x90EE90, 0x98FB98, 0x8FBC8F, 0x00FA9A, 0x00FF7F, 0x2E8B57, 0x66CDAA, 0x3CB371, 0x20B2AA, 0x2F4F4F, 0x008080, 0x008B8B, 0x00FFFF, 0x00FFFF, 0xE0FFFF, 0x00CED1, 0x40E0D0, 0x48D1CC, 0xAFEEEE, 0x7FFFD4, 0xB0E0E6, 0x5F9EA0, 0x4682B4, 0x6495ED, 0x00BFFF, 0x1E90FF, 0xADD8E6, 0x87CEEB, 0x87CEFA, 0x191970, 0x000080, 0x00008B, 0x0000CD, 0x0000FF, 0x4169E1, 0x8A2BE2, 0x4B0082, 0x483D8B, 0x6A5ACD, 0x7B68EE, 0x9370DB, 0x8B008B, 0x9400D3, 0x9932CC, 0xBA55D3, 0x800080, 0xD8BFD8, 0xDDA0DD, 0xEE82EE, 0xFF00FF, 0xDA70D6, 0xC71585, 0xDB7093, 0xFF1493, 0xFF69B4, 0xFFB6C1, 0xFFC0CB, 0xFAEBD7, 0xF5F5DC, 0xFFE4C4, 0xFFEBCD, 0xF5DEB3, 0xFFF8DC, 0xFFFACD, 0xFAFAD2, 0xFFFFE0, 0x8B4513, 0xA0522D, 0xD2691E, 0xCD853F, 0xF4A460, 0xDEB887, 0xD2B48C, 0xBC8F8F, 0xFFE4B5, 0xFFDEAD, 0xFFDAB9, 0xFFE4E1, 0xFFF0F5, 0xFAF0E6, 0xFDF5E6, 0xFFEFD5, 0xFFF5EE, 0xF5FFFA, 0x708090, 0x778899, 0xB0C4DE, 0xE6E6FA, 0xFFFAF0, 0xF0F8FF, 0xF8F8FF, 0xF0FFF0, 0xFFFFF0, 0xF0FFFF, 0xFFFAFA, 0x000000, 0x696969, 0x808080, 0xA9A9A9, 0xC0C0C0, 0xD3D3D3, 0xDCDCDC, 0xF5F5F5, 0xFFFFFF};
    
    NSMutableDictionary<NSAttributedStringKey, id> *attributesForNormal = [NSMutableDictionary<NSAttributedStringKey, id> dictionaryWithCapacity:4];
    [attributesForNormal setObject:[UIColor blackColor] forKey:NSForegroundColorAttributeName];
    
    NSMutableDictionary<NSAttributedStringKey, id> *attributesForSelected = [NSMutableDictionary<NSAttributedStringKey, id> dictionaryWithCapacity:4];
    [attributesForSelected setObject:[UIColor redColor] forKey:NSForegroundColorAttributeName];
    [attributesForSelected setObject:[UIFont systemFontOfSize:[UIFont labelFontSize] weight:UIFontWeightBold] forKey:NSFontAttributeName];
    
    m_barItems = [NSMutableArray<UIBarItem *> arrayWithCapacity:NUM_OF_ITEMS_IN_CATEGORY_BAR];
    for (NSInteger item = 0; item < NUM_OF_ITEMS_IN_CATEGORY_BAR; item++)
    {
        UIBarItem *barItem = [[UIBarButtonItem alloc] init];
        // barButtonItem
        barItem.title = [NSString stringWithFormat:@ITEM_TEXT_CATBAR_ITEM, item];
        barItem.tag = item;
        // barItem.width = self.bounds.size.width / 4;
        
        [barItem setTitleTextAttributes:attributesForNormal forState:UIControlStateNormal];
        [barItem setTitleTextAttributes:attributesForSelected forState:UIControlStateFocused];
        
        [m_barItems addObject:barItem];
    }
    
    m_items1 = [[NSMutableArray< NSMutableArray<NSMutableDictionary *> *> alloc] initWithCapacity:m_barItems.count];
    m_items2 = [[NSMutableArray< NSMutableArray<NSMutableDictionary *> *> alloc] initWithCapacity:m_barItems.count];
    
    // Using a array for variable heights
    unsigned int itemHeights1[] = {40, 65, 55, 70, 60, 85, 75, 100};
    
    [self initializeItems:m_items1 numberOfItemsInSection:NUM_OF_ITEMS_IN_SECTION_ITEM1 textFormat:@ITEM_TEXT_ITEM1 columns:ITEM_COLUMNS itemColors:itemColors numberOfItemColors:(sizeof(itemColors) / sizeof(unsigned int)) imageColors:imageColors numberOfImageColors:(sizeof(imageColors) / sizeof(unsigned int)) itemHeights:itemHeights1 numberOfItemHeights:(sizeof(itemHeights1) / sizeof(unsigned int))];
    
    unsigned int itemHeights2[] = {ITEM_HEIGHT_ITEM2};
    [self initializeItems:m_items2 numberOfItemsInSection:NUM_OF_ITEMS_IN_SECTION_ITEM2 textFormat:@ITEM_TEXT_ITEM2 columns:1 itemColors:itemColors numberOfItemColors:(sizeof(itemColors) / sizeof(unsigned int)) imageColors:imageColors numberOfImageColors:(sizeof(imageColors) / sizeof(unsigned int)) itemHeights:itemHeights2 numberOfItemHeights:(sizeof(itemHeights2) / sizeof(unsigned int))];
}

- (void)initializeItems:(NSMutableArray< NSMutableArray<NSMutableDictionary *> *> *)itemsArray numberOfItemsInSection:(NSInteger)numberOfItemsInSection textFormat:(NSString *)textFormat columns:(NSInteger)columns itemColors:(unsigned int *)itemColors numberOfItemColors:(NSInteger)numberOfItemColors imageColors:(unsigned int *)imageColors numberOfImageColors:(NSInteger)numberOfImageColors itemHeights:(unsigned int *)itemHeights numberOfItemHeights:(NSInteger)numberOfItemHeights
{
    NSMutableArray<NSMutableDictionary *> *items = nil;

    UIEdgeInsets insets = self.contentInset;
    
    NSMutableArray<NSNumber *> *widthOfColumns = [[NSMutableArray<NSNumber *> alloc] initWithCapacity:ITEM_COLUMNS];
    CGFloat availableColumnSize = self.bounds.size.width - (insets.left + insets.right + SECTION_INSET_ITEM_LEFT + SECTION_INSET_ITEM_RIGHT);
    for (NSInteger idx = 0; idx < ITEM_COLUMNS; idx++)
    {
        if (idx == (ITEM_COLUMNS - 1))
        {
            [widthOfColumns addObject:[NSNumber numberWithDouble:availableColumnSize]];
        }
        else
        {
            CGFloat itemWidth =  round((availableColumnSize - (ITEM_COLUMNS - idx - 1) * ITEM_SPACING_ITEM) / (ITEM_COLUMNS - idx));
            [widthOfColumns addObject:[NSNumber numberWithDouble:itemWidth]];
            availableColumnSize -= (itemWidth + ITEM_SPACING_ITEM);
        }
    }
    
    CGFloat itemWidth = CGFLOAT_MAX;
    for (NSNumber *number in widthOfColumns)
    {
        if (itemWidth > number.doubleValue)
        {
            itemWidth = number.doubleValue;
        }
    }
    
    for (NSInteger catIdx = 0; catIdx < NUM_OF_ITEMS_IN_CATEGORY_BAR; catIdx++)
    {
        NSInteger bgIndex = 4 * catIdx;
        NSInteger imageColorIndex = 16 * catIdx;
        
        NSInteger numberOfItems = numberOfItemsInSection;
        
        items = [[NSMutableArray<NSMutableDictionary *> alloc] initWithCapacity:numberOfItems];
        [itemsArray addObject:items];
        
        for (NSInteger idx = 0; idx < numberOfItems; idx++, imageColorIndex+=8, bgIndex++)
        {
            NSMutableDictionary* dict = [NSMutableDictionary dictionaryWithCapacity:8];
            [dict setObject:@"" forKey:@"image"];   // NO Imager right now
            
            unsigned int bgColor = itemColors[(bgIndex % numberOfItemColors)];
            [dict setObject:[NSNumber numberWithUnsignedLong:bgColor] forKey:@"bgColor"];
            
            [dict setObject:[NSNumber numberWithUnsignedLong:imageColors[(imageColorIndex % numberOfImageColors)]] forKey:@"imageColor"];
            
            [dict setObject:[NSNumber numberWithBool:NO] forKey:@"displayed"];
            [dict setObject:@(itemWidth) forKey:@"width"];
            
            CGFloat itemHeight = (columns == 1) ? itemHeights[(idx % numberOfItemHeights)] : (floor(itemWidth) + itemHeights[(idx % numberOfItemHeights)]);
            
            [dict setObject:@(itemHeight) forKey:@"height"];
            
            [dict setObject:@"item" forKey:@"itemType"];
            
            [dict setObject:[NSString stringWithFormat:textFormat, (long)catIdx, idx] forKey:@"text"];
            
            [items addObject:dict];
        }
    }
}

- (CGSize)itemSizeForItem1:(NSInteger)item AtPage:(NSInteger)page
{
    NSMutableDictionary *itemDict = [self itemDictForItem1:item AtPage:page];
    CGFloat width = [((NSNumber *)[itemDict objectForKey:@"width"]) doubleValue];
    CGFloat height= [(NSNumber *)[itemDict objectForKey:@"height"] doubleValue];
    
    return CGSizeMake(width, height);
}

- (CGSize)itemSizeForItem2:(NSInteger)item AtPage:(NSInteger)page
{
    NSMutableDictionary *itemDict = [self itemDictForItem2:item AtPage:page];
    CGFloat width = [((NSNumber *)[itemDict objectForKey:@"width"]) doubleValue];
    CGFloat height= [(NSNumber *)[itemDict objectForKey:@"height"] doubleValue];
    
    return CGSizeMake(width, height);
}

- (NSMutableDictionary *)itemDictForItem1:(NSInteger)item AtPage:(NSInteger)page
{
    NSMutableArray<NSMutableDictionary *> *items = [m_items1 objectAtIndex:page];
    NSMutableDictionary *itemDict = [items objectAtIndex:(item % items.count)];
    
    return itemDict;
}

- (NSMutableDictionary *)itemDictForItem2:(NSInteger)item AtPage:(NSInteger)page
{
    NSMutableArray<NSMutableDictionary *> *items = [m_items2 objectAtIndex:page];
    NSMutableDictionary *itemDict = [items objectAtIndex:(item % items.count)];
    
    return itemDict;
}

// Check if the section which put items (SECTION_INDEX_ITEM / SECTION_INDEX_ITEM_PAGING)
- (BOOL)isSection:(NSInteger)section itemsInCollectionView:(UICollectionView *)collectionView
{
    return ((collectionView == self && SECTION_INDEX_ITEM1 == section) || (collectionView != self && SECTION_INDEX_ITEM_PAGING1 == section));
}

#pragma mark Paging Functions

- (void)buildCategoryBarWithFrame:(CGRect)frame
{
    if (nil == m_categoryBarView)
    {
        m_categoryBarView = [[SUICategoryBar alloc] initWithFrame:frame];
        
        m_categoryBarView.viewDelegate = self;
        m_categoryBarView.items = m_barItems;
        m_categoryBarView.itemSize = CGSizeMake(self.bounds.size.width / 4, self.bounds.size.height);
    }
    
    if (m_page != m_categoryBarView.selectedItem)
    {
        [m_categoryBarView selectItemAt:m_page animated:NO];
    }
}

- (CGRect)visibleRectForScrollableSction:(NSInteger)section
{
    if (nil == m_categoryBarView || nil == m_categoryBarView.superview)
    {
        return CGRectZero;
    }
    
    CGRect frame = [self convertRect:m_categoryBarView.bounds fromView:m_categoryBarView];
    return CGRectMake(0, CGRectGetMaxY(frame), self.bounds.size.width, self.bounds.size.height - CGRectGetMaxY(frame) + self.contentOffset.y);
}

- (UICollectionView *)buildCollectionViewAtPage:(NSInteger)page forView:(UIView *)parentView withFrame:(CGRect)frame
{
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    
    //Set Scrolling Direction
    layout.scrollDirection = ((UICollectionViewFlowLayout *)self.collectionViewLayout).scrollDirection;
    layout.minimumInteritemSpacing = 0.0;
    layout.minimumLineSpacing = 0.0;

    UICollectionView *collectionView = nil;
    collectionView = [[UICollectionView alloc] initWithFrame:frame collectionViewLayout:layout];
    
    collectionView.backgroundColor = [self.backgroundColor copy];
    if (@available(iOS 11.0, *))
    {
        collectionView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
    }
    collectionView.scrollEnabled = NO;
    collectionView.bounces = NO;
    collectionView.alwaysBounceVertical = NO;
    collectionView.showsVerticalScrollIndicator = NO;
    
    collectionView.tag = page;
    
    [collectionView registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM1];
    [collectionView registerClass:[SUIImageItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_ITEM2];

#ifdef DEBUG
    // collectionView.layer.borderColor = [[UIColor redColor] CGColor];
    // collectionView.layer.borderWidth = 3;
#endif
    
    CGPoint contentOffset = CGPointZero;
    if (m_isCategoryBarSticky)
    {
        std::map<NSInteger, CGPoint>::const_iterator it = m_pageContexts.find(page);
        if (it != m_pageContexts.end())
        {
            contentOffset = it->second;
            contentOffset.y -= (m_minPagingTop - ITEM_HEIGHT_CATBAR);
        }
    }
    
    collectionView.dataSource = self;
    collectionView.delegate = self;
    
    [collectionView reloadData];
    [collectionView performBatchUpdates:^{
    } completion:^(BOOL finished) {
        [collectionView setContentOffset:contentOffset animated:NO];
    }];

    return collectionView;
}

- (void)switchPage:(NSInteger)page
{
   // Save ContentOffset for current page
    m_pageContexts[m_page] = self.contentOffset;
    
    // Switch to new page
    m_page = page;
    __block BOOL contentOffsetInvalidated = NO;
    __block CGPoint contentOffset = CGPointZero;
    if (m_isCategoryBarSticky)
    {
        std::map<NSInteger, CGPoint>::const_iterator it = m_pageContexts.find(page);
        contentOffset = (it != m_pageContexts.end()) ? it->second : CGPointMake(0, m_minPagingTop - ITEM_HEIGHT_CATBAR);
        if (!CGPointEqualToPoint(contentOffset, self.contentOffset))
        {
            contentOffsetInvalidated = YES;
        }
    }
    
    NSMutableArray<NSNumber *> *sections = [m_sections objectForKey:@(m_page)];
    __block NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSetWithIndexesInRange:NSMakeRange(SECTION_INDEX_ITEM1, sections.count - SECTION_INDEX_ITEM1)];
    // __block UICollectionViewFlexLayout *layout = (UICollectionViewFlexLayout *)self.collectionViewLayout;
    
    [self performBatchUpdates:^{
        [UIView performWithoutAnimation:^{
            [self reloadSections:indexSet];
            // layout.pagingOffset = CGPointZero;
        }];
    } completion:^(BOOL finished) {
        if (contentOffsetInvalidated)
        {
            self.contentOffset = contentOffset;
        }
        // [self cleanPagingViews];
        
        [self->m_categoryBarView selectItemAt:page animated:YES];
    }];
}

#pragma mark UICollectionViewDataSource Implementation
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    NSMutableArray<NSNumber *> *sections = [m_sections objectForKey:(collectionView == self) ? @(m_page) : @(collectionView.tag)];
    return (collectionView == self) ? sections.count : 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    NSInteger numberOfItemsInSection = 0;
    if (collectionView == self)
    {
        if (SECTION_INDEX_ENTRY == section)
        {
            numberOfItemsInSection = NUM_OF_ITEMS_IN_SECTION_ENTRY;
        }
        else if (SECTION_INDEX_TEST1 == section)
        {
            numberOfItemsInSection = NUM_OF_ITEMS_IN_SECTION_TEST1;
        }
        else if (SECTION_INDEX_TEST2 == section)
        {
            numberOfItemsInSection = NUM_OF_ITEMS_IN_SECTION_TEST2;
        }
        else if (SECTION_INDEX_ITEM1 == section)
        {
            // items
            numberOfItemsInSection = [m_items1 objectAtIndex:m_page].count;
        }
        else if (SECTION_INDEX_ITEM2 == section)
        {
            // items
            numberOfItemsInSection = [m_items2 objectAtIndex:m_page].count;
        }
    }
    else
    {
        if (SECTION_INDEX_ITEM_PAGING1 == section)
        {
            // items
            NSInteger page = collectionView.tag;
            numberOfItemsInSection = [m_items2 objectAtIndex:page].count;
        }
        else if (SECTION_INDEX_ITEM_PAGING2 == section)
            {
                // items
                NSInteger page = collectionView.tag;
                numberOfItemsInSection = [m_items2 objectAtIndex:page].count;
            }
    }
    
    return numberOfItemsInSection;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        UIEdgeInsets insets = self.contentInset;
        
        if (SECTION_INDEX_ENTRY == indexPath.section)
        {
            return CGSizeMake(self.bounds.size.width - insets.left - insets.right, ITEM_HEIGHT_ENTRY);
        }
        else if (SECTION_INDEX_TEST1 == indexPath.section)
        {
            return CGSizeMake(self.bounds.size.width - insets.left - insets.right, ITEM_HEIGHT_ENTRY);
        }
        else if (SECTION_INDEX_TEST2 == indexPath.section)
        {
            return CGSizeMake(self.bounds.size.width - insets.left - insets.right, ITEM_HEIGHT_ENTRY);
        }
        else if (SECTION_INDEX_ITEM1 == indexPath.section)
        {
            return [self itemSizeForItem1:indexPath.item AtPage:m_page];
        }
        else if (SECTION_INDEX_ITEM2 == indexPath.section)
        {
            return [self itemSizeForItem2:indexPath.item AtPage:m_page];
        }
    }
    else
    {
        if (SECTION_INDEX_ITEM_PAGING1 == indexPath.section)
        {
            NSInteger page = collectionView.tag;
            return [self itemSizeForItem1:indexPath.item AtPage:page];
        }
        if (SECTION_INDEX_ITEM_PAGING2 == indexPath.section)
        {
            NSInteger page = collectionView.tag;
            return [self itemSizeForItem2:indexPath.item AtPage:page];
        }
    }
    return CGSizeZero;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section
{
    if (collectionView == self)
    {
        UIEdgeInsets insets = self.contentInset;
        if (SECTION_INDEX_ENTRY == section)
        {
            return CGSizeMake(self.bounds.size.width - insets.left - insets.right, ITEM_HEIGHT_NAVBAR);
        }
        else if (SECTION_INDEX_CATBAR == section)
        {
            return CGSizeMake(self.bounds.size.width - insets.left - insets.right, ITEM_HEIGHT_CATBAR);
        }
    }
    
    return CGSizeZero;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForFooterInSection:(NSInteger)section
{
    if (collectionView == self)
    {
        if (SECTION_INDEX_ENTRY == section)
        {
            // return CGSizeMake(self.bounds.size.width, ITEM_HEIGHT_LOADMORE);
        }
    }
    
    return CGSizeZero;
}

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section
{
    return [self isSection:section itemsInCollectionView:collectionView] ? LINE_SPACING_ITEM : 0.0f;
}

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section
{
    return [self isSection:section itemsInCollectionView:collectionView] ? ITEM_SPACING_ITEM : 0.0f;
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section
{
    if ([self isSection:section itemsInCollectionView:collectionView])
    {
        return UIEdgeInsetsMake(SECTION_INSET_ITEM_TOP, SECTION_INSET_ITEM_LEFT, SECTION_INSET_ITEM_BOTTOM, SECTION_INSET_ITEM_RIGHT);
    }
    
    return UIEdgeInsetsZero;
}

- (UICollectionReusableView *)collectionView:(UICollectionView *)collectionView viewForSupplementaryElementOfKind:(NSString *)kind atIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        if ([kind isEqualToString:UICollectionElementKindSectionHeader])
        {
            if (SECTION_INDEX_ENTRY == indexPath.section)
            {
                SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_NAVBAR forIndexPath:indexPath];
                NSDictionary *ds = @{@"bgColor": m_navbarColor, @"text": @ITEM_TEXT_NAVBAR};
                [cell updateDataSource:[ ds mutableCopy]];

                return cell;
            }
            else if (SECTION_INDEX_CATBAR == indexPath.section)
            {
                 SUICategoryBarViewCell *cell = (SUICategoryBarViewCell *)[collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@REUSE_ID_CATBAR forIndexPath:indexPath];
                cell.backgroundColor = m_catColor;
                [self buildCategoryBarWithFrame:cell.bounds];

                return cell;
            }
        }
        else if ([kind isEqualToString:UICollectionElementKindSectionFooter])
        {
            if (SECTION_INDEX_ENTRY == indexPath.section)
            {
                SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionFooter withReuseIdentifier:@REUSE_ID_LOADMORE forIndexPath:indexPath];
                NSDictionary *ds = @{@"bgColor": m_loadMoreColor, @"text": @ITEM_TEXT_LOADMORE};
                [cell updateDataSource:[ ds mutableCopy]];
                
                return cell;
            }
        }
    }
    
    return nil;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger page = (collectionView == self) ? m_page : collectionView.tag;
    NSInteger itemIndexBase = (collectionView == self) ? SECTION_INDEX_ITEM1 : SECTION_INDEX_ITEM_PAGING1;
    
    if (indexPath.section == itemIndexBase + 1) // MUST check it first
    {
        SUIImageItemViewCell *cell = (SUIImageItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ITEM2 forIndexPath:indexPath];
        cell.fullLineMode = YES;
        [cell updateDataSource:[self itemDictForItem2:indexPath.item AtPage:page]];
        return cell;
    }
    else if (indexPath.section == itemIndexBase) // MUST check it first
    {
        SUIImageItemViewCell *cell = (SUIImageItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ITEM1 forIndexPath:indexPath];
        cell.fullLineMode = (ITEM_COLUMNS == 1);
        [cell updateDataSource:[self itemDictForItem1:indexPath.item AtPage:page]];
        return cell;
    }
    else if (indexPath.section == SECTION_INDEX_TEST2)
    {
        SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ENTRY forIndexPath:indexPath];
        NSMutableDictionary *entry = [m_test2 objectAtIndex:indexPath.item];
        [cell updateDataSource:entry];
        return cell;
    }
    else if (indexPath.section == SECTION_INDEX_TEST1)
    {
        SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ENTRY forIndexPath:indexPath];
        NSMutableDictionary *entry = [m_test1 objectAtIndex:indexPath.item];
        [cell updateDataSource:entry];
        return cell;
    }
    else if (indexPath.section == SECTION_INDEX_ENTRY)
    {
        SUIItemViewCell *cell = (SUIItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_ENTRY forIndexPath:indexPath];
        NSMutableDictionary *entry = [m_entries objectAtIndex:indexPath.item];
        [cell updateDataSource:entry];
        return cell;
    }
    
    return nil;
}

- (void)collectionView:(UICollectionView *)collectionView willDisplaySupplementaryView:(UICollectionReusableView *)view forElementKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        if ([elementKind isEqualToString:UICollectionElementKindSectionHeader])
        {
            if (SECTION_INDEX_CATBAR == indexPath.section)
            {
                if ([view isKindOfClass:[SUICategoryBarViewCell class]])
                {
                    SUICategoryBarViewCell *cell = (SUICategoryBarViewCell *)view;
                    [self buildCategoryBarWithFrame:cell.bounds];
                    [cell attachCategoryBar:m_categoryBarView];
                }
            }
        }
    }
}

- (void)collectionView:(UICollectionView *)collectionView didEndDisplayingSupplementaryView:(UICollectionReusableView *)view forElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath
{
    if (collectionView == self)
    {
        if ([elementKind isEqualToString:UICollectionElementKindSectionHeader])
        {
            if (SECTION_INDEX_CATBAR == indexPath.section)
            {
                if ([view isKindOfClass:[SUICategoryBarViewCell class]])
                {
                    SUICategoryBarViewCell *cell = (SUICategoryBarViewCell *)view;
                    [cell detachCategoryBar];
                }
            }
        }
    }
}

/*
 #pragma mark UIPagingCollectionViewDelegate Implementation
- (BOOL)collectionView:(UICollectionView *)collectionView pagingShouldBeginAtLocation:(CGPoint)location withTranslation:(CGPoint)translation andVelocity:(CGPoint)velocity onSection:(out NSInteger *)section
{
    BOOL shouldBegin = NO;
    
    CGRect rect = [self visibleRectForScrollableSction:SECTION_INDEX_ITEM1];
    if (location.y > CGRectGetMinY(rect) && location.y <= CGRectGetMaxY(rect))
    {
        *section = SECTION_INDEX_ITEM1;
        shouldBegin = YES;
    }
    
    return shouldBegin;
}

- (NSInteger)pageForSection:(NSInteger)section inPagingCollectionView:(UIPagingCollectionView *)pagingCollectionView
{
    if (pagingCollectionView == self)
    {
        return (section == SECTION_INDEX_ITEM) ? m_page : 0;
    }
    
    return 0;
}

- (NSInteger)pageSizeForSection:(NSInteger)section inPagingCollectionView:(UIPagingCollectionView *)pagingCollectionView
{
    if (pagingCollectionView == self)
    {
        return (section == SECTION_INDEX_ITEM) ? m_barItems.count : 0;
    }
    
    return 0;
}

- (UIView *)pagingCollectionView:(UIPagingCollectionView *)pagingCollectionView viewForPage:(NSInteger)page inSection:(NSInteger)section
{
    CGRect frame = [self visibleRectForScrollableSction:section];

    UICollectionView *collectionView = [self buildCollectionViewAtPage:page forView:self withFrame:frame];
    return collectionView;
}

- (BOOL)collectionView:(UIPagingCollectionView *)pagingCollectionView pagingEndedOnSection:(NSInteger)section toNewPage:(NSInteger)page
{
    if (pagingCollectionView == self)
    {
        if (SECTION_INDEX_ITEM == section)
        {
            if (m_page != page)
            {
                // [pagingCollectionView resetSwipedViews];
                // [self resetSwipedViews];
                [self switchPage:page];
            }
        }
    }
    
    return NO;
}

- (void)collectionView:(nonnull UIPagingCollectionView *)pagingCollectionView pagingWithOffset:(CGPoint) offset decelerating:(BOOL)decelerating onSection:(NSInteger)section
{
    UICollectionViewPagingLayout *layout = (UICollectionViewPagingLayout *)self.collectionViewLayout;
    
    layout.pagingOffset = offset;
}
 */

#pragma mark UICollectionViewPagingLayoutDelegate Implementation


- (UICollectionViewFlexLayoutMode)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout layoutModeForSection:(NSInteger)section
{
    return [self isSection:section itemsInCollectionView:collectionView] ? UICollectionViewFlexLayoutModeWaterfall : UICollectionViewFlexLayoutModeFlow;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout numberOfColumnsInSection:(NSInteger)section
{
    NSInteger numberOfColumns = [self isSection:section itemsInCollectionView:collectionView] ? ITEM_COLUMNS : 1;
    return numberOfColumns;
}

- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerEnterStickyModeAtSection:(NSInteger)section withOriginalPoint:(CGPoint)point
{
    if (collectionView == self)
    {
        if (SECTION_INDEX_CATBAR == section)
        {
            m_isCategoryBarSticky = YES;
            m_minPagingTop = point.y + ITEM_HEIGHT_CATBAR;
        }
    }
}

- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerExitStickyModeAtSection:(NSInteger)section
{
    if (collectionView == self)
    {
        if (SECTION_INDEX_CATBAR == section)
        {
            m_isCategoryBarSticky = NO;
            m_minPagingTop = CGFLOAT_MAX;
            m_pageContexts.clear();
        }
    }
}

@end
