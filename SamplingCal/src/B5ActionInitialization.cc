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
/// \file B5ActionInitialization.cc
/// \brief Implementation of the B5ActionInitialization class

#include "B5ActionInitialization.hh"
#include "B5PrimaryGeneratorAction.hh"
#include "B5RunAction.hh"
#include "B5EventAction.hh"
#include "B5StackingAction.hh"

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
extern bool gSaveStepLevel;

B5ActionInitialization::B5ActionInitialization(TTree* tr)
  : G4VUserActionInitialization(), fTree(tr)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5ActionInitialization::~B5ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5ActionInitialization::BuildForMaster() const
{
  SetUserAction(new B5RunAction());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5ActionInitialization::Build() const
{
  SetUserAction(new B5PrimaryGeneratorAction);
  auto runAction = new B5RunAction();
  auto eventAction = new B5EventAction(runAction,fTree);
  SetUserAction(eventAction);
  SetUserAction(runAction);
  
  eventAction -> SetRandomSeed(CLHEP::HepRandom::getTheSeed());
  eventAction -> SetSaveStepLevel(gSaveStepLevel);
  eventAction -> SetBranch();
  //SetUserAction(new B5StackingAction());
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
