#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TPad.h>
#include <TAxis.h>
#include <TH2F.h>
#include <TF2.h>
#include <TLine.h>
#include "Event.h"
#include "OutFunc.h"
#include <TMVA/Reader.h>

using namespace std;

OutFunc* fClass;         // global to communicate with tf2Func
double tf2Func(double[], double[]);

int main() {

// Set up an output file and book some histograms

  TFile* histFile = new TFile("analysis.root", "RECREATE");  
  TH1D* hSig = new TH1D("hSig", "Fisher, signal", 100, -5.0, 5.0);
  TH1D* hBkg = new TH1D("hBkg", "Fisher, background", 100, -5.0, 5.0);
  TList* hList = new TList();      // list of histograms to store
  hList->Add(hSig);
  hList->Add(hBkg);

// Set up the OutFunc object.  First argument must be one of the classifiers.
// 4th argument is offset for contour.
// 5th argument is bool array indicating which variables were used in training

  std::string dir    = "../train/weights/";
  std::string prefix = "tmvaTest";
  const double tCut = 0.;
  std::vector<bool> useVar(3);
  useVar[0] = true;      // x
  useVar[1] = true;      // y
  useVar[2] = true;      // z
  fClass = new OutFunc("Fisher", dir, prefix, tCut, useVar);

// Open input file, get the TTrees, put into a vector

  TFile* inputFile = new TFile("../generate/testData.root");
  inputFile->ls();
  TTree* sig = dynamic_cast<TTree*>(inputFile->Get("sig"));
  TTree* bkg = dynamic_cast<TTree*>(inputFile->Get("bkg"));
  std::vector<TTree*> treeVec;
  treeVec.push_back(sig);
  treeVec.push_back(bkg);

// Loop over TTrees

  int nSigAcc = 0;
  int nBkgAcc = 0;
  int nSig, nBkg;

  for (int i=0; i<treeVec.size(); i++){

    treeVec[i]->Print();
    Event evt;
    treeVec[i]->SetBranchAddress("evt", &evt);
    int numEntries = treeVec[i]->GetEntries();
    if ( i == 0 ) { nSig = numEntries; }
    if ( i == 1 ) { nBkg = numEntries; }
    std::cout << "number of entries = " << numEntries << std::endl;

// Loop over events.  The test statistic is identified by the first 
// argument used above in BookMVA (below, e.g., "Fisher", "MLP").

    for (int j=0; j<numEntries; j++){

      treeVec[i]->GetEntry(j);                   // sets evt
      double t = fClass->val(&evt);
      // cout << t << " " << evt.x << "  " << evt.y << endl;

      if ( i == 0 ){                       // signal
        hSig->Fill(t);
        if ( t >= tCut ) { nSigAcc++; }
      }
      else if ( i == 1 ){                  // background
        hBkg->Fill(t);
        if ( t >= tCut ) { 
          nBkgAcc++; 
        }
      }

    }

  }

  double epsSig = static_cast<double>(nSigAcc)/
                      static_cast<double>(nSig);
  double epsBkg = static_cast<double>(nBkgAcc)/
                      static_cast<double>(nBkg);

  cout << "nSigAcc, nSig = " << nSigAcc << " , " << nSig << endl;
  cout << "nBkgAcc, nBkg = " << nBkgAcc << " , " << nBkg << endl;

  std::cout << "signal efficiency     = " << epsSig << std::endl;
  std::cout << "background efficiency = " << epsBkg << std::endl;

// make some plots of decision boundaries using a subset of the events

  TApplication* theApp = new TApplication("App", 0, 0);
  TCanvas* canvas = new TCanvas("canvas", "canvas", 10, 10, 600, 600);

  canvas->SetFillColor(0);
  canvas->SetBorderMode(0);  
  canvas->SetFrameBorderMode(0);   // need this to turn off red hist frame!
  gROOT->SetStyle("Plain");
  canvas->UseCurrentStyle();

  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);

  gStyle->SetOptStat(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleSize(0.06);

  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.06);
  gStyle->SetTitleFont(42, "hxy");    // for histogram and axis title
  gStyle->SetLabelFont(42, "xyz");    // for axis labels (values)

  gStyle->SetTitleOffset(0.8, "h");        // what does this do?
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleY(0.99);
  gROOT->ForceStyle();

  double xMin = -2.5;
  double xMax = 4.;
  double yMin = -2.5;
  double yMax = 4.;
  TH2F* hframe = new TH2F("hframe", "", 1, xMin, xMax, 1, yMin, yMax);

  TAxis* xa = hframe->GetXaxis();
  TAxis* ya = hframe->GetYaxis();
  xa->SetTitleSize(0.06);
  ya->SetTitleSize(0.06);
  xa->SetTitleOffset(1.1);    //  factor multiplies default offset
  ya->SetTitleOffset(1.1);
  xa->SetLabelOffset(0.015);
  ya->SetLabelOffset(0.015);
  hframe->SetXTitle("x_{1}");
  hframe->SetYTitle("x_{2}");

  hframe->SetNdivisions(505, "x");
  hframe->SetNdivisions(505, "y");
  hframe->SetTitle("");
  hframe->DrawCopy();

  int numEventsPlot = 300;
  for (int i=0; i<treeVec.size(); i++){

    treeVec[i]->Print();
    Event evt;
    treeVec[i]->SetBranchAddress("evt", &evt);
    int numEntries = treeVec[i]->GetEntries();
    if ( i == 0 ) { nSig = numEntries; }
    if ( i == 1 ) { nBkg = numEntries; }

    treeVec[i]->SetMarkerSize(1.5);
    if ( i == 0 ) {
      treeVec[i]->SetMarkerColor(kBlue);
      treeVec[i]->SetMarkerStyle(24);          // open circle
    }
    else {
      treeVec[i]->SetMarkerColor(kRed);
      treeVec[i]->SetMarkerStyle(22);          // full triangle up
    }
    treeVec[i]->Draw("y:x", "", "same", numEventsPlot, 0);
    canvas->Update();

  }

  // draw decision boundary

  TF2* fcon = new TF2("fcon", tf2Func, -5., 5., -5., 5., 0);
  const int numCon = 1;
  double conVal[numCon];
  conVal[0] = 0.;
  fcon->SetContour(numCon, conVal);
  fcon->SetNpx(200);
  fcon->SetNpy(200);
  fcon->SetLineStyle(1);
  fcon->SetLineWidth(2);
  fcon->DrawCopy("same");
  canvas->Update(); 

  string gifFile = "plot.gif";
  canvas->Print(gifFile.c_str(), "gif");
  //  string epsFile = "plot.eps";
  //  canvas->Print(epsFile.c_str(), "eps");

  string dummy;
  cout << "Enter any key to continue: ";
  getline(cin, dummy);

  inputFile->Close();
  histFile->Write();
  histFile->Close();

  delete theApp, fcon, canvas;

  return 0;

}

// User must supply a function of this form for TF2

double tf2Func(double xArray[], double par[]){
  Event evt;
  evt.x = xArray[0];
  evt.y = xArray[1];
  evt.z = 0.;                       // KLUDGE
  double f = fClass->val(&evt);
  return f;

}
