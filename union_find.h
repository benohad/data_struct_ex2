#ifndef EX_2_UNION_FIND_H
#define EX_2_UNION_FIND_H

#include <memory>
#include "wet2util.h"

const int INITIAL_SIZE = 10;

template <class T>
struct unionFindNode {
    T data;
    int size;
    std::shared_ptr<unionFindNode<T>> parent;
    explicit unionFindNode(T data) : data(data), size(1), parent(nullptr) {}
};

template <class T>
class UnionFind {
public:
    int m_arraySize;
    int m_counter;
    //array of unique pointers to shared pointers
    std::unique_ptr<std::shared_ptr<unionFindNode<T>>[]> m_dynamicArray;

    explicit UnionFind(T data) : m_arraySize(INITIAL_SIZE), m_counter(0) {
        m_dynamicArray = std::unique_ptr<std::shared_ptr<unionFindNode<T>>[]>
                (new std::shared_ptr<unionFindNode<T>>[INITIAL_SIZE]);
        makeSet(data);
    }

    StatusType arrayResize();
    unionFindNode<T>* makeSet(T data);
    unionFindNode<T>* find(unionFindNode<T>* node);
    bool unionSets(unionFindNode<T>* node1, unionFindNode<T>* node2);
};

template <class T>
StatusType UnionFind<T>::arrayResize() {
    try {
        // Allocate new array with double size.
        std::unique_ptr<std::shared_ptr<unionFindNode<T>>[]>
                newArray(new std::shared_ptr<unionFindNode<T>>[m_arraySize * 2]);

        // Move the old array to the new array.
        for (int i = 0; i < m_arraySize; i++) {
            newArray[i] = m_dynamicArray[i];
        }

        // Move the new array to m_dynamicArray.
        m_dynamicArray = std::move(newArray);

        // Update the array size.
        m_arraySize *= 2;

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

template <class T>
unionFindNode<T>* UnionFind<T>::makeSet(T data) {
    // Check if the array is full, and resize if needed.
    if (m_arraySize == m_counter) {
        StatusType resizeStatus = arrayResize();
        if (resizeStatus != StatusType::SUCCESS) {
            return nullptr;
        }
    }

    // Create a new node with a shared_ptr.
    auto newNode = std::make_shared<unionFindNode<T>>(data);

    // Add the new node to the dynamic array.
    m_dynamicArray[m_counter] = newNode;

    // Update the counter.
    m_counter++;

    return m_dynamicArray[m_counter - 1];
}

template <class T>
unionFindNode<T>* UnionFind<T>::find(unionFindNode<T>* node) {
    // If the node is the root of the tree, return it.
    if (node->parent == nullptr) {
        return node;
    }

    // Path compression
    node->parent = find(node->parent);

    return node->parent;
}

template <class T>
bool UnionFind<T>::unionSets(unionFindNode<T>* node1, unionFindNode<T>* node2) {
    // Find the roots of the trees.
    auto root1 = find(node1);
    auto root2 = find(node2);

    // If the roots are the same, return false.
    if (root1 == root2) {
        return false;
    }
    if(root1->size < root2->size){
        root1->parent = root2;
        root2->size += root1->size;
    } else{
        root2->parent = root1;
        root1->size += root2->size;
    }
}

#endif //EX_2_UNION_FIND_H
