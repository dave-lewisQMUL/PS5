#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TString.h>
#include <TMVA/Factory.h>

int main() {  

// Create ouput file, factory object and open the input file

  TFile* outputFile = TFile::Open( "TMVA.root", "RECREATE" );
  TMVA::Factory* factory = new TMVA::Factory("tmvaTest", outputFile, "");
  TFile* inputFile = new TFile("../generate/testData.root");

// get the TTree objects from the input file

  TTree* sig = (TTree*)inputFile->Get("sig");
  TTree* bkg = (TTree*)inputFile->Get("bkg");
  int nSig = sig->GetEntries();
  int nBkg = bkg->GetEntries();

// global event weights (see below for setting event-wise weights)

  double sigWeight = 1.0;
  double bkgWeight = 1.0;
  factory->SetInputTrees(sig, bkg, sigWeight, bkgWeight);
   
// Define the input variables that shall be used for the MVA training
// (the variables used in the expression must exist in the original TTree).

  factory->AddVariable("x", 'F');
  factory->AddVariable("y", 'F');
  factory->AddVariable("z", 'F');

// Apply additional cuts on the signal and background sample.
// for example: TCut mycut = "abs(x)<0.5 && abs(y-0.5)<1";

  TCut mycut = "";

// Use half of the events for training, half for testing

//  Root 5.26 appears not to like TString argument...
//  TString splitOpt = "NSigTrain=0:NBkgTrain=0:NSigTest=0:NBkgTest=0";
//  factory->PrepareTrainingAndTestTree(mycut, splitOpt);
  factory->PrepareTrainingAndTestTree(mycut, 0, 0, 0, 0);

// Book MVA methods (see TMVA manual).  

  factory->BookMethod(TMVA::Types::kFisher, "Fisher", "H:!V:Fisher");   

// Train, test and evaluate all methods

  factory->TrainAllMethods();
  factory->TestAllMethods();
  // Following line used to work, causes crash with ROOT 5.20.00, should
  // be fixed in root 5.21, see https://savannah.cern.ch/bugs/?40468
  //  factory->EvaluateAllMethods();    

// Save the output and finish up

  outputFile->Close();
  std::cout << "==> wrote root file TMVA.root" << std::endl;
  std::cout << "==> TMVAnalysis is done!" << std::endl; 

  delete factory;
  return 0;

}
