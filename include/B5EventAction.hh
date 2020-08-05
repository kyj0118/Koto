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
/// \file B5EventAction.hh
/// \brief Definition of the B5EventAction class

#ifndef B5EventAction_h
#define B5EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>
#include <array>
#include "B5RunAction.hh"

#include "TTree.h"

/// Event action
const int kMaxLayer = 105;
const int kMaxSegment = 25;
const int kMaxScintillator = kMaxLayer * kMaxSegment;

struct EMHitStruct{
  int nhit;
  int nMaximumHits = kMaxScintillator;
  int one[kMaxScintillator];
  int cid[kMaxScintillator];
  int lid[kMaxScintillator];
  int segid[kMaxScintillator];
  double x[kMaxScintillator];
  double y[kMaxScintillator];
  double z[kMaxScintillator];
  double t[kMaxScintillator];
  double e[kMaxScintillator];
  
};

struct LeadHitStruct{
  int nhit;
  int nMaximumHits = kMaxLayer;
  int one[kMaxLayer];
  int cid[kMaxLayer];
  int lid[kMaxLayer];
  int segid[kMaxLayer];
  double x[kMaxLayer];
  double y[kMaxLayer];
  double z[kMaxLayer];
  double t[kMaxLayer];
  double e[kMaxLayer];
};

struct EventInfoStruct{
  int eventID;
  int runID;
  long randomSeed;
};

class B5EventAction : public G4UserEventAction
{
public:
  B5EventAction(B5RunAction *runAction, TTree *tr);
  virtual ~B5EventAction();
  void SetBranch();
  void SetRunID(G4int RunID);
  void SetRandomSeed(long seed);
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  
  EMHitStruct EMHit;
  LeadHitStruct LeadHit;
  EventInfoStruct EventInfo;
private:
  B5RunAction* fRunAction;  
  TTree *fTree;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
