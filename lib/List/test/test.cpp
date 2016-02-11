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
    REQUIRE( list.get() == 0 );
    REQUIRE( list.next() == 0 );
  }

  WHEN("the list has one element") {
    int i = 42;
    list.add(&i);
    REQUIRE( list.size() == 1 );
    REQUIRE( list.get() == &i );
    REQUIRE( list.next() == 0 );
  }

  WHEN("the list has 10 elements") {
    for (int i = 0; i < 10; i++) {
      list.add((int*)malloc(sizeof(int)));
    }

    REQUIRE( list.size() == 10 );

    WHEN("the list is emptied") {
      list.emptyList();
      
      REQUIRE( list.size() == 0 );
      REQUIRE( list.get() == 0 );
      REQUIRE( list.next() == 0 );
    }

  }
}

TEST_CASE("List iterator", "[iterator]") {
  LinkedList<int> list;
  LinkedListIterator<int> it = list.iterator();

  WHEN("the list is empty") {
    REQUIRE( it.next() == 0 );
  }

  WHEN("the list has one element") {
    int a = 1;

    list.add(&a);

    REQUIRE( it.next() == &a );
    REQUIRE( it.next() == 0 );
  }

  WHEN("the list has two elemetns") {
    int a = 1;
    int b = 2;

    list.add(&a);
    list.add(&b);

    REQUIRE( it.next() == &a );
    REQUIRE( it.next() == &b );
  }
}

