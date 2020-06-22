# UICollectionViewFlexLayout
UICollectionViewFlexLayout是继承自UICollectionViewLayout的自定义的UICollectionView布局类，实现了如下功能：

* 类似UICollectionViewFlowLayout的流式布局
* 瀑布流布局
* 对特定cells进行偏移，以支持的分页功能
* 头部视图的置顶功能

![Screenshot](https://github.com/BlueMatthew/UICollectionViewFlexLayout/raw/master/docs/res/layout.gif)


UICollectionViewFlexLayout提供的两种布局模式，流式布局和瀑布流布局，通过如下的协议，由开发者针对不同的section分别进行指定：

> UICollectionViewDelegateFlexLayout::collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout layoutModeForSection:(NSInteger)section.

如果此协议未实现，则所有sections都是默认的流式布局。

## 流式布局的用法

流式布局的控制和UICollectionViewFlowLayout完全类似，请参考文章：[Using the Flow Layout](https://developer.apple.com/library/archive/documentation/WindowsViews/Conceptual/CollectionViewPGforIOS/UsingtheFlowLayout/UsingtheFlowLayout.html)


## 瀑布流布局的用法：

瀑布流布局是基于列的布局方式，首先section被分割成数个列，然后，每个Cell依次被放入高度最小的一个列中（如果多个列的高度相同，则放入第一个最小高度的列中）。在同一个列中的cells居左对齐。

布局如下图所示：

Figure 1 - Laying out sections and cells using the waterfall layout
![Waterfall Layout](https://github.com/BlueMatthew/UICollectionViewFlexLayout/raw/master/docs/res/WaterfallLayout.png)

开发者需要实现继承自UICollectionViewDelegateFlowLayout的协议，UICollectionViewDelegateFlexLayout，提供布局需要的信息。

### 设定section的列数：

> - (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout numberOfColumnsInSection:(NSInteger)section;

如果返回了小于等于0的列数，则自动转换成1列。假设布局存在N个列，则列的宽度如下：

Column Width = (UICollectionView.contentView.bounds.size.width - (SectionInset.left - SectionInset.right) - (N - 1) * (Interitempacing)) / N

类似UICollectionViewFlowLayout, Section Inset由如下协议实现设定：

> - (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section;
如果此协议未实现，则使用UICollectionViewFlexLayout的属性：sectionInsets

### Interitempacing由如下协议设定：

> - (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section;
如果此协议未实现，则使用UICollectionViewFlexLayout的属性：minimumInteritemSpacing


### Cell Size: 

瀑布流的cell size各不相同，常见的场景是cell具有相同的宽度，但是高度各不相同。Cell Size仍然通过UICollectionViewDelegate中定义的协议来提供：

> - (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath;

### 同一个列中，上下两个cells之间的间隔由如下协议实现来设定：

- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section;
如果此协议未实现，则使用UICollectionViewFlexLayout的属性：minimumLinetemSpacing

## 对特定cells进行偏移

某些场景，在某一个特定section之下，进行了分支展示内容。在这个特定section之下，如果进行左右滑动，可以切换各个分支，这种情况下，有可能需要对这个特定section之下的所有cell进行位置的偏移。

### 属性pagingSection指定可以偏移的最小section.
### 属性pagingOffset指定偏移的距离。

## Header置顶

### Header可置顶的section设定

Header可置顶的section通过如下三个方法进行设定

> - (void)addStickyHeader:(NSInteger)section;
> - (void)removeStickyHeader:(NSInteger)section;
> - (void)removeAllStickyHeaders;

所有未置顶的section的Header不被置顶

### 置顶模式
UICollectionViewFlexLayout通过属性stackedStickyHeaders来设定Header置顶的两种模式：
NO: 类似UICollectionViewFlowLayout的sectionHeadersPinToVisibleBounds，当section的区域被滑出可见区域时，处于置顶状态的Header也会被移出。
YES：Header当往上滑动并被置顶后，就一直处于置顶状态，仅当往下滑动需要脱离置顶位置。如果有多个section的Header都满足了置顶条件，则同时叠加在顶部，即都处于置顶状态。这是默认行为。

当某一个section的Header进入或者离开置顶状态，UICollectionViewDelegateFlexLayout如下两个通知协议如果被实现，将会被调用：

> - (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerEnterStickyModeAtSection:(NSInteger)section withOriginalPoint:(CGPoint)point;
> - (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlexLayout *)layout headerExitStickyModeAtSection:(NSInteger)section;

其中，进入置顶状态时，通知会将该Header原本的位置同时传出，供需要时使用。常见的场景是配合分页使用，通过此位置，可以计算出可滑动区域的坐标和大小。

# 实现的几个注意点：

## 布局管理容器

在原有UICollectionView的分组section下，新建立了Row和Column的二级容器（UICollectionViewFlowLayout也是通过Row来组织section下面的布局），其中：

1. Section中的各个row之间，在纵向坐标上是递增的，row内的各个cell，横向坐标上是递增的

2. Section中的各个column之间，在横向坐标上是递增的，column内的各个cell，在纵向坐标上是递增的

这种单向递增的特性，通过二分查找使得确定可见区域内的cells的效率是高效的。

## UICollectionViewFlexLayoutInvalidationContext
UICollectionViewFlexLayout对于Header的置顶，或者左右滑动引起部分cells的位置偏移，都是在重载下面的函数中处理的：
> - (NSArray<UICollectionViewLayoutAttributes *> *)layoutAttributesForElementsInRect:(CGRect)rect;

而这两种行为，本质上都是对部分cell进行临时的偏移，整体layout并没有发生变化。所以在这两种情况下，我们希望重载函数：

> - (NSArray<UICollectionViewLayoutAttributes *> *)layoutAttributesForElementsInRect:(CGRect)rect;

可以随着滑动持续被触发，但是又不用真正的执行prepareLayout，避免性能的无谓损耗。基于这个考虑，新建的InvalidationContext增加了invalidatedOffset属性，滚动期间Header需要置顶，或者分页滑动的部分cells的偏移变化，控制改变之后，都基于此InvalidationContent调用invalidateLayout，同时重载函数：

> - (void)invalidateLayoutWithContext:(UICollectionViewLayoutInvalidationContext *)context;

在这个函数中检查InvalidationContext中的属性，invalidatedOffset，如果该属性值不为YES，则设置一个标记（m_layoutInvalidated）告诉prepareLayout需要被真正的执行，如果是NO，则不改变标记（m_layoutInvalidated）的值。而重载函数：

> - (void)prepareLayout中，如果发现这个标记（m_layoutInvalidated）为 YES，则执行真正的布局处理，处理结束后，重制标记（m_layoutInvalidated）为NO。而因为invalidateLayout被调用过，UICollectionView也会触发下面的重载函数：
> - (NSArray<UICollectionViewLayoutAttributes *> *)layoutAttributesForElementsInRect:(CGRect)rect;

来重新获取可见区域内的cells，从而达到更新特定cell的偏移的目的。

