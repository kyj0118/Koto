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
#include "TString.h"
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5DetectorConstruction::B5DetectorConstruction()
  : G4VUserDetectorConstruction()
{
  NumberOfLayers = 105;
  NumberOfScintillators = 25;
  //NumberOfLayers = 1;
  //NumberOfScintillators = 25;
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

  // Pb plate
  G4Material* detector_mat_pb = nist -> FindOrBuildMaterial("G4_Pb");
  G4double pb_size_x = 50.*cm;
  G4double pb_size_y = 50.*cm;
  G4double pb_size_z = 1.*mm;
  G4double pb_offset_z = 30.*mm;
  
  // EJ 200 Scintillator
  G4double scint_size_x = 50.*cm;
  G4double scint_size_y = 2.*cm;
  G4double scint_size_z = 5.*mm;
  //G4double scint_offset_z = pb_offset_z + (pb_size_z + scint_size_z)/2.0;
  G4double layer_gap_z = pb_size_z + scint_size_z;

  //G4Element* H = new G4Element("Hydrogen"  , "H", 1., 1.00*g/mole);
  //G4Element* C = new G4Element("Carbon"  , "C", 6., 12.0107*g/mole);
  //G4Element* O = new G4Element("Oxygen", "O", 8., 15.9994*g/mole);
  //G4Element* N = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
  //G4Element* Si = new G4Element("Silicon","Si", 14., 28.0855*g/mole);
  //G4Element* Al = new G4Element("Aluminum", "Al", 13., 26.981*g / mole);
  G4Material* EJ200 = nist -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  //G4Material* EJ200 = new G4Material("scintillator", 1.023*g/cm3, 2);
  //EJ200 -> AddElement(C, 91.6*perCent);
  //EJ200 -> AddElement(H, 8.4*perCent);
  
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
  
  
  G4LogicalVolume* logicScint[2625] = {NULL};
  G4LogicalVolume* logicLead[105] = {NULL};
  
  
  for (int iLayer = 0; iLayer < NumberOfLayers; iLayer++){
    TString str_leadname_tmp = Form("Lead%d",iLayer);
    G4String str_leadname = str_leadname_tmp.Data();
    for (int i = 0 ; i < NumberOfScintillators; i++){
      G4int iScint = i + iLayer * NumberOfScintillators;
      TString str_scintname_tmp = Form("Scint%d_%d",iLayer,i);
      G4String str_scintname = str_scintname_tmp.Data();
      logicScint[iScint] = new G4LogicalVolume(solidScint, EJ200, str_scintname);
      G4double scint_offset_y = ((G4double) i - 12) * scint_size_y;
      G4double scint_offset_z = pb_offset_z + (pb_size_z + scint_size_z)/2.0 + layer_gap_z *((G4double) iLayer);
      if (iLayer % 2 == 0){
	new G4PVPlacement(0,
			  G4ThreeVector(0,scint_offset_y,scint_offset_z),
			  logicScint[iScint],
			  str_scintname,
			  logicWorld,
			  false,
			  iScint,
			  false);
      }
      else {
	new G4PVPlacement(pRot,
			  G4ThreeVector(scint_offset_y,0,scint_offset_z),
			  logicScint[iScint],
			  str_scintname,
			  logicWorld,
			  false,
			  iScint,
			  false);
      }
    }
    logicLead[iLayer] = new G4LogicalVolume(solidLead,
    					    detector_mat_pb,
					    str_leadname);

    new G4PVPlacement(0,
		      G4ThreeVector(0,0,pb_offset_z + layer_gap_z * ((G4double) iLayer)),
		      logicLead[iLayer],
		      str_leadname,
		      logicWorld,
		      false,
		      0,
		      false);
  }
  
  // visualization attributes ------------------------------------------------
  /*
    auto visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(false);
    worldLogical->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
  */
  
  return physWorld;


}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5DetectorConstruction::ConstructSDandField()
{

  //Sensitive Detector
  for (int iLayer = 0 ; iLayer < NumberOfLayers; iLayer++){
    TString str_ilayer_tmp = Form("%d",iLayer);
    for (int iScint = 0 ; iScint < NumberOfScintillators; iScint++){
      TString str_iscint_tmp = Form("%d",iScint);
      //G4String str_ilayer = str_ilayer_tmp.Data();
      //G4String str_iscint = str_iscint_tmp.Data();
      G4String str_SDname = "ScintillatorSD" + std::string(str_ilayer_tmp.Data()) + "_" + std::string(str_iscint_tmp.Data());
      G4String str_LVname = "Scint" + std::string(str_ilayer_tmp.Data()) + "_" + std::string(str_iscint_tmp.Data());
      B5EmCalorimeterSD* ScintillatorSD = new B5EmCalorimeterSD(str_SDname,iLayer,iScint);
      // make the pointer of sensitive detector on thing set upper
      G4SDManager::GetSDMpointer() -> AddNewDetector(ScintillatorSD);
      // And set the pointer on logical volume
      SetSensitiveDetector(str_LVname, ScintillatorSD, true);
    }
    // Lead
    G4String str_SDnameLead = "LeadSD" + std::string(str_ilayer_tmp.Data());
    G4String str_LVnameLead = "Lead" + std::string(str_ilayer_tmp.Data());
    B5LeadSD* LeadSD = new B5LeadSD(str_SDnameLead,iLayer);
    G4SDManager::GetSDMpointer() -> AddNewDetector(LeadSD);
    SetSensitiveDetector(str_LVnameLead, LeadSD, true);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

