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
// $Id: B2SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file B2SteppingAction.cc
/// \brief Implementation of the B2SteppingAction class

#include "B2SteppingAction.hh"
#include "B2EventAction.hh"
#include "B2aDetectorConstruction.hh"
#include "B2Analysis.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2SteppingAction::B2SteppingAction(B2EventAction* eventAction)
: G4UserSteppingAction(),
fEventAction(eventAction),
fScoringVolume(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2SteppingAction::~B2SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2SteppingAction::UserSteppingAction(const G4Step* step)
{

	// G4cout << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume() << " " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume() << G4endl;

  // get volume of the current step
	G4String preVolume 
	= step->GetPreStepPoint()->GetTouchableHandle()
	->GetVolume()->GetLogicalVolume()->GetName();

	G4String postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()?step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName():"OutOfWorld";

	G4int eID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

	auto analysisManager = G4AnalysisManager::Instance();

	if (preVolume == "Silicon" || postVolume == "Silicon")
	{// fill ntuple
		analysisManager->FillNtupleIColumn(0, eID);
		analysisManager->FillNtupleIColumn(1, step->GetTrack()->GetTrackID());
		analysisManager->FillNtupleIColumn(2, step->GetTrack()->GetCurrentStepNumber());
		analysisManager->FillNtupleDColumn(3, step->GetPostStepPoint()->GetPosition().x()/mm);
		analysisManager->FillNtupleDColumn(4, step->GetPostStepPoint()->GetPosition().y()/mm);
		analysisManager->FillNtupleDColumn(5, step->GetPostStepPoint()->GetPosition().z()/mm);
		analysisManager->FillNtupleDColumn(6, step->GetTrack()->GetKineticEnergy()/MeV);
		analysisManager->FillNtupleDColumn(7, step->GetTotalEnergyDeposit()/keV);
		analysisManager->FillNtupleSColumn(8, step->GetTrack()->GetDefinition()->GetParticleName());
		analysisManager->AddNtupleRow(); 
		// G4cout << preVolume << " " << step->GetTrack()->GetKineticEnergy()/MeV << " " << step->GetPostStepPoint()->GetPosition().x()/mm << " " << step->GetPostStepPoint()->GetPosition().y()/mm << " " << step->GetPostStepPoint()->GetPosition().z()/mm << G4endl;
	}

	// G4cout << volume->GetName() << " " << step->GetTrack()->GetKineticEnergy()/MeV << " " << step->GetPostStepPoint()->GetPosition().x()/mm << " " << step->GetPostStepPoint()->GetPosition().y()/mm << " " << step->GetPostStepPoint()->GetPosition().z()/mm << G4endl;

  // analysisManager->CreateNtuple("ParicleTracks", "Tracks");
  // analysisManager->CreateNtupleIColumn("EventID");
  // analysisManager->CreateNtupleIColumn("TrackID");
  // analysisManager->CreateNtupleIColumn("StepID");
  // analysisManager->CreateNtupleDColumn("x");
  // analysisManager->CreateNtupleDColumn("y");
  // analysisManager->CreateNtupleDColumn("z");
  // analysisManager->CreateNtupleDColumn("dE");
  // analysisManager->CreateNtupleSColumn("particleType");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

