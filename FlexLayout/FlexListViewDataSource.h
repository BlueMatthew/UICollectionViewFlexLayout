//
//  FlexListViewDataSource.h
//  FlexLayout
//
//  Created by Matthew Shi on 2020/7/24.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//
#import <UIKit/UIKit.h>

#ifndef FlexListViewDataSource_h
#define FlexListViewDataSource_h

@interface ItemData : NSString

@property (nonatomic, strong) NSString *text;
@property (nonatomic, strong, readonly) NSNumber *textColor;
@property (nonatomic, strong, readonly) NSNumber *backgroundColor;
@property (nonatomic, strong) NSString *url;
@property (nonatomic, strong, readonly) NSNumber *imageColor;
@property (nonatomic, assign) BOOL displayed;
@property (nonatomic, assign) CGFloat width;
@property (nonatomic, assign) CGFloat height;

@property (nonatomic, assign, readonly) CGSize size;

@property (nonatomic, strong) NSString *itemType;
@property (nonatomic, strong, readonly) NSArray<UIBarItem *> *barItems;

- (void)setBackgroundColor:(NSInteger)backgroundColor;
- (void)setTextColor:(NSInteger)textColor;
- (void)setImageColor:(NSInteger)imageColor;

- (void)setBarItems:(NSString *)textFormat numberOfBarItems:(NSInteger)numberOfBarItems;

@end

@interface SectionData : NSObject

@property (nonatomic) NSInteger sectionId;
@property (nonatomic) NSInteger columns;
@property (nonatomic, strong)  ItemData *header;
@property (nonatomic, strong)  NSMutableArray<ItemData *> *items;
@property (nonatomic, strong)  ItemData *footer;

- (void)initializeSection:(NSInteger)sectionId forPage:(NSInteger)page withBounds:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets;

- (BOOL)isEntry;
- (BOOL)isTest1;
- (BOOL)isTest2;
- (BOOL)isItem1;
- (BOOL)isItem2;

- (ItemData *)itemAt:(NSInteger)item;
- (BOOL)deleteItemAt:(NSInteger)item;

@end


@interface PageData : NSObject

@property (nonatomic, strong)  NSMutableArray<SectionData *> *sections;

- (void)initializeSections:(CGRect)bounds contentInsets:(UIEdgeInsets)contentInsets;
- (NSInteger)addSection:(NSInteger)sectionId frame:(CGRect)frame insets:(UIEdgeInsets)insets;
- (NSInteger)removeSection:(NSInteger)sectionId;

- (SectionData *)sectionAt:(NSInteger)section;

@end

@interface DataSource : NSObject

@property (nonatomic, strong)  NSMutableArray<PageData *> *pages;

- (instancetype)initWithFrame:(CGRect)frame insets:(UIEdgeInsets)insets numberOfPages:(NSInteger)numberOfPages;

- (void)updateDataSourceAtSection:(NSInteger) section frame:(CGRect)frame insets:(UIEdgeInsets)insets onPage:(NSInteger)page;

- (SectionData *)sectionAt:(NSInteger)section forPage:(NSInteger)page;
- (ItemData *)itemAt:(NSIndexPath *)indexPath forPage:(NSInteger)page;
- (ItemData *)headerAt:(NSInteger)section forPage:(NSInteger)page;
- (ItemData *)footerAt:(NSInteger)section forPage:(NSInteger)page;
- (CGSize)sizeForItem:(NSIndexPath *)indexPath forPage:(NSInteger)page;
- (CGSize)headerSizeAtSection:(NSInteger)section forPage:(NSInteger)page;
- (CGSize)footerSizeAtSection:(NSInteger)section forPage:(NSInteger)page;

- (NSInteger)numberOfSectionsForPage:(NSInteger)page;
- (NSInteger)numberOfItemsAtSection:(NSInteger)section forPage:(NSInteger)page;


- (CGFloat)lineSpaceAtSection:(NSInteger)section forPage:(NSInteger)page;
- (CGFloat)interitemSpaceAtSection:(NSInteger)section forPage:(NSInteger)page;

- (UIEdgeInsets)insetsAtSection:(NSInteger)section forPage:(NSInteger)page;

- (NSInteger)addSection:(NSInteger)sectionId frame:(CGRect)frame insets:(UIEdgeInsets)insets onPage:(NSInteger)page;
- (NSInteger)removeSection:(NSInteger)sectionId forPage:(NSInteger)page;
- (NSInteger)deleteItems:(NSInteger)sectionId itemStart:(NSInteger)itemStart itemCount:(NSInteger)itemCount forPage:(NSInteger)page;

+ (NSIndexSet *)getSectionsForStickyHeader;
+ (NSInteger)numberOfPages;

@end


#endif /* FlexListViewDataSource_h */
