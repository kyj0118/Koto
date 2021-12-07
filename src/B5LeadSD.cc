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
extern bool gSaveStepLevel;
extern G4int gNLayers;
B5LeadSD::B5LeadSD(G4String name)
  : G4VSensitiveDetector(name), fNameSD(name), fHitsCollection(nullptr), fHCID(-1)
{

  fEdep.clear();
  fEweightedx.clear();
  fEweightedy.clear();
  fEweightedz.clear();
  fEweightedt.clear();
  
  fEdep.resize(gNLayers,0);
  fEweightedx.resize(gNLayers,0);
  fEweightedy.resize(gNLayers,0);
  fEweightedz.resize(gNLayers,0);
  fEweightedt.resize(gNLayers,0);
  
  collectionName.insert("LeadHitCollection"); 

  if (gSaveStepLevel == true){
    fStepEdep.clear();
  
    fPreStepx.clear();
    fPreStepy.clear();
    fPreStepz.clear();
    fPreStept.clear();
  
    fPostStepx.clear();
    fPostStepy.clear();
    fPostStepz.clear();
    fPostStept.clear();

    fParticlePx.clear();
    fParticlePy.clear();
    fParticlePz.clear();
    fParticleTrackID.clear();
    fParticleParentID.clear();
    fParticleCharge.clear();
    fParticleMass.clear();
    fParticlePDGID.clear();

    fStepEdep.resize(gNLayers);
  
    fPreStepx.resize(gNLayers);
    fPreStepy.resize(gNLayers);
    fPreStepz.resize(gNLayers);
    fPreStept.resize(gNLayers);
  
    fPostStepx.resize(gNLayers);
    fPostStepy.resize(gNLayers);
    fPostStepz.resize(gNLayers);
    fPostStept.resize(gNLayers);

    fParticlePx.resize(gNLayers);
    fParticlePy.resize(gNLayers);
    fParticlePz.resize(gNLayers);
    fParticleTrackID.resize(gNLayers);
    fParticleParentID.resize(gNLayers);
    fParticleCharge.resize(gNLayers);
    fParticleMass.resize(gNLayers);
    fParticlePDGID.resize(gNLayers);
  }
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
  if (edep != 0){
    auto prepoint = step -> GetPreStepPoint();
    auto postpoint = step -> GetPostStepPoint();
    auto physVolume = prepoint -> GetPhysicalVolume();
    G4int cpid = physVolume -> GetCopyNo();
    
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
    
    fEdep[cpid] += edep;
    fEweightedx[cpid] += x * edep;
    fEweightedy[cpid] += y * edep;
    fEweightedz[cpid] += z * edep;
    fEweightedt[cpid] += t * edep; 
    
    if(gSaveStepLevel == true){
      fStepEdep[cpid].push_back(edep);
    
      fPreStepx[cpid].push_back(prex);
      fPreStepy[cpid].push_back(prey);
      fPreStepz[cpid].push_back(prez);
      fPreStept[cpid].push_back(pret);

      fPostStepx[cpid].push_back(postx);
      fPostStepy[cpid].push_back(posty);
      fPostStepz[cpid].push_back(postz);
      fPostStept[cpid].push_back(postt);

      // particle info
      G4Track *tr = step -> GetTrack();
      const G4ParticleDefinition *pdef = tr -> GetParticleDefinition();
      G4ThreeVector pp = tr -> GetMomentum();
    
      G4double ppx = pp.x();
      G4double ppy = pp.y();
      G4double ppz = pp.z();
      G4int trackid = tr -> GetTrackID();
      G4int parentid = tr -> GetParentID();
      G4double pcharge = pdef -> GetPDGCharge();
      G4double pmass = pdef -> GetPDGMass();
      G4int pid = pdef -> GetPDGEncoding();
    
      fParticlePx[cpid].push_back(ppx);
      fParticlePy[cpid].push_back(ppy);
      fParticlePz[cpid].push_back(ppz);
      fParticleTrackID[cpid].push_back(trackid);
      fParticleParentID[cpid].push_back(parentid);
      fParticleCharge[cpid].push_back(pcharge);
      fParticleMass[cpid].push_back(pmass);
      fParticlePDGID[cpid].push_back(pid);
    }
  }
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5LeadSD::EndOfEvent(G4HCofThisEvent* hce){
  for (int i = 0; i < gNLayers; i++){
    if (fEdep[i] == 0) continue;
    
    fEweightedx[i]/=fEdep[i];
    fEweightedy[i]/=fEdep[i];
    fEweightedz[i]/=fEdep[i];
    fEweightedt[i]/=fEdep[i];
    
    fHitsCollection->insert(new B5LeadHit(fHCID));
    G4int CurrentHitID = fHitsCollection->GetSize()-1;
    auto hit = (B5LeadHit*) ((hce -> GetHC(fHCID)) -> GetHit(CurrentHitID));
    G4int layerID = i;
    hit -> SetXYZTE(fEweightedx[i], fEweightedy[i], fEweightedz[i], fEweightedt[i], fEdep[i]);
    hit -> SetLayerID(layerID);
    hit -> SetCellID(i);
    
    if(gSaveStepLevel == true){
      hit -> SetPreStepPos(fPreStepx[i],fPreStepy[i],fPreStepz[i],fPreStept[i]);
      hit -> SetPostStepPos(fPostStepx[i],fPostStepy[i],fPostStepz[i],fPostStept[i]);
      hit -> SetStepEdep(fStepEdep[i]);
      hit -> SetParticleTrackInfo(fParticlePx[i],fParticlePy[i],fParticlePz[i],fParticleTrackID[i],fParticleParentID[i],
				  fParticleCharge[i],fParticleMass[i],fParticlePDGID[i]);
      
      fStepEdep[i].clear();
      
      fPreStepx[i].clear();
      fPreStepy[i].clear();
      fPreStepz[i].clear();
      fPreStept[i].clear();
      
      fPostStepx[i].clear();
      fPostStepy[i].clear();
      fPostStepz[i].clear();
      fPostStept[i].clear();
      
      fParticlePx[i].clear();
      fParticlePy[i].clear();
      fParticlePz[i].clear();
      fParticleTrackID[i].clear();
      fParticleParentID[i].clear();
      fParticleCharge[i].clear();
      fParticleMass[i].clear();
      fParticlePDGID[i].clear();
    }
    fEdep[i] = 0.0; fEweightedx[i] = 0.0; fEweightedy[i] = 0.0; fEweightedz[i] = 0.0; fEweightedt[i] = 0.0;
  }
}
