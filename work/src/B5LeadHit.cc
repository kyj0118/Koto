#include "B5LeadHit.hh"


#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<B5LeadHit>* B5LeadHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5LeadHit::B5LeadHit()
  : G4VHit(), 
    fCellID(-1),fLayerID(-1), fEdep(0.), fPos(0.), fTime(0.), fPLogV(nullptr),
    fParticlePx(0), fParticlePy(0), fParticlePz(0), fParticleTrackID(0),fParticleParentID(0),
    fParticleCharge(0), fParticleMass(0), fParticlePDGID(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5LeadHit::B5LeadHit(G4int cellID)
  : G4VHit(), 
    fCellID(cellID),fLayerID(-1), fEdep(0.), fPos(0.), fTime(0.), fPLogV(nullptr),
    fParticlePx(0), fParticlePy(0), fParticlePz(0), fParticleTrackID(0),fParticleParentID(0),
    fParticleCharge(0), fParticleMass(0), fParticlePDGID(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5LeadHit::~B5LeadHit()
{}
