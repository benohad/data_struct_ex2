
#ifndef EX_2_HASHTABLE_H
#define EX_2_HASHTABLE_H
#include <memory>
#include "wet2util.h"

const double GOLDEN_RATIO = 0.61803398875;
const int INITIAL_SIZE = 10;

template <class T>
struct HashNode {
    int key;
    T data;
    std::shared_ptr<HashNode<T>> next;
    explicit HashNode(int key, T data) : key(key), data(data), next(nullptr) {}
};

template <class T>
class HashTable {
public:
    int m_tableSize;
    int m_counter;
    //array of unique pointers to shared pointers(the nodes inside a chain)
    std::unique_ptr<std::shared_ptr<HashNode<T>>[]> m_dynamicArray;

     HashTable() : m_tableSize(INITIAL_SIZE), m_counter(0) {
        m_dynamicArray = std::unique_ptr<std::shared_ptr<HashNode<T>>[]>
                (new std::shared_ptr<HashNode<T>>[INITIAL_SIZE]);
    }

    int hashFunction(int key);
    StatusType insert(int key, T data);
    StatusType remove(int key);
    std::shared_ptr<HashNode<T>> search(int key);
    StatusType arrayResize();
};

template <class T>
int HashTable<T>::hashFunction(int key) {
    return (int)(m_tableSize * (key * GOLDEN_RATIO - (int)(key * GOLDEN_RATIO)));
}

template <class T>
StatusType HashTable<T>::insert(int key, T data) {
    // Check if the array needs to be resized =>> alpha factor is 2.
    if (m_counter >= 2*m_tableSize) {
        return arrayResize();
    }

    int index = hashFunction(key);
    std::shared_ptr<HashNode<T>> newNode = std::make_shared<HashNode<T>>(key, data);
    // If the index is empty, insert the new node.
    if (m_dynamicArray[index] == nullptr) {
        m_dynamicArray[index] = newNode;
        m_counter++;
        return StatusType::SUCCESS;
    }
    //if the index is occupied, append the new node to the end of the chain
    std::shared_ptr<HashNode<T>> current = m_dynamicArray[index];
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = newNode;
    m_counter++;
    return StatusType::SUCCESS;

}

template <class T>
StatusType HashTable<T>::arrayResize() {
    try {
        // Allocate new array of unique_ptrs with double size.
        std::unique_ptr<std::shared_ptr<HashNode<T>>[]>
                newArray(new std::shared_ptr<HashNode<T>>[m_tableSize * 2]);

        // Move the old array to the new array.
        for (int i = 0; i < m_tableSize; i++) {
            newArray[i] = m_dynamicArray[i];
        }

        // Move the new array to m_dynamicArray.
        m_dynamicArray = std::move(newArray);

        // Update the array size.
        m_tableSize *= 2;

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

template <class T>
StatusType HashTable<T>::remove(int key) {
    int index = hashFunction(key);
    std::shared_ptr<HashNode<T>> current = m_dynamicArray[index];

    // If the index is empty, return failure.
    if (current == nullptr) {
        return StatusType::FAILURE;
    }

    // If the first node is the node to remove.
    if (current->key == key) {
        // if it is the first node in the chain the next is nullptr...
        m_dynamicArray[index] = current->next;
        m_counter--;
        return StatusType::SUCCESS;
    }

    // Traverse the chain to find the node to remove.
    while (current->next != nullptr) {
        if (current->next->key == key) {
            // Found the node, remove it from the chain.
            current->next = current->next->next;
            m_counter--;
            return StatusType::SUCCESS;
        }
        current = current->next;
    }

    // if we didn't find the node to remove
    return StatusType::FAILURE;
}

template <class T>
std::shared_ptr<HashNode<T>> HashTable<T>::search(int key) {
    int index = hashFunction(key);
    std::shared_ptr<HashNode<T>> current = m_dynamicArray[index];

    // Traverse the chain to find the node.
    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }

    // if we didn't find the node
    return nullptr;
}







#endif //EX_2_HASHTABLE_H
