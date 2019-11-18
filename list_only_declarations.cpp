#include <iostream>
#include <iterator>
using namespace std;

template<typename T, bool debugMod>
struct Node
{
    T *data;
    Node *prev;
    Node *next;
    Node(): data(nullptr), prev(nullptr), next(nullptr) {}
    Node(const T &data, Node *prev = nullptr, Node *next = nullptr): prev(prev), next(next)
    {
        this->data = new T(data);
    }
    Node(T &&data, Node *prev = nullptr, Node *next = nullptr): prev(prev), next(next)
    {
        this->data = new T(move(data));
    }
    Node(const Node &other): data(other.data), prev(other.prev), next(other.next)
    {
        if(other.data) data = new T(*other.data);
    }
    Node<T, debugMod>& operator=(const Node &other)
    {
        if(this == &other) return *this;
        prev = other.prev;
        next = other.next;
        if(data) delete data;
        if(other.data) data = new T(*other.data);
        else data = nullptr;
        return *this;
    }
    Node<T, debugMod>& operator=(Node &&other)
    {
        if constexpr (debugMod)
        {
            if(this == &other) throw logic_error("Cannot use move assignment operator to itself!");
        }
        prev = other.prev;
        next = other.next;
        if(data) delete data;
        this->data = new T(move(other.data));
        return *this;
    }
    ~Node()
    {
        delete data;
    }
};

template<typename T, typename Ptr, typename Ref, bool debugMod>
class Iterator: public iterator<bidirectional_iterator_tag, T, ptrdiff_t, Ptr, Ref>
{
    Node<T, debugMod> *curr;
public:
    //typedef bidirectional_iterator_tag iterator_category;
    //typedef T value_type;
    //typedef ptrdiff_t difference_type;
    //typedef Ptr pointer;
    //typedef Ref reference;
    template<typename, bool> friend class List;
    Iterator(Node<T, debugMod> *curr = nullptr): curr(curr) {}
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
    Iterator operator++(int)
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }
    Iterator operator--(int)
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }
    Ref operator*()
    {
        if constexpr (debugMod)
        {
            if(curr->data == nullptr) throw logic_error("Called operator* for an invalid iterator");
        }
        return *curr->data;
    }
    Ptr operator->() {return &(**this);}
    //List<int>::const_iterator it = (List<int>::const_iterator)a.begin();
    operator Iterator<T, const T*, const T&, debugMod>() const
    {
        return {curr};
    }
};

template<typename T, bool debugMod = true>
class List
{
    static_assert(!is_const<T>::value, "List template member cannot be constant!");
    static_assert(!is_reference<T>::value, "List template member cannot be reference!");

    size_t sz;
    Iterator<T, T*, T&, debugMod> itFront, itBack;

    void copyList(const List<T, debugMod> &other);

    void boxSwapPtr(Iterator<T, const T*, const T&, debugMod> it1, Iterator<T, const T*, const T&, debugMod> it2);
    Iterator<T, T*, T&, debugMod> insertBoxBeforePtr(Iterator<T, const T*, const T&, debugMod> it1, Iterator<T, const T*, const T&, debugMod> it2);

public:

    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef Iterator<T, T*, T&, debugMod> iterator;
    typedef Iterator<T, const T*, const T&, debugMod> const_iterator;
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
    iterator end() noexcept;
    const_iterator end()const noexcept;
    const_iterator cend()const noexcept;

    reverse_iterator rbegin()noexcept;
    const_reverse_iterator rbegin()const noexcept;
    const_reverse_iterator crbegin()const noexcept;
    reverse_iterator rend()noexcept;
    const_reverse_iterator rend()const noexcept;
    const_reverse_iterator crend()const noexcept;

    bool empty()const noexcept;
    size_type size()const noexcept;

    void clear();

    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, T&& value);
    template<typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void push_back(const T& value);
    void push_back(T&& value);
    void pop_back();

    void push_front(const T& value);
    void push_front(T&& value);
    void pop_front();

    template <typename Compare = less<T> >
    void merge(List& other, Compare comp = Compare());
    template <typename Compare = less<T> >
    void merge(List&& other, Compare comp = Compare());

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
    void unique(BinaryPredicate p = BinaryPredicate());

    template<typename Compare = less<T> >
    void sort(Compare comp = Compare());
};

template<typename T, bool debugMod>
bool operator==( const List<T, debugMod>& lhs, const List<T, debugMod>& rhs );
template<typename T, bool debugMod>
bool operator!=( const List<T, debugMod>& lhs, const List<T, debugMod>& rhs );
template<typename T, bool debugMod>
bool operator<( const List<T, debugMod>& lhs, const List<T, debugMod>& rhs );
template<typename T, bool debugMod>
bool operator<=( const List<T, debugMod>& lhs, const List<T, debugMod>& rhs );
template<typename T, bool debugMod>
bool operator>( const List<T, debugMod>& lhs, const List<T, debugMod>& rhs );
template<typename T, bool debugMod>
bool operator>=( const List<T, debugMod>& lhs, const List<T, debugMod>& rhs );

int main()
{
   return 0;
}
