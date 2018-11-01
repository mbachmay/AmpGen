#include <string>

#include "AmpGen/Expression.h"
#include "AmpGen/Factory.h"
#include "AmpGen/Lineshapes.h"
#include "AmpGen/ParticleProperties.h"
#include "AmpGen/ParticlePropertiesList.h"

using namespace AmpGen;
using namespace AmpGen::fcn; 

DEFINE_LINESHAPE( FormFactor )
{
  auto props                  = ParticlePropertiesList::get( particleName );
  Expression radius           = Parameter( particleName + "_radius", props->radius() );

  const Expression q2         = make_cse( Q2( s, s1, s2 ) );
  int Lp = L;
  if( lineshapeModifier == "L0" ) Lp = 0;
  if( lineshapeModifier == "L1" ) Lp = 1;
  if( lineshapeModifier == "L2" ) Lp = 2;
  if( lineshapeModifier == "L3" ) Lp = 3;
  
  const Expression FormFactor = lineshapeModifier.find("BL") == std::string::npos ? 
    sqrt( BlattWeisskopf_Norm( q2 * radius * radius, 0, Lp ) ) : 
    sqrt( BlattWeisskopf(q2*radius*radius, Lp ) );
  if( L != 0 ){
    ADD_DEBUG( q2      , dbexpressions );
    ADD_DEBUG( radius  , dbexpressions );
  }
  ADD_DEBUG( FormFactor, dbexpressions );
  return FormFactor ; 
}

DEFINE_LINESHAPE( ExpFF )
{
  auto props                  = ParticlePropertiesList::get( particleName );
  Expression radius           = Parameter( particleName + "_radius", props->radius() );
  const Expression q2         = Q2( s, s1, s2 );
  const Expression FormFactor = Exp( -q2 * radius * radius / 2. );
  if( L != 0 ){
    ADD_DEBUG( radius, dbexpressions );
    ADD_DEBUG( s1, dbexpressions );
    ADD_DEBUG( s2, dbexpressions );
    ADD_DEBUG( s, dbexpressions );
    ADD_DEBUG( q2, dbexpressions );
  }
  ADD_DEBUG( FormFactor, dbexpressions ); 
  return FormFactor ;
}

DEFINE_LINESHAPE( None ) { return Constant( 1); }

DEFINE_LINESHAPE( BW )
{
  auto props = ParticlePropertiesList::get( particleName );

  const Expression& mass     = Parameter( particleName + "_mass", props->mass() );
  const Expression& width0   = Parameter( particleName + "_width", props->width() );
  const Expression& radius   = Parameter( particleName + "_radius", props->radius() );
  const Expression q2_sgned  = make_cse( Q2( s, s1, s2 ) ) ;
  const Expression q20_sgned = make_cse( Q2( mass * mass, s1, s2 ) );
  const Expression J = Constant(0,1);
  const Expression q2  = Ternary( q2_sgned > 0, q2_sgned, 0 );
  const Expression q20 = abs( Q2( mass * mass, s1, s2 ) );
  Expression FormFactor                       = sqrt( BlattWeisskopf_Norm( q2 * radius * radius, 0, L ) );
  if ( lineshapeModifier == "BL" ) FormFactor = sqrt( BlattWeisskopf( q2 * radius * radius, L ) );
  Expression runningWidth                     = width( s, s1, s2, mass, width0, radius, L, dbexpressions );
  const Expression BW = FormFactor / ( mass * mass - s  -J*mass * runningWidth );
  const Expression kf = kFactor( mass, width0, dbexpressions );
  ADD_DEBUG( FormFactor, dbexpressions );
  ADD_DEBUG( runningWidth, dbexpressions );
  ADD_DEBUG( BW, dbexpressions );
  ADD_DEBUG( kf, dbexpressions );
  return kf * BW;
}

Expression aSqrtTerm( const Expression& s, const Expression& m0 )
{
  Expression a2 = 1.0 - ( 4 * m0 * m0 ) / s;
  return Ternary( a2 > Constant( 0 ), sqrt( a2 ), Constant( 0 ) );
}

Expression fSqrtTerm( const Expression& s, const Expression& m0 )
{
  return complex_sqrt( 1.0 - ( 4 * m0 * m0 ) / s );
}

DEFINE_LINESHAPE( Flatte )
{
  auto props        = ParticlePropertiesList::get( particleName );
  Expression mass   = Parameter( particleName + "_mass", props->mass() );
  Expression radius = Parameter( particleName + "_radius", props->radius() );

  Expression gPi       = Parameter( "Flatte::gPi", 0.165 );
  Expression gK_by_gPi = Parameter( "Flatte::gK_by_gPi", 4.21 );
  double mPi0    = ParticlePropertiesList::get("pi0")->mass();
  double mPiPlus = ParticlePropertiesList::get("pi+")->mass();
  double mK0     = ParticlePropertiesList::get("K0")->mass();
  double mKPlus  = ParticlePropertiesList::get("K+")->mass();

  const Expression mreco = sqrt( s );
  Expression Gpipi       = ( 1. / 3. ) * fSqrtTerm( s, mPi0 ) + ( 2. / 3. ) * fSqrtTerm( s, mPiPlus );

  Expression GKK = ( 1. / 2. ) * fSqrtTerm( s, mK0 ) + ( 1. / 2. ) * fSqrtTerm( s, mKPlus );

  if ( lineshapeModifier == "CutKK" ) GKK = ( 1. / 2. ) * aSqrtTerm( s, mK0 ) + ( 1. / 2. ) * aSqrtTerm( s, mKPlus );

  Expression FlatteWidth = gPi * ( Gpipi + gK_by_gPi * GKK );
  const Expression q2    = abs( Q2( s, s1, s2 ) );
  const Expression q20   = abs( Q2( mass * mass, s1, s2 ) );
  const Expression BW    = 1. / ( -s + mass * mass - Constant( 0, 1 ) * mass * FlatteWidth );
  ADD_DEBUG( gPi, dbexpressions );
  ADD_DEBUG( mass, dbexpressions );
  ADD_DEBUG( gK_by_gPi, dbexpressions );
  ADD_DEBUG( fSqrtTerm( s, mKPlus ), dbexpressions );
  ADD_DEBUG( q2, dbexpressions );
  ADD_DEBUG( q20, dbexpressions );
  ADD_DEBUG( Gpipi, dbexpressions );
  ADD_DEBUG( GKK, dbexpressions );
  ADD_DEBUG( FlatteWidth, dbexpressions );
  ADD_DEBUG( BW, dbexpressions );
  return BW;
}

DEFINE_LINESHAPE( SBW )
{
  auto props        = ParticlePropertiesList::get( particleName );
  Expression mass   = Parameter( particleName + "_mass", props->mass() );
  Expression radius = Parameter( particleName + "_radius", props->radius() );
  Expression width0 = Parameter( particleName + "_width", props->width() );

  const Expression q2  = Abs( Q2( s, s1, s2 ) );
  const Expression q20 = Abs( Q2( mass * mass, s1, s2 ) );
  Expression BF        = BlattWeisskopf_Norm( q2 * radius * radius, 0, L );
  const Expression BW = sqrt( kFactor( mass, width0 ) ) / ( -s + mass * mass - Constant(0,1) *mass * width0 );
  ADD_DEBUG( q2, dbexpressions );
  ADD_DEBUG( q20, dbexpressions );
  ADD_DEBUG( BF, dbexpressions );
  ADD_DEBUG( BW, dbexpressions );
  return BW;
}
