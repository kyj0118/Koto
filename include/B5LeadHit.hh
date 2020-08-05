#ifndef B5LeadHit_h
#define B5LeadHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"

class G4AttDef;
class G4AttValue;

/// EM Calorimeter hit
///
/// It records:
/// - the cell ID
/// - the energy deposit 
/// - the cell logical volume, its position and rotation

class B5LeadHit : public G4VHit
{
public:
  B5LeadHit();
  B5LeadHit(G4int cellID);
  B5LeadHit(const B5LeadHit &right);
  virtual ~B5LeadHit();
  
  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  G4int GetDetType() const {return fDetType;} // Lead : 0, Scintillator : 1

  void SetCellID(G4int z) { fCellID = z; }
  G4int GetCellID() const { return fCellID; }

  void SetLayerID(G4int z) { fLayerID = z; }
  G4int GetLayerID() const { return fLayerID; }

  void SetEdep(G4double de) { fEdep = de; }
  void AddEdep(G4double de) { fEdep += de; }
  G4double GetEdep() const { return fEdep; }
  
  void SetPos(G4ThreeVector xyz) { fPos = xyz; }
  G4ThreeVector GetPos() const { return fPos; }
  
  void SetLogV(G4LogicalVolume* val) { fPLogV = val; }
  const G4LogicalVolume* GetLogV() const { return fPLogV; }

  void SetXYZTE(G4double x,G4double y,G4double z,G4double t,G4double e){
    fPos.setX(x); fPos.setY(y); fPos.setZ(z);
    fTime = t;
    fEdep = e;
  };
  
  void GetXYZTE(G4double &x,G4double &y,G4double &z,G4double &t,G4double &e) const {
    x = fPos.x(); y = fPos.y(); z = fPos.z(); 
    t = fTime;
    e = fEdep;
  };
  void Print(){
    G4cout << "(" << fPos.x() << ", " << fPos.y() << ", " <<  fPos.z() << ", " << fTime << ", " << fEdep << ")" << G4endl;
  }
private:
  G4int fCellID;
  G4int fLayerID;

  G4double fEdep;
  G4double fTime;
  G4ThreeVector fPos;
  const G4LogicalVolume* fPLogV;
  const G4int fDetType = 0; // Lead : 0, Scintillator : 1  
};

using B5LeadHitsCollection = G4THitsCollection<B5LeadHit>;

extern G4ThreadLocal G4Allocator<B5LeadHit>* B5LeadHitAllocator;

inline void* B5LeadHit::operator new(size_t)
{
  if (!B5LeadHitAllocator) {
       B5LeadHitAllocator = new G4Allocator<B5LeadHit>;
  }
  return (void*)B5LeadHitAllocator->MallocSingle();
}

inline void B5LeadHit::operator delete(void* aHit)
{
  B5LeadHitAllocator->FreeSingle((B5LeadHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
