#ifndef _BJ_KD_TREE_H
#define _BJ_KD_TREE_H

#include <vector>
#include <memory>
#include <numeric>
#include <iostream>

template <typename PointType, int SplitDimension>
class KDNode
{
public:
    typedef KDNode<PointType, (SplitDimension + 1)%PointType::dimension> ChildType;

    KDNode(size_t ind) : treeIndex(ind) {}
    size_t treeIndex;     // 顶点在list中的位置
    std::unique_ptr<ChildType> leftChild, rightChild;
};


template <typename PointType, typename PointArray=std::vector<PointType> >
class KDTree
{
public:
    KDTree() {}
    KDTree(const PointArray& pointsIn)
    {
        buildTree(pointsIn);
    }

    void buildTree(const PointArray& pointsIn);
    void dumpTreeInorder();

    template <typename F>
    void inorderTraversal(F func);

    std::vector<size_t> getPointsWithinCube(PointType testPoint, double radius);
    size_t findMin(int dimension);

    void dumpNode(size_t i)
    {
        std::cout << points[i] << std::endl;
    }

    void deletePoint(size_t nodeIndex);
    PointType getPoint(size_t nodeIndex)
    {
        return points[nodeIndex];
    }
    void insertPoint(const PointType& p);

private:
    std::unique_ptr<KDNode<PointType, 0> > root;
    PointArray points;
    std::vector<size_t> pointIndeces;

    template <int SplitDimension>
    std::unique_ptr<KDNode<PointType, SplitDimension> > buildSubtree( std::vector<size_t>::iterator begin,
            std::vector<size_t>::iterator end);




    template<int SplitDimension>
    void dumpSubtree(std::unique_ptr<KDNode<PointType, SplitDimension> >& node);

    template<int SplitDimension>
    void getPointsWithinCubeSubtree(PointType testPoint, double queryRange[2*PointType::dimension],
                                    std::unique_ptr<KDNode<PointType, SplitDimension> >& node,
                                    std::vector<size_t>& ret);

    static bool pointInRange(PointType testPoint, double queryRange[2*PointType::dimension])
    {
        for(int i = 0; i < PointType::dimension; ++i)
        {
            if(testPoint.getDimension(i) < queryRange[2*i] || testPoint.getDimension(i) > queryRange[2*i +1])
            {
                return false;
            }
        }
        return true;
    }

    template<int SplitDimension>
    size_t findMinSubtree(int dimension,
                          std::unique_ptr<KDNode<PointType, SplitDimension> >& node);

    template<int SplitDimension>
    std::unique_ptr<KDNode<PointType, SplitDimension> >
    deleteFromSubtree(size_t nodeIndex,
                      std::unique_ptr<KDNode<PointType, SplitDimension> >& node);

    template<int SplitDimension, typename F>
    void inorderTraversalSubtree(F func,
                                 std::unique_ptr<KDNode<PointType, SplitDimension> >& node);


    template<int SplitDimension>
    std::unique_ptr<KDNode<PointType, SplitDimension> >
    insertPointSubtree(std::unique_ptr<KDNode<PointType, SplitDimension> >& node,
                       size_t pointIndex);

};


template<typename PointType, typename PointArray>
void KDTree<PointType, PointArray>::buildTree(const PointArray& pointsIn)
{
    points = pointsIn;
    pointIndeces.resize(points.size());
    std::iota(begin(pointIndeces), end(pointIndeces), 0);
    root = buildSubtree<0>(begin(pointIndeces), end(pointIndeces));
}

template<typename PointType, typename PointArray>
template<int SplitDimension>
std::unique_ptr<KDNode<PointType, SplitDimension> >
KDTree<PointType, PointArray>::buildSubtree(std::vector<size_t>::iterator begin,
        std::vector<size_t>::iterator end)
{

    auto rangeSize = std::distance(begin, end);

    if(rangeSize == 0)
    {
        return std::unique_ptr<KDNode<PointType, SplitDimension> >(nullptr);
    }
    else
    {

        std::sort(begin, end,
                  [this]( size_t a, size_t b)
        {
            return points[a].getDimension(SplitDimension) < points[b].getDimension(SplitDimension);
        });
        auto median = begin + rangeSize/2;
        while(median != begin  &&
                points[*(median)].getDimension(SplitDimension) ==
                points[*(median - 1)].getDimension(SplitDimension))
        {
            --median;
        }
        auto ret = std::unique_ptr<KDNode<PointType, SplitDimension> >
                   ( new KDNode<PointType, SplitDimension>(*median));

        ret->leftChild  = buildSubtree<(SplitDimension +1)%PointType::dimension>(begin, median);
        ret->rightChild = buildSubtree<(SplitDimension +1)%PointType::dimension>(median + 1, end);

        return ret;

    }
}

template<typename PointType, typename PointArray>
void KDTree<PointType, PointArray>::dumpTreeInorder()
{
    dumpSubtree<0>(root);
}

template<typename PointType, typename PointArray>
template<int SplitDimension>
void KDTree<PointType, PointArray>::dumpSubtree(std::unique_ptr<KDNode<PointType, SplitDimension> >& node)
{
    if(node->leftChild)
    {
        std::cout << "dumping left: " << std::endl;
        dumpSubtree<(SplitDimension +1)%PointType::dimension %3>(node->leftChild);
    }
    std::cout << "dumping this: " << std::endl;
    std::cout << node->treeIndex << ": " << points[node->treeIndex] << std::endl;
    if(node->rightChild)
    {
        std::cout << "dumping right: " << std::endl;
        dumpSubtree<(SplitDimension +1)%PointType::dimension %3>(node->rightChild);
    }
}

template<typename PointType, typename PointArray>
std::vector<size_t> KDTree<PointType, PointArray>::getPointsWithinCube(PointType testPoint, double radius)
{

    double queryRange[2*PointType::dimension];
    for(auto i = 0; i < PointType::dimension; ++i)
    {
        queryRange[2*i] = testPoint.getDimension(i) - radius;
        queryRange[2*i +1] = testPoint.getDimension(i) + radius;
    }


    std::vector<size_t> ret;
    getPointsWithinCubeSubtree<0>(testPoint, queryRange, root, ret);

    return ret;
}

template<typename PointType, typename PointArray>
template<int SplitDimension>
void KDTree<PointType, PointArray>::getPointsWithinCubeSubtree(PointType testPoint,
        double queryRange[2*PointType::dimension],
        std::unique_ptr<KDNode<PointType,SplitDimension> >& node,
        std::vector<size_t>& ret)
{

    if(node == nullptr)
        return;

    auto nodePoint = points[node->treeIndex];
    if(pointInRange(nodePoint, queryRange))
    {
        ret.push_back(node->treeIndex);
    }
    if(nodePoint.getDimension(SplitDimension) >= queryRange[2*SplitDimension])
    {
        // 查询左子树范围
        //std::cout << "recurse left" << std::endl;
        getPointsWithinCubeSubtree<(SplitDimension +1)%PointType::dimension>(testPoint,
                queryRange,
                node->leftChild,
                ret);
    }
    if(nodePoint.getDimension(SplitDimension) <= queryRange[2*SplitDimension + 1])
    {
        //查询右子树范围
        //std::cout << "recurse right" << std::endl;
        getPointsWithinCubeSubtree<(SplitDimension +1)%PointType::dimension>(testPoint,
                queryRange,
                node->rightChild,
                ret);
    }
}

template<typename PointType, typename PointArray>
size_t KDTree<PointType, PointArray>::findMin(int dimension)
{
    return findMinSubtree<0>(dimension, root);
}


template<typename PointType, typename PointArray>
template<int SplitDimension>
size_t KDTree<PointType, PointArray>::findMinSubtree(int dimension,
        std::unique_ptr<KDNode<PointType,
        SplitDimension> >& node)
{
    if(SplitDimension == dimension)
    {
        if(node->leftChild == nullptr)
        {
            return node->treeIndex;
        }
        else
        {
            return findMinSubtree<(SplitDimension+1)%PointType::dimension>(dimension,
                    node->leftChild);
        }
    }
    else
    {
        size_t leftMin = 123456, rightMin= 123456;
        if(node->leftChild)
        {
            leftMin = findMinSubtree<(SplitDimension+1)%PointType::dimension>(dimension,
                      node->leftChild);
        }
        if(node->rightChild)
        {
            rightMin = findMinSubtree<(SplitDimension+1)%PointType::dimension>(dimension,
                       node->rightChild);
        }

        auto nodeValue = points[node->treeIndex].getDimension(dimension);
        if(node->leftChild &&
                points[leftMin].getDimension(dimension) <
                nodeValue)
        {

            if(node->rightChild)
            {
                return (points[leftMin].getDimension(dimension) <
                        points[rightMin].getDimension(dimension)) ? leftMin : rightMin;

            }
            else
            {
                return leftMin;
            }
        }
        else if(node->rightChild &&
                points[rightMin].getDimension(dimension) <
                nodeValue)
        {
            return rightMin;
        }
        else
        {
            return node->treeIndex;
        }
    }
}

template<typename PointType, typename PointArray>
void KDTree<PointType, PointArray>::deletePoint(size_t nodeIndex)
{

    root = deleteFromSubtree<0>(nodeIndex, root);
}

template<typename PointType, typename PointArray>
template<int SplitDimension>
std::unique_ptr<KDNode<PointType, SplitDimension> >
KDTree<PointType, PointArray>::deleteFromSubtree(size_t nodeIndex,
        std::unique_ptr<KDNode<PointType, SplitDimension> >& node)
{

    constexpr size_t nextDimension = (SplitDimension +1)%PointType::dimension;

    if(node->treeIndex == nodeIndex)
    {
        if(node->rightChild)
        {
            auto rightMin = findMinSubtree<nextDimension>(SplitDimension, node->rightChild);
            node->treeIndex = rightMin;
            node->rightChild = deleteFromSubtree<nextDimension>(rightMin,
                               node->rightChild);
        }
        else if(node->leftChild)
        {
            auto leftMin = findMinSubtree<nextDimension>(SplitDimension, node->leftChild);
            node->treeIndex = leftMin;
            node->rightChild = deleteFromSubtree<nextDimension>(leftMin,
                               node->leftChild);
            node->leftChild = nullptr;
        }
        else
        {
            return nullptr;
        }
    }
    else if(points[nodeIndex].getDimension(SplitDimension) <
            points[node->treeIndex].getDimension(SplitDimension))
    {

        node->leftChild = deleteFromSubtree<nextDimension>(nodeIndex,
                          node->leftChild);
    }
    else
    {
        node->rightChild = deleteFromSubtree<nextDimension>(nodeIndex,
                           node->rightChild);
    }
    return std::move(node);

}

template<typename PointType, typename PointArray>
template <typename F>
void KDTree<PointType, PointArray>::inorderTraversal(F func)
{

    inorderTraversalSubtree<0, F>(func, root);

}

template<typename PointType, typename PointArray>
template<int SplitDimension, typename F>
void KDTree<PointType, PointArray>::inorderTraversalSubtree(F func,
        std::unique_ptr<KDNode<PointType,
        SplitDimension> >& node)
{
    auto constexpr nextDimension = (SplitDimension +1)%PointType::dimension;
    if(node->leftChild)
    {
        inorderTraversalSubtree<nextDimension, F>(func, node->leftChild);
    }
    func(points[node->treeIndex]);
    if(node->rightChild)
    {
        inorderTraversalSubtree<nextDimension, F>(func, node->rightChild);
    }

}

template<typename PointType, typename PointArray>
void KDTree<PointType, PointArray>::insertPoint(const PointType& point)
{
    points.push_back(point);
    root = insertPointSubtree<0>(root, points.size() -1);
}

template<typename PointType, typename PointArray>
template<int SplitDimension>
std::unique_ptr<KDNode<PointType, SplitDimension> >
KDTree<PointType, PointArray>::insertPointSubtree(std::unique_ptr<KDNode<PointType, SplitDimension> >& node,
        size_t pointIndex)
{

    auto constexpr nextDimension = (SplitDimension +1)%PointType::dimension;

    if(node == nullptr)
    {
        return std::unique_ptr<KDNode<PointType, SplitDimension> > (new KDNode<PointType, SplitDimension>(pointIndex));
    }
    else if (points[pointIndex].getDimension(SplitDimension) <
             points[node->treeIndex].getDimension(SplitDimension))
    {
        node->leftChild = insertPointSubtree<nextDimension>(node->leftChild, pointIndex);
    }
    else
    {
        node->rightChild = insertPointSubtree<nextDimension>(node->rightChild, pointIndex);
    }
    return std::move(node);

}
#endif //_BJ_KD_TREE_H
