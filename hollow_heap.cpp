#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>

using namespace std;

template <typename T>
class HollowHeap;

template <typename T>
struct Node;

template <typename T>
struct Item {
private:
    T value;
    weak_ptr<Node<T>> node;
public:
    explicit Item(T val) {
        this->value = val;
    }
    T getValue() {
        return value;
    }

    friend class HollowHeap<T>;
    friend class Node<T>;
};

template <typename T>
struct Node: std::enable_shared_from_this<Node<T>> {
private:
    T key;
    shared_ptr<Item<T>> item;
    shared_ptr<Node<T>> next;
    shared_ptr<Node<T>> child;
    weak_ptr<Node<T>> extraParent;
    int rank = 0;
public:
    explicit Node(shared_ptr<Item<T>> initItem) {
        this->item = initItem;
        this->key = initItem->value;
    }

    friend class HollowHeap<T>;
};

template <typename T>
class HollowHeap {
public:
    bool isEmpty();
    T getMin();
    int size();
    shared_ptr<Item<T>> insert(T el);
    void merge(HollowHeap<T> &hh);
    T extractMin();
    void decreaseKey(shared_ptr<Item<T>> &itemToDecrease, T val);
    void deleteItem(shared_ptr<Item<T>> &itemToDelete);
private:
    int count = 0;
    shared_ptr<Node<T>> min;

    shared_ptr<Node<T>> makeNode(shared_ptr<Item<T>> &item);
    shared_ptr<Node<T>> link(shared_ptr<Node<T>> &n1, shared_ptr<Node<T>> &n2);
    void addChild(
            shared_ptr<Node<T>> &futureChild,
            shared_ptr<Node<T>> &futureParent);

    shared_ptr<Node<T>> merge(shared_ptr<Node<T>> &newNode);
    int handleChildrenOfHollowRoot(
            shared_ptr<Node<T>> &hollowRoot,
            vector<shared_ptr<Node<T>>>& fullRoots,
            int maxRank);

    shared_ptr<Node<T>> handleHollowChild(
            shared_ptr<Node<T>>& childOfHollowRoot,
            shared_ptr<Node<T>> &hollowRoot);

    void doUnrankedLinks(
            int maxRank,
            vector<shared_ptr<Node<T>>> &fullRoots);

    void initFullRootsList(vector<shared_ptr<Node<T>>>& fullRoots);
    int doRankedLinks(
            shared_ptr<Node<T>> &node,
            int maxRank,
            vector<shared_ptr<Node<T>>> &fullRoots);
    bool equals(
            const weak_ptr<Node<T>>& weakPtr,
            const shared_ptr<Node<T>>& sharedPtr
    );
};

template <typename T>
T HollowHeap<T>::getMin() {
    if (min == nullptr) {
        throw logic_error("The heap is empty. Not able to get the minimum value");
    } else {
        return min->key;
    }
}

template <typename T>
int HollowHeap<T>::size() {
    return count;
}

template <typename T>
bool HollowHeap<T>::isEmpty() {
    return count == 0;
}

// returns inserted item
template <typename T>
shared_ptr<Item<T>> HollowHeap<T>::insert(T el) {
    shared_ptr<Item<T>> item(new Item<T>(el));
    shared_ptr<Node<T>> newNode = makeNode(item);

    shared_ptr<Node<T>> newMin = merge(newNode);
    count++;

    min = newMin;
    return newNode->item;
}

template <typename T>
void HollowHeap<T>::merge(HollowHeap<T> &hh) {
    shared_ptr<Node<T>> newMin = merge(hh.min);
    count = count + hh.count;

    min = newMin;
}

template <typename T>
T HollowHeap<T>::extractMin() {
    T minVal = min->key;
    deleteItem(min->item);
    return minVal;
}

template <typename T>
void HollowHeap<T>::decreaseKey(shared_ptr<Item<T>> &itemToDecrease, T val) {
    shared_ptr<Node<T>> nodeToDecrease = itemToDecrease->node.lock();
    itemToDecrease->value = val;

    if (nodeToDecrease == min) {
        nodeToDecrease->key = val;
        return;
    }

    shared_ptr<Node<T>> secondParent = makeNode(itemToDecrease);
    secondParent->child = nodeToDecrease;
    nodeToDecrease->item = nullptr;
    nodeToDecrease->extraParent = secondParent;

    if (nodeToDecrease->rank > 2)
        secondParent->rank = nodeToDecrease->rank - 2;

    min = link(secondParent, min);
}

template <typename T>
void HollowHeap<T>::deleteItem(shared_ptr<Item<T>> &itemToDelete) {
    shared_ptr<Node<T>> nodeToDelete = itemToDelete->node.lock();
    nodeToDelete->item->node.reset();
    nodeToDelete->item.reset();
    nodeToDelete->item = nullptr;

    if (min->item != nullptr) { // Non-minimum deletion
        count--;
        return;
    }

    int maxRank = 0;
    vector<shared_ptr<Node<T>>> fullRoots;
    initFullRootsList(fullRoots);

    // iterate through all hollow roots and destroy them
    while (min != nullptr) { // while there are still hollow roots
        shared_ptr<Node<T>> hollowRoot = min;
        min = min->next;

        maxRank = handleChildrenOfHollowRoot(
                hollowRoot,
                fullRoots,
                maxRank
        );

        hollowRoot.reset();
    }

    doUnrankedLinks(maxRank, fullRoots);
    count--;
}

template <typename T>
void HollowHeap<T>::initFullRootsList(
        vector<shared_ptr<Node<T>>> &fullRoots
) {
    fullRoots.resize(log2(count) + 1);
    for (int i = 0; i < fullRoots.size(); i++) {
        fullRoots[i] = nullptr;
    }
}

template <typename  T>
int HollowHeap<T>::handleChildrenOfHollowRoot(
        shared_ptr<Node<T>> &hollowRoot,
        vector<shared_ptr<Node<T>>> &fullRoots,
        int maxRank
) {
    shared_ptr<Node<T>> nextChildOfHollowRoot = hollowRoot->child;
    while (nextChildOfHollowRoot != nullptr) {
        shared_ptr<Node<T>> childOfHollowRoot = nextChildOfHollowRoot;

        // if child of the hollow root is hollow too
        if (childOfHollowRoot->item == nullptr) {
            nextChildOfHollowRoot = handleHollowChild(
                    childOfHollowRoot,
                    hollowRoot
            );
        } else {
            // if child is not hollow, it will become a root after destruction of its (hollow) parent
            // Hence, add it to the list of full roots
            nextChildOfHollowRoot = childOfHollowRoot->next;
            maxRank = doRankedLinks(
                    childOfHollowRoot,
                    maxRank,
                    fullRoots
            );
        }
    }
    return maxRank;
}

// returns next child of the hollow root to be processed
template <typename  T>
shared_ptr<Node<T>> HollowHeap<T>::handleHollowChild(
        shared_ptr<Node<T>> &childOfHollowRoot,
        shared_ptr<Node<T>> &hollowRoot
) {
    shared_ptr<Node<T>> nextChildOfHollowRoot = childOfHollowRoot->next;
    // if the child has only 1 parent, deleting hollowRoot makes childOfHollowRoot a root
    if (!childOfHollowRoot->extraParent.lock()) {
        childOfHollowRoot->next = min;
        min = childOfHollowRoot;
    } else { // if hollowRoot has 2 parents
        if (equals(childOfHollowRoot->extraParent, hollowRoot)) {
            nextChildOfHollowRoot = nullptr;
        } else {
            childOfHollowRoot->next = nullptr;
        }
        childOfHollowRoot->extraParent.reset();
    }
    return nextChildOfHollowRoot;
}

// returns maxRank found so far in fullRoots array
template <typename T>
int HollowHeap<T>::doRankedLinks(
        shared_ptr<Node<T>> &node,
        int maxRank,
        vector<shared_ptr<Node<T>>> &fullRoots
) {
    while (fullRoots[node->rank] != nullptr) {
        node = link(node, fullRoots[node->rank]);
        fullRoots[node->rank] = nullptr;
        node->rank += 1;
    }
    fullRoots[node->rank] = node;
    maxRank = max(maxRank, node->rank);

    return maxRank;
}

template <typename T>
void HollowHeap<T>::doUnrankedLinks(
        int maxRank,
        vector<shared_ptr<Node<T>>> &fullRoots
) {
    for (int i = 0; i <= maxRank; i++) {
        if (fullRoots[i] != nullptr) {
            if (min == nullptr) {
                min = fullRoots[i];
                min->next = nullptr;
            } else {
                min = link(min, fullRoots[i]);
            }
            fullRoots[i] = nullptr;
        }
    }
}

template <typename T>
shared_ptr<Node<T>> HollowHeap<T>::merge(shared_ptr<Node<T>> &newNode) {
    if (min == nullptr) {
        min = newNode;
        return min;
    }

    if (newNode == nullptr) {
        return min;
    }

    return link(min, newNode);
}

template <typename T>
shared_ptr<Node<T>> HollowHeap<T>::makeNode(shared_ptr<Item<T>> &item) {
    shared_ptr<Node<T>> myNode(new Node(item));
    item->node = myNode->shared_from_this();
    return myNode;
}

template <typename T>
shared_ptr<Node<T>> HollowHeap<T>::link(
        shared_ptr<Node<T>> &n1,
        shared_ptr<Node<T>> &n2
) {
    if (n1->key >= n2->key) {
        addChild(n1, n2);
        return n2;
    } else {
        addChild(n2, n1);
        return n1;
    }
}

template <typename T>
void HollowHeap<T>::addChild(
        shared_ptr<Node<T>> &futureChild,
        shared_ptr<Node<T>> &futureParent
) {
    futureChild->next = futureParent->child;
    futureParent->child = futureChild;
    // a root does not have a parent and therefore no next link
    futureParent->next = nullptr;
}

template <typename T>
bool HollowHeap<T>::equals(
        const weak_ptr<Node<T>>& weakPtr,
        const shared_ptr<Node<T>>& sharedPtr
) {
    return !weakPtr.expired() && weakPtr.lock() == sharedPtr;
}