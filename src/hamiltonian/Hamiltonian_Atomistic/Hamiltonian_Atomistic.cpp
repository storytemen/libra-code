/*********************************************************************************
* Copyright (C) 2015 Alexey V. Akimov
*
* This file is distributed under the terms of the GNU General Public License
* as published by the Free Software Foundation, either version 2 of
* the License, or (at your option) any later version.
* See the file LICENSE in the root directory of this distribution
* or <http://www.gnu.org/licenses/>.
*
*********************************************************************************/


#include <complex>
#include <cmath>
#include "Hamiltonian_Atomistic.h"


namespace libhamiltonian{
namespace libhamiltonian_atomistic{

using namespace libchemobjects;
using namespace libchemobjects::libchemsys;
using namespace libhamiltonian_mm;


using namespace libmmath;
using namespace libmmath::libmeigen;
using std::complex;
using std::sin;
using std::cos;
using std::exp;
using std::sqrt;


Hamiltonian_Atomistic::Hamiltonian_Atomistic(int _nelec, int _nnucl){

  int i;

  nelec = _nelec;
  nnucl = _nnucl;

  ham_dia = new MATRIX(nelec,nelec); *ham_dia = 0.0;
  ham_adi = new MATRIX(nelec,nelec); *ham_adi = 0.0;

  d1ham_dia = vector<MATRIX*>(nnucl);
  d1ham_adi = vector<MATRIX*>(nnucl);

  for(i=0;i<nnucl;i++){  
    d1ham_dia[i] = new MATRIX(nelec,nelec); *d1ham_dia[i] = 0.0; 
    d1ham_adi[i] = new MATRIX(nelec,nelec); *d1ham_adi[i] = 0.0; 
  }

  d2ham_dia = vector<MATRIX*>(nnucl*nnucl);
  for(i=0;i<nnucl*nnucl;i++){  d2ham_dia[i] = new MATRIX(nelec,nelec); *d2ham_dia[i] = 0.0;  }


  rep = 0; // default representation is diabatic  
  status_dia = 0;
  status_adi = 0;


  // Setup Hamiltonian types:
  ham_types = vector<int>(5,0);

}


void Hamiltonian_Atomistic::set_Hamiltonian_type(std::string ham_type){ // libchemobjects::libchemsys::System& syst){

  if(ham_type=="MM"){
    rep = 0;  // diabatic is same as adiabatic

    if(ham_types[0]==0){    
      mm_ham = new listHamiltonian_MM();
      ham_types[0] = 1;
    }

  }
  else{
    cout<<"Error: Unrecognized Hamiltonian type. Supported types are: MM\n";
    cout<<"Exiting...\n"; exit(0);
  }

}

void Hamiltonian_Atomistic::show_interactions_statistics(){
  mm_ham->show_interactions_statistics();
}

void Hamiltonian_Atomistic::set_atom_types(System& syst, vector<int>& lst, ForceField& ff){
  mm_ham->set_atom_types(syst, lst, ff);
}

void Hamiltonian_Atomistic::set_fragment_types(System& syst, vector<int>& lst, ForceField& ff){
  mm_ham->set_fragment_types(syst, lst, ff);  
}

bool Hamiltonian_Atomistic::is_active(Atom& a1, Atom& a2){
  return mm_ham->is_active(a1,a2);
}

bool Hamiltonian_Atomistic::is_active(Atom& a1, Atom& a2, Atom& a3){
  return mm_ham->is_active(a1,a2,a3);
}

bool Hamiltonian_Atomistic::is_active(Atom& a1, Atom& a2, Atom& a3, Atom& a4){
  return mm_ham->is_active(a1,a2,a3,a4);
}


void Hamiltonian_Atomistic::set_atom_interactions_for_atoms
(System& syst,string int_type,vector<Atom>& top_elt,vector<int>& lst1,vector<int>& lst2,ForceField& ff,int verb){

  mm_ham->set_atom_interactions_for_atoms(syst,int_type,top_elt,lst1,lst2,ff,verb);

}

void Hamiltonian_Atomistic::set_group_interactions_for_atoms
(System& syst,string int_type,vector<Group>& top_elt,vector<int>& lst1,vector<int>& lst2,ForceField& ff){

  mm_ham->set_group_interactions_for_atoms(syst,int_type,top_elt,lst1,lst2,ff);
}

void Hamiltonian_Atomistic::set_interactions_for_atoms
(System& syst, boost::python::list lst1, boost::python::list lst2, ForceField& ff, int verb, int assign_rings){

  mm_ham->set_interactions_for_atoms(syst,lst1,lst2,ff,verb,assign_rings);
}

void Hamiltonian_Atomistic::set_interactions_for_fragments
(System& syst, boost::python::list lst1, boost::python::list lst2, ForceField& ff){

  mm_ham->set_interactions_for_fragments(syst,lst1,lst2,ff);

}

void Hamiltonian_Atomistic::apply_pbc_to_interactions(System& syst, int int_type,int nx,int ny,int nz){

  mm_ham->apply_pbc_to_interactions(syst, int_type, nx, ny, nz);
}

void Hamiltonian_Atomistic::set_respa_types(std::string inter_type,std::string respa_type){

  mm_ham->set_respa_types(inter_type, respa_type);
}




Hamiltonian_Atomistic::~Hamiltonian_Atomistic(){
  int i;

  delete ham_dia;
  delete ham_adi;

  for(i=0;i<nnucl;i++){  
    delete d1ham_dia[i];
    delete d1ham_adi[i];
  }
  for(i=0;i<nnucl*nnucl;i++){  delete d2ham_dia[i]; }

  if(d1ham_dia.size()>0) { d1ham_dia.clear(); }
  if(d2ham_dia.size()>0) { d2ham_dia.clear(); }
  if(d1ham_adi.size()>0) { d1ham_adi.clear(); }


}


void Hamiltonian_Atomistic::set_q(vector<double>& q_){

  if(q_.size()!=nnucl){
    cout<<"Error in Hamiltonian_Atomistic::set_q - the size of input array ("<<q_.size()<<") does not match";
    cout<<"the number of nuclear degrees of freedom ("<<nnucl<<")\nExiting...\n"; exit(0);
  }

  q = q_;
  status_dia = 0;
  status_adi = 0;

  _syst->set_atomic_q(q);
  //_syst->set_fragment_q(q);

}

void Hamiltonian_Atomistic::set_v(vector<double>& v_){

  if(v_.size()!=nnucl){
    cout<<"Error in Hamiltonian_Atomistic::set_v - the size of input array ("<<v_.size()<<") does not match";
    cout<<"the number of nuclear degrees of freedom ("<<nnucl<<")\nExiting...\n"; exit(0);
  }

  v = v_;
  status_adi = 0;  // only affects adiabatic computations

  _syst->set_atomic_v(v);
  //_syst->set_fragment_v(v);
  

}



/*
void Hamiltonian_Atomistic::set_rep(int rep_){
  rep = rep_;
}

//  ham->set_q(mol->q);

void Hamiltonian_Atomistic::set_params(vector<double>& params_){

  
  params = vector<double>(params_.size(), 0.0);

  // Now copy input params:
  for(int i=0;i<params_.size(); i++){
    params[i] = params_[i];
  }

  // Since the parameters have changed - we need to recompute everything
  status_dia = 0;
  status_adi = 0;

}

void Hamiltonian_Atomistic::set_params(boost::python::list params_){

  int sz = boost::python::len(params_);
  vector<double> tmp_params(sz, 0.0);

  // Now copy input params:
  for(int i=0;i<sz; i++){
    tmp_params[i] = boost::python::extract<double>(params_[i]);
  }

  set_params(tmp_params);

}



void Hamiltonian_Atomistic::set_v(vector<double>& v_){
  v = v_;
  status_adi = 0;  // only affects adiabatic computations
}

void Hamiltonian_Atomistic::set_v(boost::python::list v_){

  int sz = boost::python::len(v_);
  vector<double> tmp_v(sz,0.0);

  for(int i=0;i<sz; i++){
    tmp_v[i] = boost::python::extract<double>(v_[i]);
  }

  set_v(tmp_v);

}


void Hamiltonian_Atomistic::compute(){

  if(rep==0){  compute_diabatic();  }
  else if(rep==1){  compute_adiabatic(); }

}

*/
void Hamiltonian_Atomistic::compute_diabatic(){
  int i;

  if(status_dia == 0){ // only compute this is the result is not up to date
  
    if(ham_types[0]==1){

      // Zero forces
      _syst->zero_forces_and_torques();

      // Do actual computations
      int sz = mm_ham->interactions.size();
      double res = 0.0;
      int tmp;
      for(int i=0;i<sz;i++){
        res += mm_ham->interactions[i].calculate(tmp);
        //cout<<"interactions #"<<i<<", energy = "<<mm_ham->interactions[i].calculate(tmp)<<endl;
      }

      // Energies
      ham_dia->M[0] = res;
      ham_adi->M[0] = res;

      
      // First derivatives     
      //_syst->update_fragment_forces_and_torques();

      // But take only atomistic forces at this time
      for(i=0;i<_syst->Number_of_atoms;i++){         

        d1ham_dia[3*i  ]->M[0] = -_syst->Atoms[i].Atom_RB.rb_force.x;
        d1ham_dia[3*i+1]->M[0] = -_syst->Atoms[i].Atom_RB.rb_force.y;
        d1ham_dia[3*i+2]->M[0] = -_syst->Atoms[i].Atom_RB.rb_force.z;

        d1ham_adi[3*i  ]->M[0] = -_syst->Atoms[i].Atom_RB.rb_force.x;
        d1ham_adi[3*i+1]->M[0] = -_syst->Atoms[i].Atom_RB.rb_force.y;
        d1ham_adi[3*i+2]->M[0] = -_syst->Atoms[i].Atom_RB.rb_force.z;

      }

      // Second derivatives
      for(i=0;i<nnucl*nnucl;i++){ 
        d2ham_dia[i]->M[0] = 0.0;
      }


    }// MM

    //==========================================================

    // class - specific computations: MM, semi-empirical, HF, etc   

    //==========================================================

    // Set status flag 
    status_dia = 1;

  }//   status_dia == 0

}



void Hamiltonian_Atomistic::compute_adiabatic(){
// This function computes adiabatic PESs and derivative couplings

//------------------------------------------------------------------

// actually, here is gonna be SCF solver - to get adiabatic states

//------------------------------------------------------------------

  compute_diabatic();

  if(status_adi == 0){

    MATRIX* S; S = new MATRIX(nelec, nelec);  S->Init_Unit_Matrix(1.0);
    MATRIX* C; C = new MATRIX(nelec, nelec);  *C = 0.0;

    // Transformation to adiabatic basis
    solve_eigen(nelec, ham_dia, S, ham_adi, C);  // H_dia * C = S * C * H_adi


    // Now compute the derivative couplings (off-diagonal, multiplied by energy difference) and adiabatic gradients (diagonal)
    for(int n=0;n<nnucl;n++){

      *d1ham_adi[n] = (*C).T() * (*d1ham_dia[n]) * (*C);

    }// for n


    delete S;
    delete C;

    // Set status flag
    status_adi = 1;

  }// status_adi == 0
  
}



}// namespace libhamiltonian_atomistic
}// namespace libhamiltonian



