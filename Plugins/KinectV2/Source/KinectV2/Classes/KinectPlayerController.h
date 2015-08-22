
#pragma once 




//#include "KinectBodyFrameListenerInterface.h"
//#include "Kismet/GameplayStatics.h"
#include "KinectEventManager.h"
#include "KinectFunctionLibrary.h"
#include "KinectPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewKinectBodyEvent, const  FBody&,NewBody);


UCLASS(MinimalApi,Blueprintable)
class AKinectPlayerController : public AActor{

	GENERATED_UCLASS_BODY()

public:

	//virtual void NewBodyFrameEvent_Implementation(int32 SkeletonIndex, const TArray<FTransform>& BoneTransforms, bool IsTracked, EHandState::Type RightHandState, EHandState::Type LeftHandState, const FPlane& FloorPlane) override;


	UPROPERTY(EditAnywhere, Category = Input)
		TEnumAsByte<EKinectPlayer::Type>	KinectPlayerControllerIndex;

	UPROPERTY(BlueprintAssignable, Category = "Kinect")
	//	bool Assigned = false;
		FNewKinectBodyEvent NewKinectBodyEvent;

private:



};