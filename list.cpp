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
        if constexpr (check)
        {
            if(curr->data == nullptr) throw logic_error("Called operator* for an invalid iterator");
        }
        return *curr->data;
    }
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

    void copyList(const List<T, check> &other)
    {
        insert(begin(), other.begin(), other.end());
    }

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
    List(InputIt first, InputIt last): sz(0)
    {
        itFront.curr = new Node<T, check>;
        itBack = itFront;
        insert(begin(), first, last);
    }
    List(const List& other): sz(0)
    {
        if constexpr (check)
        {
            if(other.itFront == other.itBack && !other.empty()) throw logic_error("Copy constructor called with bad list object");
        }
        itFront.curr = new Node<T, check>;
        itBack = itFront;
        copyList(other);
    }
    List(List&& other): sz(other.sz)
    {
        if constexpr (check)
        {
            if(other.itFront == other.itBack && !other.empty()) throw logic_error("Move constructor called with bad list object");
        }
        itFront = other.itFront;
        itBack = other.itBack;
        other.itFront.curr = new Node<T, check>;
        other.itBack = other.itFront;
    }
    ~List()
    {
        clear();
    }
    List& operator=(const List& other)
    {
        if(this == &other) return *this;
        clear();
        copyList(other);
        return *this;
    }
    List& operator=(List&& other)
    {
        if constexpr (check)
        {
            if(this == &other) throw logic_error("Object being moved to itself!");
        }
        if(!empty()) clear();
        delete itFront.curr;
        sz = other.sz;
        itFront = other.itFront;
        itBack = other.itBack;
        other.itFront.curr = new Node<T, check>;
        other.itBack = other.itFront;
    }

    reference front()
    {
        if constexpr (check)
        {
            if(empty())throw logic_error("List is empty!");
        }
        return *begin();
    }
    const_reference front()const
    {
        if constexpr (check)
        {
            if(empty())throw logic_error("List is empty!");
        }
        return *begin();
    }
    reference back()
    {
        if constexpr (check)
        {
            if(empty())throw logic_error("List is empty!");
        }
        return *end().curr->prev->data;
    }
    const_reference back()const
    {
        if constexpr (check)
        {
            if(empty())throw logic_error("List is empty!");
        }
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
    iterator end() noexcept
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

    reverse_iterator rbegin()noexcept
    {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin()const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin()const noexcept
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend()noexcept
    {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend()const noexcept
    {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crend()const noexcept
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

    void clear()
    {
        erase(begin(), end());
    }

    iterator insert(const_iterator pos, const T& value)
    {
        Node<T, check> *newNode = new Node<T, check>(value, nullptr, pos.curr);
        if(!empty()) newNode->prev = pos.curr->prev;
        if(pos == itFront)
        {
            pos.curr->prev = newNode;
            --itFront;
        }
        else
        {
            pos.curr->prev->next = newNode;
            pos.curr->prev = newNode;
        }
        sz++;
        return iterator(newNode);
    }
    iterator insert(const_iterator pos, T&& value)
    {
        Node<T, check> *newNode = new Node<T, check>(move(value), nullptr, pos.curr);
        if(!empty()) newNode->prev = pos.curr->prev;
        if(pos == itFront)
        {
            pos.curr->prev = newNode;
            --itFront;
        }
        else
        {
            pos.curr->prev->next = newNode;
            pos.curr->prev = newNode;
        }
        sz++;
        return iterator(newNode);
    }
    template<typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        if(first == last) return iterator(pos.curr);
        auto it = insert(pos, *first++);
        pos = it++;
        while(first!=last)
        {
            it = ++insert(it, *first++);
        }
        return iterator(pos.curr);
    }

    iterator erase(const_iterator pos)
    {
        if constexpr (check)
        {
            if(empty())throw logic_error("Invalid element to be erased!");
            if(pos == end()) throw logic_error("Cannot remove the end() element!");
        }
        if(pos == itFront)
        {
            itFront++;
            itFront.curr->prev = nullptr;
        }
        else
        {
            pos.curr->next->prev = pos.curr->prev;
            pos.curr->prev->next = pos.curr->next;
        }
        sz--;
        auto t=pos.curr->next;
        delete pos.curr;
        return iterator(t); /// Not tested
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        while(first != last)
        {
            auto t = first++;
            erase(t);
        }
        return iterator(first.curr); /// Not tested
    }

    void push_back(const T& value)
    {
        if(empty()) insert(begin(), value);
        else insert(end(), value);
    }
    void push_back(T&& value)
    {
        if(empty()) insert(begin(), value);
        else insert(end(), value);
    }
    void pop_back()
    {
        if constexpr (check)
        {
            if(empty()) throw logic_error("Popping from an empty list!");
        }
        erase(--end());
    }

    void push_front(const T& value)
    {
        insert(begin(), value);
    }
    void push_front(T&& value)
    {
        insert(begin(), value);
    }
    void pop_front()
    {
        if constexpr (check)
        {
            if(empty()) throw logic_error("Popping from an empty list!");
        }
        erase(begin());
    }

    template <typename Compare = less<T> >
    void merge(List& other, Compare comp = Compare())
    {
        merge(move(other));
    }
    template <typename Compare = less<T> >
    void merge(List&& other, Compare comp = Compare())
    {
        if(this == &other) return;
        auto it = begin();
        auto it2 = other.begin();
        while(it != end() && it2 != other.end())
        {
            if(comp(*it2, *it))
            {
                insert(it, move(*it2++));
            }
            else ++it;
        }
        while(it2 != other.end())
        {
            push_back(move(*it2++));
        }
        other.clear();
        other.sz=0;
    }

    void splice(const_iterator pos, List& other)
    {
        splice(pos, move(other));
    }
    void splice(const_iterator pos, List&& other)
    {
        splice(pos, other, other.begin(), other.end());
    }
    void splice(const_iterator pos, List& other, const_iterator it)
    {
        splice(pos, move(other), it);
    }
    void splice(const_iterator pos, List&& other, const_iterator it)
    {
        if constexpr (check)
        {
            if(it == other.end()) throw logic_error("Cannot transfer end() of the list!");
        }
        if(it != other.begin()) it.curr->prev->next = it.curr->next;
        else ++other.itFront; ///here
        it.curr->next->prev = it.curr->prev;
        if(pos != begin()) pos.curr->prev->next = it.curr;
        else itFront.curr = it.curr;
        it.curr->prev = pos.curr->prev;
        it.curr->next = pos.curr;
        pos.curr->prev = it.curr;

        other.sz--;
        sz++;
    }
    void splice(const_iterator pos, List& other, const_iterator first, const_iterator last)
    {
        splice(pos, move(other), first, last);
    }
    void splice(const_iterator pos, List&& other, const_iterator first, const_iterator last)
    {
        while(first!=last)
        {
            auto temp = first++;
            splice(pos, other, temp);
        }
    }

    void remove(const T& value)
    {
        remove_if([&](int n){ return n == value; });
    }
    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate p)
    {
        auto it=begin();
        while(it!=end())
        {
            if(p(*it)) it = erase(it);
            else ++it;
        }
    }

    void reverse() noexcept
    {
        if(size()<2) return;
        auto it = begin();
        itFront = itBack.curr->prev;
        while(it!=end())
        {
            auto temp = it.curr->next;
            swap(it.curr->prev,it.curr->next);
            if(it.curr->next == nullptr) it.curr->next = itBack.curr;
            if(it.curr->prev == itBack.curr) it.curr->prev = nullptr;
            it = temp;
        }
    }

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
    List<int> a, b;
    a.push_back(1);
    a.push_back(2);
    /*a.push_back(4);
    a.push_back(5);
    a.push_back(8);
    a.push_back(8);
    a.push_back(4);*/

    b.push_back(0);
    b.push_back(1);
    b.push_back(3);
    b.push_back(5);
    b.push_back(6);
    b.push_back(7);

    for(auto &&it: a) cout<<it<<' ';
    cout<<endl;
    for(auto &&it: b) cout<<it<<' ';
    cout<<endl<<endl;

    a.reverse();


    for(auto &&it: a) cout<<it<<' ';
    cout<<endl;
    for(auto &&it: b) cout<<it<<' ';
    cout<<endl<<endl;



    try
    {
        //a.merge(b);
        b.push_back(2);
        b.push_back(3);
        for(auto &&it: b)
            cout<<it<<' ';
        cout<<endl<<b.size()<<endl;

    }
    catch(exception &e)
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
