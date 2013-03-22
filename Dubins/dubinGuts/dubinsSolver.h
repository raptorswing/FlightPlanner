// Copyright (c) 2008-2010, Andrew Walker
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#ifndef DUBINSGUTS_H
#define DUBINSGUTS_H

#include <QtGlobal>

// Error codes
#define EDUBCOCONFIGS (1)   // Colocated configurations
#define EDUBPARAM     (2)   // Path parameterisitation error
#define EDUBBADRHO    (3)   // the rho value is invalid

// allow this code to interface with c-code
#ifdef __cplusplus
extern "C" {
#endif

class DubinsPath
{
public:
    qreal qi[3];       // the initial configuration
    qreal param[3];    // the lengths of the three segments
    qreal rho;         // model forward velocity / model angular velocity
    int type;           // encoded representation of the segment types
};

/**
 * Callback function for path sampling
 * 
 * @note the q parameter is a configuration
 * @note the t parameter is the distance along the path
 * @note return non-zero to denote sampling should be stopped
 */
typedef int (*DubinsPathSamplingCallback)(qreal q[3], qreal t);

/**
 * Generate a path from an initial configuration to
 * a target configuration, with a specified maximum turning
 * radii
 *
 * @param q0    - a configuration specified as an array of x, y, theta
 * @param q1    - a configuration specified as an array of x, y, theta
 * @param rho   - forward velocity of the vehicle divided by maximum angular velocity
 * @param path  - the resultant path
 * @return      - 0 on success
 */
int dubins_init( qreal q0[3], qreal q1[3], qreal rho, DubinsPath* path);

/**
 * Calculate the length of an initialised path
 *
 * @param path - the path to find the length of
 */
qreal dubins_path_length( DubinsPath* path );

/**
 * Calculate the configuration along the path, using the parameter t
 *
 * @param path - an initialised path
 * @param t    - a length measure, where 0 <= t < dubins_path_length(path)
 * @param q    - the configuration result
 * @returns    - non-zero if 't' is not in the correct range
 */
int dubins_path_sample( DubinsPath* path, qreal t, qreal q[3]);

/**
 * Walk along the path at a fixed sampling interval, calling the
 * callback function at each interval
 *
 * @param path      - the path to sample
 * @param cb        - the callback function to call for each sample
 * @param stepSize  - the distance along the path for subsequent samples
 */
int dubins_path_sample_many( DubinsPath* path, DubinsPathSamplingCallback cb, qreal stepSize );

/**
 * Convenience function to identify the endpoint of a path
 *
 * @param path - an initialised path
 * @param q    - the configuration result
 */
int dubins_path_endpoint( DubinsPath* path, qreal q[3] );

/**
 * Convenience function to extract a subset of a path
 *
 * @param path    - an initialised path
 * @param t       - a length measure, where 0 < t < dubins_path_length(path)
 * @param newpath - the resultant path
 */
int dubins_extract_subpath( DubinsPath* path, qreal t, DubinsPath* newpath );

// This group of function are only exposed for testing purposes only.
// The names and declarations of these functions may change in future
int dubins_init_normalised( qreal alpha, qreal beta, qreal d, qreal rho, DubinsPath* path );
void dubins_LSL( qreal alpha, qreal beta, qreal d, qreal* outputs );
void dubins_RSR( qreal alpha, qreal beta, qreal d, qreal* outputs );
void dubins_LSR( qreal alpha, qreal beta, qreal d, qreal* outputs );
void dubins_RSL( qreal alpha, qreal beta, qreal d, qreal* outputs );
void dubins_LRL( qreal alpha, qreal beta, qreal d, qreal* outputs );
void dubins_RLR( qreal alpha, qreal beta, qreal d, qreal* outputs );

// allow this code to interface with c-code
#ifdef __cplusplus
} // extern "C"
#endif

#endif // DUBINSGUTS_H


