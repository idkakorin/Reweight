//____________________________________________________________________________
/*
 Copyright (c) 2003-2006, GENIE Neutrino MC Generator Collaboration
 All rights reserved.
 For the licensing terms see $GENIE/USER_LICENSE.

 Author: Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory - August 17, 2004

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include <TMCParticle6.h>

#include "Fragmentation/PythiaHadronization.h"
#include "Messenger/Messenger.h"
#include "Numerical/RandomGen.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "Utils/KineUtils.h"

using namespace genie;

//-- the actual PYTHIA call

extern "C" void py2ent_(int *,  int *, int *, double *);

//____________________________________________________________________________
PythiaHadronization::PythiaHadronization() :
HadronizationModelI("genie::PythiaHadronization")
{
  fPythia = new TPythia6();
}
//____________________________________________________________________________
PythiaHadronization::PythiaHadronization(string config) :
HadronizationModelI("genie::PythiaHadronization", config)
{
  fPythia = new TPythia6();
}
//____________________________________________________________________________
PythiaHadronization::~PythiaHadronization()
{

}
//____________________________________________________________________________
void PythiaHadronization::Initialize(void) const
{

}
//____________________________________________________________________________
TClonesArray * PythiaHadronization::Hadronize(
                                        const Interaction * interaction) const
{
  LOG("PythiaHad", pINFO) << "Running PYTHIA hadronizer";

  //-- get kinematics / init-state / process-info

  const Kinematics &   kinematics = interaction->GetKinematics();
  const InitialState & init_state = interaction->GetInitialState();
  const ProcessInfo &  proc_info  = interaction->GetProcessInfo();
  const Target &       target     = init_state.GetTarget();

  double W = kinematics.W();

  assert( target.StruckQuarkIsSet() ); 

  int  probe       = init_state.GetProbePDGCode();
  int  hit_nucleon = target.StruckNucleonPDGCode();
  int  hit_quark   = target.StruckQuarkPDGCode();
  bool from_sea    = target.StruckQuarkIsFromSea();

  LOG("PythiaHad", pINFO)
          << "Hit nucleon pdgc = " << hit_nucleon << ", W = " << W;
  LOG("PythiaHad", pINFO)
            << "Selected hit quark pdgc = " << hit_quark
                           << ((from_sea) ? "[sea]" : "[valence]");

  //-- check hit-nucleon assignment, input neutrino & weak current
  bool isp  = pdg::IsProton(hit_nucleon);
  bool isn  = pdg::IsNeutron(hit_nucleon);
  bool isv  = pdg::IsNeutrino(probe);
  bool isvb = pdg::IsAntiNeutrino(probe);
  bool iscc = proc_info.IsWeakCC();
  bool isnc = proc_info.IsWeakNC();
  if( !(isp||isn) ) {
    LOG("PythiaHad", pERROR) << "Can not handle nucleon: " << hit_nucleon;
    exit(1);
  }
  if( !(iscc||isnc) ) {
    LOG("PythiaHad", pERROR) << "Can only handle weak interactions";
    exit(1);
  }
  if( !(isv||isvb) ) {
    LOG("PythiaHad", pERROR)
                      << "Can not handle non-neutrino probe: " << probe;
    exit(1);
  }

  //-- assert that the interaction mode is allowed
  bool isu  = pdg::IsUQuark     (hit_quark);
  bool isd  = pdg::IsDQuark     (hit_quark);
  bool isub = pdg::IsUAntiQuark (hit_quark);
  bool isdb = pdg::IsDAntiQuark (hit_quark);

  bool allowed = (iscc && isv  && (isd||isub)) ||
                 (iscc && isvb && (isu||isdb)) ||
                 (isnc && (isv||isvb) && (isu||isd||isub||isdb));
  assert(allowed);

  //-- generate the quark system (q + qq) initiating the hadronization
  int  final_quark = 0; // hit quark after the interaction
  int  diquark     = 0; // remnant diquark (xF<0 at hadronic CMS)

  //-- figure out the what happens to the hit quark after the interaction

  assert(proc_info.IsWeak());
  if (proc_info.IsWeakNC()) final_quark = hit_quark;
  else {
    if      (isv  && isd ) final_quark = kPdgUQuark;
    else if (isv  && isub) final_quark = kPdgDQuarkBar;
    else if (isvb && isu ) final_quark = kPdgDQuark;
    else if (isvb && isdb) final_quark = kPdgUQuarkBar;
    else {
      LOG("PythiaHad", pERROR)
        << "Not allowed mode. Refuse to make a final quark assignment!";
      exit(1);
    }
  }//CC

  //-- figure out what the remnant diquark is

  // hit quark = valence quark
  if(!from_sea) {
    if (isp && isu) diquark = kPdgUDDiquarkS1; /* u(->q) + ud */
    if (isp && isd) diquark = kPdgUUDiquarkS1; /* d(->q) + uu */
    if (isn && isu) diquark = kPdgDDDiquarkS1; /* u(->q) + dd */
    if (isn && isd) diquark = kPdgUDDiquarkS1; /* d(->q) + ud */
  }
  // hit quark = sea quark
  else {

    if(isp && isu) diquark = kPdgUDDiquarkS1; /* u(->q) + bar{u} uud (=ud) */
    if(isp && isd) diquark = kPdgUUDiquarkS1; /* d(->q) + bar{d} uud (=uu) */
    if(isn && isu) diquark = kPdgDDDiquarkS1; /* u(->q) + bar{u} udd (=dd) */
    if(isn && isd) diquark = kPdgUDDiquarkS1; /* d(->q) + bar{d} udd (=ud) */

    // Oh, crap!
    // The neutrino is scattered off a sea antiquark, materializing its quark
    // partner and leaving me with a 5q system ( <qbar + q> + qqq(valence) )
    // I will force few qbar+q annhilations below to get my quark/diquark
    // system back but it does not feel right...
    // Probably it is best to leave the qqq system in the final state and then
    // just do the fragmentation of the qbar q system? But how do I figure out
    // how to split the available energy?

    /* bar{u} (-> bar{d}) + u uud => u + uu */
    if(isp && isub && iscc) {final_quark = kPdgUQuark; diquark = kPdgUUDiquarkS1;}
    /* bar{u} (-> bar{u}) + u uud => u + ud */
    if(isp && isub && isnc) {final_quark = kPdgUQuark; diquark = kPdgUDDiquarkS1;}
    /* bar{d} (-> bar{u}) + d uud => d + ud */
    if(isp && isdb && iscc) {final_quark = kPdgDQuark; diquark = kPdgUDDiquarkS1;}
    /* bar{d} (-> bar{d}) + d uud => d + uu */
    if(isp && isdb && isnc) {final_quark = kPdgDQuark; diquark = kPdgUUDiquarkS1;}
    /* bar{u} (-> bar{d}) + u udd => u + ud */
    if(isn && isub && iscc) {final_quark = kPdgUQuark; diquark = kPdgUDDiquarkS1;}
    /* bar{u} (-> bar{u}) + u udd => u + dd */
    if(isn && isub && isnc) {final_quark = kPdgUQuark; diquark = kPdgDDDiquarkS1;}
    /* bar{d} (-> bar{u}) + d udd => d + dd */
    if(isn && isdb && iscc) {final_quark = kPdgDQuark; diquark = kPdgDDDiquarkS1;}
    /* bar{d} (-> bar{d}) + d udd => d + ud */
    if(isn && isdb && isnc) {final_quark = kPdgDQuark; diquark = kPdgUDDiquarkS1;}
  }
  assert(diquark!=0);

  //-- PYTHIA->HADRONIZE:

  LOG("PythiaHad", pINFO)
        << "Fragmentation / Init System: "
                      << "q = " << final_quark << ", qq = " << diquark;
  int ip = 0;
  py2ent_(&ip, &final_quark, &diquark, &W);

  //-- get LUJETS record
  fPythia->GetPrimaries();
  TClonesArray * pythia_particles =
                      (TClonesArray *) fPythia->ImportParticles("All");

  // copy PYTHIA container to a new TClonesArray so as to transfer ownership
  // of the container and of its elements to the calling method

  int np = pythia_particles->GetEntries();
  assert(np>0);
  TClonesArray * particle_list = new TClonesArray("TMCParticle", np);
  register unsigned int i = 0;
  TMCParticle * particle = 0;
  TIter particle_iter(pythia_particles);

  while( (particle = (TMCParticle *) particle_iter.Next()) ) {
       LOG("PythiaHad", pINFO)
               << "Adding final state particle pdgc = " << particle->GetKF();
       new ( (*particle_list)[i++] ) TMCParticle(*particle);
  }

  particle_list->SetOwner(true);
  return particle_list;
}
//____________________________________________________________________________

