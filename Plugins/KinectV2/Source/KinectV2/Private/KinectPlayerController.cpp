
#include "IKinectV2PluginPCH.h"
#include "KinectPlayerController.h"
#include "KinectEventManager.h"

AKinectPlayerController::AKinectPlayerController(const FObjectInitializer  & PCIP) :Super(PCIP){


	GetNameSafe(this);
	/*
	auto TempGameInstance =  UGameplayStatics::GetGameInstance(this);
	if (TempGameInstance){
		auto GameInstance = Cast<UKinectGameInstance>(TempGameInstance);
		if (GameInstance)
		{
			GameInstance->KinectPlayerActors.Add(KinectPlayerControllerIndex, TWeakObjectPtr<AKinectPlayerController>(this));
		}
	}
	*/
}
