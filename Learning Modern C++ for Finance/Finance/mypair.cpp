#include "mypair.h"

template<typename T>
MyPair<T>::MyPair(const T &first, const T&second) : a_(first), b_(second){}

template<typename T>
T MyPair<T>::get_min() const{
    return a_ < b_ ? a_ : b_;
}
