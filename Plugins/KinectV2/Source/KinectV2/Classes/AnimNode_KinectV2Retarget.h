#pragma once
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "KinectFunctionLibrary.h"
#include "AnimNode_KinectV2Retarget.generated.h"


USTRUCT(BlueprintType)
struct FKinectBoneRetargetAdjustment
{

	GENERATED_USTRUCT_BODY()

public:
	
	FKinectBoneRetargetAdjustment():BoneDirAxis(EAxis::Z),BoneNormalAxis(EAxis::X),BoneBinormalAxis(EAxis::Y),BoneDirAdjustment(FRotator()),BoneNormalAdjustment(FRotator())
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		FRotator BoneDirAdjustment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		FRotator BoneNormalAdjustment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		bool bInvertDir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		bool bInvertNormal = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		TEnumAsByte<EAxis::Type> BoneDirAxis = EAxis::Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		TEnumAsByte<EAxis::Type> BoneNormalAxis = EAxis::X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		TEnumAsByte<EAxis::Type> BoneBinormalAxis = EAxis::Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kinect)
		bool bDebugDraw = false;

};


USTRUCT()
struct KINECTV2_API FAnimNode_KinectV2Retarget: public FAnimNode_SkeletalControlBase
{
	
	GENERATED_USTRUCT_BODY()

public:
		/** Name of bone to control. This is the main bone chain to modify from. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, EditFixedSize, Category = Kinect, meta = (PinHiddenByDefault))
		TArray<FBoneReference> BonesToRetarget;

	/** New translation of bone to apply. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, EditFixedSize, Category = Kinect, meta = (PinHiddenByDefault))
		TArray<FKinectBoneRetargetAdjustment> BoneAdjustments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, EditFixedSize, Category = Kinect, meta = (AlwaysAsPin))
		FBody KinectBody;


public:

	FAnimNode_KinectV2Retarget();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateBoneTransforms(USkeletalMeshComponent* SkelComp, FCSPose<FCompactPose>& MeshBases, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	FBoneContainer RequiredBones;
};
