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

#include <stdlib.h>
#include "signalk.h"
#include "signalk-path.h"
#include "skkv.h"
#include "skkv-private.h"

/** Looks for an element with the given path in the list starting at
 * the given head.
 *
 * Returns a pointer to the element or NULL if it was not found.
 *
 * If previous is not NULL, it will point to the last element that has a path
 * smaller than path. A special case here is that *previous will be NULL if head
 * is the last element smaller than path.
 */
SKKV *skkv_find(SKKV *head, SKPath path, SKKV **previous) {
  if (previous) {
    *previous = 0;
  }

  for (SKKV *e = head; e != 0; e = e->next) {
    int cmp = signalk_path_cmp(path, e->path);
    if (cmp == 0) {
      return e;
    }
    if (cmp > 0) {
      // This element is bigger than the element we are looking for.
      // Since the list is ordered, that means the element we are looking
      // for does not exist.
      break;
    }
    if (previous) {
      *previous = e;
    }
  }
  return 0;
}

SKKV *skkv_set(SKKV *head, SKPath path, SKSource *source, SKValue *value) {
  SKKV *previous;
  SKKV *e = skkv_find(head, path, &previous);

  SKKV *insert = malloc(sizeof(SKKV));
  insert->path = signalk_path_copy(path);
  signalk_source_copy(source, &insert->source);
  signalk_value_copy(value, &insert->value);

  if (e == 0) {
    // We are inserting a new element.
    if (previous == 0) {
      // Insert before head
      insert->next = head;
      head = insert;
    }
    else {
      // Insert between previous and previous->next
      insert->next = previous->next;
      previous->next = insert;
    }
  }
  else {
    // We are replacing an existing element (e). Free the memory used by the element
    // that we are about to delete.
    signalk_path_release(e->path);
    signalk_source_release(&e->source);
    signalk_value_release(&e->value);

    if (previous == 0) {
      // Insert as the new head
      insert->next = head->next;
      head = insert;
    }
    else {
      // Insert in place of e
      insert->next = e->next;
      previous->next = insert;
    }
    free(e);
  }
  return head;
}

SKError skkv_get(SKKV *head, SKPath path, SKSource *source, SKValue *value) {
  SKKV *e = skkv_find(head, path, 0);
  if (e == 0) {
    return SKErrorUndefinedValue;
  }

  if (source != NULL) {
    *source = e->source;
  }
  if (value != NULL) {
    *value = e->value;
  }

  return SKErrorOK;
}

int skkv_count(SKKV *head) {
  int count = 0;
  for (SKKV *e = head; e != 0; e = e->next) {
    count++;
  }
  return count;
}

void skkv_release(SKKV *head) {
  SKKV *e = head;
  while (e != NULL) {
    SKKV *next = e->next;

    signalk_path_release(e->path);
    signalk_source_release(&e->source);
    signalk_value_release(&e->value);
    free(e);

    e = next;
  }
}
