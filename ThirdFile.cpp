#include <iostream>
#include <exception>
using namespace std;

template <typename U>
struct Node {
    U data;
    Node* next;
    Node* prev;
};

template <typename T>
class Iterator {
public:
    virtual T operator*() = 0;
    virtual Iterator& operator++() = 0;
    virtual Iterator& operator--() = 0;
    virtual bool hasNext() = 0;
    virtual Node<T>* getCurrent() = 0;
    virtual void setCurrent(Node<T>* node) = 0;
};

template <typename T>
class IList {
public:
    virtual void addElem(T elem, Iterator<T>& it) = 0;
    virtual void deleteElem(Iterator<T>& it) = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;
    virtual unsigned size() = 0;
};

template <typename U>
class LinkedList : public IList<U> {
public:
    //Самые важные методы
    using PNode = Node<U>*; //указатель на узел
    LinkedList();
    void addElem(U elem);
    bool empty();
    unsigned size();
    void clear();
    LinkedList(const LinkedList& obj);
    LinkedList(LinkedList&& obj) noexcept;
    LinkedList& operator=(const LinkedList<U>& obj);
    LinkedList& operator=(LinkedList&& obj) noexcept;
    ~LinkedList();
    //Итератор
    class listIterator : public Iterator<U> {
    public:
        listIterator();
        explicit listIterator(LinkedList<U>& obj);
        listIterator(LinkedList<U>& obj, PNode& node);
        U operator*();
        listIterator& operator++();
        listIterator operator++(int);
        listIterator& operator--();
        listIterator operator--(int);
        bool operator==(const listIterator& it) const;
        bool operator!=(const listIterator& it) const;
        PNode getCurrent();
        void setCurrent(Node<U>* node);
        bool hasNext();
        ~listIterator();
        friend class LinkedList<U>;
    private:
        PNode current; //текущий узел
        LinkedList<U>* list;
    };
    void addElem(U elem, Iterator<U>& it);
    void deleteElem(Iterator<U>& it);
    listIterator searchElem(U elem);
    listIterator begin() { return listIterator(*this); };
    listIterator end() { return listIterator(); };
private:
    PNode head;
    unsigned len;
};

template<typename U>
LinkedList<U>::LinkedList() : head(nullptr), len(0) {}

template<typename U>
void LinkedList<U>::addElem(U elem) {
    auto cell = new Node<U>;
    cell->data = elem;
    if (head == nullptr) {
        head = cell;
        head->next = head;
        head->prev = head;
    }
    else {
        cell->next = head->next;
        head->next->prev = cell;
        head->next = cell;
        cell->prev = head;
    }
    len++;
}

template<typename U>
bool LinkedList<U>::empty() {
    return head == nullptr;
}

template<typename U>
unsigned LinkedList<U>::size() {
    return len;
}

template<typename U>
LinkedList<U>::listIterator::listIterator() : current(nullptr), list(nullptr) {}

template<typename U>
LinkedList<U>::listIterator::listIterator(LinkedList<U>& obj) {
    if (obj.head != nullptr) {
        current = obj.head;
        list = (&obj);
    }
}

template<typename U>
U LinkedList<U>::listIterator::operator*() {
    return current->data;
}

template<typename U>
LinkedList<U>::listIterator::listIterator(LinkedList<U>& obj, PNode& node) {
    current = node;
    list = (&obj);
}


template<typename U>
typename LinkedList<U>::listIterator LinkedList<U>::listIterator::operator++(int) {
    auto old = (*this);
    ++(*this);
    return old;
}

template<typename U>
typename LinkedList<U>::listIterator& LinkedList<U>::listIterator::operator--() {
    if (current->prev == list->head->prev) {
        current = nullptr;
    }
    else {
        current = current->prev;
    }
    return (*this);
}

template<typename U>
typename LinkedList<U>::listIterator LinkedList<U>::listIterator::operator--(int) {
    auto old = (*this);
    --(*this);
    return old;
}

template<typename U>
bool LinkedList<U>::listIterator::hasNext() {
    return current->next != list->head;
}

template<typename U>
typename LinkedList<U>::listIterator& LinkedList<U>::listIterator::operator++() {
    current->next == list->head ? current = nullptr : current = current->next;
    return (*this);
}

template<typename U>
typename LinkedList<U>::PNode LinkedList<U>::listIterator::getCurrent() {
    return current;
}

template<typename U>
void LinkedList<U>::listIterator::setCurrent(Node<U>* node) {
    current = node;
}

template<typename U>
bool LinkedList<U>::listIterator::operator==(const LinkedList::listIterator& it) const {
    if (current == nullptr && it.current == nullptr) {
        return true;
    }
    else if (current == nullptr || it.current == nullptr || it.list == nullptr || list == nullptr) {
        return false;
    }
    else {
        return current == it.current && list && it.list;
    }
}

template<typename U>
bool LinkedList<U>::listIterator::operator!=(const LinkedList::listIterator& it) const {
    return !(*this == it);
}

template<typename U>
LinkedList<U>::listIterator::~listIterator() {
    list = nullptr;
    current = nullptr;
    delete list;
    delete current;
}

template<typename U>
void LinkedList<U>::addElem(U elem, Iterator<U>& it) {
    auto cell = new Node<U>;
    cell->data = elem;
    cell->next = it.getCurrent()->next;
    cell->prev = it.getCurrent()->prev;
    it.getCurrent()->next->prev = cell;
    it.getCurrent()->next = cell;
    len++;
}

template<typename U>
void LinkedList<U>::deleteElem(Iterator<U>& it) {
    PNode tempNode = it.getCurrent();
    if (it.getCurrent()->next != it.getCurrent()) {
        it.getCurrent()->next->prev = it.getCurrent()->prev;
        it.getCurrent()->prev->next = it.getCurrent()->next;
        it.setCurrent(tempNode->next);
    }
    len--;
    delete tempNode;
}

template<typename U>
typename LinkedList<U>::listIterator LinkedList<U>::searchElem(U elem) {
    if (empty()) {
        throw exception();
    }
    PNode temp = head;
    do {
        if (temp->data == elem) {
            return listIterator(*this, temp);
        }
        temp = temp->next;
    } while (temp != head);
    throw exception();
}

template<typename U>
void LinkedList<U>::clear() {
    PNode it = head->next;
    while (it != head) {
        PNode old = it;
        it = it->next;
        delete old;
    }
    head = nullptr;
    len = 0;
}

template<typename U>
LinkedList<U>::~LinkedList() {
    if (!empty()) {
        clear();
        delete head;
    }

}

template<typename U>
LinkedList<U>::LinkedList(const LinkedList& obj) {
    head = new Node<U>;
    head->data = obj.head->data;
    PNode it = head;
    PNode oldIterator = obj.head->next;
    while (oldIterator != obj.head) {
        auto temp = new Node<U>;
        it->next = temp;
        temp->prev = it;
        temp->data = oldIterator->data;
        it = it->next;
        oldIterator = oldIterator->next;
    }
    head->prev = it;
    it->next = head;
    len = obj.len;
}

template<typename U>
LinkedList<U>::LinkedList(LinkedList&& obj) noexcept : head(obj.head), len(obj.len) {
    obj.head = nullptr;
    obj.len = 0;
}

template<typename U>
LinkedList<U>& LinkedList<U>::operator=(const LinkedList<U>& obj) {
    if (*this == obj) {
        delete head;
        head = new Node<U>;
        head->data = obj.head->data;
        PNode it = head;
        PNode oldIterator = obj.head->next;
        while (oldIterator != obj.head) {
            auto temp = new Node<U>;
            it->next = temp;
            temp->prev = it;
            temp->data = oldIterator->data;
            it = it->next;
            oldIterator = oldIterator->next;
        }
        head->prev = it;
        it->next = head;
        len = obj.len;
    }
    return (*this);
}

template<typename U>
LinkedList<U>& LinkedList<U>::operator=(LinkedList&& obj) noexcept {
    if (*this != obj) {
        delete head;
        head = obj.head;
        len = obj.len;
        obj.head = nullptr;
        obj.len = 0;
    }
    return (*this);
}

int main()
{
    LinkedList<int> listOne;
    listOne.addElem(2);

    LinkedList<int> list1(listOne);
    listOne.addElem(4);

    auto itOne = list1.begin();
    cout << (itOne.hasNext()) << endl;

    LinkedList<int> list2(std::move(listOne));
    cout << (listOne.empty()) << endl;



    LinkedList<int> listTwo;
    listTwo.addElem(2);
    listTwo.addElem(3);
    listTwo.addElem(4);

    //2 4 3
    for (int i : listTwo) {
        std::cout << i << " ";
    }
    cout << endl;

    cout << (listTwo.size() == 3) << endl;

    auto itTwo = listTwo.searchElem(2);
    cout << (*itTwo) << endl;
    cout << (itTwo.hasNext()) << endl;

    auto it1 = listTwo.searchElem(4);
    listTwo.addElem(7, it1);
    listTwo.addElem(21, it1);
    it1++;
    cout << (*it1) << endl;
    it1++;
    cout << (*it1) << endl;

    listTwo.deleteElem(it1);

    cout << (*it1) << endl;

    //2 4 21  3
    for (int i : listTwo) {
        std::cout << i << " ";
    }
    cout << endl;

    listTwo.clear();

    cout << (listTwo.empty()) << endl;

}