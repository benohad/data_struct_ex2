
#ifndef EX_2_HASHTABLE_H
#define EX_2_HASHTABLE_H
#include <memory>
#include "wet2util.h"
#include <iostream>



const double GOLDEN_RATIO = 0.61803398875;
const int ARRAY_SIZE = 10;


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

    HashTable() : m_tableSize(ARRAY_SIZE), m_counter(0) {
        m_dynamicArray = std::unique_ptr<std::shared_ptr<HashNode<T>>[]>
                (new std::shared_ptr<HashNode<T>>[ARRAY_SIZE]);
    }

    int hashFunction(int key);
    StatusType insert(int key, T data);
    StatusType remove(int key,T data);
    std::shared_ptr<HashNode<T>> search(int key);
    StatusType arrayResize();
    bool Singleton(int key);
};

template <class T>
int HashTable<T>::hashFunction(int key) {
    return (int)(m_tableSize * (key * GOLDEN_RATIO - (int)(key * GOLDEN_RATIO)));
}

template <class T>
StatusType HashTable<T>::insert(int key, T data) {
    // Check if the array needs to be resized =>> alpha factor is 2.
    try{
        if (m_counter >= 7000*m_tableSize) {
            return arrayResize();
        }
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
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
        // 1) Keep old array and size.
        auto oldArray = std::move(m_dynamicArray);
        int oldSize = m_tableSize;

        // 2) Double m_tableSize and allocate new array.
        m_tableSize *= 2;
        m_dynamicArray.reset(new std::shared_ptr<HashNode<T>>[m_tableSize]);

        // 3) Re-insert all nodes into new array.
        for (int i = 0; i < oldSize; i++) {
            std::shared_ptr<HashNode<T>> current = oldArray[i];
            while (current != nullptr) {
                // Save 'next' before we overwrite current->next.
                std::shared_ptr<HashNode<T>> nextNode = current->next;

                // Compute new index with updated m_tableSize:
                int newIndex = hashFunction(current->key);

                // Insert at head of new chain:
                current->next = m_dynamicArray[newIndex];
                m_dynamicArray[newIndex] = current;

                current = nextNode;
            }
        }

        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

template <class T>
StatusType HashTable<T>::remove(int key,T data) {
    int index = hashFunction(key);
    std::shared_ptr<HashNode<T>> current = m_dynamicArray[index];

    // If the index is empty, return failure.
    if (current == nullptr) {
        return StatusType::FAILURE;
    }

    // If the first node is the node to remove.
    if (current->key == key && current->data == data) {
        // if it is the first node in the chain the next is nullptr...
        m_dynamicArray[index] = current->next;
        m_counter--;
        return StatusType::SUCCESS;
    }

    // Traverse the chain to find the node to remove.
    while (current->next != nullptr) {
        if (current->next->key == key&& current->next->data == data) {
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

/*template <class T>
bool HashTable<T>:: Singleton(int key){
    int index = hashFunction(key);
    std::shared_ptr<HashNode<T>> current = m_dynamicArray[index];

    if(current->key == key && current->next == nullptr){
        return true;
    }
    // if there is a next node with the same key

    while (current->next != nullptr) {
        if(current->key == key && current->next != nullptr){
            if(current->next->key == key){
                return false;
            }
        }
        current = current->next;
    }
   *//* if(current->key == key && current->next != nullptr){
        if(current->next->key != key){
            return true;
        }
    }*//*
    return true;
}*/

template <class T>
bool HashTable<T>::Singleton(int key) {
    int index = hashFunction(key);
    std::shared_ptr<HashNode<T>> current = m_dynamicArray[index];

    // If the index is empty, the key is not present.
    if (current == nullptr) {
        return false;
    }

    // Check the chain for the key.
    bool found = false;
    while (current != nullptr) {
        if (current->key == key) {
            if (found) {
                // If we find the key again, it's not a singleton.
                return false;
            }
            found = true; // First occurrence of the key.
        }
        current = current->next;
    }

    // If the key was found only once, it's a singleton.
    return found;
}








#endif //EX_2_HASHTABLE_H
