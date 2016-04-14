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
*/

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include <stdarg.h>
#define DEBUG(...) debug(__FILE__, __LINE__, __VA_ARGS__)

void debug(const char *fname, int lineno, const char *fmt, ... ) {
  printf("%s: %i ", fname, lineno);
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  printf("%s\n", tmp);
}

#include "List.h"


TEST_CASE("List basics", "[list]") {
  LinkedList<int> list;

  WHEN("the list is empty") {
    REQUIRE( list.size() == 0 );
    REQUIRE( list.begin() == list.end() );
  }

  WHEN("the list has one element") {
    int i = 42;
    list.add(i);
    REQUIRE( list.size() == 1 );
    REQUIRE( *(list.begin()) == i );

    LinkedList<int>::iterator it = list.begin();
    REQUIRE( it != list.end() );
    it++;
    REQUIRE( it == list.end() );
  }

  WHEN("the list has 10 elements") {
    for (int i = 0; i < 10; i++) {
      list.add(i);
    }

    REQUIRE( list.size() == 10 );

    LinkedList<int>::iterator it = list.begin();
    THEN("all elements should be there") {
      for (int i = 0; i < 10; i++, it++) {
        REQUIRE( *it == i );
      }
    }

    WHEN("you reset the iterator") {
      for (int i = 0; i < 5; i++, it++) {
        REQUIRE( *it == i );
      }

      THEN("you should still be able to list the firs elements") {
        it = list.begin();

        for (int i = 0; i < 5; i++, it++) {
          REQUIRE( *it == i );
        }
      }
    }

    WHEN("the list is emptied") {
      list.clear();
      REQUIRE( list.size() == 0 );
    }
  }

  WHEN("you make a copy of the list") {
    for (int i = 0; i < 10; i++) {
      list.add(i);
    }

    LinkedList<int> l2 = list;
    REQUIRE( l2.size() == 10 );
  }


}

int refcnt = 0;

class Object {
  private:
    std::string label;
  public:
    Object(const Object &o) : label(o.label) { refcnt++; };
    Object(std::string l) : label(l) { refcnt++; };
    ~Object() { refcnt--; };
};

TEST_CASE("Check for leaks", "[list]") {
  LinkedList<Object> list;

  WHEN("elements are added to the list") {
    // Note the local scope here
    {
      Object o1("a"), o2("b");

      REQUIRE( refcnt == 2 );

      list.add(o1);
      list.add(o2);

      REQUIRE( refcnt == 4 );
    }

    REQUIRE( refcnt == 2 );
  }

  WHEN("the list is cleared") {
    list.clear();

    REQUIRE( refcnt == 0 );
  }

  WHEN("you make a copy of the list") {
    {
      Object o1("a"), o2("b");

      list.add(o1);
      list.add(o2);
    }
    REQUIRE( refcnt == 2);

    {
      LinkedList<Object> l2 = list;

      REQUIRE(refcnt == 4);
    }
    REQUIRE(refcnt == 2);
  }
}

TEST_CASE("Circular iterator", "[list]") {
  LinkedList<int> list;

  LinkedList<int>::circularIterator it = list.circularBegin();

  WHEN("The list is empty") {
    REQUIRE(it == list.circularEnd());
  }

  WHEN("The list has one element") {
    int i = 1;
    list.add(i);

    it = list.circularBegin();

    REQUIRE(*it == 1);
    REQUIRE(it != list.circularEnd());
    it++;
    REQUIRE(*it == 1);
    REQUIRE(it != list.circularEnd());
    it++;
    REQUIRE(it != list.circularEnd());
    // etc...
  }

}
