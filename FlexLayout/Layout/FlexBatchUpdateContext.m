//
//  FlexBatchUpdateContext.m
//  FlexLayout
//
//  Created by Matthew on 2020/7/30.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#import "FlexBatchUpdateContext.h"

@interface UIFlexUpdateItem()
{
    // NSIndexPath *m_indexPathBeforeUpdate
}

- (instancetype)initWithItem:(NSIndexPath *)indexPath withAction:(UICollectionUpdateAction)action;

@end

@implementation UIFlexUpdateItem
@synthesize indexPathBeforeUpdate = m_indexPathBeforeUpdate;
@synthesize indexPathAfterUpdate = m_indexPathAfterUpdate;
@synthesize updateAction = m_updateAction;

- (instancetype)initWithItem:(NSIndexPath *)indexPath withAction:(UICollectionUpdateAction)updateAction
{
    if (self = [super init])
    {
        m_indexPathBeforeUpdate = nil;
        m_indexPathAfterUpdate = nil;
        m_updateAction = updateAction;
        if (UICollectionUpdateActionInsert == updateAction)
        {
            m_indexPathAfterUpdate = indexPath;
        }
        else if (UICollectionUpdateActionDelete == updateAction)
        {
            m_indexPathBeforeUpdate = indexPath;
        }
        else if (UICollectionUpdateActionReload == updateAction)
        {
            m_indexPathBeforeUpdate = indexPath;
            m_indexPathAfterUpdate = indexPath;
        }
    }
    
    return self;
}

@end

@interface UIFlexBatchUpdateContext()
{
    NSMutableArray<UIFlexUpdateItem *> *m_updateItems;
}

@end

@implementation UIFlexBatchUpdateContext
@dynamic updateItems;

- (instancetype)init
{
    if (self = [super init])
    {
        m_updateItems = [NSMutableArray<UIFlexUpdateItem *> array];
    }
    
    return self;
}

- (NSArray<UIFlexUpdateItem *> *)updateItems
{
    return m_updateItems;
}

- (void)insertSections:(NSIndexSet *)sections
{
    [self updateSections:sections withAction:UICollectionUpdateActionInsert];
}

- (void)reloadSections:(NSIndexSet *)sections
{
    [self updateSections:sections withAction:UICollectionUpdateActionReload];
}

- (void)deleteSections:(NSIndexSet *)sections
{
    [self updateSections:sections withAction:UICollectionUpdateActionDelete];
}

- (void)updateSections:(NSIndexSet *)sections withAction:(UICollectionUpdateAction)action
{
    if (nil == sections)
    {
        return;
    }
    
    [sections enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL * _Nonnull stop) {
        //
        NSIndexPath *indexPath = [NSIndexPath indexPathForItem:NSNotFound inSection:idx];
        UIFlexUpdateItem *updateItem = [[UIFlexUpdateItem alloc] initWithItem:indexPath withAction:action];
        [m_updateItems addObject:updateItem];
        
    }];
}

- (void)insertItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths
{
    [self updateItems:indexPaths withAction:UICollectionUpdateActionInsert];
}

- (void)reloadItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths
{
    [self updateItems:indexPaths withAction:UICollectionUpdateActionReload];
}

- (void)deleteItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths
{
    [self updateItems:indexPaths withAction:UICollectionUpdateActionDelete];
}

- (void)updateItems:(NSArray<NSIndexPath *> *)indexPaths withAction:(UICollectionUpdateAction)action
{
    if (nil == indexPaths)
    {
        return;
    }
    
    for (NSIndexPath *indexPath in indexPaths)
    {
        UIFlexUpdateItem *updateItem = [[UIFlexUpdateItem alloc] initWithItem:indexPath withAction:action];
        [m_updateItems addObject:updateItem];
    }
}

@end
