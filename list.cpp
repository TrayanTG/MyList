#include <iostream>
#include <iterator>
using namespace std;

template<typename T>
struct Node
{
    T *data;
    Node *prev;
    Node *next;
    Node(const T &data, Node *prev = nullptr, Node *next = nullptr): data(data), prev(prev), next(next) {}
};

template<typename T, typename Ptr, typename Ref>
class Iterator:public iterator<bidirectional_iterator_tag, T, ptrdiff_t, Ptr, Ref>
{
    Node<T> *curr;
public:
    //typedef bidirectional_iterator_tag iterator_category;
    //typedef T value_type;
    //typedef ptrdiff_t difference_type;
    //typedef Ptr pointer;
    //typedef Ref reference;
    Iterator(const Node<T> *curr = nullptr): curr(curr) {}
    bool operator!=(const Iterator &other)const {return curr!=other.curr;}
    bool operator==(const Iterator &other)const {return curr==other.curr;}
    Iterator& operator++()
    {
        curr=curr->next;
        return *this;
    }
    Iterator& operator--()
    {
        curr=curr->prev;
        return *this;
    }
    Iterator& operator++(int)
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }
    Iterator& operator--(int)
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }
    Ref operator*() {return curr->data;}
    Ptr operator->() {return &(**this);}
    operator Iterator<T, const T*, const T&>() const
    {
        return {curr};
    }
};


int main()
{

    return 0;
}
