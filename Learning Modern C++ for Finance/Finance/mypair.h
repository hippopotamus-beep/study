#ifndef MYPAIR_H
#define MYPAIR_H

template <typename T = double>
class MyPair
{
public:
    MyPair(const T &first, const T &second);

    T get_min() const;

private:
    T a_, b_;
};

template class MyPair<>;

#endif // MYPAIR_H
