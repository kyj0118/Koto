#ifndef B5StackingAction_H
#define B5StackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class B5StackingAction : public G4UserStackingAction
{
public:
  B5StackingAction();
  virtual ~B5StackingAction();

public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void NewStage();
  virtual void PrepareNewEvent();

private:
  std::vector<G4int> fTrackIDvector;
  std::vector<G4int> fPDGEncodingvector;

  G4int fTrackCount;
  G4int fGammaCount;
  G4int felectronCount;
  G4int fpositronCount;
  G4int fOtherCount;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

