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
/// \file exampleB5.cc
/// \brief Main program of the analysis/B5 example

#include "globals.hh"
#include "Randomize.hh"
#include "time.h"

// User Defined Detector
#include "B5DetectorConstruction.hh"
#include "B5PrimaryGeneratorAction.hh"
#include "B5DetectorConstruction.hh"
#include "B5ActionInitialization.hh"

// Geant4
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4VModularPhysicsList.hh"
#include "G4UImanager.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

// Physics list package
#include "FTFP_BERT.hh"

// Root
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TRandom3.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Global variables set by user
bool gSaveStepLevel = false;    // 
long gSeed = 0;                 // Random seed number. 0 for time seed 
bool gUseGPS = true;            //
bool gGenerateStepTheta;  //
bool gGenerateUniformPhi = true;  //
bool gGenearteUniformMomentum = false;
bool gGenearteUniformPosition = false;

G4double gBeamMomentumMax;
G4double gBeamMomentumMin;
G4double gThetaLimitMin;  
G4double gThetaLimitMax;
G4double gGeneratePhi;
G4double gBeamMomentum;
G4String gParticle;
G4ThreeVector gPrimaryPosition;
G4double gPrimaryParticlePositionXmin;
G4double gPrimaryParticlePositionXmax;
G4double gPrimaryParticlePositionYmin;
G4double gPrimaryParticlePositionYmax;

G4double gNsteps;
G4double gTheta_step;

int main(int argc,char** argv)
{
  if (argc != 1 && argc != 4){
    std::cout << "./exampleB5 [1. Macro file name] [2. Output file name] [3. Random seed number]" << std::endl;
    std::cout << "ex) ./exampleB5 run.mac example 1" << std::endl;
    return 0;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                      User Defined Parameters                                                //
  
  
  gSaveStepLevel = false;  // whether save all the step information or not
  gUseGPS = false;  // [true] : use General Particle Source described in your.mac file (/gps/position ...)   [false] : defined at bellow
  gGenerateStepTheta = false;
  gPrimaryPosition = G4ThreeVector(0,0,0); // Primary particle position  
  gBeamMomentum = 1000 * CLHEP::MeV; // Primary particle momentum
  gParticle = "gamma"; // Particle name

  int generation_mode = 0;  
  // generation mode
  // 0 : (training sample) Uniform Random theta (0 ~ 50 deg) and phi (0 ~ 2 pi) generation.
  // 1 : (test sample) Step theta genration (0 ~ 30 deg, 5 deg step), phi(0 ~ 2 pi)
  // 2 : uniform energy (0.1 ~ 2.0 GeV), uniform theta (0 ~ 50 deg), phi (0 ~ 2 pi)
  // 3 : uniform xy position, uniform theta (0 ~ 50 deg), phi (0 ~ 2 pi), uniform energy (0.1 ~ 2.0 GeV)
  // 4 : user defined ...

  switch (generation_mode){
  case 0:
    gUseGPS = false;
    gGenerateStepTheta = false;
    gGenearteUniformMomentum = false;
    gThetaLimitMin = 0;    // polar angle min [deg]
    gThetaLimitMax = 50;   // polar angle max [deg]
    gGenerateUniformPhi = true;
    break;
  case 1:
    gUseGPS = false;
    gGenerateStepTheta = true;
    gGenearteUniformMomentum = false;
    gNsteps = 7; // number of steps: Generated polar angle [0 ~ (Nstep-1)*step]
    gTheta_step = 5; // step size of theta [deg]
    gGenerateUniformPhi = true;
    break;

  case 2:
    gUseGPS = false;
    gGenearteUniformMomentum = true;
    gBeamMomentumMax = 2.0 *CLHEP::GeV;
    gBeamMomentumMin = 0.1 *CLHEP::GeV;
    gGenerateStepTheta = false;
    gThetaLimitMin = 0;    // polar angle min [deg]
    gThetaLimitMax = 50;   // polar angle max [deg]
    gGenerateUniformPhi = true;
    break;
    
  case 3:
    gUseGPS = false;
    gGenearteUniformMomentum = false;
    gGenerateStepTheta = false;
    gThetaLimitMin = 0;    // polar angle min [deg]
    gThetaLimitMax = 50;   // polar angle max [deg]
    gGenerateUniformPhi = true;
    
    gGenearteUniformPosition = true;
    gPrimaryParticlePositionXmin = -10.0*CLHEP::cm;
    gPrimaryParticlePositionXmax = 10.0*CLHEP::cm;
    gPrimaryParticlePositionYmin = -10.0*CLHEP::cm;
    gPrimaryParticlePositionYmax = 10.0*CLHEP::cm;
    break;
    
  case 4: // user defined ... 
    //gUseGPS = false;
    //gGenearteUniformMomentum = false;
    //gGenerateStepTheta = false;
    //gThetaLimitMin = 0;    // polar angle min [deg]
    //gThetaLimitMax = 50;   // polar angle max [deg]
    //gGenerateUniformPhi = true;

    //gGenearteUniformPosition = false;
    break;

    
  default:
    gUseGPS = true;
    break;
  }
  
  //                                      User Defined Parameters                                                //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  if (argc == 1) gUseGPS = true; // use vis.mac
  if (argc == 4){
    //Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
    gSeed = (long) atol(argv[3]);
    CLHEP::HepRandom::setTheSeed(gSeed);
    gRandom -> SetSeed(gSeed);
  }
  
  G4cout << "Save Step Level : " << gSaveStepLevel << G4endl;
  if (gUseGPS)
    G4cout << "Use GPS in .mac file" << G4endl;
  
  if (generation_mode == 0)
    G4cout << "Training sample generation" << G4endl;
  else
    G4cout << "Test sample generation" << G4endl;

  TString str_fname = argv[2];
  if (!str_fname.EndsWith(".root")){
    str_fname += ".root";
  }
  else if (str_fname == ""){
    str_fname = "VisMac.root";
  }

  
  TFile *tf = new TFile(str_fname,"RECREATE");
  auto tr = new TTree("tree","Geant4 output");
  tr -> SetAutoSave();
  auto physicsList = new FTFP_BERT();
  G4RunManager* runManager = new G4RunManager;
  runManager -> SetUserInitialization(physicsList);
  runManager -> SetUserInitialization(new B5ActionInitialization(tr));
  runManager -> SetUserInitialization(new B5DetectorConstruction());
  runManager -> SetUserAction(new B5PrimaryGeneratorAction());
  runManager -> Initialize();
  
  G4VisManager* visManager = new G4VisExecutive;
  visManager -> Initialize();
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  if (argc != 1) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager -> ApplyCommand(command+fileName);
  }
  else 
  {
    gUseGPS = true;
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    UImanager -> ApplyCommand("/control/execute vis.mac"); 
    ui -> SessionStart();
    
    delete ui;
  }

  tf -> cd();
  tr -> Write();
  tf -> Close();

  delete visManager;

  return 0;
}
