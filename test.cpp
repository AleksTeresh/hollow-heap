#include <cassert>
#include "hollow_heap.cpp"

// insert
void insertToEmptyHeap() {
    HollowHeap<int> f1;
    f1.insert(2);

    assert(f1.extractMin() == 2);
}

void insertLargerValueToNonEmptyHeap() {
    HollowHeap<int> f1;
    f1.insert(2);
    f1.insert(6);

    assert(f1.extractMin() == 2);
}

void insertNewMinToNonEmptyHeap() {
    HollowHeap<int> f1;
    f1.insert(2);
    f1.insert(6);
    f1.insert(1);

    assert(f1.extractMin() == 1);
}


// merge

void merge2FullHeaps() {
    HollowHeap<int> f1;
    f1.insert(2);
    f1.insert(4);
    HollowHeap<int> f2;
    f2.insert(1);
    f2.insert(3);
    f2.insert(0);

    f1.merge(f2);

    assert(f1.extractMin() == 0);
}

void merge2SingleNodedHeaps() {
    HollowHeap<int> f1;
    f1.insert(2);
    HollowHeap<int> f2;
    f2.insert(3);

    f1.merge(f2);

    assert(f1.extractMin() == 2);
}

void mergeFullAndEmpty() {
    HollowHeap<int> f1;
    f1.insert(2);
    f1.insert(4);
    HollowHeap<int> f2;

    f1.merge(f2);

    assert(f1.extractMin() == 2);
}

void mergeEmptyAndFull() {
    HollowHeap<int> f1;
    HollowHeap<int> f2;
    f2.insert(1);
    f2.insert(3);
    f2.insert(0);

    f1.merge(f2);

    assert(f1.extractMin() == 0);
}

void merge2EmptyHeaps() {
    HollowHeap<int> f1;
    HollowHeap<int> f2;

    f1.merge(f2);

    assert(f1.size() == 0);
}

// extractMin
void extractTheOnlyNode() {
    HollowHeap<int> f1;
    f1.insert(2);

    assert(f1.extractMin() == 2);
    assert(f1.size() == 0);
}

void extractNodeWithoutChildren() {
    HollowHeap<int> f1;
    f1.insert(1);
    f1.insert(2);
    f1.insert(3);
    f1.insert(4);
    f1.insert(5);
    f1.insert(6);
    f1.extractMin();

    assert(f1.extractMin() == 2);
}

void extractNodeWithChildren() {
    HollowHeap<int> f1;
    f1.insert(1);
    f1.insert(2);
    f1.insert(1);
    f1.insert(4);
    f1.insert(5);
    f1.insert(6);
    f1.extractMin();

    assert(f1.extractMin() == 1);
}

void extractMinAndMergeAllToOneRoot() {
    HollowHeap<int> f1;
    f1.insert(1);
    f1.insert(2);
    f1.insert(3);
    f1.insert(4);
    f1.insert(5);
    f1.extractMin();

    assert(f1.size() == 4);
    assert(f1.extractMin() == 2);
}

void extractMinAndDo4RecursiveMerges() {
    HollowHeap<int> f1;
    for (int i = 0; i < 17; i++) {
        f1.insert(i);
    }
    f1.extractMin();

    assert(f1.size() == 16);
    assert(f1.extractMin() == 1);
}

// decreaseKey
void decreaseKeyWhenAllNodesAreRoots() {
    HollowHeap<int> f1;
    f1.insert(1);
    f1.insert(2);
    shared_ptr<Item<int>> n = f1.insert(3);
    f1.insert(4);
    f1.insert(5);
    f1.decreaseKey(n, 0);

    assert(f1.size() == 5);
    assert(f1.extractMin() == 0);
}

void decreaseKeyOfRoot() {
    HollowHeap<int> f1;

    shared_ptr<Item<int>> nodeToDecrease;
    for (int i = 0; i < 17; i++) {
        shared_ptr<Item<int>> n = f1.insert(i);

        if (i == 1) {
            nodeToDecrease = n;
        }
    }
    f1.extractMin();

    f1.decreaseKey(nodeToDecrease, 0);

    assert(f1.size() == 16);
    assert(f1.extractMin() == 0);
}

void decreaseKeyOfMinNode() {
    HollowHeap<int> f1;
    shared_ptr<Item<int>> n = f1.insert(1);
    f1.insert(2);
    f1.insert(3);
    f1.insert(4);
    f1.insert(5);
    f1.decreaseKey(n, 0);

    assert(f1.size() == 5);
    assert(f1.extractMin() == 0);
}

void decreaseKeyOfLeaf() {
    HollowHeap<int> f1;

    shared_ptr<Item<int>> nodeToDecrease;
    for (int i = 0; i < 17; i++) {
        shared_ptr<Item<int>> n = f1.insert(i);

        if (i == 12) {
            nodeToDecrease = n;
        }
    }
    f1.extractMin();

    f1.decreaseKey(nodeToDecrease, 0);

    assert(f1.size() == 16);
    assert(f1.extractMin() == 0);
}

void decreaseKeyOfNodeWithMarkedParent() {
    HollowHeap<int> f1;

    shared_ptr<Item<int>> nodeToDecrease1;
    shared_ptr<Item<int>> nodeToDecrease2;
    for (int i = 0; i < 17; i++) {
        shared_ptr<Item<int>> n = f1.insert(i);

        if (i == 14) {
            nodeToDecrease1 = n;
        }
        if (i == 15) {
            nodeToDecrease2 = n;
        }
    }
    f1.extractMin();

    f1.decreaseKey(nodeToDecrease1, 2);
    f1.decreaseKey(nodeToDecrease2, 3);

    assert(f1.size() == 16);
    assert(f1.extractMin() == 1);
}

void decreaseKeyOfMinNodeWithAllAncestorsMarked() {
    HollowHeap<int> f1;

    shared_ptr<Item<int>> nodeToDecrease1;
    shared_ptr<Item<int>> nodeToDecrease2;
    shared_ptr<Item<int>> nodeToDecrease3;
    for (int i = 0; i < 17; i++) {
        shared_ptr<Item<int>> n = f1.insert(i);

        if (i == 10) {
            nodeToDecrease1 = n;
        }
        if (i == 14) {
            nodeToDecrease2 = n;
        }
        if (i == 15) {
            nodeToDecrease3 = n;
        }
    }
    f1.extractMin();

    f1.decreaseKey(nodeToDecrease1, 2);
    f1.decreaseKey(nodeToDecrease2, 3);
    f1.decreaseKey(nodeToDecrease3, 0);

    assert(f1.size() == 16);
    assert(f1.extractMin() == 0);
}


// delete node
void deleteRoot() {
    HollowHeap<int> f1;
    shared_ptr<Item<int>> root = f1.insert(2);
    f1.insert(6);
    f1.insert(1);

    f1.deleteItem(root);

    assert(f1.extractMin() == 1);
}

void deleteCurrentMin() {
    HollowHeap<int> f1;
    f1.insert(2);
    f1.insert(6);
    shared_ptr<Item<int>> currMin = f1.insert(1);

    f1.deleteItem(currMin);

    assert(f1.extractMin() == 2);
}

void deleteItemInTheMiddle() {
    HollowHeap<int> f1;
    f1.insert(1);
    f1.insert(2);
    f1.insert(3);
    f1.insert(4);
    shared_ptr<Item<int>> midNode = f1.insert(5);
    f1.insert(6);
    f1.extractMin();
    f1.deleteItem(midNode);

    assert(f1.size() == 4);
    assert(f1.extractMin() == 2);
}

void deleteLeaf() {
    HollowHeap<int> f1;
    f1.insert(1);
    f1.insert(2);
    f1.insert(3);
    f1.insert(4);
    f1.insert(5);
    shared_ptr<Item<int>> child = f1.insert(6);
    f1.extractMin();
    f1.deleteItem(child);

    assert(f1.size() == 4);
    assert(f1.extractMin() == 2);
}

// general tests
void basicTest1() {
    HollowHeap<int> fib;
    shared_ptr<Item<int>> five = fib.insert(5);
    assert(fib.size() == 1);
    fib.insert(8);
    assert(fib.size() == 2);
    fib.insert(5);
    assert(fib.size() == 3);

    HollowHeap<int> fib1;
    fib1.insert(1);
    fib1.insert(2);
    fib1.insert(4);

    fib.merge(fib1);
    assert(fib.size() == 6);

    int extractedMin = fib.extractMin();
    assert(extractedMin == 1);
    assert(fib.size() == 5);
    fib.decreaseKey(five, 3);
    assert(fib.size() == 5);
    extractedMin = fib.extractMin();
    assert(fib.size() == 4);
    assert(extractedMin == 2);
    fib.deleteItem(five);
    assert(fib.size() == 3);
    extractedMin = fib.extractMin();
    assert(fib.size() == 2);
    assert(extractedMin == 4);
}

// general tests
void charTest1() {
    HollowHeap<char> fib;
    shared_ptr<Item<char>> eNode = fib.insert('e');
    fib.insert('f');
    fib.insert('e');

    HollowHeap<char> fib1;
    fib1.insert('a');
    fib1.insert('b');
    fib1.insert('d');

    fib.merge(fib1);

    char extractedMin = fib.extractMin();
    assert(extractedMin == 'a');
    fib.decreaseKey(eNode, 'c');
    extractedMin = fib.extractMin();
    assert(extractedMin == 'b');
    fib.deleteItem(eNode);
    extractedMin = fib.extractMin();
    assert(extractedMin == 'd');
}

void basicTest2() {
    HollowHeap<int> fib;
    fib.insert(2);
    fib.insert(3);
    fib.insert(1);
    fib.insert(4);
    int extractedMin = fib.extractMin();
    assert(extractedMin == 1);
    extractedMin = fib.extractMin();
    assert(extractedMin == 2);
    fib.insert(5);
    fib.insert(7);
    extractedMin = fib.extractMin();
    assert(extractedMin == 3);
    fib.insert(2);
    shared_ptr<Item<int>> ninety = fib.insert(90);
    extractedMin = fib.extractMin();
    assert(extractedMin == 2);
    extractedMin = fib.extractMin();
    assert(extractedMin == 4);
    extractedMin =  fib.extractMin();
    assert(extractedMin == 5);

    shared_ptr<Item<int>> first28;
    for(int i=0;i<20;i+=2) {
        shared_ptr<Item<int>> n = fib.insert(30-i);

        if (30 - i == 28) {
            first28 = n;
        }
    }
    for(int i=0;i<4;i++) {
        extractedMin = fib.extractMin();
    }

    shared_ptr<Item<int>> second28;
    for(int i=0;i<20;i+=2) {
        shared_ptr<Item<int>> n = fib.insert(30-i);

        if (30 - i == 28) {
            second28 = n;
        }
    }
    shared_ptr<Item<int>> last23 = fib.insert(23);
    for(int i=0;i<7;i++) {
        extractedMin = fib.extractMin();
    }
    fib.decreaseKey(ninety, 1);

    fib.decreaseKey(first28,2);
    fib.decreaseKey(second28,2);


    fib.decreaseKey(last23,3);

    for(int i=0;i<7;i++) {
        extractedMin = fib.extractMin();
    }

    assert(extractedMin == 24);
}

void basicTest3() {
    HollowHeap<int> fib;
    fib.insert(14);
    fib.insert(11);
    shared_ptr<Item<int>> five = fib.insert(5);
    fib.insert(9);
    fib.insert(0);
    shared_ptr<Item<int>> eight = fib.insert(8);
    fib.insert(10);
    shared_ptr<Item<int>> three = fib.insert(3);
    fib.insert(6);
    fib.insert(12);
    fib.insert(13);
    fib.insert(4);

    assert(fib.size() == 12);

    assert(fib.extractMin() == 0);
    assert(fib.size() == 11);
    fib.decreaseKey(five, 1);
    fib.decreaseKey(three, 2);
    fib.decreaseKey(eight, 7);

    assert(fib.extractMin() == 1);
    assert(fib.size() == 10);
    assert(fib.extractMin() == 2);

    assert(fib.size() == 9);
}

void charTest2() {
    HollowHeap<char> fib;
    fib.insert('b');
    fib.insert('c');
    fib.insert('a');
    fib.insert('d');
    char extractedMin = fib.extractMin();
    extractedMin=  fib.extractMin();
    fib.insert('e');
    fib.insert('g');
    extractedMin = fib.extractMin();
    fib.insert('b');
    shared_ptr<Item<char>> zNode = fib.insert('z');
    extractedMin = fib.extractMin();
    extractedMin = fib.extractMin();
    extractedMin =  fib.extractMin();
    assert(extractedMin == 'e');

    fib.decreaseKey(zNode,'b');
    extractedMin = fib.extractMin();
    assert(extractedMin == 'b');

    fib.insert('b');
    fib.insert('c');
    fib.insert('a');
    shared_ptr<Item<char>> aNode = fib.insert('d');
    fib.insert('b');
    fib.insert('c');
    fib.insert('a');
    shared_ptr<Item<char>> anotherNode = fib.insert('d');
    fib.insert('b');
    fib.insert('c');
    fib.insert('a');
    fib.insert('d');
    fib.extractMin();
    assert(fib.size() == 12);

    fib.deleteItem(anotherNode);
    assert(fib.size() == 11);

    fib.deleteItem(aNode);
    assert(fib.size() == 10);
}


void runTests() {
    basicTest1();
    basicTest2();
    basicTest3();

    charTest1();
    charTest2();

    insertToEmptyHeap();
    insertNewMinToNonEmptyHeap();
    insertLargerValueToNonEmptyHeap();

    merge2FullHeaps();
    merge2EmptyHeaps();
    merge2SingleNodedHeaps();
    mergeFullAndEmpty();
    mergeEmptyAndFull();

    extractTheOnlyNode();
    extractNodeWithoutChildren();
    extractNodeWithChildren();
    extractMinAndMergeAllToOneRoot();
    extractMinAndDo4RecursiveMerges();

    decreaseKeyWhenAllNodesAreRoots();

    decreaseKeyOfRoot();
    decreaseKeyOfLeaf();
    decreaseKeyOfMinNode();
    decreaseKeyOfNodeWithMarkedParent();
    decreaseKeyOfMinNodeWithAllAncestorsMarked();

    deleteCurrentMin();
    deleteLeaf();
    deleteRoot();
    deleteItemInTheMiddle();
}