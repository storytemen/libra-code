/*********************************************************************************
* Copyright (C) 2015-2017 Alexey V. Akimov
*
* This file is distributed under the terms of the GNU General Public License
* as published by the Free Software Foundation, either version 2 of
* the License, or (at your option) any later version.
* See the file LICENSE in the root directory of this distribution
* or <http://www.gnu.org/licenses/>.
*
*********************************************************************************/
/**
  \file Surface_Hopping.h
  \brief The file describes the functions used in surface hopping methods
    
*/

#ifndef SURFACE_HOPPING_H
#define SURFACE_HOPPING_H


// External dependencies
#include "../math_linalg/liblinalg.h"
#include "../hamiltonian/libhamiltonian.h"

// Dynamics classes
#include "nuclear/libnuclear.h"
#include "electronic/libelectronic.h"
#include "ensemble/libensemble.h"

/// liblibra namespace
namespace liblibra{

using namespace libio;
using namespace libhamiltonian;
namespace bp = boost::python;


/// libdyn namespace
namespace libdyn{

using namespace libnuclear;
using namespace libelectronic;
using namespace libensemble;


///================  In tsh_prob_fssh.cpp  ===================================

MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, CMATRIX* Hvib, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, CMATRIX* Hvib, double dt);
MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, CMATRIX& Hvib, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, CMATRIX& Hvib, double dt);

MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, nHamiltonian* ham, int rep, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, nHamiltonian* ham, int rep, double dt);
MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, nHamiltonian& ham, int rep, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_fssh(CMATRIX& Coeff, nHamiltonian& ham, int rep, double dt);

/// Backward-compatibility
void compute_hopping_probabilities_fssh(Nuclear* mol, Electronic* el, Hamiltonian* ham, MATRIX* g,
                                        double dt, int use_boltz_factor,double T);
void compute_hopping_probabilities_fssh(Nuclear& mol, Electronic& el, Hamiltonian& ham, MATRIX& g,
                                        double dt, int use_boltz_factor,double T);
void compute_hopping_probabilities_fssh(Ensemble& ens, int i, MATRIX& g, double dt, int use_boltz_factor,double T);


///================  In tsh_prob_gfsh.cpp  ===================================

MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, CMATRIX* Hvib, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, CMATRIX* Hvib, double dt);
MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, CMATRIX& Hvib, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, CMATRIX& Hvib, double dt);

MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, nHamiltonian* ham, int rep, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, nHamiltonian* ham, int rep, double dt);
MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, nHamiltonian& ham, int rep, double dt, int use_boltz_factor, double T);
MATRIX compute_hopping_probabilities_gfsh(CMATRIX& Coeff, nHamiltonian& ham, int rep, double dt);

/// Backward-compatibility
void compute_hopping_probabilities_gfsh(Nuclear* mol, Electronic* el, Hamiltonian* ham, MATRIX* g,
                                        double dt, int use_boltz_factor,double T);
void compute_hopping_probabilities_gfsh(Nuclear& mol, Electronic& el, Hamiltonian& ham, MATRIX& g,
                                        double dt, int use_boltz_factor,double T);
void compute_hopping_probabilities_gfsh(Ensemble& ens, int i, MATRIX& g, double dt, int use_boltz_factor,double T);



///================  In tsh_prob_mssh.cpp  ===================================

MATRIX compute_hopping_probabilities_mssh(CMATRIX& Coeff);

/// Backward-compatibility
void compute_hopping_probabilities_mssh(Nuclear* mol, Electronic* el, Hamiltonian* ham, MATRIX* g,
                                        double dt, int use_boltz_factor,double T);
void compute_hopping_probabilities_mssh(Nuclear& mol, Electronic& el, Hamiltonian& ham, MATRIX& g,
                                        double dt, int use_boltz_factor,double T);
void compute_hopping_probabilities_mssh(Ensemble& ens, int i, MATRIX& g, double dt, int use_boltz_factor,double T);


///================  In tsh_prob_esh.cpp  ===================================

void compute_hopping_probabilities_esh(Ensemble& ens, MATRIX* g, double dt, int use_boltz_factor,double T);



///================  In tsh_aux_rescale.cpp  ===================================

int rescale_velocities_adiabatic
(MATRIX& p, MATRIX& invM, CMATRIX* ham_adi, vector<CMATRIX*>& dc1_adi, int new_st,int old_st, int do_reverse);
int rescale_velocities_adiabatic
(MATRIX& p, MATRIX& invM, CMATRIX& ham_adi, vector<CMATRIX>& dc1_adi, int new_st,int old_st, int do_reverse);
int rescale_velocities_adiabatic
(MATRIX& p, MATRIX& invM, nHamiltonian* ham, int new_st,int old_st, int do_reverse);
int rescale_velocities_adiabatic
(MATRIX& p, MATRIX& invM, nHamiltonian& ham, int new_st,int old_st, int do_reverse);

int rescale_velocities_diabatic(MATRIX& p, MATRIX& invM, CMATRIX* ham_adi, int new_st,int old_st);
int rescale_velocities_diabatic(MATRIX& p, MATRIX& invM, CMATRIX& ham_adi, int new_st,int old_st);
int rescale_velocities_diabatic(MATRIX& p, MATRIX& invM, nHamiltonian* ham, int new_st,int old_st);
int rescale_velocities_diabatic(MATRIX& p, MATRIX& invM, nHamiltonian& ham, int new_st,int old_st);

/// Backward-compatibility
int rescale_velocities_adiabatic(vector<double>& p, vector<double>& masses, 
                                 CMATRIX& ham_adi, vector<CMATRIX>& dc1_adi,
                                 int new_st,int old_st, int do_reverse);
void rescale_velocities_adiabatic(Nuclear* mol, Hamiltonian* ham, int& new_st,int& old_st, int do_reverse);
int rescale_velocities_adiabatic(Nuclear& mol, Hamiltonian& ham, int old_st, int do_reverse);

void rescale_velocities_diabatic(Nuclear* mol, Hamiltonian* ham, int& new_st,int& old_st);
int rescale_velocities_diabatic(Nuclear& mol, Hamiltonian& ham, int old_st);


/// Backward-compatibility - Entangled trajectories
void rescale_velocities_adiabatic(int ntraj, vector<Nuclear*>& mol, vector<Hamiltonian*>& ham,
      vector<int>& new_st, vector<int>& old_st, int do_reverse);
void rescale_velocities_adiabatic(int ntraj, vector<Nuclear>& mol, vector<Hamiltonian>& ham,
      vector<int>& new_st, vector<int>& old_st, int do_reverse);




///================  In tsh_aux_hop.cpp  ===================================

int hop(int initstate, MATRIX& g, double ksi);

/// Backward-compatibility
void hop(int& initstate, Nuclear* mol, Hamiltonian* ham, double ksi, MATRIX* g, int do_rescaling, int rep, int do_reverse);
int hop(int initstate, Nuclear& mol, Hamiltonian& ham, double ksi, MATRIX& g, int do_rescaling, int rep, int do_reverse);
int hop(int initstate, Ensemble& ens, int i, double ksi, MATRIX& g, int do_rescaling, int rep, int do_reverse);


/// Backward-compatibility - entangled trajectories
void hop(int ntraj, vector<int>& initstate, vector<Nuclear*>& mol, vector<Hamiltonian*>& ham, 
         vector<double> ksi, vector<MATRIX*>& g, int do_rescaling, int rep, int do_reverse);

vector<int>hop(int ntraj, vector<int> initstate, vector<Nuclear>& mol, vector<Hamiltonian>& ham, 
           vector<double> ksi, vector<MATRIX>& g, int do_rescaling, int rep, int do_reverse);

boost::python::list hop(int ntraj, boost::python::list initstate, boost::python::list mol, boost::python::list ham, 
    boost::python::list ksi, boost::python::list g, int do_rescaling, int rep, int do_reverse);




///================  In tsh_methods_tsh.cpp  ===================================

int tsh0(double dt, MATRIX& q, MATRIX& p, MATRIX& invM, CMATRIX& C, int state,
         nHamiltonian& ham, bp::object py_funct, bp::object params,  boost::python::dict params1, Random& rnd);


///================  In tsh_methods_ida.cpp  ===================================

int ida(CMATRIX& Coeff, int old_st, int new_st, double E_old, double E_new, double T, double ksi);


///================  In tsh_methods_dish.cpp  ===================================

MATRIX coherence_intervals(CMATRIX& Coeff, MATRIX& rates);
int dish(Electronic& el, MATRIX& t_m, const MATRIX& tau_m, const CMATRIX& Hvib,
          int use_boltz_flag, double Ekin, double T, double ksi1, double ksi2);
int dish(Electronic& el, Nuclear& mol, Hamiltonian& ham, 
          MATRIX& t_m, const MATRIX& tau_m, int use_boltz_flag, double T, double ksi1, double ksi2);


}// namespace libdyn
}// liblibra

#endif // SURFACE_HOPPING_H
