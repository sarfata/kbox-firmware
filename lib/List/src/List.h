/*

  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.


  This file is part of KBox. Unlike the rest of the project, this file is MIT
  because no-one should ever have to write that manually. I found very very few
  list implementation for Arduino I liked.

  A big thanks to NorthWestern University ~riesbeck on implementing iterators.
  Could not have done it without you!
  http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html

*/

#pragma once

template <class T> class LinkedListIterator;
template <class T> class LinkedListConstIterator;
template <class T> class LinkedListCircularIterator;

template <class T> class LinkedList {
  private:
    class node {
      public:
        T value;
        node *next;

        node(const T &v) : value(v), next(0) {};
    };

    node *head;

    friend class LinkedListIterator<T>;
    friend class LinkedListConstIterator<T>;
    friend class LinkedListCircularIterator<T>;

  public:
    typedef LinkedListIterator<T> iterator;
    typedef LinkedListConstIterator<T> constIterator;
    typedef LinkedListCircularIterator<T> circularIterator;

    LinkedList() : head(0) {};

    LinkedList(const LinkedList &l) : head(0) {
      for (constIterator it = l.begin(); it != l.end(); it++) {
        add(*it);
      }
    };

    ~LinkedList() {
      clear();
    };

    LinkedList<T>& operator=(const LinkedList<T> &l) {
      clear();
      for (constIterator it = l.begin(); it != l.end(); it++) {
        add(*it);
      }
      return *this;
    };

    void add(const T &v) {
      if (head == 0) {
        head = new node(v);
      }
      else {
        node *n = head;
        while (n->next != 0) {
          n = n->next;
        }
        n->next = new node(v);
      }
    };

    void clear() {
      node *n = head;
      while (n != 0) {
        node *next = n->next;
        delete(n);
        n = next;
      }
      head = 0;
    };

    void removeFirst() {
      node *n = head;
      if (head) {
        head = head->next;
        delete(n);
      }
    };

    iterator begin() {
      return iterator(head);
    };

    constIterator begin() const {
      return constIterator(head);
    };

    iterator end() {
      return iterator(0);
    };

    constIterator end() const {
      return constIterator(0);
    };

    circularIterator circularBegin() {
      return circularIterator(head);
    };

    circularIterator circularEnd() {
      return circularIterator(0);
    };

    int size() {
      LinkedList<T>::node *n = head;
      int sz = 0;
      while (n != 0) {
        n = n->next;
        sz++;
      }
      return sz;
    };

    bool operator==(const LinkedList<T> &l) const {
      return l.head == head;
    };

    bool operator!=(const LinkedList<T> &l) const {
      return l.head != head;
    };

};

template <class T> class LinkedListIterator {
  protected:
    typename LinkedList<T>::node *_current;

  public:
    LinkedListIterator(typename LinkedList<T>::node *node) : _current(node) {};

    LinkedListIterator<T>& operator=(const LinkedListIterator<T> &l) {
      _current = l._current;
      return *this;
    };

    bool operator==(LinkedListIterator<T> l) {
      return l._current == _current;
    }

    bool operator!=(LinkedListIterator<T> l) {
      return l._current != _current;
    }

    LinkedListIterator<T> & operator++() {
      _current = _current->next;
      return *this;
    }

    // Postfix operator takes an argument (that we do not use)
    LinkedListIterator<T> operator++(int) {
      LinkedListIterator<T> clone(*this);
      ++(*this);
      return clone;
    }

    T & operator*() {
      return _current->value;
    }

    T * operator->() {
      return &(operator*());
    }
};

template <class T> class LinkedListConstIterator {
  protected:
    typename LinkedList<T>::node const *_current;

  public:
    LinkedListConstIterator(typename LinkedList<T>::node const *node) : _current(node) {};

    bool operator==(LinkedListConstIterator<T> l) {
      return l._current == _current;
    }

    bool operator!=(LinkedListConstIterator<T> l) {
      return l._current != _current;
    }

    LinkedListConstIterator<T> & operator++() {
      _current = _current->next;
      return *this;
    }

    // Postfix operator takes an argument (that we do not use)
    LinkedListConstIterator<T> operator++(int) {
      LinkedListConstIterator<T> clone(*this);
      _current = _current->next;
      return clone;
    }

    const T & operator*() {
      return _current->value;
    }
};

template <class T> class LinkedListCircularIterator : public LinkedListIterator<T> {
  protected:
    typename LinkedList<T>::node *_head;

  public:
    LinkedListCircularIterator(typename LinkedList<T>::node *head) : LinkedListIterator<T>(head), _head(head) {};

    LinkedListCircularIterator<T> & operator++() {
      if (this->_current->next) {
        this->_current = this->_current->next;
      }
      else {
        this->_current = this->_head;
      }
      return *this;
    };

    LinkedListIterator<T> operator++(int) {
      LinkedListIterator<T> clone(*this);
      ++(*this);
      return clone;
    };
};
