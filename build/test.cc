void test(){
  auto tf = new TFile("test.root");
  auto tr = (TTree*) tf -> Get("tree");
  vector<vector<double>> *EMStepEdep = 0;
  tr -> SetBranchAddress("EMStepEdep",&EMStepEdep);

  auto h1 = new TH1D("h1","Step hit",100,0,10);
  auto h2 = new TH1D("h2","Detector hit",100,0,10);
  for (int i = 0; i < tr -> GetEntries(); i++){
    tr -> GetEntry(i);
    int nDet = EMStepEdep->size();
    for (int idet = 0; idet < nDet; idet++){
      vector<double> hits = EMStepEdep->at(idet);
      int nStep = hits.size();
      double edep = 0;
      for (int istep = 0; istep < nStep; istep++){
	h1 -> Fill(hits[istep]);
	edep += hits[istep];
      }
      h2 -> Fill(edep);
    }
  }
  auto c1 = new TCanvas("c1","",1200,600);
  c1 -> Divide(2,1);
  c1-> cd(1);
  h1 -> Draw();
  c1->cd(2);
  h2 -> Draw();
}
