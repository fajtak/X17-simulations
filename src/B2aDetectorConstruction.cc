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
// $Id: B2aDetectorConstruction.cc 101658 2016-11-21 09:00:41Z gcosmo $
//
/// \file B2aDetectorConstruction.cc
/// \brief Implementation of the B2aDetectorConstruction class
 
#include "B2aDetectorConstruction.hh"
#include "B2aDetectorMessenger.hh"
#include "B2TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4ThreadLocal 
G4GlobalMagFieldMessenger* B2aDetectorConstruction::fMagFieldMessenger = 0;

B2aDetectorConstruction::B2aDetectorConstruction()
:G4VUserDetectorConstruction(), 
 fStepLimit(NULL),
 fCheckOverlaps(true)
{
	fMessenger = new B2aDetectorMessenger(this);
	fCreateTracker = false;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
B2aDetectorConstruction::~B2aDetectorConstruction()
{
	delete fStepLimit;
	delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4VPhysicalVolume* B2aDetectorConstruction::Construct()
{
	// Define materials
	DefineMaterials();
	// ConstructSDandField();

	// Define volumes
	return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::DefineMaterials()
{
	// Material definition 

	G4NistManager* nistManager = G4NistManager::Instance();

	// Air defined using NIST Manager
	nistManager->FindOrBuildMaterial("G4_AIR");
	nistManager->FindOrBuildMaterial("G4_Galactic");
	nistManager->FindOrBuildMaterial("G4_Al");
	nistManager->FindOrBuildMaterial("G4_Si");
	nistManager->FindOrBuildMaterial("G4_C");
	
	fDetectorMaterial = nistManager->FindOrBuildMaterial("G4_Ar");

	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2aDetectorConstruction::DefineVolumes()
{
	G4Material* air  = G4Material::GetMaterial("G4_AIR");
	G4Material* vacuum  = G4Material::GetMaterial("G4_Galactic");
	G4Material* aluminium  = G4Material::GetMaterial("G4_Al");
	G4Material* silicon = G4Material::GetMaterial("G4_Si");
	G4Material* carbon = G4Material::GetMaterial("G4_C");


	// Sizes of the principal geometrical components (solids)
	
	G4double detectorX = 10*cm; // from chamber center to center!
	G4double detectorY = 10*cm; // from chamber center to center!
	G4double detectorZ = 10*cm; // from chamber center to center!

	G4double worldLength = 1.2 * (detectorX);
	G4double siliconThickness = 0.50*mm;
	G4double siliconSize = 14*mm;

	// Definitions of Solids, Logical Volumes, Physical Volumes

	// World

	G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

	G4cout << "Computed tolerance = "
				 << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
				 << " mm" << G4endl;

	G4Box* worldS
		= new G4Box("world",                                    //its name
								worldLength/2,worldLength/2,worldLength/2); //its size
	G4LogicalVolume* worldLV
		= new G4LogicalVolume(
								 worldS,   //its solid
								 vacuum,      //its material
								 "World"); //its name
	
	//  Must place the World Physical volume unrotated at (0,0,0).
	// 
	G4VPhysicalVolume* worldPV
		= new G4PVPlacement(
								 0,               // no rotation
								 G4ThreeVector(), // at (0,0,0)
								 worldLV,         // its logical volume
								 "World",         // its name
								 0,               // its mother  volume
								 false,           // no boolean operations
								 0,               // copy number
								 fCheckOverlaps); // checking overlaps 

	// Detector

	if (fCreateTracker)
	{
		G4Box* detectorS
			= new G4Box("detector",                                    //its name
									detectorX/2+0.8*mm,detectorY/2+0.8*mm,detectorZ/2+0.8*mm); //its size

		G4LogicalVolume* detectorLV
			= new G4LogicalVolume(
									 detectorS,   //its solid
									 aluminium,      //its material
									 "DetectorLV"); //its name
		
			G4VPhysicalVolume* detectorPV
			= new G4PVPlacement(
									 0,               // no rotation
									 G4ThreeVector(), // at (0,0,0)
									 detectorLV,         // its logical volume
									 "Detector",         // its name
									 worldLV,               // its mother  volume
									 false,           // no boolean operations
									 0,               // copy number
									 fCheckOverlaps); // checking overlaps 

		G4Box* gasS
			= new G4Box("gas",                                    //its name
									detectorX/2,detectorY/2,detectorZ/2); //its size

		G4LogicalVolume* gasLV
			= new G4LogicalVolume(
									 gasS,   //its solid
									 fDetectorMaterial,      //its material
									 "GasLV"); //its name
		
		G4VPhysicalVolume* gasPV
			= new G4PVPlacement(
									 0,               // no rotation
									 G4ThreeVector(), // at (0,0,0)
									 gasLV,         // its logical volume
									 "Gas",         // its name
									 detectorLV,               // its mother  volume
									 false,           // no boolean operations
									 0,               // copy number
									 fCheckOverlaps); // checking overlaps 

		G4double maxStep = 1.0*mm;
		fStepLimit = new G4UserLimits(maxStep);
		gasLV->SetUserLimits(fStepLimit);

	}else{
		G4Box* detectorS
			= new G4Box("detector",                                    //its name
									siliconSize/2,siliconSize/2,siliconThickness/2); //its size

		G4LogicalVolume* detectorLV
			= new G4LogicalVolume(
									 detectorS,   //its solid
									 silicon,      //its material
									 "Silicon"); //its name
		
			G4VPhysicalVolume* detectorPV
			= new G4PVPlacement(
									 0,               // no rotation
									 G4ThreeVector(0,0,-detectorZ/2), // at (0,0,0)
									 detectorLV,         // its logical volume
									 "Detector",         // its name
									 worldLV,               // its mother  volume
									 false,           // no boolean operations
									 0,               // copy number
									 fCheckOverlaps); // checking overlaps 

		G4double maxStep = 0.01*mm;
		fStepLimit = new G4UserLimits(maxStep);
		detectorLV->SetUserLimits(fStepLimit);
	}

	// Example of User Limits
	//
	// Below is an example of how to set tracking constraints in a given
	// logical volume
	//
	// Sets a max step length in the tracker region, with G4StepLimiter

	// G4double maxStep = 1.0*mm;
	// fStepLimit = new G4UserLimits(maxStep);
	// gasLV->SetUserLimits(fStepLimit);
 
	/// Set additional contraints on the track, with G4UserSpecialCuts
	///
	/// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
	/// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
	///                                           maxLength,
	///                                           maxTime,
	///                                           minEkin));

	// Always return the physical world

	return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void B2aDetectorConstruction::ConstructSDandField()
{
	// Sensitive detectors

	G4String trackerChamberSDname = "B2/TrackerChamberSD";
	B2TrackerSD* aTrackerSD = new B2TrackerSD(trackerChamberSDname,
																						"TrackerHitsCollection");
	G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
	// Setting aTrackerSD to all logical volumes with the same name 
	// of "Chamber_LV".
	if (fCreateTracker)
	{
		SetSensitiveDetector("GasLV", aTrackerSD, true);
		SetSensitiveDetector("World", aTrackerSD, true);
		SetSensitiveDetector("DetectorLV", aTrackerSD, true);		
	}

	// Create global magnetic field messenger.
	// Uniform magnetic field is then created automatically if
	// the field value is not zero.
	G4ThreeVector fieldValue = G4ThreeVector(0.3*tesla,0,0);
	// G4ThreeVector fieldValue = G4ThreeVector(0,0,0);
	// fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
	// fMagFieldMessenger->SetVerboseLevel(1);
	
	// Register the field messenger for deleting
	G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetMaxStep(G4double maxStep)
{
	if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
	fCheckOverlaps = checkOverlaps;
}  
