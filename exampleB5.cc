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


// if gUseGPS = false, user defined beam conditions
G4double gThetaLimitMin;  
G4double gThetaLimitMax;
G4double gBeamMomentum;
G4String gParticle;

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
  gUseGPS = false;  // [true] : use General Particle Source described in your.mac file (/gps/position ...)   [false] : Random angle generation
  gGenerateStepTheta = false;
  if (gUseGPS == false){ // Random Beam Generation Setting
    
    if (gGenerateStepTheta == true){
      // special generation: 5 deg step (0 ~ 30 deg) in polar angle theta
      gNsteps = 7; // number of steps: Generated angle [0 ~ (Nstep-1)*step]
      gTheta_step = 5; // step size of theta [deg]
    }
    
    if (gGenerateStepTheta == false){
      //                     // Uniform azimuthal angle [0 ~ 2pi]    
      gThetaLimitMin = 0;    // polar angle min [deg]
      gThetaLimitMax = 50;   // polar angle max [deg]
    }
    
    gBeamMomentum = 1000 * CLHEP::MeV; // Primary particle momentum
    gParticle = "gamma";
  }

  if (argc == 1) gUseGPS = true; // use vis.mac
  
  //                                      User Defined Parameters                                                //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
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
  else 
    G4cout << "Random Theta Generation" << G4endl;
  
  TString str_fname = argv[2];
  if (!str_fname.EndsWith(".root")){
    str_fname += ".root";
  }
  else if (str_fname == ""){
    str_fname = "VisMac.root";
  }

  auto tr = new TTree("tree","Geant4 output");
  
  auto physicsList = new FTFP_BERT;
  G4RunManager* runManager = new G4RunManager;
  runManager -> SetUserInitialization(physicsList);
  runManager -> SetUserInitialization(new B5ActionInitialization(tr));
  runManager -> SetUserInitialization(new B5DetectorConstruction());
  runManager -> SetUserAction(new B5PrimaryGeneratorAction());
  runManager -> Initialize();
  
  TFile *tf = new TFile(str_fname,"RECREATE");
  
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
