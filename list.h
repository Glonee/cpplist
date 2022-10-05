#include <utility>
#ifndef GLONEE_GO_LIST
#define GLONEE_GO_LIST
namespace go {
template <typename T>
class List {
   public:
    class iterator;
    class Element {
       public:
        T value;

       private:
        Element *prev, *next;
        List* list;
        friend class List;
        Element(const T& val)
            : value(val), prev(nullptr), next(nullptr), list(nullptr) {}
        Element(T&& val)
            : value(std::move(val)),
              prev(nullptr),
              next(nullptr),
              list(nullptr) {}
        Element(Element* prev, Element* next, List* list)
            : prev(prev), next(next), list(list) {}
        Element() = delete;
        Element(const Element&) = delete;
        Element& operator=(const Element&) = delete;

       public:
        Element* Next() {
            if (list != nullptr && next != &list->root) {
                return next;
            }
            return nullptr;
        }
        Element* Prev() {
            if (list != nullptr && prev != &list->root) {
                return prev;
            }
            return nullptr;
        }
    };

   private:
    Element root;
    int len;
    Element* insert(Element* e, Element* at) {
        e->prev = at;
        e->next = at->next;
        at->next = e;
        e->next->prev = e;
        e->list = this;
        len++;
        return e;
    }
    void move(Element* e, Element* at) {
        if (e == at) {
            return;
        }
        e->prev->next = e->next;
        e->next->prev = e->prev;

        e->prev = at;
        e->next = at->next;
        at->next = e;
        e->next->prev = e;
    }
    void copy(List&& li) {
        if (li.len > 0) {
            root.prev = li.root.prev;
            root.next = li.root.next;
            root.prev->next = &root;
            root.next->prev = &root;
            len = li.len;
            li.root.prev = &li.root;
            li.root.next = &li.root;
            li.len = 0;
            for (auto cur = root.next; cur != &root; cur = cur->next) {
                cur->list = this;
            }
        } else {
            root.prev = &root;
            root.next = &root;
            len = 0;
        }
    }
    List(const List&) = delete;
    List& operator=(const List&) = delete;

   public:
    List() noexcept : root(&root, &root, this), len(0) {}
    List(List&& li) noexcept : root(nullptr, nullptr, this) {
        copy(std::move(li));
    }
    List& operator=(List&& li) noexcept {
        auto cur = root.next;
        while (cur != &root) {
            auto next = cur->next;
            delete cur;
            cur = next;
        }
        copy(std::move(li));
        return *this;
    }
    ~List() {
        auto cur = root.next;
        while (cur != &root) {
            auto next = cur->next;
            delete cur;
            cur = next;
        }
    }
    int size() { return len; }
    Element* front() {
        if (len == 0) {
            return nullptr;
        }
        return root.next;
    }
    Element* back() {
        if (len == 0) {
            return nullptr;
        }
        return root.prev;
    }
    Element* push_front(const T& val) {
        return insert(new Element(val), &root);
    }
    Element* push_front(T&& val) {
        return insert(new Element(std::move(val), &root));
    }
    Element* push_back(const T& val) {
        return insert(new Element(val), root.prev);
    }
    Element* push_back(T&& val) {
        return insert(new Element(std::move(val)), root.prev);
    }
    Element* insert_before(const T& val, Element* mark) {
        if (mark->list != this) {
            return nullptr;
        }
        return insert(new Element(val), mark->prev);
    }
    Element* insert_before(T&& val, Element* mark) {
        if (mark->list != this) {
            return nullptr;
        }
        return insert(new Element(std::move(val)), mark->prev);
    }
    Element* insert_after(const T& val, Element* mark) {
        if (mark->list != this) {
            return nullptr;
        }
        return insert(new Element(val), mark);
    }
    Element* insert_after(T&& val, Element* mark) {
        if (mark->list != this) {
            return nullptr;
        }
        return insert(new Element(std::move(val)), mark);
    }
    void move_to_front(Element* e) {
        if (e->list != this || root.next == e) {
            return;
        }
        move(e, &root);
    }
    void move_to_back(Element* e) {
        if (e->list != this || root.prev == e) {
            return;
        }
        move(e, root.prev);
    }
    // delete e from the list
    // Note: Don't use e after calling this function since e has been deleted!
    void remove(Element* e) {
        if (e->list == this) {
            e->prev->next = e->next;
            e->next->prev = e->prev;
            e->next = nullptr;
            e->prev = nullptr;
            e->list = nullptr;
            delete e;
            len--;
        }
    }
    void move_before(Element* e, Element* mark) {
        if (e == mark || e->list != this || mark->list != this) {
            return;
        }
        move(e, mark->prev);
    }
    void move_after(Element* e, Element* mark) {
        if (e == mark || e->list != this || mark->list != this) {
            return;
        }
        move(e, mark);
    }
    iterator begin() { return front(); }
    iterator end() { return nullptr; }
};
template <typename T>
class List<T>::iterator {
    friend class List;
    Element* cur;
    iterator(Element* cur) : cur(cur) {}

   public:
    T& operator*() { return cur->value; }
    iterator& operator++() {
        cur = cur->Next();
        return *this;
    }
    bool operator!=(iterator& c) { return cur != c.cur; }
};
}  // namespace go
#endif