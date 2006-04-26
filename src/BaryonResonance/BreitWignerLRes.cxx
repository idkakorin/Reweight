//____________________________________________________________________________
/*
 Copyright (c) 2003-2006, GENIE Neutrino MC Generator Collaboration
 All rights reserved.
 For the licensing terms see $GENIE/USER_LICENSE.

 Author: Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory - November 22, 2004

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include "BaryonResonance/BreitWignerLRes.h"
#include "BaryonResonance/BaryonResDataSetI.h"
#include "BaryonResonance/BaryonResParams.h"
#include "BaryonResonance/BaryonResUtils.h"
#include "Messenger/Messenger.h"
#include "Utils/BWFunc.h"

using namespace genie;

//______________________________________________________________________
BreitWignerLRes::BreitWignerLRes() :
BreitWignerI("genie::BreitWignerLRes")
{

}
//______________________________________________________________________
BreitWignerLRes::BreitWignerLRes(string config) :
BreitWignerI("genie::BreitWignerLRes", config)
{

}
//______________________________________________________________________
BreitWignerLRes::~BreitWignerLRes()
{

}
//______________________________________________________________________
double BreitWignerLRes::Eval(Resonance_t res, double W) const
{
  //-- instantiate a BaryonResParams object & set the table to lookup
  BaryonResParams res_params;
  res_params.SetDataSet(fBaryonResDataSet);
  res_params.RetrieveData(res);

  //-- get mass, width and norm
  double mass  = res_params.Mass();
  double width = res_params.Width();
  double norm  = res_params.BreitWignerNorm();
  int    L     = res_params.OrbitalAngularMom();

  //-- call the actual Breit-Wigner function
  double bw = utils::bwfunc::BreitWignerL(W, L, mass, width, norm);
  return bw;
}
//______________________________________________________________________
void BreitWignerLRes::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadSubAlg();
}
//____________________________________________________________________________
void BreitWignerLRes::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadSubAlg();
}
//____________________________________________________________________________
void BreitWignerLRes::LoadSubAlg(void)
{
// Load the "baryon resonance table" sub-algorithm specified at the algorithm
// configuration

  fBaryonResDataSet =
         dynamic_cast<const BaryonResDataSetI *> (this->SubAlg(
                              "baryon-res-alg-name", "baryon-res-param-set"));
}
//____________________________________________________________________________

