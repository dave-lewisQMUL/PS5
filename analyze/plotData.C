//Plot histograms from analyzeData.cc

{
  TFile* f = new TFile("analysis.root");
  f->ls();
  TH1D* h1 = (TH1D*)f->Get("hFishSig");
  TH1D* h2 = (TH1D*)f->Get("hFishBkg");
  h1->SetXTitle("tFisher sig");
  h2->SetXTitle("tFisher bkg");
  h1->Draw();
  h2->Draw("same");
}
