//
//  InfoViewCell.h
//  UICollectionViewFlexLayout
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef InfoViewCell_h
#define InfoViewCell_h

#import <UIKit/UIKit.h>

@interface SUIInfoViewCell : UICollectionViewCell

- (void)updateDataSource:(nullable NSMutableDictionary *)item;

@end

#endif /* InfoViewCell_h */
