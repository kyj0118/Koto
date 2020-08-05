//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B5EventAction.cc
/// \brief Implementation of the B5EventAction class

#include "B5EventAction.hh"
#include "B5RunAction.hh"
#include "B5EmCalorimeterHit.hh"
#include "B5LeadHit.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "TClonesArray.h"
#include "TTree.h"
#include "TObject.h"
#include <Riostream.h>

//using std::array;
//using std::vector;
using namespace std;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5EventAction::B5EventAction(B5RunAction *runAction, TTree *tr)
  : G4UserEventAction(), fRunAction(runAction), fTree(tr)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5EventAction::~B5EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5EventAction::BeginOfEventAction(const G4Event*)
{
  SetRunID(fRunAction -> GetRunID());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5EventAction::EndOfEventAction(const G4Event* event)
{
  
  EventInfo.eventID = event -> GetEventID();
  
  auto hce = event -> GetHCofThisEvent();

  int iarrayEMHit = 0;
  int iarrayLeadHit = 0;
  
  for (int i = 0; i < hce -> GetCapacity(); i++){
    if (hce -> GetHC(i) -> GetSize() == 0) continue;
    G4String iHCName = hce -> GetHC(i) -> GetName();
    // EM Hit
    if (iHCName == "EMCalHitCollection"){
      auto hit = (B5EmCalorimeterHit*) (hce -> GetHC(i) -> GetHit(0));
      EMHit.cid[iarrayEMHit] = hit -> GetCellID();
      EMHit.lid[iarrayEMHit] = hit -> GetLayerID();
      EMHit.segid[iarrayEMHit] = hit -> GetSegmentID();
      double xx,yy,zz,tt,ee;
      hit -> GetXYZTE(xx,yy,zz,tt,ee);
      EMHit.one[iarrayEMHit] = 1;
      EMHit.x[iarrayEMHit] = xx;
      EMHit.y[iarrayEMHit] = yy;
      EMHit.z[iarrayEMHit] = zz;
      EMHit.t[iarrayEMHit] = tt;
      EMHit.e[iarrayEMHit] = ee;	
      iarrayEMHit++;    
    }

    // Lead Hit
    else if (iHCName == "LeadHitCollection"){
      auto hit = (B5LeadHit*) (hce -> GetHC(i) -> GetHit(0));
      LeadHit.cid[iarrayLeadHit] = hit -> GetCellID();
      LeadHit.lid[iarrayLeadHit] = hit -> GetLayerID();
      double xx,yy,zz,tt,ee;
      hit -> GetXYZTE(xx,yy,zz,tt,ee);
      LeadHit.one[iarrayLeadHit] = 1;
      LeadHit.x[iarrayLeadHit] = xx;
      LeadHit.y[iarrayLeadHit] = yy;
      LeadHit.z[iarrayLeadHit] = zz;
      LeadHit.t[iarrayLeadHit] = tt;
      LeadHit.e[iarrayLeadHit] = ee;	
      iarrayLeadHit++;
    }

  }
  
  EMHit.nhit = iarrayEMHit;
  LeadHit.nhit = iarrayLeadHit;
  fTree -> Fill();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5EventAction::SetBranch(){
  fTree -> Branch("eventID",&EventInfo.eventID,"eventID/I");
  fTree -> Branch("runID",&EventInfo.runID,"runID/I");
  fTree -> Branch("randomSeed",&EventInfo.randomSeed,"randomSeed/L");
  
  fTree -> Branch("nEMHit",&EMHit.nhit,"nEMHit/I");
  fTree -> Branch("EMHit.one",EMHit.one,"EMHit.one[nEMHit]/I");
  fTree -> Branch("EMHit.CellID",EMHit.cid,"EMHit.CellID[nEMHit]/I");
  fTree -> Branch("EMHit.LayerID",EMHit.lid,"EMHit.LayerID[nEMHit]/I");
  fTree -> Branch("EMHit.SegmentID",EMHit.segid,"EMHit.SegmentID[nEMHit]/I");
  fTree -> Branch("EMHit.x",EMHit.x,"EMHit.x[nEMHit]/D");
  fTree -> Branch("EMHit.y",EMHit.y,"EMHit.y[nEMHit]/D");
  fTree -> Branch("EMHit.z",EMHit.z,"EMHit.z[nEMHit]/D");
  fTree -> Branch("EMHit.t",EMHit.t,"EMHit.t[nEMHit]/D");
  fTree -> Branch("EMHit.e",EMHit.e,"EMHit.e[nEMHit]/D");
  
  fTree -> Branch("nLeadHit",&LeadHit.nhit,"nLeadHit/I");
  fTree -> Branch("LeadHit.one",LeadHit.one,"LeadHit.one[nLeadHit]/I");
  fTree -> Branch("LeadHit.CellID",LeadHit.cid,"LeadHit.CellID[nLeadHit]/I");
  fTree -> Branch("LeadHit.LayerID",LeadHit.lid,"LeadHit.LayerID[nLeadHit]/I");
  fTree -> Branch("LeadHit.x",LeadHit.x,"LeadHit.x[nLeadHit]/D");
  fTree -> Branch("LeadHit.y",LeadHit.y,"LeadHit.y[nLeadHit]/D");
  fTree -> Branch("LeadHit.z",LeadHit.z,"LeadHit.z[nLeadHit]/D");
  fTree -> Branch("LeadHit.t",LeadHit.t,"LeadHit.t[nLeadHit]/D");
  fTree -> Branch("LeadHit.e",LeadHit.e,"LeadHit.e[nLeadHit]/D");

}
void B5EventAction::SetRunID(G4int RunID){
  EventInfo.runID = RunID;
}
void B5EventAction::SetRandomSeed(long seed){
  EventInfo.randomSeed = seed;
}
