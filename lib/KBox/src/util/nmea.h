/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Computes the NMEA Checksum of a NMEA sentence.  The string should be
 * null-terminated or contain a '*' (the NMEA end separator).
 */
uint8_t nmea_compute_checksum(const char *s);

/* Returns the checsum included in an NMEA sentence or -1 if it could not be
 * found.
 */
int nmea_read_checksum(const char *s);

/* Applies some heuristic and validates the checksum to see if a NMEA sentence
 * is valid. */
bool nmea_is_valid(const char *s);

#ifdef __cplusplus
}
#endif
