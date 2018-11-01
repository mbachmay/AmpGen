#ifndef AMPGEN_WIGNER_H
#define AMPGEN_WIGNER_H
#include "AmpGen/Expression.h"
#include "AmpGen/Tensor.h"
#include "AmpGen/Particle.h"

namespace AmpGen {
  Expression wigner_d( const Expression& cb, const double& j, const double& m, const double& n );
  Expression wigner_D( const Tensor& P, const double& J, const double& lA, const double& lB, const double& lC, DebugSymbols* db ); 

  double CG( const double& j1,
    const double& m1,
    const double& j2, 
    const double& m2,
    const double& J,
    const double& M );

  /** @function rotationMatrix
    Generates a rotation tensor (matrix) that aligns Tensor P (four-vector) to the +ve z-axis, i.e. to construct the helicity frame. 
   */
  Tensor rotationMatrix( const Tensor& p , const bool& handleZeroCase = false );
  /** @function helicityTransformMatrix
    Generates a helicity transform tensor (matrix) that aligns tensor P (four-vector) to the +/- ve z-axis, then boosts to the rest frame. The mass may be seperately specified. The parameter ve specifies whether the initial Euler rotation is to the +/- z-axis. In the case where ve =-1, a second rotation is applied about the x-axis that aligns P to the +ve z-axis. This ensures that singly and doubly primed helicity frames remain orthonormal.
    */
  Tensor helicityTransformMatrix( const Tensor& P, const Expression& M, const int& ve =1, const bool& handleZeroCase = false );   
  
  Expression helicityAmplitude( const Particle& particle, const Tensor& parentFrame, const double& Mz,    DebugSymbols* db );

}

#endif
