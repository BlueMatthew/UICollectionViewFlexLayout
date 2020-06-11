//
//  CategoryBar.m
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "CategoryBar.h"
#import "CategoryBarItemViewCell.h"

#define REUSE_ID_BARITEM      "baritem"

@interface SUICategoryBar() <UICollectionViewDataSource, UICollectionViewDelegateFlowLayout>
{
    NSInteger m_prevSelectedItem;
}

@end

@implementation SUICategoryBar
@synthesize viewDelegate = m_viewDelegate;
@synthesize items = m_items;
@synthesize selectedItem = m_selectedItem;

- (nonnull instancetype)initWithFrame:(CGRect)frame;
{
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    
    [layout setScrollDirection:UICollectionViewScrollDirectionHorizontal];

    layout.minimumInteritemSpacing = 0.0;
    layout.minimumLineSpacing = 0.0;
    layout.sectionInset = UIEdgeInsetsZero;
    layout.footerReferenceSize = CGSizeZero;
    layout.headerReferenceSize = CGSizeZero;
    
    if (self = [super initWithFrame:frame collectionViewLayout:layout])
    {
        m_prevSelectedItem = NSNotFound;
        m_selectedItem = 0;
        
        if (@available(iOS 11.0, *))
        {
            self.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
        
        self.allowsSelection = YES;
        self.allowsMultipleSelection = NO;
        self.dataSource = self;
        self.delegate = self;

        self.backgroundColor = [UIColor colorWithRed:245.0 / 255.0 green:245.0 / 255.0 blue:245.0 / 255.0 alpha:1.0];
        self.showsVerticalScrollIndicator = NO;
        
        [self registerClass:[SUICategoryBarItemViewCell class] forCellWithReuseIdentifier:@REUSE_ID_BARITEM];
    }
    
    return self;
}

- (void)setTintColor:(UIColor *)tintColor
{
    [super setTintColor:tintColor];
}

- (void)setItems:(NSArray<UIBarButtonItem *> *)items
{
    m_items = items;
    if (m_selectedItem > m_items.count - 1)
    {
        m_selectedItem = 0;
    }
    [self reloadData];
}

- (NSInteger)selectedItem
{
    return m_selectedItem;
}

- (void)selectItemAt:(NSInteger)item animated:(BOOL)animated
{
    [self selectItemAtIndexPath:[NSIndexPath indexPathForItem:item inSection:0] animated:animated scrollPosition:UICollectionViewScrollPositionCenteredHorizontally];
    
    NSIndexPath *previousIndexPath = [NSIndexPath indexPathForItem:m_selectedItem inSection:0];
    SUICategoryBarItemViewCell *cell = (SUICategoryBarItemViewCell *)[self cellForItemAtIndexPath:previousIndexPath];
    cell.selectedState = NO;
    
    m_selectedItem = item;
    NSIndexPath *indexPath = [NSIndexPath indexPathForItem:m_selectedItem inSection:0];
    cell = (SUICategoryBarItemViewCell *)[self cellForItemAtIndexPath:indexPath];
    cell.selectedState = YES;
}

- (void)doItemSelectionAtIndexPath:(NSIndexPath *)indexPath
{
    if (m_selectedItem == indexPath.item)
    {
        return;
    }

    NSIndexPath *previousIndexPath = [NSIndexPath indexPathForItem:m_selectedItem inSection:indexPath.section];
    SUICategoryBarItemViewCell *cell = (SUICategoryBarItemViewCell *)[self cellForItemAtIndexPath:previousIndexPath];
    cell.selectedState = NO;
    
    m_selectedItem = indexPath.item;
    cell = (SUICategoryBarItemViewCell *)[self cellForItemAtIndexPath:indexPath];
    cell.selectedState = YES;
    
    if (nil != m_viewDelegate && [m_viewDelegate conformsToProtocol:@protocol(SUICategoryBarDelegate)] && [m_viewDelegate respondsToSelector:@selector(categoryBar:didSelectItemAtIndex:)])
    {
        [m_viewDelegate categoryBar:self didSelectItemAtIndex:indexPath.item];
    }
}

// UICollectionViewDataSource implementation
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return nil == m_items ? 0 : m_items.count;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    return self.itemSize;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (0 == indexPath.section)
    {
        SUICategoryBarItemViewCell *cell = (SUICategoryBarItemViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@REUSE_ID_BARITEM forIndexPath:indexPath];
        
        cell.barItem = [m_items objectAtIndex:indexPath.item];
        cell.selectedState = (indexPath.item == m_selectedItem);
        // cell.backgroundColor = [m_entryColors objectAtIndex: indexPath.item % (m_entryColors.count)];
        // cell.text = [NSString stringWithFormat:@"Entry %lu", indexPath.item];
        
        return cell;
    }
    
    return nil;
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (m_selectedItem == indexPath.item)
    {
        return;
    }

    NSIndexPath *previousIndexPath = [NSIndexPath indexPathForItem:m_selectedItem inSection:indexPath.section];
    SUICategoryBarItemViewCell *cell = (SUICategoryBarItemViewCell *)[self cellForItemAtIndexPath:previousIndexPath];
    cell.selectedState = NO;
    
    m_selectedItem = indexPath.item;
    cell = (SUICategoryBarItemViewCell *)[self cellForItemAtIndexPath:indexPath];
    cell.selectedState = YES;
    
    [self scrollToItemAtIndexPath:indexPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:YES];
    
    if (nil != m_viewDelegate && [m_viewDelegate conformsToProtocol:@protocol(SUICategoryBarDelegate)] && [m_viewDelegate respondsToSelector:@selector(categoryBar:didSelectItemAtIndex:)])
    {
        [m_viewDelegate categoryBar:self didSelectItemAtIndex:indexPath.item];
    }
}

@end;
