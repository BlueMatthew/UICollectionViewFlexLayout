//
//  FlexBatchUpdateContext.h
//  FlexLayout
//
//  Created by Matthew on 2020/7/30.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef FlexBatchUpdateContext_h
#define FlexBatchUpdateContext_h

#import <UIKit/UIKit.h>

// #define USING_INTERNAL_UPDATE_ITEMS_FOR_BATCH_UPDATES
#define USING_MANUAL_UPDATE_ITEMS_FOR_BATCH_UPDATES

// As we can't get NSArray<UICollectionViewUpdateItem> _updateItems from UICollectionViewInvalidationContext
// have to ask caller to provide those info
@interface UIFlexUpdateItem : NSObject

@property (nonatomic, readonly, nullable) NSIndexPath *indexPathBeforeUpdate; // nil for UICollectionUpdateActionInsert
@property (nonatomic, readonly, nullable) NSIndexPath *indexPathAfterUpdate; // nil for UICollectionUpdateActionDelete
@property (nonatomic, readonly) UICollectionUpdateAction updateAction;

@end

@interface UIFlexBatchUpdateContext : NSObject

@property (nonatomic, strong, nullable, readonly) NSArray<UIFlexUpdateItem *> *updateItems;

- (void)insertSections:(nullable NSIndexSet *)sections;
- (void)reloadSections:(nullable NSIndexSet *)sections;
- (void)deleteSections:(nullable NSIndexSet *)sections;
- (void)insertItemsAtIndexPaths:(nullable NSArray<NSIndexPath *> *)indexPaths;
- (void)reloadItemsAtIndexPaths:(nullable NSArray<NSIndexPath *> *)indexPaths;
- (void)deleteItemsAtIndexPaths:(nullable NSArray<NSIndexPath *> *)indexPaths;

@end


#endif /* FlexBatchUpdateContext_h */
