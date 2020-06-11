//
//  ImageItemViewCell.h
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef ImageItemViewCell_h
#define ImageItemViewCell_h

#import <UIKit/UIKit.h>

@interface SUIImageItemViewCell : UICollectionViewCell

@property (nonatomic, assign) BOOL fullLineMode;

- (void)updateDataSource:(nullable NSMutableDictionary *)item;

@end

#endif /* ImageItemViewCell_h */
