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

#ifdef __cplusplus
extern "C" {
#endif

/** In-memory representation of the SignalK model.
 *
 * This implementation uses a simple ordered linked-list. When inserting we replace
 * pre-existing values (keeping the list ordered makes this faster).
 *
 * All strings used to define the path, the source, etc are duplicated in memory and 
 * freed when we get rid of the element.
 */
typedef struct SKKV SKKV;

/** Inserts this value at the given path, replacing any pre-existing element.
 * @return the new head for the list
 */
SKKV *skkv_set(SKKV *head, SKPath path, SKSource *source, SKValue *value);

/** Looks for an element at the given path and updates the structures source
 * and value with the source and value of this element.
 *
 * The caller should immediately make copies of string pointed to by the source
 * and value struct as those may be destroyed when the signalk model is updated.
 *
 * @param head the head of the in-memory signalk model
 * @param path the path to the element
 * @param source a structure that will be filled with the source description
 * @param value a structure that will be filled with the value of the element
 * @return SKOK if the element was found or an error
 */
SKError skkv_get(SKKV *head, SKPath path, SKSource *source, SKValue *value);

/** Returns the number of key-value stored in memory.
 */
int skkv_count(SKKV *head);

/** Release the memory used by this list and it's elements.
 */
void skkv_release(SKKV *head);

#ifdef __cplusplus
}
#endif
