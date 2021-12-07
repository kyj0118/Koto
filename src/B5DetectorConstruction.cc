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
/// \file B5DetectorConstruction.cc
/// \brief Implementation of the B5DetectorConstruction class

#include "B5DetectorConstruction.hh"
#include "B5EmCalorimeterSD.hh"
#include "B5LeadSD.hh"

#include "G4TransportationManager.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"
#include "G4tgbRotationMatrix.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

// Root classes
//#include "TString.h"
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int gNLayers;
G4int gNStrips;
G4int gTotalNScintillators;

B5DetectorConstruction::B5DetectorConstruction()
  : G4VUserDetectorConstruction()
{
  fNumberOfLayers = 105;
  fNumberOfScintillators = 35; // 15mm width
  
  gNLayers = fNumberOfLayers;
  gNStrips = fNumberOfScintillators;
  gTotalNScintillators = fNumberOfLayers*fNumberOfScintillators;
  
  fScintLength = 52.5*cm;
  fScintWidth = 1.5*cm;
  fScintThickness = 5.0*mm;
  
  fConverterLength = fScintLength;
  fConverterThickness = 1.0*mm;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5DetectorConstruction::~B5DetectorConstruction()
{
  
  for (auto visAttributes: fVisAttributes) {
    delete visAttributes;
  }  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B5DetectorConstruction::Construct(){
  G4NistManager* nist = G4NistManager::Instance();
  
  // -----------------------------------------------------
  // World

  G4Material* world_mat = nist -> FindOrBuildMaterial("G4_AIR");
  G4double world_size = 5.*m;
  
  G4Box* solidWorld =    
    new G4Box("World",                       // its name
              0.5*world_size,                // half x
              0.5*world_size,                // half y
              0.5*world_size);               // half z
  
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
  
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      false);                 //overlaps checking


  // -----------------------------------------------------

  // Detector

  // Materials
  G4Material* Material_Pb = nist -> FindOrBuildMaterial("G4_Pb");
  G4Material* Material_Scint = nist -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  
  // Pb plate
  G4double pb_size_x = fConverterLength;
  G4double pb_size_y = fConverterLength;
  G4double pb_size_z = fConverterThickness; 
  
  // EJ 200 Scintillator
  G4double scint_size_x = fScintLength;
  G4double scint_size_y = fScintWidth;
  G4double scint_size_z = fScintThickness;
  
  G4double layer_gap_z = pb_size_z + scint_size_z;
  
  std::vector<G4double> vRot;
  vRot.clear();
  vRot.push_back(0.);vRot.push_back(-1.);vRot.push_back(0.);
  vRot.push_back(1.);vRot.push_back(0.);vRot.push_back(0.);
  vRot.push_back(0.);vRot.push_back(0.);vRot.push_back(1.);
  
  G4tgbRotationMatrix* RotBuilder = new G4tgbRotationMatrix();
  G4RotationMatrix* pRot = (G4RotationMatrix*) RotBuilder->BuildG4RotMatrixFrom9(vRot); 
  


  G4Box* solidLead =    
    new G4Box("Lead",
              0.5*pb_size_x,
              0.5*pb_size_y,
              0.5*pb_size_z);
  
  G4Box* solidScint = new G4Box("Scint",
				0.5*scint_size_x,
				0.5*scint_size_y,
				0.5*scint_size_z);
  
  G4String str_scintname = "LogicScint";
  G4String str_leadname = "LogicLead";


  logicScint = new G4LogicalVolume(solidScint, Material_Scint, str_scintname);
  logicPb = new G4LogicalVolume(solidLead, Material_Pb, str_leadname);  
  
  for (int iLayer = 0; iLayer < fNumberOfLayers; iLayer++){
    G4double scint_position_z = scint_size_z/2.0 + layer_gap_z *((G4double) iLayer);
    G4double pb_position_z = scint_position_z + layer_gap_z/2.0;
    
    for (int i = 0 ; i < fNumberOfScintillators; i++){
      G4int iScint = i + iLayer * fNumberOfScintillators;
      G4double dnscint = (G4double) (fNumberOfScintillators-1);
      G4double scint_position_y = ((G4double) i - dnscint/2.0) * scint_size_y;
      // EmCal
      if (iLayer % 2 == 0){
	new G4PVPlacement(0,
			  G4ThreeVector(0,scint_position_y,scint_position_z),
			  logicScint,
			  str_scintname,
			  logicWorld,
			  false,
			  iScint,
			  false);
      }
      else {
	new G4PVPlacement(pRot,
			  G4ThreeVector(scint_position_y,0,scint_position_z),
			  logicScint,
			  str_scintname,
			  logicWorld,
			  false,
			  iScint,
			  false);
      }
    }
    // Lead
    new G4PVPlacement(0,
		      G4ThreeVector(0,0,pb_position_z),
		      logicPb,
		      str_leadname,
		      logicWorld,
		      false,
		      iLayer,
		      false);
  }

  auto visAttributes = new G4VisAttributes(G4Colour(0,1.0,1.0,0.5));
  logicScint->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  return physWorld;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5DetectorConstruction::ConstructSDandField()
{
  B5EmCalorimeterSD* ScintillatorSD = new B5EmCalorimeterSD("ScintSD");
  B5LeadSD* LeadSD = new B5LeadSD("LeadSD");
  
  G4SDManager::GetSDMpointer() -> AddNewDetector(ScintillatorSD);
  G4SDManager::GetSDMpointer() -> AddNewDetector(LeadSD);
  
  SetSensitiveDetector(logicScint, ScintillatorSD);
  SetSensitiveDetector(logicPb, LeadSD);
  
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

