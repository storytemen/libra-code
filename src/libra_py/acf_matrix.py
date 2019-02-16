#*********************************************************************************                     
#* Copyright (C) 2017-2019 Brendan Smith, Wei Li, Alexey V. Akimov                                                   
#*                                                                                                     
#* This file is distributed under the terms of the GNU General Public License                          
#* as published by the Free Software Foundation, either version 2 of                                   
#* the License, or (at your option) any later version.                                                 
#* See the file LICENSE in the root directory of this distribution   
#* or <http://www.gnu.org/licenses/>.          
#***********************************************************************************
"""
.. module:: acf_matrix
   :platform: Unix, Windows
   :synopsis: 
       This module implements the functionality to compute Autocorrelation Functions (ACF)
       and do some transformations of them

       The assumption is that data are provided in a matrix form - not vectors, so we can handle the
       data of arbitrary dimensionality


.. moduleauthor:: Brendan Smith, Wei Li, Alexey V. Akimov

"""

import os
import sys
import math
import copy

if sys.platform=="cygwin":
    from cyglibra_core import *
elif sys.platform=="linux" or sys.platform=="linux2":
    from liblibra_core import *

import units


def average(data):
    """This function computes the average value of the data series

    Args:
        data ( list of MATRIX(ndof, 1) objects ): sequence of real-valued ndof-dimensional vectors

    Returns:
        MATRIX(ndof, 1): the average value for each DOF in the time-series
    """

    ndof = data[0].num_of_rows  
    ave = MATRIX(ndof, 1)

    sz = len(data)
    for i in xrange(sz):
        ave = ave + data[i] 

    ave = ave/float(sz)
    return ave


def center_data(data):
    """

    This function centers data on zero, by subtracting the average 
    value from each element, dof by dof

    Args:
        data ( list of MATRIX(ndof, 1) objects ): sequence of real-valued ndof-dimensional vectors

    Returns:
        MATRIX(ndof, 1): the fluctuation (deviation) value for each DOF in the time-series (dX(t) = X(t) -<X> )
    """

    data_new = []    
    ave = average(data)
    for d in data:
        data_new.append(d - ave)

    return data_new


def acf(data, dt, opt=0):
    """Compute the autocorrelation function of the given data set

    Args:
        data ( list of MATRIX(ndof, 1) objects ): sequence of real-valued ndof-dimensional vectors
        dt ( double ): time distance between the adjacent data points [units: general]
        opt ( int ): selector of the convention to to compute ACF

            * 0 : the chemist convention,  (1/(N-h)) Sum_{t=1,N-h} (Y[t]*Y[t+h])
            * 1 : the statistician convention, (1/N) Sum_{t=1,N-h} (Y[t]*Y[t+h])

    Returns:
        tuple: (T, nautocorr, autocorr), where:

            T (list of sz doubles ): lag time scale for the ACF [units: same as for dt]
            nautocorr (list of sz doubles ): normalized ACF
            autocorr (list of sz doubles ): un-normalized ACF

    SeeAlso:
        https://www.itl.nist.gov/div898/handbook/eda/section3/autocopl.htm


    """

    sz = len(data)    # For now, we will use the full data set 

                      ###               how many elements we have in the time series
                      ###  old comments we use only a half of the point, because of the 
                      ###               poorer statistics we get otherwise
    autocorr = []
    ndof = data[0].num_of_rows

    for i in xrange(sz):
        total = 0.0
        for j in xrange(sz-i):
            total += (data[j].T()*data[j+i]).get(0)   # scalar product
        if opt==0:
            autocorr.append( total/((sz-i)*ndof) )  # less bias, chemistry adopted
        elif opt==1:
            autocorr.append( total/(sz*ndof) )      # statistically-preferred option

    #normalize the ACF	
    nautocorr = []
    norm = 1.0
    if math.fabs(autocorr[0])>0.0:
        norm = 1.0/autocorr[0]

    T = []
    for it in range(0,sz):
        T.append(it*dt)
        nautocorr.append( norm * autocorr[it] )

    return T, nautocorr, autocorr



def ft(X, wspan, dw, dt):  
    """Discrete Fourier transform

    We do have a number of FT and FFT functions in the Libra core, but
    this one may be also convenient to have

    Args:
        X ( list of floats ): data time-series
        wspan ( float ): is the range (the maximal value) of frequencies we want to compute
        dw ( float ): is the distance between the nearby points on the frequency scale
        dt ( float ): is the time step

    Returns: 
        tuple: (W, J): where

            W ( list of npoints doubles): frequencies               
            J ( list of npoints doubles): amplitudes of the cos-transform

    """

    ############### based on the code from Pyxaid ###################
    sz=len(X)    # the # of input points
    npoints = int(wspan/dw)   # the # of output points    

    J = [0.0] * npoints   # FT
    W = [0.0] * npoints   # frequencies

    for iw in xrange(npoints):
        w = iw * dw

        J[iw] = 1.0  # corresponds to it = 0
        for it in range(1,sz):
            t = it * dt
            J[iw] += 2.0*math.cos(w * t)*X[it]

        W[iw] = w
        J[iw] *= dt

    return W, J


def py_cft(X, dt):  
    """Complex Discrete Fourier transform

    We do have a number of FT and FFT functions in the Libra core, but
    this one may be also convenient to have

    According to this definition: http://mathworld.wolfram.com/DiscreteFourierTransform.html

    Args:
        X ( list of floats ): data time-series
        wspan ( float ): is the range (the maximal value) of frequencies we want to compute
        dw ( float ): is the distance between the nearby points on the frequency scale
        dt ( float ): is the time step

    Returns: 
        tuple: (W, C, S): where

            W ( list of npoints doubles): frequencies               
            C ( list of npoints doubles): amplitudes of the cos-transform
            S ( list of npoints doubles): amplitudes of the sin-transform

    """

    ############### based on the code from Pyxaid ###################
    N = len(X)    # the # of input points
    dv = 1.0/(N*dt)
    dw = 2.0*math.pi*dv

    W = [0.0] * npoints   # frequencies
    C = [0.0] * npoints   # FT
    S = [0.0] * npoints   # FT

    for iw in xrange(N):
        w = iw * dw

        C[iw], S[iw] = 0.0, 0.0
        for it in xrange(N):
            t = it * dt            
            C[iw] += math.cos(w * t)*X[it]   
            S[iw] -= math.sin(w * t)*X[it]

        W[iw] = w

    return W, C, S




def recipe1(data, dt, wspan, dw, acf_filename="acf.txt", spectrum_filename="spectrum.txt", do_center=1, opt=0):
    """
    
    Args:
        data ( list of MATRIX(ndof, 1) objects ): sequence of real-valued ndof-dimensional vectors
        dt ( double ): time distance between the adjacent data points [units: fs]
        wspan ( double ): window of frequencies for the Fourier transform [ units: cm^-1 ]
        dw ( double ): grid points spacing in the frequency domain [ units: cm^-1 ]
        acf_filename ( string ): the name of the file where to print the ACF
        spectrum_filename ( string ): the name of the file where to print the spectrum
        do_center ( int ): a flag controlling whether to center data (=1) or not (=0)
            Centering means we subtract the average value (over all the data points) from all
            the data points - this way, we convert values into their fluctuations.

        opt ( int ): selector of the convention to to compute ACF

            * 0 : the chemist convention,  (1/(N-h)) Sum_{t=1,N-h} (Y[t]*Y[t+h])
            * 1 : the statistician convention, (1/N) Sum_{t=1,N-h} (Y[t]*Y[t+h])


    Returns:
        None: 


    """

    wspan = wspan * units.inv_cm2Ha  # convert to Ha (atomic units)
    dw = dw * units.inv_cm2Ha        # convert to Ha (atomic units)
    dt = dt * units.fs2au            # convert to  atomic units of time
    
    # ACFs
    T, norm_acf, row_acf = acf( center_data(data) , dt, opt)
    sz = len(T)

    f = open(acf_filename,"w")
    for it in xrange(sz):
        f.write("%8.5f  %8.5f  %8.5f  \n" % (T[it]/units.fs2au , norm_acf[it], row_acf[it]))
    f.close()

    # FT
    W, J = ft(norm_acf, wspan, dw, dt)
    sz = len(W)
    f = open(spectrum_filename,"w")
    for iw in xrange(sz):
        f.write("%8.5f  %8.5f  \n" % (W[iw]/units.inv_cm2Ha, J[iw] ) )
    f.close()




def recipe2(data, dt, wspan, dw, acf_filename="acf.txt", spectrum_filename="spectrum.txt", do_center=1):
    """
    data (MATRIX (ndof x 1) ) - data points (each is a multidimensional)
    dt (float) [ fs ] - timestep between adjacent data points
    dspan (float) [ cm^-1 ] - window of frequencies for the Fourier transform
    dw (float) [ cm^-1 ] - grid points spacing in the frequency domain
    acf_filename (string) - the name of the file where to print the ACF
    spectrum_filename (string) - the name of the file where to print the spectrum
    do_center (int) - a flag controlling whether to center data (=1) or not (=0)
    Centering means we subtract the average value (over all the data points) from all
    the data points - this way, we convert values into their fluctuations.
    """

    wspan = wspan * units.inv_cm2Ha  # convert to Ha (atomic units)
    dw = dw * units.inv_cm2Ha        # convert to Ha (atomic units)
    dt = dt * units.fs2au            # convert to  atomic units of time


    # ACFs
    T, norm_acf, row_acf = acf2(data,dt)
    sz = len(norm_acf)

    f = open(acf_filename,"w")
    for it in xrange(sz):
        f.write("%8.5f  %8.5f  \n" % (T[it]/fs2au, norm_acf[it]))
    f.close()

    # FT
    W, J = ft(norm_acf, wspan, dw, dt)
    sz = len(W)
    f = open(spectrum_filename,"w")
    for iw in xrange(sz):
        f.write("%8.5f  %8.5f  \n" % (W[iw]/inv_cm2Ha, J[iw] ) )
    f.close()


    
if __name__ == '__main__':

    # Parameters
    inv_cm2ev = (1.0/8065.54468111324)
    ev2Ha = (1.0/27.211)    # 27.2 ev is 1 Ha 
    inv_cm2Ha = inv_cm2ev * ev2Ha
    fs2au = (1.0/0.02419)   # 40 a.u. is 1 fs 

    # Test case: 3 frequences
    data = []
    dt = 1.0 * fs2au
    dw = 1.0 * inv_cm2Ha
    w1 = 500.0 * inv_cm2Ha
    w2 = 1400.0 * inv_cm2Ha
    w3 = 850.0 * inv_cm2Ha
    wspan = 2000.0 * inv_cm2Ha

    for it in xrange(1000):
        t = it * dt
        d = MATRIX(3,1)
        d.set(0, 0, math.sin(w1*t) )
        d.set(1, 0, math.cos(w2*t) )
        d.set(2, 0, math.sin(w3*t) )
        data.append( d )
    
    recipe1(data, 1.0, 2000.0, 1.0)
    #recipe2(data, 1.0, 2000.0, 1.0)

