TFile* fin;
TTree* tin;

const int nMaxHits = 10000;
const int nMat = 2; //0 for Scint, 1 for Pb

const int NumberOfLayers = 105;
const int NumberOfScintillators = 25;

int nHits[nMat];
int lid[nMat][nMaxHits];
int segid[nMat][nMaxHits];
double e[nMat][nMaxHits];
double x[nMat][nMaxHits];
double y[nMat][nMaxHits];
double z[nMat][nMaxHits];

const double det_offset = 30.0;
const double Thickness_Pb = 1.0;
const double Thcikness_Scint = 5.0;

const double transverse_offset = -250.0;
const double seg_width = 20.0;

const int nPos = 2; //0 for x-seg, 1 for y-seg

const double intrin_unc_xy = seg_width/sqrt(12.0);
const double intrin_unc_z = Thcikness_Scint/sqrt(12.0);

const double min_edep = 0.0;


TH2D* hTotalTracks[nPos];
TH1D* hSlope_proc1;

double Layer2Z(int lay){
 return det_offset + Thickness_Pb + Thcikness_Scint/2.0 + (Thcikness_Scint+Thickness_Pb)*lay;
}
double Seg2XY(int seg){
 return transverse_offset + seg_width*(0.5 + (double)seg);
}

void CreateHist(){
 for(int i=0;i<nPos;i++){
	hTotalTracks[i] = new TH2D(Form("hTotalTracks%d",i),Form("hTotalTracks%d",i),130,-60,720,30,-300,300);
 }
 hSlope_proc1 = new TH1D("hSlope_proc1","hSlope_proc1",200,-20,20);
}


void GetBranches(){
 if( !tin ) return;
 tin->SetBranchAddress("nEMHit",&nHits[0]);
 tin->SetBranchAddress("EMHit.LayerID",lid[0]);
 tin->SetBranchAddress("EMHit.SegmentID",segid[0]);

 tin->SetBranchAddress("EMHit.e",e[0]);
 tin->SetBranchAddress("EMHit.x",x[0]);
 tin->SetBranchAddress("EMHit.y",y[0]);
 tin->SetBranchAddress("EMHit.z",z[0]);
}


void AnaInLoop(int i, int proc=0){
 tin->GetEntry(i);
 for(int j=0;j<nHits[0];j++){
	hTotalTracks[ lid[0][j]%2 ]->Fill( Layer2Z( lid[0][j] ), Seg2XY( segid[0][j] ) );
 }
 if( proc == 1 ){
	tin->GetEntry(i);
	TGraphErrors* gSTrack[2];
	for(int j=0;j<nPos;j++){
		gSTrack[j] = new TGraphErrors();
	}
	int npoints[nPos] = {0,0};

	double edepsum[nPos][NumberOfLayers] = {0};
	double meanx[NumberOfLayers] = {0};
	double meany[NumberOfLayers] = {0};
	double meanz[nPos][NumberOfLayers] = {0};

        double meanx_unc[NumberOfLayers] = {0};
        double meany_unc[NumberOfLayers] = {0};
        double meanz_unc[nPos][NumberOfLayers] = {0};

	for(int j=0;j<nHits[0];j++){
		gSTrack[ lid[0][j]%2 ]->SetPoint( npoints[lid[0][j]%2], Layer2Z( lid[0][j] ), Seg2XY( segid[0][j] ) );
		gSTrack[ lid[0][j]%2 ]->SetPointError( npoints[lid[0][j]%2], intrin_unc_z/sqrt( e[0][j] ), intrin_unc_xy/sqrt( e[0][j] ) );
		npoints[lid[0][j]%2]++;

		edepsum[lid[0][j]%2][lid[0][j]/2] += e[0][j];
		if( lid[0][j]%2== 0 ){
			meanx[lid[0][j]/2] += e[0][j]* Seg2XY( segid[0][j] );
			meanz[0][lid[0][j]/2] += e[0][j]*z[0][j];

			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy/sqrt( e[0][j] ),2 );
			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z/sqrt( e[0][j] ),2 );
		} else{
			meany[lid[0][j]/2] += e[0][j]* Seg2XY( segid[0][j] );
			meanz[1][lid[0][j]/2] += e[0][j]*z[0][j];

			meany_unc[lid[0][j]/2] += pow( intrin_unc_xy/sqrt( e[0][j] ),2 );
			meanz_unc[1][lid[0][j]/2] += pow( intrin_unc_z/sqrt( e[0][j] ),2 );
		}
	}

	for(int j=0;j<NumberOfLayers;j++){
		meanx[j] = edepsum[0][j];
		meany[j] = edepsum[1][j];
		meanx_unc[j] = sqrt( meanx_unc[j] );
		meany_unc[j] = sqrt( meany_unc[j] );
		for(int k=0;k<nPos;k++){
			meanz[k][j] /= edepsum[k][j];
			meanz_unc[k][j] = sqrt( meanz_unc[k][j] );
		}
	}

	TGraphErrors* gSMeanTrack[nPos];
	for(int k=0;k<nPos;k++){
		gSMeanTrack[k] = new TGraphErrors();
		npoints[k] = 0;
		for(int j=0;j<NumberOfLayers;j++){
			if( edepsum[k][j] <= min_edep ) continue;
			gSMeanTrack[k]->SetPoint( npoints[k], meanz[k][j], meanx[j] );
			gSMeanTrack[k]->SetPointError( npoints[k], meanz_unc[k][j], meanx_unc[j] );
			npoints[k]++;
		}
	}
	
	TF1* f1[nPos];
	for(int k=0;k<nPos;k++){
		f1[k] = new TF1("f1","[0]+[1]*x",0,1000);
		gSMeanTrack[k]->Fit(f1[k],"q");
	}
	hSlope_proc1->Fill( sqrt( pow( f1[0]->GetParameter(1),2 ) + pow( f1[1]->GetParameter(1),2 ) )*180.0/TMath::Pi() );
 }
}


void ana(){
 fin = new TFile("out.root","read");
 tin = (TTree*)fin->Get("tree");

 GetBranches();
 CreateHist();
 for(int i=0;i<tin->GetEntries();i++){
//	AnaInLoop(i);
	AnaInLoop(i,1);
 }
 hSlope_proc1->Draw();
// AnaInLoop(0,1);
// hTotalTracks[0]->Draw("colz");
// hTotalTracks[1]->Draw("colz");

}
