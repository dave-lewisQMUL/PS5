#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include "Event.h"
#include <TMVA/Reader.h>

int main() {

// Set up an output file and book some histograms

  TFile* histFile = new TFile("analysis.root", "RECREATE");
  TH1D* hFishSig = new TH1D("hFishSig", "Fisher, signal", 100, -10.0, 10.0);
  TH1D* hFishBkg = new TH1D("hFishBkg", "Fisher, background", 100, -10.0, 10.0);

// Set up the TMVA Reader object.
// The names in AddVariable must be same as in the input (weight) file.

  TMVA::Reader* reader = new TMVA::Reader();
  float x, y, z;                       // TMVA needs float, not double
  reader->AddVariable("x", &x);  
  reader->AddVariable("y", &y);
  reader->AddVariable("z", &z);
  std::string dir    = "../train/weights/";
  std::string prefix = "tmvaTest";
  reader->BookMVA("Fisher", dir + prefix + "_Fisher.weights.xml");

// Open input file, get the TTrees, put into a vector

  TFile* inputFile = new TFile("../generate/testData.root");
  inputFile->ls();
  TTree* sig = dynamic_cast<TTree*>(inputFile->Get("sig"));
  TTree* bkg = dynamic_cast<TTree*>(inputFile->Get("bkg"));
  std::vector<TTree*> treeVec;
  treeVec.push_back(sig);
  treeVec.push_back(bkg);

// Loop over TTrees

  for (int i=0; i<treeVec.size(); i++){

    treeVec[i]->Print();
    Event evt;
    treeVec[i]->SetBranchAddress("evt", &evt);
    int numEntries = treeVec[i]->GetEntries();
    std::cout << "number of entries = " << numEntries << std::endl;

// Loop over events.  The test statistic is identified by the first 
// argument used above in BookMVA (below, e.g., "Fisher").

    for (int j=0; j<numEntries; j++){
      treeVec[i]->GetEntry(j);                // sets evt
      x = evt.x;                              // set variables of reader
      y = evt.y;
      z = evt.z;
      double tFisher = reader->EvaluateMVA("Fisher");
      // std::cout << evt.x << "  " << evt.y << "  " << evt.z << std::endl;
      // std::cout << "tFisher = " << tFisher << std::endl;

      if ( i == 0 ){ 
        hFishSig->Fill(tFisher);
      }
      else if ( i == 1 ){
        hFishBkg->Fill(tFisher);
      }

    }

  }

  histFile->Write();
  histFile->Close();
  return 0;

}
