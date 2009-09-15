//____________________________________________________________________________
/*
 Copyright (c) 2003-2009, GENIE Neutrino MC Generator Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory - May 04, 2005

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include <TMath.h>

#include "Conventions/Constants.h"
#include "Conventions/RefFrame.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "PDG/PDGLibrary.h"
#include "VHE/GlashowResonancePXSec.h"

using namespace genie;
using namespace genie::constants;

//____________________________________________________________________________
GlashowResonancePXSec::GlashowResonancePXSec() :
XSecAlgorithmI("genie::GlashowResonancePXSec")
{

}
//____________________________________________________________________________
GlashowResonancePXSec::GlashowResonancePXSec(string config) :
XSecAlgorithmI("genie::GlashowResonancePXSec", config)
{

}
//____________________________________________________________________________
GlashowResonancePXSec::~GlashowResonancePXSec()
{

}
//____________________________________________________________________________
double GlashowResonancePXSec::XSec(
   const Interaction * /*interaction*/, KinePhaseSpace_t /*kps*/) const
{
  return 0;
}
//____________________________________________________________________________
double GlashowResonancePXSec::Integral(const Interaction * interaction) const
{
  if(! this -> ValidProcess    (interaction) ) return 0.;
  if(! this -> ValidKinematics (interaction) ) return 0.;

  const InitialState & init_state = interaction->InitState();
  double E = init_state.ProbeE(kRfLab);

  double gf    = kGF2/(3*kPi);
  double me    = kElectronMass;
  double Mw    = kMw;
  double Gw    = PDGLibrary::Instance()->Find(kPdgWM)->Width();
  double Mw2   = TMath::Power(Mw,  2);
  double Mw4   = TMath::Power(Mw2, 2);
  double Gw2   = TMath::Power(Gw,  2);

  double s     = 2*me*E;
  double bw    = Mw4 / (TMath::Power(s-Mw2,2) + Gw2*Mw2);
  double xsec  = gf*s*bw;

  LOG("GlashowResXSec", pDEBUG) << "XSec (E = " << E << ") = " << xsec;

  return xsec;
}
//____________________________________________________________________________
bool GlashowResonancePXSec::ValidProcess(const Interaction* interaction) const
{
  if(interaction->TestBit(kISkipProcessChk)) return true;

  const InitialState & init_state = interaction->InitState();
  const ProcessInfo &  proc_info  = interaction->ProcInfo();

  bool nuok    = pdg::IsAntiNuE(init_state.ProbePdg());
  bool nucok   = !(init_state.Tgt().HitNucIsSet());
  bool ccprcok = proc_info.IsWeakCC();

  if ( !nuok    ) return false;
  if ( !nucok   ) return false;
  if ( !ccprcok ) return false;
  
  return true;
}
//____________________________________________________________________________
bool GlashowResonancePXSec::ValidKinematics(const Interaction * /*in*/) const
{
  return true;
}
//____________________________________________________________________________
