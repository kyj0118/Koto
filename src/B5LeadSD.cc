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
/// \file B5EmCalorimeterSD.cc
/// \brief Implementation of the B5EmCalorimeterSD class

#include "B5LeadSD.hh"
#include "B5LeadHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5LeadSD::B5LeadSD(G4String name, G4int layerNumber)
  : G4VSensitiveDetector(name), fNameSD(name), fLayerId(layerNumber), fHitsCollection(nullptr), fHCID(-1)
{
  fEdep = 0.0; fEweightedx = 0.0; fEweightedy = 0.0; fEweightedz = 0.0; fEweightedt = 0.0;
  collectionName.insert("LeadHitCollection"); 

  fStepEdep.clear();

  fPreStepx.clear();
  fPreStepy.clear();
  fPreStepz.clear();
  fPreStept.clear();
  
  fPostStepx.clear();
  fPostStepy.clear();
  fPostStepz.clear();
  fPostStept.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5LeadSD::~B5LeadSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5LeadSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection = new B5LeadHitsCollection(fNameSD,collectionName[0]);
  if (fHCID<0) {
    fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
  }
  hce->AddHitsCollection(fHCID,fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B5LeadSD::ProcessHits(G4Step*step, G4TouchableHistory*)
{
  auto edep = step->GetTotalEnergyDeposit();
  if (edep != 0.0){
    auto prepoint = step -> GetPreStepPoint();
    auto postpoint = step -> GetPostStepPoint();
    
    G4double prex = (prepoint -> GetPosition()).x();
    G4double prey = (prepoint -> GetPosition()).y();
    G4double prez = (prepoint -> GetPosition()).z();
    G4double pret = prepoint -> GetGlobalTime();
    
    G4double postx = (postpoint -> GetPosition()).x();
    G4double posty = (postpoint -> GetPosition()).y();
    G4double postz = (postpoint -> GetPosition()).z();
    G4double postt = prepoint -> GetGlobalTime();
    
    G4double x = (prex + postx)/2.0;
    G4double y = (prey + posty)/2.0;
    G4double z = (prez + postz)/2.0;
    G4double t = (pret + postt)/2.0;
    
    fEdep += edep;
    fEweightedx += x * edep;
    fEweightedy += y * edep;
    fEweightedz += z * edep;
    fEweightedt += t * edep; 
    
    fStepEdep.push_back(edep);
    
    fPreStepx.push_back(prex);
    fPreStepy.push_back(prey);
    fPreStepz.push_back(prez);
    fPreStept.push_back(pret);

    fPostStepx.push_back(postx);
    fPostStepy.push_back(posty);
    fPostStepz.push_back(postz);
    fPostStept.push_back(postt);
  }

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5LeadSD::EndOfEvent(G4HCofThisEvent* hce){
  if (fEdep == 0) return;
  else {
    fEweightedx/=fEdep;
    fEweightedy/=fEdep;
    fEweightedz/=fEdep;
    fEweightedt/=fEdep;
    
    fHitsCollection->insert(new B5LeadHit(fHCID));
    auto hit = (B5LeadHit*) ((hce -> GetHC(fHCID)) -> GetHit(0));
    hit -> SetXYZTE(fEweightedx, fEweightedy, fEweightedz, fEweightedt, fEdep);
    hit -> SetLayerID(fLayerId);

    hit -> SetPreStepPos(fPreStepx,fPreStepy,fPreStepz,fPreStept);
    hit -> SetPostStepPos(fPostStepx,fPostStepy,fPostStepz,fPostStept);
    hit -> SetStepEdep(fStepEdep);
    
    fEdep = 0.0; fEweightedx = 0.0; fEweightedy = 0.0; fEweightedz = 0.0; fEweightedt = 0.0;

    fStepEdep.clear();
    
    fPreStepx.clear();
    fPreStepy.clear();
    fPreStepz.clear();
    fPreStept.clear();
    
    fPostStepx.clear();
    fPostStepy.clear();
    fPostStepz.clear();
    fPostStept.clear();
  }
}
