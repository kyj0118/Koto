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
#include "TRandom3.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4ThreeVector gPrimaryParticlePosition;
G4ThreeVector gPrimaryParticleMomentumDirection;
int gPrimaryParticlePDG;
double gPrimaryParticleEnergy;
double gPrimaryParticleMass;
extern bool gUseGPS;
extern bool gGenerateStepTheta;
extern G4double gNsteps;
extern G4double gTheta_step;

extern G4double gThetaLimitMin;  
extern G4double gThetaLimitMax;
extern G4double gBeamMomentum;
extern G4String gParticle;

B5PrimaryGeneratorAction::B5PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),     
  fParticleGun(nullptr),
  fGeneralParticleSource(nullptr), 
  fParticle(nullptr),
  fMomentum(1000.*MeV)
{
  fMomentum = gBeamMomentum;
  if(gUseGPS){
    fGeneralParticleSource  = new G4GeneralParticleSource();
  }
  else {
    auto particleTable = G4ParticleTable::GetParticleTable();
    fParticle = particleTable->FindParticle(gParticle);
    fParticleGun  = new G4ParticleGun(fParticle);
    fParticleGun->SetParticleMomentum(fMomentum);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5PrimaryGeneratorAction::~B5PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGeneralParticleSource;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  if(gUseGPS){
    fGeneralParticleSource -> GeneratePrimaryVertex(event);
    gPrimaryParticlePosition = fGeneralParticleSource -> GetParticlePosition();
    gPrimaryParticleEnergy = fGeneralParticleSource -> GetParticleEnergy();
    gPrimaryParticleMomentumDirection = fGeneralParticleSource -> GetParticleMomentumDirection();
    gPrimaryParticlePDG = fGeneralParticleSource -> GetParticleDefinition() -> GetPDGEncoding();
    gPrimaryParticleMass = fGeneralParticleSource -> GetParticleDefinition() -> GetPDGMass();
  }
  else {
    fParticleGun->SetParticleDefinition(fParticle);  
    fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
    fParticleGun->SetParticleTime(0.0*ns);
    // random generation
    double dx,dy,dz;
    if (gGenerateStepTheta == true){
      G4double GenTheta = ( (int) (gNsteps * gRandom -> Uniform()) );
      GenTheta = GenTheta*gTheta_step * 3.14159265358979/180.0;
      dz = cos(GenTheta);
    }
    else{
      double theta = gRandom -> Uniform(gThetaLimitMin/180.0*3.14159265358979,gThetaLimitMax/180.0*3.14159265358979);
      dz = cos(theta);
    }
    double phi = gRandom -> Uniform(0,2*3.14159265358979); // uniform phi 
    double sin_theta= sqrt(1.0-dz*dz);
    dx = sin_theta * cos(phi);
    dy = sin_theta * sin(phi); 
    
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dx,dy,dz));
    
    gPrimaryParticlePosition = fParticleGun -> GetParticlePosition();
    gPrimaryParticleEnergy = fParticleGun -> GetParticleEnergy();
    gPrimaryParticleMomentumDirection = fParticleGun -> GetParticleMomentumDirection();
    gPrimaryParticlePDG = fParticleGun -> GetParticleDefinition() -> GetPDGEncoding();
    gPrimaryParticleMass = fParticleGun -> GetParticleDefinition() -> GetPDGMass();
    fParticleGun->GeneratePrimaryVertex(event);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
