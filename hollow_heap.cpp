#include <vector>
#include <cmath>

using namespace std;

template <typename T>
class HollowHeap;

template <typename T>
struct Node;

template <typename T>
struct Item {
private:
    T value;
    Node<T>* node;
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
struct Node {
private:
    T key;
    Item<T> *item;
    Node<T> *next = nullptr;
    Node<T> *child = nullptr;
    Node<T> *extraParent = nullptr;
    int rank = 0;
public:
    explicit Node(Item<T> *initItem) {
        this->item = initItem;
        initItem->node = this;
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
    Item<T>* insert(T el);
    void merge(HollowHeap<T> &hh);
    T extractMin();
    void decreaseKey(Item<T>* itemToDecrease, T val);
    void deleteItem(Item<T>* itemToDelete);
private:
    int count = 0;
    Node<T>* min = nullptr;

    Node<T>* makeNode(Item<T>* item);
    Node<T>* link(Node<T>* n1, Node<T>* n2);
    void addChild(Node<T>* futureChild, Node<T>* futureParent);
    Node<T>* merge(Node<T>* newNode);
    int handleChildrenOfHollowRoot(Node<T>* hollowRoot, vector<Node<T>*>& fullRoots, int maxRank);
    Node<T>* handleHollowChildOfHollowRoot(Node<T>* childOfHollowRoot,  Node<T>* hollowRoot);
    void doUnrankedLinks(int maxRank, vector<Node<T>*> &fullRoots);
    void initFullRootsList(vector<Node<T>*>& fullRoots);
    int doRankedLinks(Node<T>* node, int maxRank, vector<Node<T>*>& fullRoots);
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
Item<T>* HollowHeap<T>::insert(T el) {
    Item<T>* item = new Item(el);
    Node<T>* newNode = makeNode(item);

    Node<T>* newMin = merge(newNode);
    count++;

    min = newMin;
    return newNode->item;
}

template <typename T>
void HollowHeap<T>::merge(HollowHeap<T> &hh) {
    Node<T>* newMin = merge(hh.min);
    count = count + hh.count;

    // set min pointer of hh to nullptr
    // to avoid destruction of nodes that were just merged
    hh.min = nullptr;
    min = newMin;
}

template <typename T>
T HollowHeap<T>::extractMin() {
    T minVal = min->key;
    deleteItem(min->item);
    return minVal;
}

template <typename T>
void HollowHeap<T>::decreaseKey(Item<T> *itemToDecrease, T val) {
    Node<T>* nodeToDecrease = itemToDecrease->node;
    itemToDecrease->value = val;

    if (nodeToDecrease == min) {
        nodeToDecrease->key = val;
        return;
    }

    Node<T>* secondParent = makeNode(itemToDecrease);
    secondParent->child = nodeToDecrease;
    nodeToDecrease->item = nullptr;
    nodeToDecrease->extraParent = secondParent;

    if (nodeToDecrease->rank > 2)
        secondParent->rank = nodeToDecrease->rank - 2;

    min = link(secondParent, min);
}

template <typename T>
void HollowHeap<T>::deleteItem(Item<T> *itemToDelete) {
    Node<T>* nodeToDelete = itemToDelete->node;
    nodeToDelete->item = nullptr;
    itemToDelete->node = nullptr;

    if (min->item != nullptr) { // Non-minimum deletion
        count--;
        return;
    }

    int maxRank = 0;
    vector<Node<T>*> fullRoots;
    initFullRootsList(fullRoots);

    // iterate through all hollow roots and destroy them
    while (min != nullptr) { // while there are still hollow roots
        Node<T>* hollowRoot = min;
        min = min->next;

        maxRank = handleChildrenOfHollowRoot(hollowRoot, fullRoots, maxRank);

        delete hollowRoot;
    }

    doUnrankedLinks(maxRank, fullRoots);
    count--;
}

template <typename T>
void HollowHeap<T>::initFullRootsList(vector<Node<T>*>& fullRoots) {
    fullRoots.resize(log2(count) + 1);
    for (int i = 0; i < fullRoots.size(); i++) {
        fullRoots[i] = nullptr;
    }
}

template <typename  T>
int HollowHeap<T>::handleChildrenOfHollowRoot(
        Node<T>* hollowRoot,
        vector<Node<T>*>& fullRoots,
        int maxRank
) {
    Node<T>* nextChildOfHollowRoot = hollowRoot->child;
    while (nextChildOfHollowRoot != nullptr) {
        Node<T>* childOfHollowRoot = nextChildOfHollowRoot;

        // if child of the hollow root is hollow too
        if (childOfHollowRoot->item == nullptr) {
            nextChildOfHollowRoot = handleHollowChildOfHollowRoot(childOfHollowRoot, hollowRoot);
        } else {
            // if child is not hollow, it will become a root after destruction of its (hollow) parent
            // Hence, add it to the list of full roots
            nextChildOfHollowRoot = childOfHollowRoot->next;
            maxRank = doRankedLinks(childOfHollowRoot, maxRank, fullRoots);
        }
    }
    return maxRank;
}

// returns next child of the hollow root to be processed
template <typename  T>
Node<T>* HollowHeap<T>::handleHollowChildOfHollowRoot(Node<T>* childOfHollowRoot,  Node<T>* hollowRoot) {
    Node<T>* nextChildOfHollowRoot = childOfHollowRoot->next;
    // if the child has only 1 parent, deleting hollowRoot makes childOfHollowRoot a root
    if (childOfHollowRoot->extraParent == nullptr) {
        childOfHollowRoot->next = min;
        min = childOfHollowRoot;
    } else { // if hollowRoot has 2 parents
        if (childOfHollowRoot->extraParent == hollowRoot) {
            nextChildOfHollowRoot = nullptr;
        } else {
            childOfHollowRoot->next = nullptr;
        }
        childOfHollowRoot->extraParent = nullptr;
    }
    return nextChildOfHollowRoot;
}

// returns maxRank found so far in fullRoots array
template <typename T>
int HollowHeap<T>::doRankedLinks(Node<T>* node, int maxRank, vector<Node<T>*> &fullRoots) {
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
void HollowHeap<T>::doUnrankedLinks(int maxRank, vector<Node<T>*>& fullRoots) {
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
Node<T>* HollowHeap<T>::merge(Node<T>* newNode) {
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
Node<T>* HollowHeap<T>::makeNode(Item<T>* item) {
    auto* node = new Node<T>(item);
    return node;
}

template <typename T>
Node<T>* HollowHeap<T>::link(Node<T>* n1, Node<T>* n2) {
    if (n1->key >= n2->key) {
        addChild(n1, n2);
        return n2;
    } else {
        addChild(n2, n1);
        return n1;
    }
}

template <typename T>
void HollowHeap<T>::addChild(Node<T>* futureChild, Node<T>* futureParent) {
    futureChild->next = futureParent->child;
    futureParent->child = futureChild;
    // a root does not have a parent and therefore no next link
    futureParent->next = nullptr;
}