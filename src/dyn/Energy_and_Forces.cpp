#include "Energy_and_Forces.h"

namespace libdyn{


double compute_kinetic_energy(Nuclear* mol){

  double Ekin = 0.0;

  for(int i=0;i<mol->nnucl;i++){  Ekin += mol->p[i]*mol->p[i]/mol->mass[i];   }
  Ekin *= 0.5;

  return Ekin;

}

double compute_kinetic_energy(Nuclear& mol){  return compute_kinetic_energy(&mol);  }


double compute_potential_energy(Nuclear* mol, Electronic* el, Hamiltonian* ham, int opt){
// opt == 0   -  Ehrenfest/MF
// opt == 1   -  FSSH

  int i,j;
  double Heff = 0.0;
  double Epot = 0.0;


  // Calculate all surfaces,
  ham->set_q(mol->q);
  ham->compute();

  // Mean-field/Ehrenfest mixing
  if(opt==0){

    // Potential energy - electronic Hamiltonian
    // working in a.u., so hbar = 1
    for(i=0;i<el->nstates;i++){
      for(j=0;j<el->nstates;j++){
        Heff += 0.5*ham->Hvib(i,j).real() * (el->q[i] * el->q[j] + el->p[i] * el->p[j]);
        Heff += ham->Hvib(i,j).imag() * el->p[i] * el->q[j]; 
      }// for j
    }// for i

    Epot = 2.0*Heff;  // in a.u. - hbar is assumed = 1
  }// algo = "MF"

  // FSSH mixing 
  else if(opt==1){   
    i = el->istate; // current electornic state

    Epot = ham->H(i,i).real();  
  }

  return Epot; 

}

double compute_potential_energy(Nuclear& mol, Electronic& el, Hamiltonian& ham, int opt){

  return compute_potential_energy(&mol, &el, &ham, opt);

}


void compute_forces(Nuclear* mol, Electronic* el, Hamiltonian* ham, int opt){
// opt == 0   -  Ehrenfest/MF
// opt == 1   -  FSSH

  int i,j,k;

  // Calculate all surfaces, if needed
  ham->set_q(mol->q);
  ham->compute();

  // Zero all forces in mol
  for(i=0;i<mol->nnucl;i++){ mol->f[k] = 0.0; }

  
  // Mean-field/Ehrenfest mixing
  if(opt==0){

    // Potential energy - electronic Hamiltonian
    // working in a.u., so hbar = 1

    for(i=0;i<el->nstates;i++){
      for(j=0;j<el->nstates;j++){

        double cij_re = (el->q[i] * el->q[j] + el->p[i] * el->p[j]);
        double cij_im = el->p[i] * el->q[j];

        for(k=0;k<mol->nnucl;k++){
          
          mol->f[k] -= 2.0 * 0.5*ham->dHdq(i,j,k).real() * cij_re;
          mol->f[k] -= 2.0 * ham->dHdq(i,j,k).imag() * cij_im; 

        }// for k

      }// for j
    }// for i

  }// algo = "MF"

  // FSSH mixing 
  else if(opt==1){

    i = el->istate; // current electornic state

    for(k=0;k<mol->nnucl;k++){  
      mol->f[k] = -ham->dHdq(i,i,k).real(); 
    }// for k
  }// opt == 1

}

void compute_forces(Nuclear& mol, Electronic& el, Hamiltonian& ham, int opt){

  compute_forces(&mol, &el, &ham, opt);

}



void compute_energies(Ensemble* ens, double& Epot, double& Ekin, double& Etot,int opt){

  Epot = 0.0;
  Ekin = 0.0;
  Etot = 0.0;
  
  for(int traj=0;traj<ens->ntraj;traj++){

    double ek = compute_kinetic_energy(ens->mol[traj]);
    double ep = compute_potential_energy(ens->mol[traj], ens->el[traj], ens->ham[traj], opt);

    Epot += ep;
    Ekin += ek;
  } 

  Epot /= ((double)ens->ntraj);
  Ekin /= ((double)ens->ntraj);
  Etot = Ekin + Epot;

}


}// namespace libdyn