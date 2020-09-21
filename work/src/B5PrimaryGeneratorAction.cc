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
/// \file B5PrimaryGeneratorAction.cc
/// \brief Implementation of the B5PrimaryGeneratorAction class

#include "B5PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5PrimaryGeneratorAction::B5PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),     
  fParticleGun(nullptr), 
  fGamma(nullptr),
  fMomentum(1000.*MeV)
{
  G4int nofParticles = 1;
  //fParticleGun  = new G4ParticleGun(nofParticles);
  fParticleGun  = new G4GeneralParticleSource();
  
  //auto particleTable = G4ParticleTable::GetParticleTable();
  //fPositron = particleTable->FindParticle("e+");
  //fMuon = particleTable->FindParticle("mu+");
  //fPion = particleTable->FindParticle("pi+");
  //fKaon = particleTable->FindParticle("kaon+");
  //fProton = particleTable->FindParticle("proton");
  //fGamma = particleTable->FindParticle("gamma");
  //fGamma = particleTable->FindParticle("mu-");
  
  // default particle kinematics
  /*
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
  fParticleGun->SetParticleDefinition(fGamma);
  fParticleGun->SetParticleTime(0.0*ns);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleMomentum(fMomentum);
  */
  
  // define commands for this class
  //DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5PrimaryGeneratorAction::~B5PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  fParticleGun->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
