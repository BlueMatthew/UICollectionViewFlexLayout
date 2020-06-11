//
//  CategoryBarItemViewCell.h
//  PagingSubList
//
//  Created by Matthew Shi on 2020/6/7.
//  Copyright © 2020 Matthew Shi. All rights reserved.
//

#ifndef CategoryBarItemViewCell_h
#define CategoryBarItemViewCell_h

#import <UIKit/UIKit.h>

@interface SUICategoryBarItemViewCell : UICollectionViewCell

@property (nonatomic, nullable, strong) UIBarItem *barItem;
@property (nonatomic) BOOL selectedState;


@end

#endif /* CategoryBarItemViewCell_h */
