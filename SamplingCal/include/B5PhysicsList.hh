#ifndef B5PhysicsList_h
#define B5PhysicsList_h 1

#include "globals.hh"
#include "G4VUserPhysicsList.hh"

class G4Cerenkov;
class G4Scintillation;
class G4OpAbsorption;
class G4OpRayleigh;
class G4OpMieHG;
class G4OpBoundaryProcess;
class G4OpWLS;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class B5PhysicsList : public G4VUserPhysicsList
{
public:

  B5PhysicsList();
  virtual ~B5PhysicsList();

public:

  virtual void ConstructParticle();
  virtual void ConstructProcess();

  virtual void SetCuts();

  //these methods Construct physics processes and register them
  void ConstructDecay();
  void ConstructEM();
  void ConstructNuclearProcess();
  void ConstructOp();

  //for the Messenger
  void SetVerbose(G4int);
  void SetNbOfPhotonsCerenkov(G4int);
  
private:

  static G4ThreadLocal G4int fVerboseLevel;
  static G4ThreadLocal G4int fMaxNumPhotonStep;

  static G4ThreadLocal G4Cerenkov* fCerenkovProcess;
  static G4ThreadLocal G4Scintillation* fScintillationProcess;
  static G4ThreadLocal G4OpAbsorption* fAbsorptionProcess;
  static G4ThreadLocal G4OpRayleigh* fRayleighScatteringProcess;
  static G4ThreadLocal G4OpMieHG* fMieHGScatteringProcess;
  static G4ThreadLocal G4OpBoundaryProcess* fBoundaryProcess;

  static G4ThreadLocal G4OpWLS* fWLSProcess;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
