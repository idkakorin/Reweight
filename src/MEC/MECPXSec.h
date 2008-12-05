//____________________________________________________________________________
/*!

\class    genie::MECPXSec

\brief    Computes the MEC differential cross section.
          Is a concrete implementation of the XSecAlgorithmI interface. \n

\ref      

\author   

\created  May 05, 2004

\cpright  Copyright (c) 2003-2008, GENIE Neutrino MC Generator Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _MEC_PXSEC_H_
#define _MEC_PXSEC_H_

#include "Base/XSecAlgorithmI.h"

namespace genie {

class MECPXSec : public XSecAlgorithmI {

public:
  MECPXSec();
  MECPXSec(string config);
  virtual ~MECPXSec();

  //-- XSecAlgorithmI interface implementation
  double XSec            (const Interaction * i, KinePhaseSpace_t k) const;
  double Integral        (const Interaction * i) const;
  bool   ValidProcess    (const Interaction * i) const;

  //-- override the Algorithm::Configure methods to load configuration
  //   data to private data members
  void Configure (const Registry & config);
  void Configure (string param_set);

private:
  void LoadConfig (void);

  double fMaMEC;
  double fMass;
  double fWidth;
  double fNorm;
};

}       // genie namespace
#endif  // _MEC_PARTIAL_XSEC_H_