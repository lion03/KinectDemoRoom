#include "KinectV2EditorModulePCH.h"
#include "KinectAnimInstance.h"
#include "CompilerResultsLog.h"


#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_KinectV2Retarget::UAnimGraphNode_KinectV2Retarget(class FObjectInitializer const &ObjectInitializer) :Super(ObjectInitializer)
{

}

FText UAnimGraphNode_KinectV2Retarget::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText NodeTitle;
	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		NodeTitle = GetControllerDescription();
	}
	else
	{
		// we don't have any run-time information, so it's limited to print  
		// anymore than what it is it would be nice to print more data such as 
		// name of bones for wheels, but it's not available in Persona
		NodeTitle = FText(LOCTEXT("AnimGraphNode_KinectV2Retarget_Title", "Kinect V2 Retarget"));
	}
	return NodeTitle;
}

FText UAnimGraphNode_KinectV2Retarget::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_KinectV2Retarget_Tooltip", "This alters the skeleton to bone transforms from Kinect V2 Sensor. This only works when the owner is KinectAnimInstence.");
}

void UAnimGraphNode_KinectV2Retarget::ValidateAnimNodePostCompile(class FCompilerResultsLog& MessageLog, class UAnimBlueprintGeneratedClass* CompiledClass, int32 CompiledNodeIndex)
{
	if (CompiledClass->IsChildOf(UKinectAnimInstance::StaticClass()) == false)
	{
		MessageLog.Error(TEXT("@@ is only allowwed in KinectAnimInstance. If this is for Kinect Actor, please change parent to be KinectAnimInstance (Reparent Class)."), this);
	}
}

bool UAnimGraphNode_KinectV2Retarget::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	return (Blueprint != nullptr) && Blueprint->ParentClass->IsChildOf<UKinectAnimInstance>() && Super::IsCompatibleWithGraph(TargetGraph);
}

FText UAnimGraphNode_KinectV2Retarget::GetControllerDescription() const
{
	return LOCTEXT("AnimGraphNode_KinectV2Retarget", "KinectV2 Skeleton Retargeting");
}

#undef LOCTEXT_NAMESPACE
