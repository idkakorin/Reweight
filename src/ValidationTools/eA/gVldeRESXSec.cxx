//____________________________________________________________________________
/*!

\program gvld_e_res_xsec

\brief   Compares GENIE with electron scattering data in the resonance region

         Syntax:
           gvld_e_res_xsec 
                [-h host] [-u user] [-p passwd] [-m model]

         Options:

           [] Denotes an optional argument.

           -h NuVld MySQL URL (eg mysql://localhost/NuScat).
           -u NuVld MySQL username.
           -p NuVld MySQL password.
           -m GENIE model

         Example:

            % gvld_e_res_xsec -u costas \
                              -p &^@7287 \
                              -h mysql://localhost/NuScat
                              -m genie::ReinSeghalRESPXSec/Default

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created Oct 16, 2009 

\cpright Copyright (c) 2003-2009, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#include <cstdlib>
#include <cassert>
#include <sstream>
#include <string>

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TGraph.h>
#include <TPostScript.h>
#include <TLatex.h>
#include <TH1D.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TPavesText.h>
#include <TText.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TBox.h>

#include "Algorithm/AlgFactory.h"
#include "Base/XSecAlgorithmI.h"
#include "BaryonResonance/BaryonResonance.h"
#include "BaryonResonance/BaryonResUtils.h"
#include "Conventions/GBuild.h"
#include "Conventions/Constants.h"
#include "Conventions/Units.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGUtils.h"
#include "PDG/PDGCodes.h"
#include "Utils/CmdLineArgParserUtils.h"
#include "Utils/CmdLineArgParserException.h"
#include "Utils/StringUtils.h"
#include "ValidationTools/NuVld/DBI.h"
#include "ValidationTools/NuVld/DBStatus.h"

using std::ostringstream;
using std::string;

using namespace genie;
using namespace genie::nuvld;
using namespace genie::constants;

/* 
..............................................................................
ELECTRON SCATTERING CROSS SECTION DATA IN THE RESONANCE REGION
..............................................................................
*/
const int    kElXSecDataSets = 17;
const char * kElXSecDataSetLabel[kElXSecDataSets] = 
{
/*  0 */ "JLAB (Hydrogen),  E = 2.445 GeV, #theta = 20.0 deg",
/*  1 */ "JLAB (Hydrogen),  E = 2.445 GeV, #theta = 30.0 deg",
/*  2 */ "JLAB (Hydrogen),  E = 2.445 GeV, #theta = 38.5 deg",
/*  3 */ "JLAB (Hydrogen),  E = 2.445 GeV, #theta = 70.0 deg",
/*  4 */ "JLAB (Hydrogen),  E = 3.245 GeV, #theta = 27.0 deg",
/*  5 */ "JLAB (Hydrogen),  E = 4.045 GeV, #theta = 48.0 deg",
/*  6 */ "JLAB (Hydrogen),  E = 4.054 GeV, #theta = 24.0 deg",
/*  7 */ "JLAB (Hydrogen),  E = 4.054 GeV, #theta = 30.0 deg",
/*  8 */ "JLAB (Hydrogen),  E = 4.054 GeV, #theta = 40.0 deg",
/*  9 */ "JLAB (Deuterium), E = 2.445 GeV, #theta = 20.0 deg",
/* 10 */ "JLAB (Deuterium), E = 2.445 GeV, #theta = 30.0 deg",
/* 11 */ "JLAB (Deuterium), E = 2.445 GeV, #theta = 70.0 deg",
/* 12 */ "JLAB (Deuterium), E = 3.245 GeV, #theta = 27.0 deg",
/* 13 */ "JLAB (Deuterium), E = 4.045 GeV, #theta = 30.0 deg",
/* 14 */ "JLAB (Deuterium), E = 4.045 GeV, #theta = 40.0 deg",
/* 15 */ "JLAB (Deuterium), E = 4.045 GeV, #theta = 48.0 deg",
/* 16 */ "JLAB (Deuterium), E = 4.054 GeV, #theta = 24.0 deg"

};
const char * kElXSecKeyList[kElXSecDataSets] = {
/*  0 */ "JLAB,0",
/*  1 */ "JLAB,0",
/*  2 */ "JLAB,0",
/*  3 */ "JLAB,0",
/*  4 */ "JLAB,0",
/*  5 */ "JLAB,0",
/*  6 */ "JLAB,0",
/*  7 */ "JLAB,0",
/*  8 */ "JLAB,0",
/*  9 */ "JLAB,1",
/* 10 */ "JLAB,1",
/* 11 */ "JLAB,1",
/* 12 */ "JLAB,1",
/* 13 */ "JLAB,1",
/* 14 */ "JLAB,1",
/* 15 */ "JLAB,1",
/* 16 */ "JLAB,1"

};
float kElXSecEnergy[kElXSecDataSets] = {
/*  0 */ 2.445,
/*  1 */ 2.445,
/*  2 */ 2.445,
/*  3 */ 2.445,
/*  4 */ 3.245,
/*  5 */ 4.045,
/*  6 */ 4.054,
/*  7 */ 4.054,
/*  8 */ 4.054,
/*  9 */ 2.445,
/* 10 */ 2.445,
/* 11 */ 2.445,
/* 12 */ 3.245,
/* 13 */ 4.045,
/* 14 */ 4.045,
/* 15 */ 4.045,
/* 16 */ 4.054

};
float kElXSecTheta[kElXSecDataSets] = {
/*  0 */ 20.00,
/*  1 */ 30.00,
/*  2 */ 38.50,
/*  3 */ 70.01,
/*  4 */ 26.98,
/*  5 */ 47.99,
/*  6 */ 24.03,
/*  7 */ 30.00,
/*  8 */ 39.99,
/*  9 */ 20.00,
/* 10 */ 30.00,
/* 11 */ 70.01,
/* 12 */ 26.98,
/* 13 */ 30.00,
/* 14 */ 39.99,
/* 15 */ 48.00,
/* 16 */ 24.03

};
int kElXSecTarget[kElXSecDataSets] = {
/*  0 */ 1000010010, /* proton */
/*  1 */ 1000010010,
/*  2 */ 1000010010,
/*  3 */ 1000010010,
/*  4 */ 1000010010,
/*  5 */ 1000010010,
/*  6 */ 1000010010,
/*  7 */ 1000010010,
/*  8 */ 1000010010,
/*  9 */ 1000020010, /* deuterium */
/* 10 */ 1000020010,
/* 11 */ 1000020010,
/* 12 */ 1000020010,
/* 13 */ 1000020010,
/* 14 */ 1000020010,
/* 15 */ 1000020010,
/* 16 */ 1000020010,

};

typedef DBQueryString                 DBQ;
typedef DBTable<DBElDiffXSecTableRow> DBT;

// function prototypes
void     Init               (void);
void     Plot               (void);
void     End                (void);
void     SetStyle           (void);
void     AddCoverPage       (void);
bool     Connect            (void);
DBQ      FormQuery          (const char * key_list, float energy, float theta);
DBT *    Data               (int iset);
TGraph * Model              (int iset, int imodel);
void     Draw               (int iset);
void     Format             (TGraph* gr, int lcol, int lsty, int lwid, int mcol, int msty, double msiz);
void     GetCommandLineArgs (int argc, char ** argv);
void     PrintSyntax        (void);

// command-line arguments
string gOptDbURL;
string gOptDbUser;
string gOptDbPasswd;
string gOptGModelName;
string gOptGModelConf;

// dbase information
const char * kDefDbURL = "mysql://localhost/NuScat";  

// globals
bool            gCmpWithData  = true;
DBI *           gDBI          = 0;
TPostScript *   gPS           = 0;
TCanvas *       gC            = 0;
bool            gShowModel    = false;

// consts

const int kNCx = 2; // number of columns in TCanvas::Divide()
const int kNCy = 2; // number of rows    in TCanvas::Divide()

const int kNRes=18;  
Resonance_t kResId[kNRes] = {
   kP33_1232, kS11_1535, kD13_1520, kS11_1650,
   kD13_1700, kD15_1675, kS31_1620, kD33_1700,
   kP11_1440, kP33_1600, kP13_1720, kF15_1680,
   kP31_1910, kP33_1920, kF35_1905, kF37_1950,
   kP11_1710, kF17_1970 
};

// current program draws predictions only for the explicit resonance-production
// model at W<Wcut
const bool kDrawHatchcedScalingRegion = true; 

const double kWcut = 1.7; // Wcut from UserPhysicsOptions.xml

//_________________________________________________________________________________
int main(int argc, char ** argv)
{
  GetCommandLineArgs (argc,argv);

  Init();
  Plot();
  End();

  LOG("gvldtest", pINFO)  << "Done!";
  return 0;
}
//_________________________________________________________________________________
void Plot(void)
{
#ifdef __GENIE_MYSQL_ENABLED__

  // connect to the NuValidator MySQL dbase
  bool ok = Connect();
  if(!ok) {
    return;
  }
 
  // loop over data sets
  for(int iset = 0; iset < kElXSecDataSets; iset++) 
  {
    Draw(iset);
  }
#endif
}
//_________________________________________________________________________________
void Init(void)
{
  LOG("vldtest", pNOTICE) << "Initializing...";

  // genie style
  SetStyle();

  // canvas
  gC = new TCanvas("c","",20,20,500,650);
  gC->SetBorderMode(0);
  gC->SetFillColor(0);
  gC->SetGridx();
  gC->SetGridy();

  // output postscript file
  gPS = new TPostScript("genie_eres_vs_data.ps", 111);

  // cover page
  AddCoverPage();
}
//_________________________________________________________________________________
void AddCoverPage(void)
{
  // header
  gPS->NewPage();
  gC->Range(0,0,100,100);
  TPavesText hdr(10,40,90,70,3,"tr");
  hdr.AddText(" ");
  hdr.AddText("GENIE Resonance Electro-Production vs Data");
  hdr.AddText(" ");
  hdr.Draw();
  gC->Update();
}
//_________________________________________________________________________________
void End(void)
{
  LOG("vldtest", pNOTICE) << "Cleaning up...";

  gPS->Close();

  delete gC;
  delete gPS;
}
//_________________________________________________________________________________
// Corresponding GENIE prediction for the `iset' data set 
//.................................................................................
TGraph * Model(int iset, int imodel)
{
  if(!gShowModel) return 0;

  LOG("vldtest", pNOTICE) 
    << "Getting GENIE prediction (model ID = " 
    << imodel << ", data set ID = " << iset << ")";

  AlgFactory * algf = AlgFactory::Instance();
  const XSecAlgorithmI * xsec_alg = 
     dynamic_cast<const XSecAlgorithmI *> (
          algf->GetAlgorithm(gOptGModelName, gOptGModelConf));
  if(!xsec_alg) return 0;

  double M     = kNucleonMass;
  double M2    = M*M;

  double E     = (double) kElXSecEnergy[iset];
  double theta = (double) kElXSecTheta [iset];
  double costh = TMath::Cos(2*kPi*theta/360.);

  LOG("vldtest", pNOTICE) 
     << " ** E = " << E << ", theta = " << theta 
     << " (cos(theta) = " << costh << ")";


  int target_pdgc = kElXSecTarget[iset];
  int Z = pdg::IonPdgCodeToZ(target_pdgc);
  int A = pdg::IonPdgCodeToA(target_pdgc);
  int N = A-Z;
  bool   tgt_has_p = (Z>0);
  bool   tgt_has_n = (N>0);
  double frac_p    = (double) Z / (double) A;
  double frac_n    = (double) N / (double) A;

  Interaction * ep_res = 0;
  Interaction * en_res = 0;

  if(tgt_has_p) {
     ep_res = Interaction::RESEM(1000010010, kPdgProton, kPdgElectron, E);
  }
  if(tgt_has_n) {
     en_res = Interaction::RESEM(1000000010, kPdgProton, kPdgElectron, E);
  }

  const int n = 150;
  double d2sig_dEpdOmega_array[n];
  double W2_array[n];

  double Epmin = 0.;
  double Epmax = E;
  double dEp = (Epmax-Epmin)/(n-1);

  for(int i=0; i<n; i++) {

     double Ep = Epmin + i*dEp;

     LOG("vldtest", pNOTICE) << " ** Ep = " << Ep;

     double Q2 = 2*E*Ep*(1-costh);
     double W2 = M2 + 2*M*(E-Ep)-Q2;
     double W  = TMath::Sqrt( TMath::Max(0.,W2) );

     if(tgt_has_p) {
       ep_res->KinePtr()->SetW (W);
       ep_res->KinePtr()->SetQ2(Q2);
     }
     if(tgt_has_n) {
       en_res->KinePtr()->SetW (W);
       en_res->KinePtr()->SetQ2(Q2);
     }

     double d2sig_dWdQ2 = 0;
    
     for(int ires=0; ires<kNRes; ires++) {

        double d2sig_dWdQ2_res_p = 0.;
        double d2sig_dWdQ2_res_n = 0.;

        if(tgt_has_p) {
          ep_res->ExclTagPtr()->SetResonance(kResId[ires]);
          d2sig_dWdQ2_res_p = xsec_alg->XSec(ep_res,kPSWQ2fE) / units::nb;

          d2sig_dWdQ2_res_p = TMath::Max(0., d2sig_dWdQ2_res_p);

          LOG("vldtest", pNOTICE) 
              << "d2xsec_dWdQ2(ep;" << utils::res::AsString(kResId[ires])
              << "; E=" << E << ", W=" << W << ", Q2=" << Q2 << ") = "
              << d2sig_dWdQ2_res_p << " nbarn/GeV^3";
        }
        if(tgt_has_n) {
          en_res->ExclTagPtr()->SetResonance(kResId[ires]);
          d2sig_dWdQ2_res_n = xsec_alg->XSec(en_res,kPSWQ2fE) / units::nb;

          d2sig_dWdQ2_res_n = TMath::Max(0., d2sig_dWdQ2_res_n);

          LOG("vldtest", pNOTICE) 
              << "d2xsec_dWdQ2(en;" << utils::res::AsString(kResId[ires])
              << "; E=" << E << ", W=" << W << ", Q2=" << Q2 << ") = "
              << d2sig_dWdQ2_res_p << " nbarn/GeV^3";
        }

        d2sig_dWdQ2 += (frac_p*d2sig_dWdQ2_res_p + frac_n*d2sig_dWdQ2_res_n);
     }

     // d^2 sigma / dW dQ^2 --> d^2sigma / dE' dOmega
     double jacobian = (E*Ep)*(M+2*E*(1-costh))/(kPi*W);
     double d2sig_dEpdOmega = jacobian * d2sig_dWdQ2;

     d2sig_dEpdOmega_array[i] = TMath::Max(0., d2sig_dEpdOmega);
     W2_array[i] = W2;
  }


  if(tgt_has_p) { delete ep_res; }
  if(tgt_has_n) { delete en_res; }
  
  TGraph * gr = new TGraph(n,W2_array,d2sig_dEpdOmega_array);

  return gr;
}
//_________________________________________________________________________________
// Download cross section data from NuVld MySQL dbase 
//.................................................................................
bool Connect(void)
{
  if(!gCmpWithData) return true;

  // Get a data-base interface
  TSQLServer * sql_server = TSQLServer::Connect(
      gOptDbURL.c_str(),gOptDbUser.c_str(),gOptDbPasswd.c_str());

  if(!sql_server) return false;
  if(!sql_server->IsConnected()) return false;

  gDBI = new DBI(sql_server);
  return true;
}
//_________________________________________________________________________________
DBQ FormQuery(const char * key_list, float energy, float theta)
{
// forms a DBQueryString for extracting neutrino cross section data from the input 
// key-list and for the input energy range
//  
  ostringstream query_string;
  
  query_string 
    << "KEY-LIST:" << key_list
    << "$CUTS:E_min=" << energy-0.001 << ";E_max=" << energy+0.001 
    << ";Theta_min=" << theta-0.001 << ";Theta_max=" << theta+0.001 
    << "$DRAW_OPT:none$DB-TYPE:eN-Diff-XSec";
  
  DBQ query(query_string.str());
  
  return query;
}
//_________________________________________________________________________________
DBT * Data(int iset)
{
  if(!gCmpWithData) return 0;

  DBT * dbtable = new DBT;

  const char * keylist = kElXSecKeyList[iset];
  float        energy  = kElXSecEnergy [iset];
  float        theta   = kElXSecTheta  [iset];

  DBQ query = FormQuery(keylist, energy, theta);
  assert( gDBI->FillTable(dbtable, query) == eDbu_OK );

  return dbtable;
}
//_________________________________________________________________________________
void Draw(int iset)
{
  // get all measurements for the current channel from the NuValidator MySQL dbase
  DBT * dbtable = Data(iset);

  // get the corresponding GENIE model prediction
  TGraph * model = Model(iset,0);

  if(!model && !dbtable) return;

  int plots_per_page = kNCx * kNCy;
  int iplot = 1 + iset % plots_per_page;

  if(iplot == 1) {
     gPS->NewPage();
     gC -> Clear();
     gC -> Divide(kNCx,kNCy);
  }

  gC -> GetPad(iplot) -> Range(0,0,100,100);
  gC -> GetPad(iplot) -> SetFillColor(0);
  gC -> GetPad(iplot) -> SetBorderMode(0);
  gC -> GetPad(iplot) -> cd();

  double xmin = 0.0, scale_xmin = 0.5;
  double xmax = 0.0, scale_xmax = 1.2;
  double ymin = 0.0, scale_ymin = 0.4;
  double ymax = 0.0, scale_ymax = 1.2;

  TH1F * hframe = 0;
  bool have_frame = false;

  if(dbtable) {
    TGraphAsymmErrors * graph = dbtable->GetGraph("err","W2");
    xmin  = ( graph->GetX() )[TMath::LocMin(graph->GetN(),graph->GetX())];
    xmax  = ( graph->GetX() )[TMath::LocMax(graph->GetN(),graph->GetX())];
    ymin  = ( graph->GetY() )[TMath::LocMin(graph->GetN(),graph->GetY())];
    ymax  = ( graph->GetY() )[TMath::LocMax(graph->GetN(),graph->GetY())];
    hframe = (TH1F*) gC->GetPad(1)->DrawFrame(
        scale_xmin*xmin, scale_ymin*ymin, scale_xmax*xmax, scale_ymax*ymax);
    have_frame = true;

    MultiGraph * mgraph = dbtable->GetMultiGraph("err","W2");
    for(unsigned int igraph = 0; igraph < mgraph->NGraphs(); igraph++) {
       Format(mgraph->GetGraph(igraph), 1,1,1,1,8,0.8);
       mgraph->GetGraph(igraph)->Draw("P");
    }
  }//dbtable?

  if(model) {
    if(!have_frame) {
       xmin  = ( model->GetX() )[TMath::LocMin(model->GetN(),model->GetX())];
       xmax  = ( model->GetX() )[TMath::LocMax(model->GetN(),model->GetX())];
       ymin  = ( model->GetY() )[TMath::LocMin(model->GetN(),model->GetY())];
       ymax  = ( model->GetY() )[TMath::LocMax(model->GetN(),model->GetY())];
       hframe = (TH1F*) gC->GetPad(1)->DrawFrame(
         scale_xmin*xmin, scale_ymin*ymin, scale_xmax*xmax, scale_ymax*ymax);
    }
    Format(model, 1,1,1,1,1,1);
    model->Draw("L");
  }

  //hframe->Draw();
  hframe->GetXaxis()->SetTitle("W^{2} (GeV^{2})");
  hframe->GetYaxis()->SetTitle("d^{2}#sigma / d#Omega dE (nb/sr/GeV)");

  // scaling region
  TBox * scaling_region = 0;
  if(kDrawHatchcedScalingRegion) {
    double W2c = kWcut*kWcut;
    if(W2c > scale_xmin*xmin && W2c < scale_xmax*xmax) {
       scaling_region = new TBox(
           W2c, scale_ymin*ymin, scale_xmax*xmax, scale_ymax*ymax);
       scaling_region->SetFillColor(kRed);
       scaling_region->SetFillStyle(3005);
       scaling_region->Draw();
    }
  }

  // title
  TLatex * title = new TLatex(
     scale_ymin*xmin,1.01*scale_ymax*ymax,kElXSecDataSetLabel[iset]);
  title->SetTextSize(0.04);
  title->Draw();

  gC->GetPad(iplot)->Update();
  gC->Update();
}
//_________________________________________________________________________________
// Formatting
//.................................................................................
void SetStyle(void)
{
  gROOT->SetStyle("Plain");
  
  gStyle -> SetPadTickX (1);
  gStyle -> SetPadTickY (1);
  
  // Turn off all borders
  //
  gStyle -> SetCanvasBorderMode (0);
  gStyle -> SetFrameBorderMode  (0);
  gStyle -> SetPadBorderMode    (0);
  gStyle -> SetDrawBorder       (0);
  gStyle -> SetCanvasBorderSize (0);
  gStyle -> SetFrameBorderSize  (0);
  gStyle -> SetPadBorderSize    (0);
  gStyle -> SetTitleBorderSize  (0);
  
  // Set the size of the default canvas
  //
  gStyle -> SetCanvasDefH (600);
  gStyle -> SetCanvasDefW (730);
  gStyle -> SetCanvasDefX  (10);
  gStyle -> SetCanvasDefY  (10);
  
  // Set marker style
  //
  gStyle -> SetMarkerStyle (20);
  gStyle -> SetMarkerSize   (1);
            
  // Set line widths
  //
  gStyle -> SetFrameLineWidth (1);
  gStyle -> SetFuncWidth      (2);
  gStyle -> SetHistLineWidth  (3);
  gStyle -> SetFuncColor      (2);
  gStyle -> SetFuncWidth      (3);
  
  // Set margins
  //     
  gStyle -> SetPadTopMargin    (0.10);
  gStyle -> SetPadBottomMargin (0.20);
  gStyle -> SetPadLeftMargin   (0.15);
  gStyle -> SetPadRightMargin  (0.03);
   
  // Set tick marks and turn off grids
  //
  gStyle -> SetNdivisions (505,"xyz");
      
  // Adjust size and placement of axis labels
  //                                                                             
  gStyle -> SetLabelSize   (0.040,  "xyz");
  gStyle -> SetLabelOffset (0.005,  "x"  );
  gStyle -> SetLabelOffset (0.005,  "y"  );
  gStyle -> SetLabelOffset (0.005,  "z"  );
  gStyle -> SetTitleSize   (0.060,  "xyz");
  gStyle -> SetTitleOffset (1.200,  "xz" );
  gStyle -> SetTitleOffset (1.000,  "y"  );

  // Set Data/Stat/... and other options
  //
  gStyle -> SetOptDate          (0);
  gStyle -> SetOptFile          (0);
  gStyle -> SetOptStat          (0);
  gStyle -> SetStatFormat       ("6.2f");
  gStyle -> SetFitFormat        ("8.4f");
  gStyle -> SetOptFit           (1);
  gStyle -> SetStatH            (0.20);
  gStyle -> SetStatStyle        (0);
  gStyle -> SetStatW            (0.30);
  gStyle -> SetStatX            (0.845);
  gStyle -> SetStatY            (0.845);
  gStyle -> SetOptTitle         (0);
  gStyle -> SetTitleX           (0.15);
  gStyle -> SetTitleW           (0.75);
  gStyle -> SetTitleY           (0.90);
  gStyle -> SetPalette          (1);
  gStyle -> SetLegendBorderSize (0);

  // Set paper size for life in the US or EU
  //
  gStyle -> SetPaperSize (TStyle::kA4);       //<-- tartes aux fraises
//gStyle -> SetPaperSize (TStyle::kUSLetter); //<-- donuts
}
//_________________________________________________________________________________
void Format(
    TGraph* gr, int lcol, int lsty, int lwid, int mcol, int msty, double msiz)
{
  if(!gr) return;

  if (lcol >= 0) gr -> SetLineColor   (lcol);
  if (lsty >= 0) gr -> SetLineStyle   (lsty);
  if (lwid >= 0) gr -> SetLineWidth   (lwid);

  if (mcol >= 0) gr -> SetMarkerColor (mcol);
  if (msty >= 0) gr -> SetMarkerStyle (msty);
  if (msiz >= 0) gr -> SetMarkerSize  (msiz);
}
//_________________________________________________________________________________
// Parsing command-line arguments, check/form filenames, etc
//.................................................................................
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("gvldtest", pNOTICE) << "*** Parsing command line arguments";

  gCmpWithData = true;

  // get DB URL
  try {
     string model = utils::clap::CmdLineArgAsString(argc,argv,'m');
     vector<string> modelv = utils::str::Split(model,"/");
     assert(modelv.size()==2);
     gOptGModelName = modelv[0];
     gOptGModelConf = modelv[1];
     gShowModel     = true;
  } catch(exceptions::CmdLineArgParserException e) {
     if(!e.ArgumentFound()) {
        gShowModel = false;
     }
  }

  // get DB URL
  try {
     gOptDbURL = utils::clap::CmdLineArgAsString(argc,argv,'h');
  } catch(exceptions::CmdLineArgParserException e) {
     if(!e.ArgumentFound()) {
       gOptDbURL = kDefDbURL;
     }
  }

  // get DB username
  try {
     gOptDbUser = utils::clap::CmdLineArgAsString(argc,argv,'u');
  } catch(exceptions::CmdLineArgParserException e) {
     if(!e.ArgumentFound()) {
       gCmpWithData = false;
     }
  }

  // get DB passwd
  try {
     gOptDbPasswd = utils::clap::CmdLineArgAsString(argc,argv,'p');
  } catch(exceptions::CmdLineArgParserException e) {
     if(!e.ArgumentFound()) {
       gCmpWithData = false;
     }
  }

}
//_________________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gvldtest", pNOTICE)
    << "\n\n" << "Syntax:" << "\n"
    << "   gvld_nuxsec_vs_world_data [-h host] [-u user] [-p passwd] [-m model]\n";
}
//_________________________________________________________________________________
