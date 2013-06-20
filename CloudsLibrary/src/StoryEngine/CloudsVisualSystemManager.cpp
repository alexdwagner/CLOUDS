
#include "CloudsVisualSystemManager.h"

#include "CloudsVisualSystemRezanator.h"
#include "CloudsVisualSystemComputationTicker.h"
#include "CloudsVisualSystemLSystems.h"
#include "CloudsVisualSystemVoro.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemCities.h"
#include "CloudsVisualSystemAmber.h"
#include "CloudsVisualSystemCollaboration1.h"
#include "CloudsVisualSystemVerletForm.h"
#include "CloudsVisualSystemAmber.h"

CloudsVisualSystemManager::CloudsVisualSystemManager(){

}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::populateVisualSystems(){
	
	nameToVisualSystem.clear();
	systems.clear();
	presets.clear();
	
	registerVisualSystem( new CloudsVisualSystemComputationTicker() );
	registerVisualSystem( new CloudsVisualSystemLSystems() );
	registerVisualSystem( new CloudsVisualSystemVoro() );
	registerVisualSystem( new CloudsVisualSystemCollaboration1() );
	registerVisualSystem( new CloudsVisualSystemCities() );
	registerVisualSystem( new CloudsVisualSystemVerletForm() );
//	registerVisualSystem( new CloudsVisualSystemAmber() );
}

//--------------------------------------------------------------------
void CloudsVisualSystemManager::registerVisualSystem(CloudsVisualSystem* system){
	
	ofLogVerbose() << "Registering system " << system->getSystemName();
	
	system->setup();
	
	systems.push_back( system );
	nameToVisualSystem[system->getSystemName()] = system;
	
	vector<string> systemPresets = system->getPresets();
	for(int i = 0; i < systemPresets.size(); i++){
		
		CloudsVisualSystemPreset preset;
		preset.presetName = systemPresets[i];
		preset.system = system;
		
		//TODO: get keywords
		
		presets.push_back(preset);
	}
}

//--------------------------------------------------------------------
CloudsVisualSystemPreset& CloudsVisualSystemManager::getRandomVisualSystem(){
	return presets[ ofRandom(presets.size()) ];
}