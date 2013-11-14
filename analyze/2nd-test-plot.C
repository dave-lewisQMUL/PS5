{

  TFile *f = new TFile("analysis.root");
  f.ls();
  TH1F *h1 = (TH1F*)f.Get("hMLPSig");
  TH1F *h2 = (TH1F*)f.Get("hMLPBkg");
  h1.SetXTitle("tMLP value");

  h1.Draw();
  h2.SetLineColor(kRed);
  h2.Draw("sames");

}
