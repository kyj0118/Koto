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
double t[nMat][nMaxHits];

const double det_offset = 0.0;
const double Thickness_Pb = 1.0;
const double Thcikness_Scint = 5.0;

const double transverse_offset = -250.0;
const double seg_width = 20.0;

const int nPos = 2; //0 for x-seg, 1 for y-seg

const double intrin_unc_xy = seg_width/sqrt(12.0);
const double intrin_unc_z = Thcikness_Scint/sqrt(12.0);

const double min_edep = 0.5;

TH1D* hEneDist_Scint;
TH1D* hEneDist_Lead;

TH1D* hTotalEne;
TH1D* hTotalEne_Scint;
TH1D* hTotalEne_Lead;

TH1D* hTimeDist;
TH1D* hTimeDist_Scint;
TH1D* hTimeDist_Lead;


TH2D* hTotalTracks[nPos];
TH1D* hSlope_proc1;

TGraphErrors* gRawTrackSample[nPos];
TGraphErrors* gTrackSample[nPos];
TGraphErrors* gTrackEne[nPos]; //storage for energy weight

const int nsmoo = 10;
TGraphErrors* gTrackSampleSmooth[nPos];


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
 hSlope_proc1 = new TH1D("hSlope_proc1","hSlope_proc1",200,-5,45);

 hEneDist_Scint = new TH1D("hEneDist_Scint","hEneDist_Scint",2000,0,100);
 hEneDist_Lead = new TH1D("hEneDist_Lead","hEneDist_Lead",2000,0,100);

 hTotalEne = new TH1D("hTotalEne","hTotalEne",220,-50,1050);
 hTotalEne_Scint = new TH1D("hTotalEne_Scint","hTotalEne_Scint",220,-50,1050);
 hTotalEne_Lead = new TH1D("hTotalEne_Lead","hTotalEne_Lead",220,-50,1050);

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
 tin->SetBranchAddress("EMHit.t",t[0]);

 tin->SetBranchAddress("nLeadHit",&nHits[1]);
 tin->SetBranchAddress("LeadHit.LayerID",lid[1]);

 tin->SetBranchAddress("LeadHit.e",e[1]);
 tin->SetBranchAddress("LeadHit.x",x[1]);
 tin->SetBranchAddress("LeadHit.y",y[1]);
 tin->SetBranchAddress("LeadHit.z",z[1]);
 tin->SetBranchAddress("LeadHit.t",t[1]);

}

TGraphErrors* Smoothing( TGraphErrors* gorg ){
 TGraphErrors* gSmooth = new TGraphErrors();
 gSmooth->SetPoint( 0, gorg->GetX()[0], gorg->GetY()[0] );
 gSmooth->SetPointError( 0, gorg->GetErrorX(0), gorg->GetErrorY(0) );
 for(int i=1;i<gorg->GetN()-1;i++){
	gSmooth->SetPoint( i, ( gorg->GetX()[i-1]+gorg->GetX()[i]+gorg->GetX()[i+1] )/3.0,
		( gorg->GetY()[i-1]+gorg->GetY()[i]+gorg->GetY()[i+1] )/3.0 );
	gSmooth->SetPointError( i, sqrt( pow( gorg->GetErrorX(i-1)/3.0,2 ) + pow( gorg->GetErrorX(i)/3.0,2 ) + pow( gorg->GetErrorX(i+1)/3.0,2 ) ),
		sqrt( pow( gorg->GetErrorY(i-1)/3.0,2 ) + pow( gorg->GetErrorY(i)/3.0,2 ) + pow( gorg->GetErrorY(i+1)/3.0,2 ) ) );
 }
 gSmooth->SetPoint( gorg->GetN()-1, gorg->GetX()[gorg->GetN()-1], gorg->GetY()[gorg->GetN()-1] );
 gSmooth->SetPointError( gorg->GetN()-1, gorg->GetErrorX(gorg->GetN()-1), gorg->GetErrorY(gorg->GetN()-1) );

 return gSmooth;
}

TGraphErrors* SmoothingEneWeight( TGraphErrors* gorg, TGraphErrors* gene ){
 TGraphErrors* gSmooth = new TGraphErrors();
 gSmooth->SetPoint( 0, gorg->GetX()[0], gorg->GetY()[0] );
 gSmooth->SetPointError( 0, gorg->GetErrorX(0), gorg->GetErrorY(0) );
 double ene;
 for(int i=1;i<gorg->GetN()-1;i++){
	ene = (gene->GetY()[i-1]+gene->GetY()[i]+gene->GetY()[i+1]);
	gSmooth->SetPoint( i, ( gorg->GetX()[i-1]*gene->GetY()[i-1] +
				gorg->GetX()[i]  *gene->GetY()[i] +
				gorg->GetX()[i+1]*gene->GetY()[i+1] )/ene,
			      ( gorg->GetY()[i-1]*gene->GetY()[i-1] +
				gorg->GetY()[i]  *gene->GetY()[i] + 
				gorg->GetY()[i+1]*gene->GetY()[i+1] )/ene );

	gSmooth->SetPointError( i, sqrt(
		pow( gorg->GetErrorX(i-1) * gene->GetY()[i-1] / ene,2 ) +
		pow( gorg->GetErrorX(i)   * gene->GetY()[i]   / ene,2 ) +
		pow( gorg->GetErrorX(i+1) * gene->GetY()[i+1] / ene,2 ) ),
		sqrt(
		pow( gorg->GetErrorY(i-1) * gene->GetY()[i-1] / ene,2 ) +
		pow( gorg->GetErrorY(i)   * gene->GetY()[i]   / ene,2 ) +
		pow( gorg->GetErrorY(i+1) * gene->GetY()[i+1] / ene,2 ) ) );

 }
 gSmooth->SetPoint( gorg->GetN()-1, gorg->GetX()[gorg->GetN()-1], gorg->GetY()[gorg->GetN()-1] );
 gSmooth->SetPointError( gorg->GetN()-1, gorg->GetErrorX(gorg->GetN()-1), gorg->GetErrorY(gorg->GetN()-1) );

 return gSmooth;
}
void QAInLoop(int i, int opt=0){
 tin->GetEntry(i);
 double ene_total_scint = 0.0;
 double ene_total_lead = 0.0;
 for(int j=0;j<nHits[0];j++){
	hEneDist_Scint->Fill( e[0][j] );	
	ene_total_scint += e[0][j];
 }
 for(int j=0;j<nHits[1];j++){ 
	hEneDist_Lead->Fill( e[1][j] );
	ene_total_lead += e[1][j];
 }
 hTotalEne_Scint->Fill( ene_total_scint );
 hTotalEne_Lead->Fill( ene_total_lead );
 hTotalEne->Fill( ene_total_scint+ene_total_lead );
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
//		gSTrack[ lid[0][j]%2 ]->SetPointError( npoints[lid[0][j]%2], intrin_unc_z/sqrt( e[0][j] ), intrin_unc_xy/sqrt( e[0][j] ) );
		gSTrack[ lid[0][j]%2 ]->SetPointError( npoints[lid[0][j]%2], intrin_unc_z, intrin_unc_xy );
		npoints[lid[0][j]%2]++;

		edepsum[lid[0][j]%2][lid[0][j]/2] += e[0][j];
		if( lid[0][j]%2== 0 ){
			meanx[lid[0][j]/2] += e[0][j]* Seg2XY( segid[0][j] );
			meanz[0][lid[0][j]/2] += e[0][j]*z[0][j];

//			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy/sqrt( e[0][j] ),2 );
//			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z/sqrt( e[0][j] ),2 );
//			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy*sqrt( e[0][j] ),2 );
//			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z*sqrt( e[0][j] ),2 );
			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy*e[0][j],2 );
			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z*e[0][j],2 );
		} else{
			meany[lid[0][j]/2] += e[0][j]* Seg2XY( segid[0][j] );
			meanz[1][lid[0][j]/2] += e[0][j]*z[0][j];

//			meany_unc[lid[0][j]/2] += pow( intrin_unc_xy/sqrt( e[0][j] ),2 );
//			meanz_unc[1][lid[0][j]/2] += pow( intrin_unc_z/sqrt( e[0][j] ),2 );
//			meany_unc[lid[0][j]/2] += pow( intrin_unc_xy*sqrt( e[0][j] ),2 );
//			meanz_unc[1][lid[0][j]/2] += pow( intrin_unc_z*sqrt( e[0][j] ),2 );
			meany_unc[lid[0][j]/2] += pow( intrin_unc_xy*e[0][j],2 );
			meanz_unc[1][lid[0][j]/2] += pow( intrin_unc_z*e[0][j],2 );
		}
	}

	for(int j=0;j<NumberOfLayers;j++){
		meanx[j] /= edepsum[0][j];
		meany[j] /= edepsum[1][j];

		meanx_unc[j] /= pow( edepsum[0][j],2 );
		meany_unc[j] /= pow( edepsum[1][j],2 );

		meanx_unc[j] = sqrt( meanx_unc[j] );
		meany_unc[j] = sqrt( meany_unc[j] );
		for(int k=0;k<nPos;k++){
			meanz[k][j] /= edepsum[k][j];

			meanz_unc[k][j] /= pow( edepsum[k][j],2 );
			meanz_unc[k][j] = sqrt( meanz_unc[k][j] );
		}
	}

	TGraphErrors* gSMeanTrack[nPos];
	TGraphErrors* gEneStorage[nPos]; // to gTrackEne

	for(int k=0;k<nPos;k++){
		gSMeanTrack[k] = new TGraphErrors();
		gEneStorage[k] = new TGraphErrors();
		npoints[k] = 0;
		for(int j=0;j<NumberOfLayers;j++){
			if( edepsum[k][j] <= min_edep ) continue;

			if( k==0 ){
				gSMeanTrack[k]->SetPoint( npoints[k], meanz[k][j], meanx[j] );
				gSMeanTrack[k]->SetPointError( npoints[k], meanz_unc[k][j], meanx_unc[j] );
				gEneStorage[k]->SetPoint( npoints[k], meanz[k][j], edepsum[0][j] );
			} else{
				gSMeanTrack[k]->SetPoint( npoints[k], meanz[k][j], meany[j] );
				gSMeanTrack[k]->SetPointError( npoints[k], meanz_unc[k][j], meany_unc[j] );
				gEneStorage[k]->SetPoint( npoints[k], meanz[k][j], edepsum[1][j] );
			}
			npoints[k]++;
		}
	}
	
	TF1* f1[nPos];
	for(int k=0;k<nPos;k++){
		f1[k] = new TF1("f1","[0]+[1]*x",0,1000);
		gSMeanTrack[k]->Fit(f1[k],"q","",0,1000);
	}
	double final_angle = atan( sqrt( pow( f1[0]->GetParameter(1),2 ) + pow( f1[1]->GetParameter(1),2 ) ) )*180.0/TMath::Pi();
	hSlope_proc1->Fill( final_angle );

	if( fabs(final_angle-10) < 1 ){
		for(int k=0;k<2;k++){
			gTrackSample[k] = (TGraphErrors*)gSMeanTrack[k]->Clone();
			gRawTrackSample[k] = (TGraphErrors*)gSTrack[k]->Clone();
			gTrackEne[k] = (TGraphErrors*)gEneStorage[k]->Clone();
		}
	}
 }
}


void DrawQA(){
 TCanvas* c = new TCanvas("c","c",700,600);
 gStyle->SetOptStat(0);
 gPad->SetLeftMargin(0.14);
 gPad->SetBottomMargin(0.125);
 
 hEneDist_Scint->GetXaxis()->SetTitle(" energy deposit (MeV)");
 hEneDist_Scint->GetYaxis()->SetTitle(Form("Counts / %.2lf MeV",hEneDist_Scint->GetBinWidth(1)));
 hEneDist_Scint->GetXaxis()->SetRangeUser(0,10);
 hEneDist_Scint->Draw();

 c->SaveAs("hEneDist_Scint.pdf");



 hEneDist_Lead->GetXaxis()->SetTitle(" energy deposit (MeV)");
 hEneDist_Lead->GetYaxis()->SetTitle(Form("Counts / %.2lf MeV",hEneDist_Scint->GetBinWidth(1)));
 hEneDist_Lead->GetXaxis()->SetRangeUser(0,10);
 hEneDist_Lead->Draw();

 c->SaveAs("hEneDist_Lead.pdf");



 TLegend* leg_hTotalEne = new TLegend(0.2,0.55,0.6,0.85);
 leg_hTotalEne->SetLineWidth(0.0);
 leg_hTotalEne->SetFillColorAlpha(0,0);

 hTotalEne->GetXaxis()->SetTitle("Total energy deposit (MeV)");
 hTotalEne->GetYaxis()->SetTitle(Form("Counts / %.2lf MeV",hTotalEne->GetBinWidth(1)));
 hTotalEne->SetLineColor(kBlack);

 hTotalEne_Scint->SetLineColor(kRed);
 hTotalEne_Lead->SetLineColor(kBlue);

 hTotalEne->Draw();
 hTotalEne_Scint->Draw("same");
 hTotalEne_Lead->Draw("same");

 leg_hTotalEne->AddEntry( hTotalEne, "Total","l");
 leg_hTotalEne->AddEntry( hTotalEne_Scint, "Scint","l");
 leg_hTotalEne->AddEntry( hTotalEne_Lead, "Pb","l");
 leg_hTotalEne->Draw();
 
 c->SaveAs("hTotalEne.pdf"); 

 

 gRawTrackSample[0]->GetXaxis()->SetTitle("z (mm)");
 gRawTrackSample[0]->GetYaxis()->SetTitle("x (mm)");

 gRawTrackSample[0]->Draw("AP");
 gTrackSample[0]->SetLineColor(kRed);
 gTrackSample[0]->Draw("P");

 c->SaveAs("gTrackSample_x.pdf");


 gRawTrackSample[1]->GetXaxis()->SetTitle("z (mm)");
 gRawTrackSample[1]->GetYaxis()->SetTitle("y (mm)");

 gRawTrackSample[1]->Draw("AP");
 gTrackSample[1]->SetLineColor(kRed);
 gTrackSample[1]->Draw("P");

 c->SaveAs("gTrackSample_y.pdf");

 hSlope_proc1->GetXaxis()->SetTitle("Recon. Angle (deg)");
 hSlope_proc1->GetYaxis()->SetTitle(Form("Counts / %.2lf",hSlope_proc1->GetBinWidth(1)));
 hSlope_proc1->Draw();

 c->SaveAs("hSlope_proc.pdf");


 gTrackSample[1]->Draw("AP");
 gTrackSampleSmooth[1]->Draw("P");

}

void ana(){
// fin = new TFile("out_deg10.root","read");
 fin = new TFile("out.root","read");
 tin = (TTree*)fin->Get("tree");

 GetBranches();
 CreateHist();
 for(int i=0;i<tin->GetEntries();i++){
//	AnaInLoop(i);
	QAInLoop(i);
	AnaInLoop(i,1);
 }
 for(int k=0;k<nPos;k++){
	gTrackSampleSmooth[k] = (TGraphErrors*)gTrackSample[k]->Clone();
//	for(int i=0;i<nsmoo;i++){
        for(int i=0;i<1;i++){
//		gTrackSampleSmooth[k] = (TGraphErrors*)Smoothing( gTrackSampleSmooth[k] );
		gTrackSampleSmooth[k] = (TGraphErrors*)SmoothingEneWeight( gTrackSampleSmooth[k], gTrackEne[k] );
	}
 }

// AnaInLoop(0,1);

// AnaInLoop(0,1);
// hTotalTracks[0]->Draw("colz");
// hTotalTracks[1]->Draw("colz");

// hEneDist_Lead->Draw();
// hEneDist_Scint->Draw();

 DrawQA();
}
