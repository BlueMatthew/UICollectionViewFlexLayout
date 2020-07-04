# UICollectionViewFlexLayout 
[中文说明](https://github.com/BlueMatthew/UICollectionViewFlexLayout/edit/master/README_CN.md)

Custom Layout of UICollectionView including below features:
* FlowLayout just like UICollectionViewFlowLayout
* Waterfall Layout
* Pagination
* Sticky Headers

![Screenshot](https://github.com/BlueMatthew/UICollectionViewFlexLayout/raw/master/docs/res/layout.gif)

UICollectionViewFlexLayout provides two layout mode, one is as UICollectionFlowLayout and another is waterfall layout. Developer can implements following protocol to tell layout which layout mode the section is:

UICollectionViewDelegateFlexLayout::collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout layoutModeForSection:(NSInteger)section.

* ## Using the Flow Layout

You may refer to the article of [Using the Flow Layout](https://developer.apple.com/library/archive/documentation/WindowsViews/Conceptual/CollectionViewPGforIOS/UsingtheFlowLayout/UsingtheFlowLayout.html) from Apple developer Docs for UICollectionFlowLayout usage. Next, we will focus on the Waterfall Layout mode.


* ## Using the Waterfall Layout

The waterfall layout is a column-based spliting layout. With waterfall layout mode, a section is splited by multiple columns and each cell is placed into the column which height is the lowest. All of cells in the same column are left-aligned.

Figure 1 - Laying out sections and cells using the waterfall layout
![Waterfall Layout](https://github.com/BlueMatthew/UICollectionViewFlexLayout/raw/master/docs/res/WaterfallLayout.png)

## Customizing the Waterfall Layout Attributes

The waterfall layout object use as same properties as flow layout objects for configuring the appearance of the content. When set, these properties are applied to all items equally in the layout.

If you want to vary the spacing or size of items dynamically, you can do so using the methods of the UICollectionViewDelegateFlowLayout protocol. You implement these methods on the same delegate object you assigned to the collection view itself. If a given method exists, the flow layout object calls that method instead of using the fixed value it has. Your implementation must then return appropriate values for all of the items in the collection view.

### Specifying the Size of Items in the Flow Layout

If all of the items in the collection view are the same size, assign the appropriate width and height values to the itemSize property of the flow layout object. (Always specify the size of items in points.) This is the fastest way to configure the layout object for content whose size does not vary.

If you want to specify different sizes for your cells, you must implement the collectionView:layout:sizeForItemAtIndexPath: method on the collection view delegate. You can use the provided index path information to return the size of the corresponding item. During layout, the flow layout object centers items vertically on the same line, as shown in Figure 3-2. The overall height or width of the line is then determined by the largest item in that dimension.

Specifying the Space Between Items and Lines

Using the flow layout, you can specify the minimum spacing between items on the same line and the minimum spacing between successive lines. Keep in mind that the spacing you provide is only the minimum spacing. Because of how it lays out content, the flow layout object may increase the spacing between items to a value greater than the one you specified. The layout object may similarly increase the actual line-spacing when the items being laid out are different sizes.

During layout, the flow layout object adds items to the current line until there is not enough space left to fit an entire item. If the line is just big enough to fit an integral number of items with no extra space, then the space between the items would be equal to the minimum spacing. If there is extra space at the end of the line, the layout object increases the interitem spacing until the items fit evenly within the line boundaries, as shown in Figure 3-3. Increasing the spacing improves the overall look of the items and prevents large gaps at the end of each line.

Using Section Insets to Tweak the Margins of Your Content

Section insets are a way to adjust the space available for laying out cells. You can use insets to insert space after a sections header view and before its footer view. You can also use insets to insert space around the sides of the content. Figure 3-5 demonstrates how insets affect some content in a vertically scrolling flow layout.












