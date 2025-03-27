#pragma once
#include <vector>
#include <stdexcept>
#include <utility>
#include <mutex>
#include <condition_variable>

template <typename T>
class RingBuffer
{
public:
	explicit RingBuffer(size_t capacity)
		: m_buffer(capacity),
		m_capacity(capacity),
		m_head(0),
		m_tail(0),
		m_size(0) {}

	void push(const T& item);
	T pop();

	bool empty() const;
	bool full() const;
	size_t size() const;
	size_t capacity() const noexcept;

private:
	std::vector<T> m_buffer;
	const size_t   m_capacity;
	size_t         m_head;
	size_t         m_tail;
	size_t         m_size;

	mutable std::mutex      m_mutex;
	std::condition_variable m_not_empty;
	std::condition_variable m_not_full;
};


template <typename T>
void RingBuffer<T>::push(const T& item)
{
	//Lagre elementet
	std::unique_lock<std::mutex> lock(m_mutex);

	m_not_full.wait(lock, [this] {return m_size < m_capacity; });
	// Flytter elementet til bufferet på posisjonen indikert av m_head
	// Bruker std::move til å endre eierskap uten å kopiere
	m_buffer[m_head] = item;

	// Får m_head til å peke til neste posisjon i bufferet
	// Bruker modulus til å få den til å peke til starten hvis den er på enden av bufferet
	m_head = (m_head + 1) % m_capacity;

	++m_size;
	m_not_empty.notify_one();
}


template <typename T>
T RingBuffer<T>::pop()
//Pop ett element fra ringbufferet
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_not_empty.wait(lock, [this] {return m_size > 0; });
	//Hent ett element
	T item = std::move(m_buffer[m_tail]);
	m_tail = (m_tail + 1) % m_capacity;

	--m_size;
	m_not_full.notify_one();


	return item;
}

template <typename T>
size_t RingBuffer<T>::size() const
{
	return m_size;
}

template <typename T>
bool RingBuffer<T>::empty() const
//Returnerer true hvis bufferet er tomt, ellers false
{
	return (m_size == 0);
}

template <typename T>
//Returnerer true hvis bufferet er fullt, ellers false
bool RingBuffer<T>::full() const
{
	return(m_size == m_capacity);
}

template <typename T>
//Returnerer kapasiteten ( max antall elementer ringen kan holde
size_t RingBuffer<T>::capacity() const noexcept
{
	return m_capacity;
}