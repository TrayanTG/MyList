#include <iostream>
#include <iterator>
using namespace std;

template<typename T, bool check>
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
    Node<T, check>& operator=(const Node &other)
    {
        if(this == &other) return *this;
        prev = other.prev;
        next = other.next;
        if(data) delete data;
        if(other.data) data = new T(*other.data);
        else data = nullptr;
        return *this;
    }
    Node<T, check>& operator=(Node &&other)
    {
        if constexpr (check)
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

template<typename T, typename Ptr, typename Ref, bool check>
class Iterator: public iterator<bidirectional_iterator_tag, T, ptrdiff_t, Ptr, Ref>
{
    Node<T, check> *curr;
public:
    //typedef bidirectional_iterator_tag iterator_category;
    //typedef T value_type;
    //typedef ptrdiff_t difference_type;
    //typedef Ptr pointer;
    //typedef Ref reference;
    template<typename, bool> friend class List;
    Iterator(Node<T, check> *curr = nullptr): curr(curr) {}
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
    Ref operator*() {return *curr->data;}
    Ptr operator->() {return &(**this);}
    operator Iterator<T, const T*, const T&, check>() const
    {
        return {curr};
    }
};

template<typename T, bool check = true>
class List
{
    static_assert(!is_const<T>::value, "List template member cannot be constant!");
    static_assert(!is_reference<T>::value, "List template member cannot be reference!");

    size_t sz;
    Iterator<T, T*, T&, check> itFront, itBack;

    void copyList(const List<T, check> &other);

public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef Iterator<T, T*, T&, check> iterator;
    typedef Iterator<T, const T*, const T&, check> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    List(): sz(0)
    {
        itFront.curr = new Node<T, check>;
        itBack = itFront;
    }
    template<typename InputIt>
    List(InputIt first, InputIt last);
    List(const List& other);
    List(List&& other);
    //~List();
    List& operator=(const List& other);
    List& operator=(List&& other);

    reference front()
    {
        return *begin();
    }
    const_reference front()const
    {
        return *begin();
    }
    reference back()
    {
        return *end().curr->prev->data;
    }
    const_reference back()const
    {
        return *end().curr->prev->data;
    }

    iterator begin()noexcept
    {
        return iterator(itFront);
    }
    const_iterator begin()const noexcept
    {
        return const_iterator(itFront);
    }
    const_iterator cbegin()const noexcept
    {
        return const_iterator(itFront);
    }
    iterator end()noexcept
    {
        return iterator(itBack);
    }
    const_iterator end()const noexcept
    {
        return const_iterator(itBack);
    }
    const_iterator cend()const noexcept
    {
        return const_iterator(itBack);
    }

    iterator rbegin()noexcept
    {
        return reverse_iterator(end());
    }
    const_iterator rbegin()const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_iterator crbegin()const noexcept
    {
        return const_reverse_iterator(end());
    }
    iterator rend()noexcept
    {
        return reverse_iterator(begin());
    }
    const_iterator rend()const noexcept
    {
        return const_reverse_iterator(begin());
    }
    const_iterator crend()const noexcept
    {
        return const_reverse_iterator(begin());
    }

    bool empty()const noexcept
    {
        return sz==0;
    }
    size_type size()const noexcept
    {
        return sz;
    }

    void clear();

    iterator insert(const_iterator pos, const T& value)
    {
        Node<T, check> *newNode = new Node<T, check>(value, nullptr, pos.curr);
        if(!empty()) newNode->prev = pos.curr->prev;
        pos.curr->prev = newNode;
        if(pos.curr == itFront.curr) --itFront;
        sz++;
        return iterator(newNode);
    }
    iterator insert(const_iterator pos, T&& value)
    {
        Node<T, check> *newNode = new Node<T, check>(move(value), nullptr, pos.curr);
        if(!empty()) newNode->prev = pos.curr->prev;
        pos.curr->prev = newNode;
        if(pos.curr == itFront.curr) --itFront;
        sz++;
        return iterator(newNode);
    }
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
    List<int> a;
    a.insert(a.begin(), 7);
    a.insert(a.begin(), 8);
    try{

    cout<<a.front()<<endl;
    cout<<a.back()<<endl;

    }catch(exception &e)
    {
        cout<<e.what()<<endl;
    }
    return 0;
}

/**
if constexpr (check)
    {
        if() throw;
    }
*/
