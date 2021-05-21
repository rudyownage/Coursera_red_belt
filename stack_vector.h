#pragma once

#include <stdexcept>
#include <array>
using namespace std;

template <typename T, size_t N>
class StackVector {
public:
	explicit StackVector(size_t a_size = 0);

	T& operator[](size_t index);
	const T& operator[](size_t index) const;

	T* begin();
	T* end();
	const T* begin() const;
    const T* end() const;

	size_t Size() const;
	size_t Capacity() const;

	void PushBack(const T& value);
	T PopBack();

private:
	 array<T, N> Data;
	 size_t size;
};

template <typename T, size_t N>
StackVector<T, N>::StackVector(size_t a_size):
	size(a_size)
{
	if (size > N) {
		throw invalid_argument("");
	}
}

template <typename T, size_t N>
T& StackVector<T, N>::operator[](size_t index) {
	return Data[index];
}

template <typename T, size_t N>
const T& StackVector<T, N>::operator[](size_t index) const{
	return Data[index];
}

template <typename T, size_t N>
T* StackVector<T, N>::begin() {
	return Data.begin();
}

template <typename T, size_t N>
const T* StackVector<T, N>::begin() const{
	return Data.begin();
}

template <typename T, size_t N>
T* StackVector<T, N>::end() {
	return Data.begin() + size;
}

template <typename T, size_t N>
const T* StackVector<T, N>::end() const {
	return Data.begin() + size;
}

template <typename T, size_t N>
size_t StackVector<T, N>::Size() const {
	return size;
}

template <typename T, size_t N>
size_t StackVector<T, N>::Capacity() const {
	return N;
}

template <typename T, size_t N>
void StackVector<T, N>::PushBack(const T& value) {
	if (size == N) {
		throw overflow_error("");
	}
	else {
		Data[size++] = value;
	}
}

template <typename T, size_t N>
T StackVector<T, N>::PopBack(){
	if (size == 0) {
		throw underflow_error("");
	}
	else {
		return Data[--size];
	}
}