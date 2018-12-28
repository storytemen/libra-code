#*********************************************************************************
#* Copyright (C) 2018 Alexey V. Akimov
#*
#* This file is distributed under the terms of the GNU General Public License
#* as published by the Free Software Foundation, either version 2 of
#* the License, or (at your option) any later version.
#* See the file LICENSE in the root directory of this distribution
#* or <http://www.gnu.org/licenses/>.
#*
#*********************************************************************************/

import sys
import cmath
import math
import os

if sys.platform=="cygwin":
    from cyglibra_core import *
elif sys.platform=="linux" or sys.platform=="linux2":
    from liblibra_core import *

from libra_py import units


fs = units.fs2au

method = 0 
tmax = 40 
dt = 0.20
dtau = dt/5.0

gamma = 0.1

nomega = 500
w_c = 1.0
dw = 15.0*w_c/float(nomega)




i1 = -1
for w_DA in [0.0]: #, 2.0]:   # Donor-Acceptor energy gap
    i1 += 1
    i2 = -1
    for s in [-1.0]: # -1.0, 1.0, 3.0]:   # Noneq. initial shift of primary mode
        i2 += 1
        i3 = -1
        for beta in [1.0]: #, 2.0, 5.0]:   # themal energy
            i3 += 1
            i4 = -1
            for etha in [0.5]: #, 1.0, 2.0]:  # friction
                i4 += 1

                print "w_DA = ", w_DA
                print "s = ", s
                print "beta = ", beta
                print "etha = ", etha

                #============ Setup the parameters ===============

                params = {}
                params["Er"] = 0.5 * w_c
                params["omega_DA"] = w_DA * w_c
                params["omega"] = [0.5*w_c]
                params["coup"] = [0.0]

                for a in xrange(1, nomega-1):
                    w_a = a*dw
                    params["omega"].append(w_a)

                    J_a = etha * w_a * math.exp(-w_a/w_c)      # Eq. 45
                    c_a = math.sqrt((2.0/math.pi)*J_a*w_a*dw)  # Eq. 62
                    params["coup"].append(c_a)

                #============ Setup the parameters =============== 
                ndof = len(params["omega"])
                print "ndof = ", ndof
                omega = Py2Cpp_double(params["omega"])
                coeff = Py2Cpp_double(params["coup"])

                U = MATRIX(ndof, ndof)
                omega_nm = normal_modes(omega, coeff, U)
                #print "U = "; U.show_matrix()

                dE = params["omega_DA"]

                print "Omega = ", params["omega"][0]
                print "Er = ", params["Er"]
                y0 = eq_shift(params["Er"], params["omega"][0])
                print "y0 = ", y0

                req_nm = compute_req(omega, coeff, y0, U)
                #print "req_nm = ", Cpp2Py(req_nm)

                gamma_nm = compute_TT_scaled(U, gamma)
                #print "gamma_nm = ", Cpp2Py(gamma_nm)

                shift_NE = compute_TT_scaled(U, s)
                #print "shift_NE = ", Cpp2Py(shift_NE)

                V = coupling_Condon(gamma, dE, params["Er"], y0)
                print "V = ", V

                """
                for step in xrange(10):
                    t = step*dt
                    k = NEFGRL_rate(V, params["omega_DA"], t, dtau, omega_nm, gamma_nm, req_nm, shift_NE, method, beta)

                    for w in range(nomega-1):
                        tau = t
                        integ = Integrand_NE_exact(params["omega_DA"], omega_nm[w], t, tau, shift_NE[w], req_nm[w], beta)
                        lin = Linear_NE_exact(gamma_nm[w], omega_nm[w], t, tau, shift_NE[w], req_nm[w], beta)
                        print w, omega_nm[w], integ, lin

                    print step, k

                sys.exit(0)
                """
                res = NEFGRL_population(V, params["omega_DA"], dtau, omega_nm, gamma_nm, req_nm, shift_NE, method, tmax, dt, beta)

                res.show_matrix("res-%i-%i-%i-%i.txt" % (i1,i2,i3,i4))


