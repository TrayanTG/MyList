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
class Iterator: public iterator<bidirectional_iterator_tag, T, ptrdiff_t, Ptr, Ref>
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

template<typename T>
class List
{
    static_assert(!is_const<T>::value, "List template member cannot be constant!");
    static_assert(!is_reference<T>::value, "List template member cannot be reference!");

    size_t sz;
    Node<T> *itFront, *itBack;
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef Iterator<T, T*, T&> iterator;
    typedef Iterator<T, const T*, const T&> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    List();
    template<typename InputIt>
    List(InputIt first, InputIt last);
    List(const List& other);
    List(List&& other);
    ~List();
    List& operator=(const List& other);
    List& operator=(List&& other);

    reference front();
    const_reference front()const;
    reference back();
    const_reference back()const;

    iterator begin()noexcept;
    const_iterator begin()const noexcept;
    const_iterator cbegin()const noexcept;
    iterator end()noexcept;
    const_iterator end()const noexcept;
    const_iterator cend()const noexcept;

    iterator rbegin()noexcept;
    const_iterator rbegin()const noexcept;
    const_iterator crbegin()const noexcept;
    iterator rend()noexcept;
    const_iterator rend()const noexcept;
    const_iterator crend()const noexcept;

    bool empty()const noexcept;
    size_type size() const noexcept;

    void clear();

    iterator insert(iterator pos, const T& value);
    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, T&& value);
    template<typename InputIt>
    void insert(iterator pos, InputIt first, InputIt last);
    template<typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(iterator first, iterator last);
    iterator erase(const_iterator first, const_iterator last);

    void push_back(const T& value);
    void push_back(T&& value);
    void pop_back();

    void push_front(const T& value);
    void push_front(T&& value);
    void pop_front();

    template <typename Compare = less<T> >
    void merge(List& other, Compare comp);
    template <class Compare = less<T> >
    void merge(List&& other, Compare comp);

    void splice(const_iterator pos, List& other);
    void splice(const_iterator pos, List&& other);
    void splice(const_iterator pos, List& other, const_iterator it);
    void splice(const_iterator pos, List&& other, const_iterator it);
    void splice(const_iterator pos, List& other, const_iterator first, const_iterator last);
    void splice(const_iterator pos, List&& other, const_iterator first, const_iterator last);

    void remove(const T& value);
    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate p);

    void reverse() noexcept;

    template<typename BinaryPredicate = equal_to<T> >
    void unique(BinaryPredicate p);

    template<typename Compare = less<T> >
    void sort(Compare comp);
};

template<typename T>
bool operator==( const List<T>& lhs, const List<T>& rhs );
template<typename T>
bool operator!=( const List<T>& lhs, const List<T>& rhs );
template<typename T>
bool operator<( const List<T>& lhs, const List<T>& rhs );
template<typename T>
bool operator<=( const List<T>& lhs, const List<T>& rhs );
template<typename T>
bool operator>( const List<T>& lhs, const List<T>& rhs );
template<typename T>
bool operator>=( const List<T>& lhs, const List<T>& rhs );

int main()
{

    return 0;
}
