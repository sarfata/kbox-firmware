/******************************************************************************
* iirfilter.h
*
* Project:  KBox
* Purpose:  Class that implements single order inifinite-impulse-response filter
* Author:   Transmitterdan, adaptions for KBox by Ronnie Zeiller 2017
***************************************************************************
*   Copyright (C) 2016                                                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
***************************************************************************
*/

/**************************************************************************
 * How to use:                                                            *
 *                                                                        *
 * To create a filter object declare on instance of iirfilter. There are  *
 * 2 optional parameters to the constructor. The first (Fc) determines    *
 * the filter cutoff frequency. A value of 0.5 is basically no filtering  *
 * and smaller values decrease the cutoff frequency. If you think of the  *
 * filter as being "fast" or "slow" then 0.5 is fastest and smaller values*
 * are "slower". The second parameter (tp) selects whether the underlying *
 * filtered values represent a linear value (such as speed) or a circular *
 * angle such as direction. The angle can be either in radians or degrees.*
 * These values can be changed on the "fly" with the setFC() and setType()*
 * methods.                                                               *
 * The main method is filter() which accepts a new unfiltered value and   *
 * returns a fitered value. To obtain the most recent filter output use   *
 * the get() method. Lesser used methods are getType (returns tp) and     *
 * getFC() (returns FC). The reset() method resets the filter to zero.    *
 **************************************************************************
 */
#if ! defined( IIRFILTER_CLASS_HEADER )
#define IIRFILTER_CLASS_HEADER

#define wxIsNaN(x) isnan(x)

// Define filter types
enum filterType
{
    IIRFILTER_TYPE_LINEAR = 1 << 0,
    IIRFILTER_TYPE_DEG = 1 << 1,
    IIRFILTER_TYPE_RAD = 1 << 2
};

class iirfilter
{
public:

    iirfilter(double fc = 0.5, filterType tp = IIRFILTER_TYPE_LINEAR);
    double filter(double data); // Return filtered data given new data point
    void reset(double a = 0.0); // Clear filter
    void setFC(double fc = 0.5, bool resetAccum=false);// Set cutoff frequency
    void setType(filterType tp);       // Set type of filter (linear or angle type)
    double getFc(void) const;         // Return cutoff frequency
    filterType getType( void ) const { return type; };    // Return type of filter
    double get(void) const;           // Return the current filtered data

protected:

    void unwrap(double ang);

private:

    double a0;
    double b1;
    double accum;
    double oldAng;
    int wraps;
    filterType type;
    bool initialized;
};

#endif
