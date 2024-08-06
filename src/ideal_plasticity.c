/** Interface to an isotropic linear elastic, linearly hardening plastic solid with four parameters E, nu, sigYs0 and ETiso. 
/** 

/** You are allowed to use, modify, and publish this External Material File and your modifications of it subject
*   to the terms and conditions of the COMSOL Software License Agreement (www.comsol.com/sla). */

/** Copyright © 2017 by COMSOL. */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT double mises(double s[6]);


EXPORT int eval(double e[6],         // Input: Green-Lagrange strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
                double s[6],         // Output: Second Piola-Kirchhoff stress components in Voigt order (xx,yy,zz,yz,zx,xy)
                double Jac[6][6],    // Output: Jacobian of stress with respect to strain, 6-by-6 matrix in row-major order
                int *nPar,           // Input: Number of material model parameters, scalar
                double *par,         // Input: Parameters: par[0] = E, par[1] = nu, par[2] = sigyYs0, par[3] = ETiso
                int *nStates1,       // Input: Number of states, scalar        
                double *states1,     // Internal state: Plastic strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
				int *nStates2,       // Input: Number of states, scalar        
                double *states2,     // Internal state: Accumulated effective plastic strain
				int *nStates3,       // Input: Number of states, scalar        
                double *states3) {   // Internal state: Plastic work

  int i,j;                       // Iteration indicies
  double E, nu, sigYs0, ETiso;   // Material parameters
  double G, K;                   // Isotropic shear and bulk moduli
  double ep[6];                  // Plastic strain tensor        
  double sTrial[6];              // Trial stress deviator
  double eTrial[6];              // Trial strain deviator
  double sHat[6];                // Auxiliary stress vector used in the elastic-plastic Jacobian computation
  double sigTrialEff,sigYs;      // Effective trial stress and current yield stress
  double dLam,dep,epEff;         // Plastic multiplier, plastic strain increment and effective plastic strain
  double trE,q1,q2;              // Volumetric strain and two coefficients for the elastic-plastic Jacobian
  double Eiso;                   // Plastic modulus

  // Check inputs
  if (nPar[0] != 4)              // Exactly four parameters needed, E, nu, sigYs0 and ETiso
    return 1;                    // error code 1 = "Wrong number of parameters"
  if (nStates1[0] !=6)           // space for the plastic strain tensor (six components)
    return 2;                    // error code 2 = "Wrong number of states"
  if (nStates2[0] !=1)           // space for the effective plastic strain
    return 2;                    // error code 2 = "Wrong number of states" 
  if (nStates3[0] !=1)           // space for the plastic work
    return 2;                    // error code 2 = "Wrong number of states" 

  // Read input parameters from the parameter vector      
  E      = par[0];
  nu     = par[1];
  sigYs0 = par[2];
  ETiso  = par[3];

  // Check values of input parameters
  if (E <= 0.0) return -1;
  if (nu >= 0.5 || nu <= -1.0) return -1;
  if (sigYs0<=0.0) return -1;
  if (ETiso<0.0) return -1;

  // Compute the elastic shear and bulk moduli, as well as the plastic modulus
  G = 0.5*E/(1.0+nu);
  K = E/3.0/(1.0-2.0*nu);
  Eiso = ETiso*E/(E-ETiso);

  // Read the plastic strain tensor, effective plastic strain from the state variables
  for (i=0;i<6;i++) ep[i] = states1[i];
  epEff = states2[0];

  // Initialize the Jacobian
  for (i = 0; i < 6; i++){
    for (j = 0; j < 6; j++) {
      Jac[i][j] = 0.0;
      }
  }
  
  /*
   Stress update algorithm & Jacobian computation
  */

  // 1. Compute (deviatoric) trial stress tensor and its effective value
  for (i=0;i<6;i++) eTrial[i] = e[i]-ep[i];
  trE = eTrial[0] + eTrial[1] + eTrial[2];
  for (i=0;i<3;i++) eTrial[i] = eTrial[i]-trE/3.0;
  for (i=0;i<6;i++) sTrial[i] = 2.0*G*eTrial[i]; 
  sigTrialEff = mises(sTrial);

  // 2. Re-construct the yield stress from the previous increment
  sigYs = sigYs0 + Eiso*epEff;

  // 4. Check yield condition
 
  if (sigTrialEff<sigYs) {

     // Elastic step

     // E1. Compute the stress tensor
     for (i=0;i<6;i++) s[i] = sTrial[i];
     for (i=0;i<3;i++) s[i] += K*trE;

     // E2. Compute the elastic Jacobian
	 Jac[0][0] = Jac[1][1] = Jac[2][2] = 4.0/3.0*G + K;                        
     Jac[3][3] = Jac[4][4] = Jac[5][5] = 2.0*G;                                                      
     Jac[0][1] = Jac[0][2] = Jac[1][0] = Jac[1][2] = Jac[2][0] = Jac[2][1] = K - 2.0/3.0*G;

     // No need to update states (plastic strains and plastic work are unaffected by this elastic step)

     return 0; // Return value 0 if success, any other value triggers an exception
  }
  else {

     // Elastic-Plastic step

     // EP1. Compute the effective plastic strain increment
     dep = (sigTrialEff-sigYs)/(3.0*G+Eiso);

	 // EP2. Compute the current yield stress, based on the hardening curve
	 sigYs += Eiso*dep;

	 // EP3. Compute the plastic multiplier and the stress deviator, and update plastic strains 
	 dLam = 1.5/sigYs*dep;

     for (i=0;i<6;i++) s[i] = sTrial[i] /(1.0 + 2.0*G*dLam);

     for (i=0;i<6;i++) states1[i] = ep[i] + dLam*s[i]; 
	 epEff += dep;
	 states2[0] = epEff; 

	
     // EP4. Compute Jacobian (elastic-plastic)       
  
	 q1 =  2.0*G/(1.0 + 2.0*G*dLam);
	 Jac[0][0] = Jac[1][1] = Jac[2][2] = 2.0/3.0*q1; 
     Jac[3][3] = Jac[4][4] = Jac[5][5] = q1; 
     Jac[0][1] = Jac[0][2] = Jac[1][0] = Jac[1][2] = Jac[2][0] = Jac[2][1] = -1.0/3.0*q1; 

     for (i=0;i<3;i++) {
       for (j=0;j<3;j++) {
         Jac[i][j] += K;
         }
     }

     q2 =  q1*(3.0*G/sigYs)/(3.0*G+Eiso)*1.5*(1.0/sigYs-dep*Eiso/pow(sigYs,2.0));

     // An auxiliary stress vector sHat is introduced where the shear components are doubled. These doubled
	 // shear terms result from taking a derivative of the effective trial stress.
	 //
	 // In general, caution is needed in converting (symmetric) tensor inner products to their
     // vector versions.
	 // Aij*Aij = ... + A23*A23 + ... + A32*A32 + ...
	 // AI*AI   = ... + 2*A4*A4 + ...
	 //
   	 sHat[0]=s[0];
	 sHat[1]=s[1];
     sHat[2]=s[2];
	 sHat[3]=2.0*s[3];
	 sHat[4]=2.0*s[4];
	 sHat[5]=2.0*s[5];

  	 for (i=0;i<6;i++) {
       for (j=0;j<6;j++) {
         Jac[i][j] += -q2*s[i]*sHat[j];
		 }
     }

     // EP7. Add the hydrostatic part
	 for (i=0;i<3;i++) s[i] += K*trE;

	 // Computation of plastic work (not required for the External Material computation).
	 states3[0] += sigYs*dep;

      return 0;  // Return value 0 if success, any other value triggers an exception
  }

}


// Computation of effective (Mises) stress based on a stress deviator, on vector form, as input
EXPORT double mises(double s[]) {
 double S = 0.0;
 int i;
 for (i=0;i<3;i++) S += s[i]*s[i] + 2.0*s[i+3]*s[i+3]; // Double the shear terms to correctly compute the
 S *= 1.5;                                             // vector form of the tensor inner product
 S = sqrt(S);
 return S;
}