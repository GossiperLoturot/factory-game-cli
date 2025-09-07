#pragma once

#include <cassert>
#include <cstddef>

template<class T>
union Node {
	alignas(T) char data[sizeof(T)];
	Node* next;
};

template<class T, size_t MAXSIZE>
class PoolAllocator
{
public:
	PoolAllocator() : allocated_count(0), m_buffer(), m_freelist(nullptr)
	{
		for (size_t i = 0; i < MAXSIZE - 1; ++i) {
			m_buffer[i].next = &m_buffer[i + 1];
		}
		m_buffer[MAXSIZE - 1].next = nullptr;

		m_freelist = &m_buffer[0];
	}

	~PoolAllocator() {}

	T* Alloc() {
		if (m_freelist == nullptr) {
			return nullptr;
		}

		Node<T>* node = m_freelist;
		m_freelist = m_freelist->next;
		T* result = reinterpret_cast<T*>(&node->data);
		new (result) T();

		allocated_count++;

		return result;
	}

	void Free(T* ptr) {
		if (ptr == nullptr) {
			return;
		}

		ptr->~T();
		Node<T>* node = reinterpret_cast<Node<T>*>(ptr);
		node->next = m_freelist;
		m_freelist = node;

		allocated_count--;
	}
private:
	size_t allocated_count;
	Node<T> m_buffer[MAXSIZE];
	Node<T>* m_freelist;
};
