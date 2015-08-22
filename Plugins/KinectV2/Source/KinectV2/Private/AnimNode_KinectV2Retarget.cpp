#include "IKinectV2PluginPCH.h"
#include "AnimNode_KinectV2Retarget.h"
#include "Kismet/KismetMathLibrary.h"

FAnimNode_KinectV2Retarget::FAnimNode_KinectV2Retarget()
{
	if (BonesToRetarget.Num() < 25)
	{
		BonesToRetarget.Empty();
		BonesToRetarget.AddZeroed(25);
	}

	if (BoneAdjustments.Num() < 25)
	{
		BoneAdjustments.AddZeroed(25);
	}
}


void FAnimNode_KinectV2Retarget::GatherDebugData(FNodeDebugData& DebugData)
{

}

void FAnimNode_KinectV2Retarget::EvaluateBoneTransforms(USkeletalMeshComponent* SkelComp, FCSPose<FCompactPose>& MeshBases, TArray<FBoneTransform>& OutBoneTransforms)
{


	
	uint8 i = 0;

	if (!KinectBody.bIsTracked)
	{
		return;
	}

	const FBoneContainer BoneContainer = MeshBases.GetPose().GetBoneContainer();

	FA2CSPose TempPose;

	TempPose.AllocateLocalPoses(BoneContainer, SkelComp->LocalAtoms);


	for (auto Bone : KinectBody.KinectBones)
	{


		
		if (BonesToRetarget[i].IsValid(BoneContainer))
		{

			auto DeltaTranform = Bone.MirroredJointTransform.GetRelativeTransform(SkelComp->GetBoneTransform(0));


			//AxisMeshes[Bone.JointTypeEnd]->SetRelativeLocation(PosableMesh->GetBoneLocationByName(RetargetBoneNames[Bone.JointTypeEnd], EBoneSpaces::ComponentSpace));

			auto BoneBaseTransform = DeltaTranform*SkelComp->GetBoneTransform(0);



			FRotator PreAdjusmentRotator = BoneBaseTransform.Rotator();

			FRotator PostBoneDirAdjustmentRotator = (BoneAdjustments[Bone.JointTypeEnd].BoneDirAdjustment.Quaternion()*PreAdjusmentRotator.Quaternion()).Rotator();

			FRotator CompSpaceRotator = (PostBoneDirAdjustmentRotator.Quaternion()*BoneAdjustments[Bone.JointTypeEnd].BoneNormalAdjustment.Quaternion()).Rotator();

			FVector Normal, Binormal, Dir;

			UKismetMathLibrary::BreakRotIntoAxes(CompSpaceRotator, Normal, Binormal, Dir);

			Dir *= BoneAdjustments[Bone.JointTypeEnd].bInvertDir ? -1 : 1;

			Normal *= BoneAdjustments[Bone.JointTypeEnd].bInvertNormal ? -1 : 1;


			FVector X, Y, Z;

			switch (BoneAdjustments[Bone.JointTypeEnd].BoneDirAxis)
			{
			case EAxis::X:
				X = Dir;
				break;
			case EAxis::Y:
				Y = Dir;
				break;
			case EAxis::Z:
				Z = Dir;
				break;
			default:
				;
			}

			switch (BoneAdjustments[Bone.JointTypeEnd].BoneBinormalAxis)
			{
			case EAxis::X:
				X = Binormal;
				break;
			case EAxis::Y:
				Y = Binormal;
				break;
			case EAxis::Z:
				Z = Binormal;
				break;
			default:
				;
			}

			switch (BoneAdjustments[Bone.JointTypeEnd].BoneNormalAxis)
			{
			case EAxis::X:
				X = Normal;
				break;
			case EAxis::Y:
				Y = Normal;
				break;
			case EAxis::Z:
				Z = Normal;
				break;
			default:
				;
			}

			FRotator SwiveledRot = UKismetMathLibrary::MakeRotationFromAxes(X, Y, Z);

			SwiveledRot = (SkelComp->GetBoneTransform(0).Rotator().Quaternion()*SwiveledRot.Quaternion()).Rotator();
			//PosableMesh->SetBoneRotationByName(RetargetBoneNames[Bone.JointTypeEnd], (PosableMesh->GetBoneTransform(0).Rotator().Quaternion()*SwiveledRot.Quaternion()).Rotator(), EBoneSpaces::ComponentSpace);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (BoneAdjustments[i].bDebugDraw)
			{
				DrawDebugCoordinateSystem(SkelComp->GetWorld(), SkelComp->GetBoneLocation(BonesToRetarget[i].BoneName), SwiveledRot, 100.f, false, 0.1f);
			}
#endif

			

			FCompactPoseBoneIndex CompactPoseBoneToModify = BonesToRetarget[i].GetCompactPoseIndex(BoneContainer);
			FTransform NewBoneTM = MeshBases.GetComponentSpaceTransform(CompactPoseBoneToModify);

			FAnimationRuntime::ConvertCSTransformToBoneSpace(SkelComp, MeshBases, NewBoneTM, CompactPoseBoneToModify, BCS_ComponentSpace);

			const FQuat BoneQuat(SwiveledRot);

			NewBoneTM.SetRotation(BoneQuat);

			// Convert back to Component Space.
			FAnimationRuntime::ConvertBoneSpaceTransformToCS(SkelComp, MeshBases, NewBoneTM, CompactPoseBoneToModify, BCS_ComponentSpace);


			FAnimationRuntime::SetSpaceTransform(TempPose, BonesToRetarget[i].BoneIndex, NewBoneTM);

			OutBoneTransforms.Add(FBoneTransform(BonesToRetarget[i].GetCompactPoseIndex(BoneContainer), NewBoneTM));
		}

		++i;



		

	}


}

bool FAnimNode_KinectV2Retarget::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	/*
	for (auto BoneToRetarget : BonesToRetarget)
	{
		if (!BoneToRetarget.IsValid(RequiredBones))
			return false;
	}
	*/
	return true;
}

void FAnimNode_KinectV2Retarget::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	for (int32 i = 0; i < BonesToRetarget.Num(); ++i)
	{
		
		if (BonesToRetarget[i].BoneName != NAME_None)
		{
			BonesToRetarget[i].Initialize(RequiredBones);
		}
		
	}
}
