//
//  FlexListViewDataSource.m
//  FlexLayout
//
//  Created by Matthew Shi on 2020/7/24.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "FlexListViewDataSource.h"

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

#define SECTION_INDEX_ITEM_PAGING1      0
#define SECTION_INDEX_ITEM_PAGING2      1

#define NUM_OF_ITEMS_IN_CATEGORY_BAR    8

#define NUM_OF_ITEMS_IN_SECTION_ENTRY   1
#define NUM_OF_ITEMS_IN_SECTION_TEST1   1
#define NUM_OF_ITEMS_IN_SECTION_TEST2   1
#define NUM_OF_ITEMS_IN_SECTION_ITEM1   20
#define NUM_OF_ITEMS_IN_SECTION_ITEM2   8

#define ITEM_HEIGHT_NAVBAR              100
#define ITEM_HEIGHT_ENTRY               120
#define ITEM_HEIGHT_TEST1               100
#define ITEM_HEIGHT_TEST2               80
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


@interface ItemData()
{
    NSMutableDictionary             *m_data;
    NSMutableArray<UIBarItem *>     *m_barItems;
}

@end

@implementation ItemData
@dynamic text;
@dynamic textColor;
@dynamic backgroundColor;
@dynamic imageColor;
@dynamic url;
@dynamic size;

- (instancetype)init
{
    if (self = [super init])
    {
        m_data = [NSMutableDictionary dictionary];
    }
    
    return self;
}

- (NSString *)text
{
    NSString *text = [m_data objectForKey:@"text"];
    return text;
}

- (NSString *)url
{
    return nil;
}

- (void)setText:(NSString *)text
{
    [m_data setObject:text forKey:@"text"];
}

- (NSNumber *)backgroundColor
{
    return [m_data objectForKey:@"backgroundColor"];
}

- (NSNumber *)textColor
{
    return [m_data objectForKey:@"textColor"];
}

- (NSNumber *)imageColor
{
    return  [m_data objectForKey:@"imageColor"];
}

- (CGSize)size
{
    return CGSizeMake(self.width, self.height);
}

- (void)setBackgroundColor:(NSInteger)backgroundColor
{
    [m_data setObject:[NSNumber numberWithInteger:backgroundColor] forKey:@"backgroundColor"];
}

- (void)setTextColor:(NSInteger)textColor
{
    [m_data setObject:[NSNumber numberWithInteger:textColor] forKey:@"textColor"];
}

- (void)setImageColor:(NSInteger)imageColor
{
    [m_data setObject:[NSNumber numberWithInteger:imageColor] forKey:@"imageColor"];
}

- (NSArray<UIBarItem *> *)barItems
{
    return m_barItems;
}

- (void)setBarItems:(NSString *)textFormat numberOfBarItems:(NSInteger)numberOfBarItems
{
    NSMutableDictionary<NSAttributedStringKey, id> *attributesForNormal = [NSMutableDictionary<NSAttributedStringKey, id> dictionaryWithCapacity:4];
    [attributesForNormal setObject:[UIColor blackColor] forKey:NSForegroundColorAttributeName];
    
    NSMutableDictionary<NSAttributedStringKey, id> *attributesForSelected = [NSMutableDictionary<NSAttributedStringKey, id> dictionaryWithCapacity:4];
    [attributesForSelected setObject:[UIColor redColor] forKey:NSForegroundColorAttributeName];
    [attributesForSelected setObject:[UIFont systemFontOfSize:[UIFont labelFontSize] weight:UIFontWeightBold] forKey:NSFontAttributeName];
    
    m_barItems = [NSMutableArray<UIBarItem *> arrayWithCapacity:numberOfBarItems];
    for (NSInteger item = 0; item < numberOfBarItems; item++)
    {
        UIBarItem *barItem = [[UIBarButtonItem alloc] init];
        // barButtonItem
        barItem.title = [NSString stringWithFormat:textFormat, item];
        barItem.tag = item;
        // barItem.width = self.bounds.size.width / 4;
        
        [barItem setTitleTextAttributes:attributesForNormal forState:UIControlStateNormal];
        [barItem setTitleTextAttributes:attributesForSelected forState:UIControlStateFocused];
        
        [m_barItems addObject:barItem];
    }
}

@end


@interface SectionData()
{
}

@end

@implementation SectionData
@synthesize sectionId = m_sectionId;
@synthesize columns = m_columns;
@synthesize header = m_header;
@synthesize items = m_items;
@synthesize footer = m_footer;

- (instancetype)init
{
    if (self = [super init])
    {
        m_columns = 1;
    }
    
    return self;
}

- (ItemData *)itemAt:(NSInteger)item
{
    if (item >= m_items.count)
    {
        return nil;
    }
    
    return [m_items objectAtIndex:item];
}

- (BOOL)isEntry
{
    return m_sectionId == SECTION_INDEX_ENTRY;
}
- (BOOL)isTest1
{
    return m_sectionId == SECTION_INDEX_TEST1;
}

- (BOOL)isTest2
{
    return m_sectionId == SECTION_INDEX_TEST2;
}

- (BOOL)isItem1
{
    return m_sectionId == SECTION_INDEX_ITEM1;
}

- (BOOL)isItem2
{
    return m_sectionId == SECTION_INDEX_ITEM2;
}

- (void)initializeSection:(NSInteger)sectionId forPage:(NSInteger)page withBounds:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets
{
    m_sectionId = sectionId;
    [self initializeDataSource:page withBounds:bounds contentInsets:contentInsets];
}

- (void)initializeDataSource:(NSInteger)page withBounds:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets
{
    // m_sections = [NSMutableDictionary<NSNumber *, NSMutableArray< NSNumber * > *> dictionaryWithCapacity:NUM_OF_ITEMS_IN_CATEGORY_BAR];
    // m_sections = [NSMutableDictionary<NSString *, NSMutableArray< NSNumber * > *> dictionaryWithCapacity:NUM_OF_ITEMS_IN_CATEGORY_BAR];
    
    unsigned int itemColors[] = {0xB0E0E6, 0x87CEFA, 0x87CEEB, 0x00BFFF, 0x1E90FF, 0x6495ED, 0x4169E1, 0x0000FF, 0xB0E0E6, 0x87CEFA, 0x87CEEB, 0x00BFFF, 0x1E90FF, 0x6495ED, 0x4169E1, 0x0000FF};
    
    unsigned int imageColors[] = {0x800000, 0x8B0000, 0xA52A2A, 0xB22222, 0xDC143C, 0xFF0000, 0xFF6347, 0xFF7F50, 0xCD5C5C, 0xF08080, 0xE9967A, 0xFA8072, 0xFFA07A, 0xFF4500, 0xFF8C00, 0xFFA500, 0xFFD700, 0xB8860B, 0xDAA520, 0xEEE8AA, 0xBDB76B, 0xF0E68C, 0x808000, 0xFFFF00, 0x9ACD32, 0x556B2F, 0x6B8E23, 0x7CFC00, 0x7FFF00, 0xADFF2F, 0x006400, 0x008000, 0x228B22, 0x00FF00, 0x32CD32, 0x90EE90, 0x98FB98, 0x8FBC8F, 0x00FA9A, 0x00FF7F, 0x2E8B57, 0x66CDAA, 0x3CB371, 0x20B2AA, 0x2F4F4F, 0x008080, 0x008B8B, 0x00FFFF, 0x00FFFF, 0xE0FFFF, 0x00CED1, 0x40E0D0, 0x48D1CC, 0xAFEEEE, 0x7FFFD4, 0xB0E0E6, 0x5F9EA0, 0x4682B4, 0x6495ED, 0x00BFFF, 0x1E90FF, 0xADD8E6, 0x87CEEB, 0x87CEFA, 0x191970, 0x000080, 0x00008B, 0x0000CD, 0x0000FF, 0x4169E1, 0x8A2BE2, 0x4B0082, 0x483D8B, 0x6A5ACD, 0x7B68EE, 0x9370DB, 0x8B008B, 0x9400D3, 0x9932CC, 0xBA55D3, 0x800080, 0xD8BFD8, 0xDDA0DD, 0xEE82EE, 0xFF00FF, 0xDA70D6, 0xC71585, 0xDB7093, 0xFF1493, 0xFF69B4, 0xFFB6C1, 0xFFC0CB, 0xFAEBD7, 0xF5F5DC, 0xFFE4C4, 0xFFEBCD, 0xF5DEB3, 0xFFF8DC, 0xFFFACD, 0xFAFAD2, 0xFFFFE0, 0x8B4513, 0xA0522D, 0xD2691E, 0xCD853F, 0xF4A460, 0xDEB887, 0xD2B48C, 0xBC8F8F, 0xFFE4B5, 0xFFDEAD, 0xFFDAB9, 0xFFE4E1, 0xFFF0F5, 0xFAF0E6, 0xFDF5E6, 0xFFEFD5, 0xFFF5EE, 0xF5FFFA, 0x708090, 0x778899, 0xB0C4DE, 0xE6E6FA, 0xFFFAF0, 0xF0F8FF, 0xF8F8FF, 0xF0FFF0, 0xFFFFF0, 0xF0FFFF, 0xFFFAFA, 0x000000, 0x696969, 0x808080, 0xA9A9A9, 0xC0C0C0, 0xD3D3D3, 0xDCDCDC, 0xF5F5F5, 0xFFFFFF};
    
    // Using a array for variable heights
    unsigned int itemHeights1[] = {40, 65, 55, 70, 60, 85, 75, 100};
    unsigned int itemHeights2[] = {ITEM_HEIGHT_ITEM2};
    
    if (m_sectionId == SECTION_INDEX_ENTRY)
    {
        // Nav + Entry
        m_header = [[ItemData alloc] init];
        m_header.text = @ITEM_TEXT_NAVBAR;
        m_header.backgroundColor = 0xFF7F50; // coral
        m_header.width = [self calcWidth:bounds contentInsets:contentInsets];
        m_header.height = ITEM_HEIGHT_NAVBAR;
        
        unsigned int entryColors[] = {0x7CFC00, 0x32CD32, 0x006400, 0x9ACD32, 0x00FA9A, 0x98FB98, 0x808000, 0x6B8E23};
        int bgColorIndex = (int)m_sectionId * 4;
        m_items = [[NSMutableArray<ItemData *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_ENTRY];
        for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_SECTION_ENTRY; idx++, bgColorIndex++)
        {
            ItemData *item = [[ItemData alloc] init];
            
            item.backgroundColor = entryColors[bgColorIndex % (sizeof(entryColors) / sizeof(unsigned int))];
            item.text = [NSString stringWithFormat:@"Entry %ld", idx];
            item.width = [self calcWidth:bounds contentInsets:contentInsets];
            item.height = ITEM_HEIGHT_ENTRY;
            
            [m_items addObject:item];
        }
        
    }
    else if (m_sectionId == SECTION_INDEX_TEST1)
    {
        unsigned int entryColors[] = {0x7CFC00, 0x32CD32, 0x006400, 0x9ACD32, 0x00FA9A, 0x98FB98, 0x808000, 0x6B8E23};
        int bgColorIndex = (int)(m_sectionId * 4);
        m_items = [[NSMutableArray<ItemData *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_TEST1];
        for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_SECTION_ENTRY; idx++, bgColorIndex++)
        {
            ItemData *item = [[ItemData alloc] init];
            
            item.backgroundColor = entryColors[bgColorIndex % (sizeof(entryColors) / sizeof(unsigned int))];
            item.text = [NSString stringWithFormat:@"Test1 %ld", idx];
            item.width = [self calcWidth:bounds contentInsets:contentInsets];
            item.height = ITEM_HEIGHT_TEST1;
            
            [m_items addObject:item];
        }
        
    }
    else if (m_sectionId == SECTION_INDEX_TEST2)
    {
        
        unsigned int entryColors[] = {0x7CFC00, 0x32CD32, 0x006400, 0x9ACD32, 0x00FA9A, 0x98FB98, 0x808000, 0x6B8E23};
        int bgColorIndex = (int)(m_sectionId * 4);
        m_items = [[NSMutableArray<ItemData *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_TEST2];
        for (NSInteger idx = 0; idx < NUM_OF_ITEMS_IN_SECTION_ENTRY; idx++, bgColorIndex++)
        {
            ItemData *item = [[ItemData alloc] init];
            
            item.backgroundColor = entryColors[bgColorIndex % (sizeof(entryColors) / sizeof(unsigned int))];
            item.text = [NSString stringWithFormat:@"Test2 %ld", idx];
            item.width = [self calcWidth:bounds contentInsets:contentInsets];
            item.height = ITEM_HEIGHT_TEST2;
            
            [m_items addObject:item];
        }
    }
    else if (m_sectionId == SECTION_INDEX_CATBAR)
    {
        m_header = [[ItemData alloc] init];
        // m_header.text = @ITEM_TEXT_NAVBAR;
        m_header.backgroundColor = 0xFEA460; // sandybrown
        [m_header setBarItems:@ITEM_TEXT_CATBAR_ITEM numberOfBarItems:NUM_OF_ITEMS_IN_CATEGORY_BAR];
        
        m_header.width = [self calcWidth:bounds contentInsets:contentInsets];
        m_header.height = ITEM_HEIGHT_CATBAR;
        
    }
    else if (m_sectionId == SECTION_INDEX_ITEM1 || m_sectionId == SECTION_INDEX_ITEM2)
    {
        m_items = [[NSMutableArray<ItemData *> alloc] initWithCapacity:NUM_OF_ITEMS_IN_SECTION_ITEM1];
        NSMutableArray *itemColorArray = [NSMutableArray array];
        for (int idx = 0; idx < (sizeof(itemColors) / sizeof(unsigned int)); idx++)
        {
            [itemColorArray addObject:[NSNumber numberWithInteger:itemColors[idx]]];
        }
        NSMutableArray *imageColorArray = [NSMutableArray array];
        for (int idx = 0; idx < (sizeof(imageColors) / sizeof(unsigned int)); idx++)
        {
            [imageColorArray addObject:[NSNumber numberWithInteger:imageColors[idx]]];
        }
        NSMutableArray *itemHeights1Array = [NSMutableArray array];
        if (m_sectionId == SECTION_INDEX_ITEM1)
        {
            for (int idx = 0; idx < (sizeof(itemHeights1) / sizeof(unsigned int)); idx++)
            {
                [itemHeights1Array addObject:[NSNumber numberWithInteger:itemHeights1[idx]]];
            }
        }
        else
        {
            for (int idx = 0; idx < (sizeof(itemHeights2) / sizeof(unsigned int)); idx++)
            {
                [itemHeights1Array addObject:[NSNumber numberWithInteger:itemHeights2[idx]]];
            }
        }
        
        NSInteger columns = (m_sectionId == SECTION_INDEX_ITEM1) ? ITEM_COLUMNS : 1;
        NSString *textFormat = (m_sectionId == SECTION_INDEX_ITEM1) ? @ITEM_TEXT_ITEM1 : @ITEM_TEXT_ITEM2;
        NSInteger numberOfItems = (m_sectionId == SECTION_INDEX_ITEM1) ? NUM_OF_ITEMS_IN_SECTION_ITEM1 : NUM_OF_ITEMS_IN_SECTION_ITEM2;
        
        [self initializeItems:m_items numberOfItemsInSection:numberOfItems textFormat:textFormat columns:columns itemColors:itemColorArray  imageColors:imageColorArray itemHeights:itemHeights1Array forPage:page withBounds:bounds contentInsets:contentInsets];
    }
}

- (CGFloat)calcWidth:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets
{
    CGFloat width = bounds.size.width - contentInsets.left - contentInsets.right;
    return ceil(width / self.columns);
}

- (void)initializeItems:(NSMutableArray<ItemData *> *)items numberOfItemsInSection:(NSInteger)numberOfItems textFormat:(NSString *)textFormat columns:(NSInteger)columns itemColors:(NSArray *)itemColors imageColors:(NSArray *)imageColors itemHeights:(NSArray *)itemHeights forPage:(NSInteger)page  withBounds:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets
{
    NSMutableArray<NSNumber *> *widthOfColumns = [[NSMutableArray<NSNumber *> alloc] initWithCapacity:ITEM_COLUMNS];
    CGFloat availableColumnSize = bounds.size.width - (contentInsets.left + contentInsets.right + SECTION_INSET_ITEM_LEFT + SECTION_INSET_ITEM_RIGHT);
    for (NSInteger idx = 0; idx < columns; idx++)
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
    
    itemWidth = [self calcWidth:bounds contentInsets:contentInsets];
    
    NSInteger bgIndex = 4 * page;
    NSInteger imageColorIndex = 16 * page;
    
    for (NSInteger idx = 0; idx < numberOfItems; idx++, imageColorIndex+=8, bgIndex++)
    {
        ItemData* item = [[ItemData alloc] init];
        
        NSNumber *color = [itemColors objectAtIndex:(bgIndex % itemColors.count)];
        [item setBackgroundColor:[color integerValue]];
        
        color = [imageColors objectAtIndex:(imageColorIndex % imageColors.count)];
        [item setImageColor:[color integerValue]];
        
        item.displayed = NO;
        
        item.width = itemWidth;
        CGFloat itemHeight = 0;
        if (columns == 1)
        {
            itemHeight = [[itemHeights objectAtIndex:(idx % itemHeights.count)] integerValue];
        }
        else
        {
            itemHeight = floor(itemWidth) + [[itemHeights objectAtIndex:(idx % itemHeights.count)] integerValue];
        }
        item.height = itemHeight;
        item.itemType = @"item";
        item.text = [NSString stringWithFormat:textFormat, page, idx];
        
        [items addObject:item];
    }
}

@end


@implementation PageData
@synthesize sections = m_sections;

- (void)initializeSections:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets
{
    m_sections = [NSMutableArray<SectionData *> array];
    
    NSArray<NSNumber *> *sections = @[/*@SECTION_INDEX_NAVBAR, */@SECTION_INDEX_ENTRY, @SECTION_INDEX_TEST1, @SECTION_INDEX_TEST2, @SECTION_INDEX_CATBAR, @SECTION_INDEX_ITEM1, @SECTION_INDEX_ITEM2];
    
    for (NSInteger idx = 0; idx < sections.count; idx++)
    {
        SectionData *sectionData = [[SectionData alloc] init];
        sectionData.sectionId = [[sections objectAtIndex:idx] integerValue];
        sectionData.columns = (sectionData.sectionId == SECTION_INDEX_ITEM1) ? 2 : 1;
        [sectionData initializeDataSource:idx withBounds:bounds contentInsets:contentInsets];
        
        [m_sections addObject:sectionData];
    }
}

- (NSInteger)removeSection:(NSInteger)sectionId
{
    NSInteger secionIndex = NSNotFound;
    for (int idx = 0; idx < m_sections.count; idx++)
    {
        SectionData *sectionData = [m_sections objectAtIndex:idx];
        if (sectionData.sectionId == sectionId)
        {
            secionIndex = idx;
            [m_sections removeObjectAtIndex:idx];
            break;
        }
    }
    
    return secionIndex;
}

- (SectionData *)sectionAt:(NSInteger)section
{
    if (section >= m_sections.count)
    {
        return nil;
    }
    
    return [m_sections objectAtIndex:section];
}

@end

@implementation DataSource
@synthesize pages = m_pages;

- (instancetype)initWithFrame:(CGRect)frame insets:(UIEdgeInsets)insets numberOfPages:(NSInteger)numberOfPages
{
    if (self = [super init])
    {
        m_pages = [NSMutableArray<PageData *> array];
        for (NSInteger idx = 0; idx < numberOfPages; idx++)
        {
            PageData *pageData = [[PageData alloc] init];
            
            [pageData initializeSections:frame contentInsets:insets];
            
            [m_pages addObject:pageData];
        }
    }
    
    return self;
}

- (SectionData *)sectionAt:(NSInteger)section forPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return nil;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    return [pageData sectionAt:section];
}

- (ItemData *)itemAt:(NSIndexPath *)indexPath forPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return nil;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    SectionData *sectionData = [pageData sectionAt:indexPath.section];
    if (nil == sectionData)
    {
        return nil;
    }
    
    return [sectionData itemAt:indexPath.item];
}

- (CGSize)sizeForItem:(NSIndexPath *)indexPath forPage:(NSInteger)page
{
    ItemData *itemData = [self itemAt:indexPath forPage:page];
    return nil == itemData ? CGSizeZero : itemData.size;
}

- (ItemData *)headerAt:(NSInteger)section forPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return nil;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    SectionData *sectionData = [pageData sectionAt:section];
    if (nil == sectionData)
    {
        return nil;
    }
    
    return sectionData.header;
}

- (ItemData *)footerAt:(NSInteger)section forPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return nil;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    SectionData *sectionData = [pageData sectionAt:section];
    if (nil == sectionData)
    {
        return nil;
    }
    
    return sectionData.footer;
}

- (CGSize)headerSizeAtSection:(NSInteger)section forPage:(NSInteger)page
{
    ItemData *header = [self headerAt:section forPage:page];
    return nil == header ? CGSizeZero : header.size;
}

- (CGSize)footerSizeAtSection:(NSInteger)section forPage:(NSInteger)page
{
    ItemData *footer = [self footerAt:section forPage:page];
    return nil == footer ? CGSizeZero : footer.size;
}

- (CGFloat)lineSpaceAtSection:(NSInteger)section forPage:(NSInteger)page
{
    return 0;
}

- (CGFloat)interitemSpaceAtSection:(NSInteger)section forPage:(NSInteger)page
{
    return 0;
}

- (UIEdgeInsets)insetsAtSection:(NSInteger)section forPage:(NSInteger)page
{
    return UIEdgeInsetsZero;
    // return UIEdgeInsetsMake(SECTION_INSET_ITEM_TOP, SECTION_INSET_ITEM_LEFT, SECTION_INSET_ITEM_BOTTOM, SECTION_INSET_ITEM_RIGHT);
}

- (NSInteger)numberOfSectionsForPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return 0;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    return pageData.sections.count;
}

- (NSInteger)numberOfItemsAtSection:(NSInteger)section forPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return 0;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    SectionData *sectionData = [pageData sectionAt:section];
    if (nil == sectionData)
    {
        return 0;
    }
    
    return sectionData.items.count;
}

- (NSInteger)removeSection:(NSInteger)sectionId forPage:(NSInteger)page
{
    if (page >= m_pages.count)
    {
        return NSNotFound;
    }
    
    PageData *pageData = [m_pages objectAtIndex:page];
    return [pageData removeSection:sectionId];
}

+ (NSIndexSet *)getSectionsForStickyHeader
{
    NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSet];
    [indexSet addIndex:SECTION_INDEX_ENTRY];
    [indexSet addIndex:SECTION_INDEX_CATBAR];
    
    return indexSet;
}

+ (NSInteger)numberOfPages
{
    return NUM_OF_ITEMS_IN_CATEGORY_BAR;
}

@end
