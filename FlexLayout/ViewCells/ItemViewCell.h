//
//  ItemViewCell.h
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef ItemViewCell_h
#define ItemViewCell_h

#import <UIKit/UIKit.h>

@interface SUIItemViewCell : UICollectionViewCell

- (void)updateDataSource:(nullable NSMutableDictionary *)item;

- (void)setText:(nullable NSString *)text;

@end

#endif /* ItemViewCell_h */
