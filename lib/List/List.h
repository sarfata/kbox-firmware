
#pragma once

template <class T> class LinkedListIterator;
template <class T> class CircularLinkedListIterator;

/* LinkedList template.
 *
 * Implemented with a head sentinel so the first element of the list can be null (current = 0) when the list is empty.
 */
template <class T> class LinkedList {
  private:
    T *_e;
    LinkedList<T> *_next;

  public:
    LinkedList() : _e(0), _next(0) {};
    LinkedList(T *e) : _e(e), _next(0) {};

    void add(T *element) {
      // The list can be created empty so we need to support inserting in the first element.
      if (_e == 0) {
        _e = element;
      }
      else {
        if (_next == 0) {
          _next = new LinkedList<T>(_e);
        }
        else {
          _next->add(_e);
        }
      }
    };

    T *get() {
      return _e;
    };

    LinkedList<T> *next() {
      return _next;
    };

    LinkedListIterator<T> iterator() {
      return LinkedListIterator<T>(this);
    };

    CircularLinkedListIterator<T> circularIterator() {
      return CircularLinkedListIterator<T>(this);
    };

    int size() {
      int size = 0;
      for (LinkedList<T> *it = this; it != 0 && it->get() != 0; size++, it = it->next()) {
      }
      return size;
    }

    void emptyList() {
      // Delete all the LinkedList containers that we created when adding elements.
      // Actual data of the list should be deleted by caller.
      LinkedList<T> *e = _next;
      while (e) {
        LinkedList<T> *toDelete = e;
        e = toDelete->next();
        delete(e);
      }
      _e = 0;
      _next = 0;
    };
};

template <class T> class LinkedListIterator {
  protected:
    LinkedList<T> *_current;

  public:
    LinkedListIterator(LinkedList<T> *list) : _current(list) {};

    T *current() {
      if (_current)
        return _current->get();
      else
        return 0;
    };

    /* Returns the current element and moves to next one. Returns 0 when list is exhausted.
     */
    T *next() {
      if (_current) {
        T *c = _current->get();
        _current = _current->next();
        return c;
      }
      else {
        return 0;
      }
    };
};

template <class T> class CircularLinkedListIterator : public LinkedListIterator<T> {
  private:
    LinkedList<T> *_first;

  public:
    CircularLinkedListIterator(LinkedList<T> *list) : LinkedListIterator<T>(list), _first(list) {};

    T *next() {
      T *e = LinkedListIterator<T>::next();

      // Resets the list when we have reached the end.
      if (!this->current()) {
        this->_current = _first;
      }
      return e;
    };
};

