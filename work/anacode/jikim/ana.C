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

const double min_edep = 0.0;

TH1D* hEneDist_Scint;
TH1D* hEneDist_Lead;

TH1D* hTotalEne;
TH1D* hTotalEne_Scint;
TH1D* hTotalEne_Lead;

TH1D* hTimeDist;
TH1D* hTimeDist_Scint;
TH1D* hTimeDist_Lead;

TH2D* hTimeDist_Scint_z;

TH1D* hTimeDist_ScintPerLayer[NumberOfLayers];


TH1D* hTimeDiff_Scint;
TH2D* hTimeDiffPerLayer_Scint;

TH1D* hTimeRmsInLayer;


TH2D* hTotalTracks[nPos];
TH2D* hTotalMeanTracks[nPos];

TH2D* hZX;

TH1D* hSlope_proc1;
TH1D* hSlope_proc_smooth;

TH1D* hSlope_zt;

TGraphErrors* gRawTrackSample[nPos];
TGraphErrors* gTrackSample[nPos];
TGraphErrors* gTrackEne[nPos]; //storage for energy weight
TGraphErrors* gTrackTime[nPos];

TGraphErrors* gTrackTimeXY[nPos];

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
	hTotalMeanTracks[i] = new TH2D(Form("hTotalMeanTracks%d",i),Form("hTotalMeanTracks%d",i),65,-60,720,30,-300,300);
 }
 hSlope_proc1 = new TH1D("hSlope_proc1","hSlope_proc1",1000,-5,45);
 hSlope_proc_smooth = new TH1D("hSlope_proc_smooth","hSlope_proc_smooth",200,-5,45);

 hEneDist_Scint = new TH1D("hEneDist_Scint","hEneDist_Scint",2000,0,100);
 hEneDist_Lead = new TH1D("hEneDist_Lead","hEneDist_Lead",2000,0,100);

 hTotalEne = new TH1D("hTotalEne","hTotalEne",220,-50,1050);
 hTotalEne_Scint = new TH1D("hTotalEne_Scint","hTotalEne_Scint",220,-50,1050);
 hTotalEne_Lead = new TH1D("hTotalEne_Lead","hTotalEne_Lead",220,-50,1050);

 hTimeDist_Scint = new TH1D("hTimeDist_Scint","hTimeDist_Scint",200,0,5);
 hTimeDist_Lead = new TH1D("hTimeDist_Lead","hTimeDist_Lead",200,0,5);
 for(int i=0;i<NumberOfLayers;i++) hTimeDist_ScintPerLayer[i] = new TH1D(Form("hTimeDist_ScintPerLayer_%d",i),Form("hTimeDist_ScintPerLayer_%d",i),200,0,3);



 hTimeDist_Scint_z = new TH2D("hTimeDist_Scint_z","hTimeDist_Scint_z",200,0,3,200,-20,680);
 hSlope_zt = new TH1D("hSlope_zt","hSlope_zt",200,0,500);


 hTimeDiff_Scint = new TH1D("hTimeDiff_Scint","hTimeDiff_Scint",200,-2,2);
 hTimeDiffPerLayer_Scint = new TH2D("hTimeDiffPerLayer_Scint","hTimeDiffPerLayer_Scint",200,-2,2,200,-500,500);

 hTimeRmsInLayer = new TH1D("hTimeRmsInLayer","hTimeRmsInLayer",200,0,1);

 hZX = new TH2D("hZX","hZX",200,0,650,200,-300,300);
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

	hTimeDist_Scint->Fill( t[0][j] );
	hTimeDist_Scint_z->Fill( t[0][j], z[0][j] );
	hTimeDist_ScintPerLayer[ lid[0][j] ]->Fill( t[0][j] );
	if( j > 0 ){
		hTimeDiff_Scint->Fill( t[0][j+1]-t[0][j] );
//		hTimeDiffPerLayer_Scint->Fill( t[0][j+1]-t[0][j], lid[0][j+1]-lid[0][j] );
		hTimeDiffPerLayer_Scint->Fill( t[0][j+1]-t[0][j], 
			( z[0][j+1]-z[0][j]>0 ? 1.0 : -1.0 ) * sqrt( pow( x[0][j+1]-x[0][j],2) + pow( y[0][j+1]-y[0][j],2) + pow( z[0][j+1]-z[0][j],2) ) );
	}
 }
 for(int j=0;j<nHits[1];j++){ 
	hEneDist_Lead->Fill( e[1][j] );
	ene_total_lead += e[1][j];

	hTimeDist_Lead->Fill( t[1][j] );
 }
 hTotalEne_Scint->Fill( ene_total_scint );
 hTotalEne_Lead->Fill( ene_total_lead );
 hTotalEne->Fill( ene_total_scint+ene_total_lead );
}


void AnaInLoop(int i, int proc=0, double edep_det=0.0, double fit_range=800.0){
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
	double meant[nPos][NumberOfLayers] = {0};
	double meant_rms[nPos][NumberOfLayers] = {0};

        double meanx_unc[NumberOfLayers] = {0};
        double meany_unc[NumberOfLayers] = {0};
        double meanz_unc[nPos][NumberOfLayers] = {0};

	for(int j=0;j<nHits[0];j++){
		if( e[0][j] < edep_det ) continue;

		gSTrack[ lid[0][j]%2 ]->SetPoint( npoints[lid[0][j]%2], Layer2Z( lid[0][j] ), Seg2XY( segid[0][j] ) );
//		gSTrack[ lid[0][j]%2 ]->SetPointError( npoints[lid[0][j]%2], intrin_unc_z/sqrt( e[0][j] ), intrin_unc_xy/sqrt( e[0][j] ) );
		gSTrack[ lid[0][j]%2 ]->SetPointError( npoints[lid[0][j]%2], intrin_unc_z, intrin_unc_xy );
		npoints[lid[0][j]%2]++;

		edepsum[lid[0][j]%2][lid[0][j]/2] += e[0][j];
		if( lid[0][j]%2== 0 ){
			meanx[lid[0][j]/2] += e[0][j]* Seg2XY( segid[0][j] );
			meanz[0][lid[0][j]/2] += e[0][j]*z[0][j];

			meant[0][lid[0][j]/2] += e[0][j]*t[0][j];
			meant_rms[0][lid[0][j]/2] += pow( t[0][j],2 )*e[0][j];
//			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy/sqrt( e[0][j] ),2 );
//			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z/sqrt( e[0][j] ),2 );
//			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy*sqrt( e[0][j] ),2 );
//			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z*sqrt( e[0][j] ),2 );
			meanx_unc[lid[0][j]/2] += pow( intrin_unc_xy*e[0][j],2 );
			meanz_unc[0][lid[0][j]/2] += pow( intrin_unc_z*e[0][j],2 );
		} else{
			meany[lid[0][j]/2] += e[0][j]* Seg2XY( segid[0][j] );
			meanz[1][lid[0][j]/2] += e[0][j]*z[0][j];

			meant[1][lid[0][j]/2] += e[0][j]*t[0][j];
			meant_rms[1][lid[0][j]/2] += pow( t[0][j],2 )*e[0][j];
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
			meant[k][j] /= edepsum[k][j];

			meanz_unc[k][j] /= pow( edepsum[k][j],2 );
			meanz_unc[k][j] = sqrt( meanz_unc[k][j] );

			meant_rms[k][j] /= edepsum[k][j];
			meant_rms[k][j] -= pow( meant[k][j],2 );
			meant_rms[k][j] = sqrt( meant_rms[k][j] );

		}
	}

	TGraphErrors* gSMeanTrack[nPos];
	TGraphErrors* gEneStorage[nPos]; // to gTrackEne
	TGraphErrors* gMeanTime[nPos];
	TGraphErrors* gMeanTimeXY[nPos];

	for(int k=0;k<nPos;k++){
		gSMeanTrack[k] = new TGraphErrors();
		gEneStorage[k] = new TGraphErrors();
		gMeanTime[k] = new TGraphErrors();
		gMeanTimeXY[k] = new TGraphErrors();
		npoints[k] = 0;
		for(int j=0;j<NumberOfLayers;j++){
			if( edepsum[k][j] <= min_edep ) continue;
			hTimeRmsInLayer->Fill( meant_rms[k][j] );

//			if( meant_rms[k][j] > 0.01 ) continue;
			if( k==0 ){
				gSMeanTrack[k]->SetPoint( npoints[k], meanz[k][j], meanx[j] );
				gSMeanTrack[k]->SetPointError( npoints[k], meanz_unc[k][j], meanx_unc[j] );
				gEneStorage[k]->SetPoint( npoints[k], meanz[k][j], edepsum[0][j] );

				gMeanTime[k]->SetPoint( npoints[k], meanz[k][j], meant[k][j] );
				gMeanTime[k]->SetPointError( npoints[k], 0, meant_rms[k][j] );

				gMeanTimeXY[k]->SetPoint( npoints[k], meanx[j], meant[k][j] );
				gMeanTimeXY[k]->SetPointError( npoints[k], 0, meant_rms[k][j] );

				hTotalMeanTracks[k]->Fill( meanz[k][j], meanx[j] );
			} else{
				gSMeanTrack[k]->SetPoint( npoints[k], meanz[k][j], meany[j] );
				gSMeanTrack[k]->SetPointError( npoints[k], meanz_unc[k][j], meany_unc[j] );
				gEneStorage[k]->SetPoint( npoints[k], meanz[k][j], edepsum[1][j] );

				gMeanTime[k]->SetPoint( npoints[k], meanz[k][j], meant[k][j] );
				gMeanTime[k]->SetPointError( npoints[k], 0, meant_rms[k][j] );

				gMeanTimeXY[k]->SetPoint( npoints[k], meany[j], meant[k][j] );
				gMeanTimeXY[k]->SetPointError( npoints[k], 0, meant_rms[k][j] );

				hTotalMeanTracks[k]->Fill( meanz[k][j], meany[j] );
			}
			npoints[k]++;
		}
	}
	
	TF1* f1[nPos];
	for(int k=0;k<nPos;k++){
		f1[k] = new TF1("f1","[0]+[1]*x",0,1000);
//		gSMeanTrack[k]->Fit(f1[k],"q","",0,1000);
		gSMeanTrack[k]->Fit(f1[k],"q","",gSMeanTrack[k]->GetX()[0],gSMeanTrack[k]->GetX()[0]+fit_range);
	}
	double final_angle = atan( sqrt( pow( f1[0]->GetParameter(1),2 ) + pow( f1[1]->GetParameter(1),2 ) ) )*180.0/TMath::Pi();
	if( f1[0]->GetNDF()>3 && f1[1]->GetNDF()>3 ) hSlope_proc1->Fill( final_angle );

//	if( fabs(final_angle-10) < 1 ){
		for(int k=0;k<nPos;k++){
			gTrackSample[k] = (TGraphErrors*)gSMeanTrack[k]->Clone();
			gRawTrackSample[k] = (TGraphErrors*)gSTrack[k]->Clone();
			gTrackEne[k] = (TGraphErrors*)gEneStorage[k]->Clone();
			gTrackTime[k] = (TGraphErrors*)gMeanTime[k]->Clone();
			gTrackTimeXY[k] = (TGraphErrors*)gMeanTimeXY[k]->Clone();
		}
//	}

	for(int k=0;k<nPos;k++){
		gMeanTime[k]->Fit(f1[k],"q");
	}

	hSlope_zt->Fill( 1.0/sqrt( f1[0]->GetParameter(1)*f1[1]->GetParameter(1) ) );

	TGraphErrors* gSMeanTrack_smooth[nPos];
	for(int k=0;k<nPos;k++){
		gSMeanTrack_smooth[k] = (TGraphErrors*)gSMeanTrack[k]->Clone();
		for(int i=0;i<1;i++){
			gSMeanTrack_smooth[k] = (TGraphErrors*)SmoothingEneWeight( gSMeanTrack_smooth[k], gEneStorage[k] );
		}
//		gSMeanTrack_smooth[k]->Fit(f1[k],"q","",0,300);
		gSMeanTrack_smooth[k]->Fit(f1[k],"q","",gSMeanTrack_smooth[k]->GetX()[0],gSMeanTrack_smooth[k]->GetX()[0]+fit_range);
	}
	double final_angle_smooth = atan( sqrt( pow( f1[0]->GetParameter(1),2 ) + pow( f1[1]->GetParameter(1),2 ) ) )*180.0/TMath::Pi();
	hSlope_proc_smooth->Fill(final_angle_smooth);

//	if( fabs(final_angle-10) < 1 ){
		for(int k=0;k<nPos;k++){
			gTrackSampleSmooth[k] = (TGraphErrors*)gSMeanTrack_smooth[k]->Clone();
		}
//	}
 }
}


void DrawQA(char* fname){
 TCanvas* c = new TCanvas("c","c",700,600);
// gStyle->SetOptStat(0);
 gPad->SetLeftMargin(0.14);
 gPad->SetBottomMargin(0.125);
 
 hEneDist_Scint->GetXaxis()->SetTitle(" energy deposit (MeV)");
 hEneDist_Scint->GetYaxis()->SetTitle(Form("Counts / %.2lf MeV",hEneDist_Scint->GetBinWidth(1)));
 hEneDist_Scint->GetXaxis()->SetRangeUser(0,10);
 hEneDist_Scint->Draw();

 c->SaveAs(Form("%s/hEneDist_Scint.pdf",fname));



 hEneDist_Lead->GetXaxis()->SetTitle(" energy deposit (MeV)");
 hEneDist_Lead->GetYaxis()->SetTitle(Form("Counts / %.2lf MeV",hEneDist_Scint->GetBinWidth(1)));
 hEneDist_Lead->GetXaxis()->SetRangeUser(0,10);
 hEneDist_Lead->Draw();

 c->SaveAs(Form("%s/hEneDist_Lead.pdf",fname));



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
 
 c->SaveAs(Form("%s/hTotalEne.pdf",fname)); 

 

 gRawTrackSample[0]->GetXaxis()->SetTitle("z (mm)");
 gRawTrackSample[0]->GetYaxis()->SetTitle("x (mm)");

 gRawTrackSample[0]->Draw("AP");
 gTrackSample[0]->SetLineColor(kRed);
 gTrackSample[0]->Draw("P");

 c->SaveAs(Form("%s/gTrackSample_x.pdf",fname));


 gRawTrackSample[1]->GetXaxis()->SetTitle("z (mm)");
 gRawTrackSample[1]->GetYaxis()->SetTitle("y (mm)");

 gRawTrackSample[1]->Draw("AP");
 gTrackSample[1]->SetLineColor(kRed);
 gTrackSample[1]->Draw("P");

 c->SaveAs(Form("%s/gTrackSample_y.pdf",fname));

 hSlope_proc1->GetXaxis()->SetTitle("Recon. Angle (deg)");
 hSlope_proc1->GetYaxis()->SetTitle(Form("Counts / %.2lf",hSlope_proc1->GetBinWidth(1)));
 hSlope_proc1->Draw();

 c->SaveAs(Form("%s/hSlope_proc.pdf",fname));


 gTrackSample[1]->Draw("AP");
 gTrackSampleSmooth[1]->Draw("P");
 c->SaveAs(Form("%s/hSlope_proc_smooth_y.pdf",fname));

 gTrackSample[0]->Draw("AP");
 gTrackSampleSmooth[0]->Draw("P");
 c->SaveAs(Form("%s/hSlope_proc_smooth_x.pdf",fname));


 hTimeDist_Scint->GetXaxis()->SetTitle(" t (ns) ");
 hTimeDist_Scint->GetYaxis()->SetTitle(Form("Counts / %.3lf",hTimeDist_Scint->GetBinWidth(1)));
 hTimeDist_Scint->SetLineColor(kBlack);
 hTimeDist_Scint->Draw();
 c->SaveAs(Form("%s/time1d.pdf",fname));

 gPad->SetLogz();
 hTimeDist_Scint_z->GetXaxis()->SetTitle(" t (ns) ");
 hTimeDist_Scint_z->GetYaxis()->SetTitle(" z (mm) ");
 hTimeDist_Scint_z->Draw("colz");
 c->SaveAs(Form("%s/time2d.pdf",fname));
 gPad->SetLogz(0);

 hTimeDist_ScintPerLayer[0]->GetXaxis()->SetTitle("t (ns)");
 hTimeDist_ScintPerLayer[0]->GetYaxis()->SetTitle(Form("Counts / %.3lf",hTimeDist_ScintPerLayer[1]->GetBinWidth(1)));
 hTimeDist_ScintPerLayer[0]->SetMaximum( 8e3 );
 hTimeDist_ScintPerLayer[0]->SetLineColor(kBlack);
 hTimeDist_ScintPerLayer[11]->SetLineColor(kRed);
 hTimeDist_ScintPerLayer[21]->SetLineColor(kBlue);
 hTimeDist_ScintPerLayer[51]->SetLineColor(kGreen);
 hTimeDist_ScintPerLayer[101]->SetLineColor(kMagenta);
 
 gPad->SetLogy();

 hTimeDist_ScintPerLayer[0]->Draw();
 hTimeDist_ScintPerLayer[11]->Draw("same");
 hTimeDist_ScintPerLayer[21]->Draw("same");
 hTimeDist_ScintPerLayer[51]->Draw("same");
 hTimeDist_ScintPerLayer[101]->Draw("same");

 TLegend* leg_time1d_dif = new TLegend(0.5,0.7,0.85,0.85);
 leg_time1d_dif->SetLineWidth(0.0);
 leg_time1d_dif->SetFillColorAlpha(0,0);
 leg_time1d_dif->AddEntry(  hTimeDist_ScintPerLayer[0],  "Layer number = 0", "l");
 leg_time1d_dif->AddEntry(  hTimeDist_ScintPerLayer[11], "Layer number = 11", "l");
 leg_time1d_dif->AddEntry(  hTimeDist_ScintPerLayer[21], "Layer number = 21", "l");
 leg_time1d_dif->AddEntry(  hTimeDist_ScintPerLayer[51], "Layer number = 51", "l");
 leg_time1d_dif->AddEntry(  hTimeDist_ScintPerLayer[101],"Layer number = 101", "l");
 leg_time1d_dif->Draw();


 c->SaveAs(Form("%s/time1d_dif.pdf",fname));





 TCanvas* c2 = new TCanvas("c2","c2",800,600);

 gTrackTime[0]->GetYaxis()->SetTitle(" t (ns)");
 gTrackTime[0]->GetXaxis()->SetTitle(" z (mm)");

 gTrackTime[0]->SetMarkerStyle(20);
 gTrackTime[0]->SetMinimum(0);
 gTrackTime[0]->SetMaximum(2.5);
 gTrackTime[0]->Draw("AP");
 gTrackTime[1]->SetMarkerColor(kRed);
 gTrackTime[1]->SetLineColor(kRed);
 gTrackTime[1]->SetMarkerStyle(23);
 gTrackTime[1]->Draw("P");

 TLegend* LegTrackTime = new TLegend(0.2,0.6,0.4,0.8);
 LegTrackTime->SetLineWidth(0.0);
 LegTrackTime->SetFillColorAlpha(0,0);

 LegTrackTime->AddEntry( gTrackTime[0], "x layers", "p");
 LegTrackTime->AddEntry( gTrackTime[1], "y layers", "p");
 LegTrackTime->Draw();


 c2->SaveAs(Form("%s/t_vs_xy.pdf",fname));


 TCanvas* c3 = new TCanvas("c3","c3",800,600);
 c3->Divide(2,1);

 gTrackTimeXY[0]->SetMarkerStyle(20);
 gTrackTimeXY[1]->SetMarkerStyle(20);

 gTrackTimeXY[0]->GetXaxis()->SetTitle(" x (mm)");
 gTrackTimeXY[0]->GetYaxis()->SetTitle(" t (ns)");
 gTrackTimeXY[1]->GetXaxis()->SetTitle(" y (mm)");
 gTrackTimeXY[1]->GetYaxis()->SetTitle(" t (ns)");

 c3->cd(1); gTrackTimeXY[0]->Draw("AP");
 c3->cd(2); gTrackTimeXY[1]->Draw("AP");
 c3->SaveAs(Form("%s/t_vs_xyprop.pdf",fname));

 TCanvas* c4 = new TCanvas("c4","c4",800,600);
 c4->Divide(2,1);
 gTrackEne[0]->SetMarkerStyle(20);
 gTrackEne[1]->SetMarkerStyle(20);
 c4->cd(1); gTrackEne[0]->Draw("AP");
 c4->cd(2); gTrackEne[1]->Draw("AP");


}

void anaFile( char* fname ){
 fin = new TFile(Form("%s.root",fname),"read");
 tin = (TTree*)fin->Get("tree");

 GetBranches();
 CreateHist();
/*
 for(int i=0;i<tin->GetEntries();i++){
	QAInLoop(i);
	AnaInLoop(i,1);
 }
// DrawQA(fname);
*/
 const int nedep_sel = 4;
 const int nfit_range = 4;

 double edep_sel[nedep_sel] = {
	0.0, 0.5, 1.0, 3.0 };

 double fitrange[nfit_range] = {
	600, 300, 150, 80 };

 TH1D* hAngle[nfit_range][nedep_sel];
 TH1D* hSmoothedAngle[nfit_range][nedep_sel];

 TGraphErrors* gSingleTrack[nfit_range][nedep_sel][nPos];
 TGraphErrors* gSmoothedSingleTrack[nfit_range][nedep_sel][nPos];


 for(int r=0;r<nfit_range;r++){
// for(int r=2;r<3;r++){
	for(int e=0;e<nedep_sel;e++){
		cout << r << ", " << e << endl;
		for(int i=0;i<tin->GetEntries();i++){
			AnaInLoop(i,1,edep_sel[e],fitrange[r]);
		}
		hAngle[r][e] = (TH1D*)hSlope_proc1->Clone();		
		hSmoothedAngle[r][e] = (TH1D*)hSlope_proc_smooth->Clone();

		hSlope_proc1->Reset();
		hSlope_proc_smooth->Reset();
	}
 }

 for(int r=0;r<nfit_range;r++){
	for(int e=0;e<nedep_sel;e++){
		for(int k=0;k<nPos;k++){
			AnaInLoop(10,1,edep_sel[e],fitrange[r]);

			gSingleTrack[r][e][k] = (TGraphErrors*)gTrackSample[k]->Clone();
			gSmoothedSingleTrack[r][e][k] = (TGraphErrors*)gTrackSampleSmooth[k]->Clone();
		}
	}
 }

 TFile* fout = new TFile("topol_out.root","recreate");

 for(int r=0;r<nfit_range;r++){
        for(int e=0;e<nedep_sel;e++){
		hAngle[r][e]->SetName(Form("hAngle_%d_%d",r,e));
		hSmoothedAngle[r][e]->SetName(Form("hSmoothedAngle_%d_%d",r,e));

		hAngle[r][e]->Write();
		hSmoothedAngle[r][e]->Write();
		for(int k=0;k<nPos;k++){
			gSingleTrack[r][e][k]->SetName(Form("gSingleTrack_%d_%d_%d",r,e,k));
			gSmoothedSingleTrack[r][e][k]->SetName(Form("gSmoothedSingleTrack_%d_%d_%d",r,e,k));

			gSingleTrack[r][e][k]->Write();
			gSmoothedSingleTrack[r][e][k]->Write();
		}
	}
 }

}


void ana(){

 const int nfiles = 2;
 char file_name[nfiles][1000] = {
	"out",
	"out_deg10" };

 TH1D* hSlope_zt_files[nfiles];

 for(int i=0;i<1;i++){
	anaFile( file_name[i] );
 }


}
