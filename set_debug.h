//
// Created by atem1 on 10.03.2018.
//

#ifndef TRY1_SET_DEBUG_H
#define TRY1_SET_DEBUG_H

#include <cassert>
#include <iostream>

template<typename T>
struct set_debug {
private:

    struct my_iterator;

    struct node;

    struct base_node {
        node *left_son;
        node *right_son;
        base_node *parent;
        my_iterator *start;

        base_node() : left_son(nullptr), right_son(nullptr), parent(nullptr), start(nullptr) {}

        base_node(node *_left, node *_right, base_node *_parent) :
                left_son(_left),
                right_son(_right),
                parent(_parent),
                start(nullptr) {}

        ~base_node() {
            invalid_all();
            delete (left_son);
            delete (right_son);
        }

        void change_all(set_debug *_base) {
            my_iterator *temp = this->start;
            while (temp != nullptr) {
                temp->base = _base;
                temp = temp->next;
            }
        }

        void invalid_all() {
            my_iterator *temp = this->start;
            while (temp != nullptr) {
                temp->invalid_iter();
                temp = temp->next;
            }
            this->start = nullptr;
        }

        void swap_all(base_node *_root) {
            my_iterator *temp = this->start;
            while (temp != nullptr) {
                temp->ptr = _root;
                temp = temp->next;
            }
        }
    };

    struct node : public base_node {
        T value;

        node() = delete;

        node(node *_left, node *_right, base_node *_parent, const T &_val) :
                base_node(_left, _right, _parent),
                value(_val) {}

    };

    struct my_iterator {
    private:
        friend node;
        friend base_node;
        friend class set_debug;
        base_node *ptr;
        set_debug const *base;
        my_iterator *prev;
        my_iterator *next;

        void invalid_iter() {
            base = nullptr;
        }

        void delete_iter() {
            if (prev == nullptr) {
                ptr->start = next;
                if (next) {
                    ptr->start->prev = nullptr;
                }
                next = nullptr;
                return;
            }
            if (prev != nullptr) {
                prev->next = next;
            }
            if (next != nullptr) {
                next->prev = prev;
            }
            prev = next = nullptr;
        }

        void insert_iter() {
            if (ptr->start == nullptr) {
                ptr->start = this;
                next = nullptr;
                prev = nullptr;
                return;
            }
            next = ptr->start;
            next->prev = this;
            ptr->start = this;
            prev = nullptr;
        }

        base_node *next_node(base_node *_root) {
            if (_root->right_son == nullptr) {
                while (_root->parent != nullptr && _root->parent->right_son == _root) {
                    _root = _root->parent;
                }
                _root = _root->parent;
                return _root;
            }
            _root = _root->right_son;
            while (_root->left_son != nullptr) {
                _root = _root->left_son;
            }
            return _root;
        }

        base_node *prev_node(base_node *_root) {
            if (_root->left_son == nullptr) {
                while (_root->parent != nullptr && _root->parent->left_son == _root) {
                    _root = _root->parent;
                }
                if (_root->parent == nullptr) {
                    return nullptr;
                }
                _root = _root->parent;
                return _root;
            }
            _root = _root->left_son;
            while (_root->right_son != nullptr) {
                _root = _root->right_son;
            }
            return _root;
        }

        void check() const {
            assert(base != nullptr);
        }

        void check(set_debug *_base) const {
            assert(base == _base);
        }

    public:

        my_iterator() : ptr(nullptr),
                        base(nullptr),
                        prev(nullptr),
                        next(nullptr) {}

        my_iterator(base_node *_ptr, set_debug const *_base)
                : ptr(_ptr),
                  base(_base),
                  prev(nullptr),
                  next(nullptr) {
            insert_iter();
        }

        my_iterator(my_iterator const &other) : ptr(other.ptr),
                                                base(other.base),
                                                prev(other.prev),
                                                next(other.next) {
            check();
            insert_iter();
        }

        ~my_iterator() {
            if (base != nullptr) {
                delete_iter();
            }
        }

        T &operator*() {
            check();
            assert(ptr != base->root);
            return static_cast<node *>(ptr)->value;
        }

        my_iterator &operator=(my_iterator const &other) {
            other.check();
            if (base != nullptr) {
                delete_iter();
            }
            ptr = other.ptr;
            base = other.base;
            insert_iter();
            return *this;
        }

        friend bool operator==(my_iterator const &a, my_iterator const &b) {
            a.check();
            b.check();
            return a.ptr == b.ptr;
        }

        friend bool operator!=(my_iterator const &a, my_iterator const &b) {
            return !(a == b);
        }

        friend void swap(my_iterator &a, my_iterator &b) {
            a.check();
            b.check();
            assert(a.base == b.base);
            a.delete_iter();
            b.delete_iter();
            std::swap(a.ptr, b.ptr);
            a.insert_iter();
            b.insert_iter();
        }

        my_iterator &operator++() {
            check();
            assert(ptr != base->root);
            delete_iter();
            ptr = next_node(ptr);
            insert_iter();
            return *this;
        }

        my_iterator operator++(int) {
            my_iterator copy(*this);
            ++(*this);
            return copy;
        }

        my_iterator &operator--() {
            check();
            base_node *tmp = prev_node(ptr);
            assert(tmp != nullptr);
            delete_iter();
            ptr = tmp;
            insert_iter();
            return *this;
        }

        my_iterator operator--(int) {
            my_iterator copy(*this);
            --(*this);
            return copy;
        }

        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        base_node *getNode() {
            return ptr;
        }
    };

    void walk(base_node *tmp, set_debug *_base) {
        if (tmp->left_son != nullptr) {
            walk(tmp->left_son, _base);
        }
        if (tmp->right_son != nullptr) {
            walk(tmp->right_son, _base);
        }
        tmp->change_all(_base);
    }

    node *lower(node *_ptr, T const &elem) const {
        if (_ptr == nullptr || _ptr->value == elem) {
            return _ptr;
        }
        if (_ptr->value < elem) {
            return lower(_ptr->right_son, elem);
        }
        node *res = lower(_ptr->left_son, elem);
        if (res != nullptr && res->value >= elem) {
            return res;
        }
        return _ptr;
    }

    node *my_find(node *_ptr, T const &elem) const {
        if (_ptr == nullptr || elem == _ptr->value) {
            return _ptr;
        }
        if (elem > _ptr->value) {
            return my_find(_ptr->right_son, elem);
        }
        return my_find(_ptr->left_son, elem);
    }

    void eraseNode(base_node *_ptr) {
        if (_ptr->left_son != nullptr && _ptr->right_son != nullptr) {
            node *tmp = _ptr->right_son;
            while (tmp->left_son != nullptr) {
                tmp = tmp->left_son;
            }
            if (tmp->parent != _ptr) {
                tmp->parent->left_son = nullptr;
                tmp->parent = _ptr->parent;
                tmp->right_son = _ptr->right_son;
            } else {
                tmp->parent = _ptr->parent;
                tmp->right_son = nullptr;
            }
            tmp->left_son = _ptr->left_son;
            if (tmp->right_son != nullptr) {
                tmp->right_son->parent = tmp;
            }
            if (tmp->left_son != nullptr) {
                tmp->left_son->parent = tmp;
            }
            if (_ptr == _ptr->parent->left_son) {
                _ptr->parent->left_son = tmp;
            } else {
                _ptr->parent->right_son = tmp;
            }
        } else if (_ptr->right_son != nullptr) {
            node *tmp = _ptr->right_son;
            tmp->parent = _ptr->parent;
            if (_ptr == _ptr->parent->left_son) {
                _ptr->parent->left_son = tmp;
            } else {
                _ptr->parent->right_son = tmp;
            }
        } else if (_ptr->left_son != nullptr) {
            node *tmp = _ptr->left_son;
            tmp->parent = _ptr->parent;
            if (_ptr == _ptr->parent->left_son) {
                _ptr->parent->left_son = tmp;
            } else {
                _ptr->parent->right_son = tmp;
            }
        } else {
            if (_ptr == _ptr->parent->left_son) {
                _ptr->parent->left_son = nullptr;
            } else {
                _ptr->parent->right_son = nullptr;
            }
        }
        _ptr->parent = nullptr;
        _ptr->left_son = nullptr;
        _ptr->right_son = nullptr;
        delete static_cast<node *>(_ptr);
    }

    base_node *root;
public:
    typedef my_iterator iterator;
    typedef my_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    friend void swap(set_debug &a, set_debug &b) {
        a.walk(a.root, &b);
        b.walk(b.root, &a);
        a.root->swap_all(b.root);
        b.root->swap_all(a.root);
        std::swap(a.root, b.root);
    }

    const_iterator begin() const {
        base_node *tmp = root;
        while (tmp->left_son != nullptr) {
            tmp = tmp->left_son;
        }
        return const_iterator(tmp, this);
    }

    const_iterator end() const { return const_iterator(root, this); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    set_debug() : root() {
        root = new base_node();
    }

    set_debug(set_debug const &other) : set_debug() {
        for (auto a = other.begin(); a != other.end(); ++a) {
            insert(*a);
        }
    }

    ~set_debug() {
        delete (root->left_son);
    }

    set_debug &operator=(set_debug const &other) {
        set_debug tmp(other);
        swap(*this, tmp);
        return *this;
    }

    bool empty() const {
        return root->left_son == nullptr;
    }

    void clear() {
        root->invalid_all();
        delete (root->right_son);
        delete (root->left_son);
        root->left_son = root->right_son = nullptr;
    }

    std::pair<const_iterator, bool> insert(T const &elem) {
        if (root->left_son == nullptr) {
            root->left_son = new node(nullptr, nullptr, root, elem);
            return {const_iterator(root->left_son, this), true};
        }
        base_node *tmp = root->left_son;
        while (tmp != nullptr) {
            T val = static_cast<node *>(tmp)->value;
            if (elem == val) {
                return {const_iterator(tmp, this), false};
            }
            if (elem > val) {
                if (tmp->right_son == nullptr) {
                    tmp->right_son = new node(nullptr, nullptr, tmp, elem);
                    return {const_iterator(tmp->right_son, this), true};
                } else {
                    tmp = tmp->right_son;
                }
            }
            if (elem < val) {
                if (tmp->left_son == nullptr) {
                    tmp->left_son = new node(nullptr, nullptr, tmp, elem);
                    return {const_iterator(tmp->left_son, this), true};
                } else {
                    tmp = tmp->left_son;
                }
            }
        }
        return {const_iterator(root, this), false};
    }

    const_iterator erase(const_iterator elem) {
        assert(elem != end());
        elem.check(this);
        const_iterator copy(elem);
        ++copy;
        eraseNode(elem.getNode());
        return copy;
    }

    const_iterator find(T const &elem) const {
        node *res = my_find(root->left_son, elem);
        if (res == nullptr) {
            return const_iterator(root, this);
        }
        return const_iterator(res, this);
    }

    const_iterator lower_bound(T const &elem) const {
        if (root->left_son == nullptr) {
            return const_iterator(root, this);
        }
        node *res = lower(root->left_son, elem);
        if (res == nullptr) {
            return const_iterator(root, this);
        }
        return const_iterator(res, this);
    }

    const_iterator upper_bound(T const &elem) const {
        if (root->left_son == nullptr) {
            return const_iterator(root, this);
        }
        const_iterator res = find(elem);
        if (res == end()) {
            return lower_bound(elem);
        }
        return ++res;
    }
};

#endif //TRY1_SET_DEBUG_H
