////////////////////////////////
/// usage : 1.	dynamic allocated array follows RAII idiom.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SZX_CPPUTILIBS_ARR_H
#define SZX_CPPUTILIBS_ARR_H


#include <algorithm>
#include <initializer_list>


namespace szx {

template<typename T, typename IndexType = int>
class Arr {
public:
    /// it is always valid before copy assignment due to no reallocation.
    using Iterator = T*;
    using ConstIterator = T const *;

    enum ResetOption { AllBits0 = 0, AllBits1 = -1 };

    explicit Arr() : arr(nullptr), len(0) {}
    explicit Arr(IndexType length) { allocate(length); }
    explicit Arr(IndexType length, T *data) : arr(data), len(length) {} // make sure `this` will be the only one who owns `data`.
    explicit Arr(IndexType length, const T &defaultValue) : Arr(length) {
        std::fill(arr, arr + length, defaultValue);
    }
    explicit Arr(std::initializer_list<T> l) : Arr(static_cast<IndexType>(l.size())) {
        std::copy(l.begin(), l.end(), arr);
    }

    Arr(const Arr &a) : Arr(a.len) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr(Arr &&a) : Arr(a.len, a.arr) { a.arr = nullptr; }

    Arr& operator=(const Arr &a) {
        if (this != &a) {
            if (len != a.len) {
                clear();
                init(a.len);
            }
            copyData(a.arr);
        }
        return *this;
    }
    Arr& operator=(Arr &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr() { clear(); }

    /// allocate memory if it has not been init before.
    bool init(IndexType length) {
        if (arr == nullptr) { // avoid re-init and memory leak.
            allocate(length);
            return true;
        }
        return false;
    }

    /// remove all items.
    void clear() {
        delete[] arr;
        arr = nullptr;
    }

    /// set all data to val. any value other than 0 or -1 is undefined behavior.
    void reset(ResetOption val = ResetOption::AllBits0) { memset(arr, val, sizeof(T) * len); }

    T& operator[](IndexType i) { return arr[i]; }
    const T& operator[](IndexType i) const { return arr[i]; }

    T& at(IndexType i) { return arr[i]; }
    const T& at(IndexType i) const { return arr[i]; }

    Iterator begin() { return arr; }
    Iterator end() { return (arr + len); }
    ConstIterator begin() const { return arr; }
    ConstIterator end() const { return (arr + len); }
    T& front() { return at(0); }
    T& back() { return at(len - 1); }
    const T& front() const { return at(0); }
    const T& back() const { return at(len - 1); }

    IndexType size() const { return len; }
    bool empty() const { return (len == 0); }

protected:
    /// must not be called except init.
    void allocate(IndexType length) {
        // TODO[szx][2]: length > (1 << 32)?
        arr = new T[static_cast<size_t>(length)];
        len = length;
    }

    void copyData(T *data) {
        // TODO[szx][1]: what if data is shorter than arr?
        // OPTIMIZE[szx][8]: use memcpy() if all callers are POD type.
        std::copy(data, data + len, arr);
    }


    T *arr;
    IndexType len;
};


template<typename T, typename IndexType = int>
class Arr2D : public Arr<T, IndexType> {
public:
    explicit Arr2D() : len1(0), len2(0) {}
    explicit Arr2D(IndexType length1, IndexType length2) { allocate(length1, length2); }
    explicit Arr2D(IndexType length1, IndexType length2, T *data)
        : Arr(length1 * length2, data), len1(length1), len2(length2) {}
    explicit Arr2D(IndexType length1, IndexType length2, const T &defaultValue) : Arr2D(length1, length2) {
        std::fill(arr, arr + len, defaultValue);
    }

    Arr2D(const Arr2D &a) : Arr2D(a.len1, a.len2) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr2D(Arr2D &&a) : Arr2D(a.len1, a.len2, a.arr) { a.arr = nullptr; }

    Arr2D& operator=(const Arr2D &a) {
        if (this != &a) {
            if (len != a.len) {
                clear();
                init(a.len1, a.len2);
            } else {
                len1 = a.len1;
                len2 = a.len2;
            }
            copyData(a.arr);
        }
        return *this;
    }
    Arr2D& operator=(Arr2D &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len1 = a.len1;
            len2 = a.len2;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr2D() { clear(); }

    /// allocate memory if it has not been init before.
    bool init(IndexType length1, IndexType length2) {
        if (arr == nullptr) { // avoid re-init and memory leak.
            allocate(length1, length2);
            return true;
        }
        return false;
    }

    IndexType getFlatIndex(IndexType i1, IndexType i2) const { return (i1 * len2 + i2); }

    using Arr<T, IndexType>::at;
    T& at(IndexType i1, IndexType i2) { return arr[getFlatIndex(i1, i2)]; }
    const T& at(IndexType i1, IndexType i2) const { return arr[getFlatIndex(i1, i2)]; }

    using Arr<T, IndexType>::begin;
    using Arr<T, IndexType>::end;
    using Arr<T, IndexType>::front;
    using Arr<T, IndexType>::back;
    Iterator begin(IndexType i1) { return arr + (i1 * len2); }
    Iterator end(IndexType i1) { return arr + (i1 * len2) + len2; }
    ConstIterator begin(IndexType i1) const { return arr + (i1 * len2); }
    ConstIterator end(IndexType i1) const { return arr + (i1 * len2) + len2; }
    T& front(IndexType i1) { return at(i1, 0); }
    T& back(IndexType i1) { return at(i1, len - 1); }
    const T& front(IndexType i1) const { return at(i1, 0); }
    const T& back(IndexType i1) const { return at(i1, len - 1); }

    IndexType size1() const { return len1; }
    IndexType size2() const { return len2; }

protected:
    /// must not be called except init.
    void allocate(IndexType length1, IndexType length2) {
        len1 = length1;
        len2 = length2;
        Arr<T, IndexType>::allocate(length1 * length2);
    }


    IndexType len1;
    IndexType len2;
};


template<typename T, typename IndexType = int>
class Arr3D : public Arr<T, IndexType> {
public:
    explicit Arr3D() : len1(0), len2(0), len3(0), len2len3(0) {}
    explicit Arr3D(IndexType length1, IndexType length2, IndexType length3) { allocate(length1, length2, length3); }
    explicit Arr3D(IndexType length1, IndexType length2, IndexType length3, T *data)
        : Arr(length1 * length2 * length3, data), len1(length1), len2(length2), len3(length3), len2len3(length2 * length3) {}
    explicit Arr3D(IndexType length1, IndexType length2, IndexType length3, const T &defaultValue) {
        allocate(length1, length2, length3);
        std::fill(arr, arr + len, defaultValue);
    }

    Arr3D(const Arr3D &a) : Arr3D(a.len1, a.len2, a.len3) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr3D(Arr3D &&a) : Arr3D(a.len1, a.len2, a.len3, a.arr) { a.arr = nullptr; }

    Arr3D& operator=(const Arr3D &a) {
        if (this != &a) {
            clear();
            init(a.len1, a.len2, a.len3);
            copyData(a.arr);
        }
        return *this;
    }
    Arr3D& operator=(Arr3D &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len1 = a.len1;
            len2 = a.len2;
            len3 = a.len3;
            len2len3 = a.len2len3;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr3D() { clear(); }

    /// allocate memory if it has not been init before.
    bool init(IndexType length1, IndexType length2, IndexType length3) {
        if (arr == nullptr) { // avoid re-init and memory leak.
            allocate(length1, length2, length3);
            return true;
        }
        return false;
    }

    IndexType getFlatIndex(IndexType i1, IndexType i2, IndexType i3) const { return (i1 * len2len3 + i2 * len3 + i3); }

    using Arr<T, IndexType>::at;
    T& at(IndexType i1, IndexType i2, IndexType i3) { return arr[getFlatIndex(i1, i2, i3)]; }
    const T& at(IndexType i1, IndexType i2, IndexType i3) const { return arr[getFlatIndex(i1, i2, i3)]; }

    using Arr<T, IndexType>::begin;
    using Arr<T, IndexType>::end;
    Iterator begin(IndexType i1) { return arr + (i1 * len2len3); }
    Iterator end(IndexType i1) { return arr + (i1 * len2len3) + len2len3; }
    Iterator begin(IndexType i1, IndexType i2) { return arr + (i1 * len2len3 + i2 * len3); }
    Iterator end(IndexType i1, IndexType i2) { return arr + (i1 * len2len3 + i2 * len3) + len3; }
    ConstIterator begin(IndexType i1) const { return arr + (i1 * len2len3); }
    ConstIterator end(IndexType i1) const { return arr + (i1 * len2len3) + len2len3; }
    ConstIterator begin(IndexType i1, IndexType i2) const { return arr + (i1 * len2len3 + i2 * len3); }
    ConstIterator end(IndexType i1, IndexType i2) const { return arr + (i1 * len2len3 + i2 * len3) + len3; }

    IndexType size1() const { return len1; }
    IndexType size2() const { return len2; }
    IndexType size3() const { return len3; }

protected:
    /// must not be called except init.
    void allocate(IndexType length1, IndexType length2, IndexType length3) {
        len1 = length1;
        len2 = length2;
        len3 = length3;
        len2len3 = len2 * len3;
        Arr<T, IndexType>::allocate(length1 * len2len3);
    }


    IndexType len1;
    IndexType len2;
    IndexType len3;
    IndexType len2len3;
};

}


#endif // SZX_CPPUTILIBS_ARR_H
