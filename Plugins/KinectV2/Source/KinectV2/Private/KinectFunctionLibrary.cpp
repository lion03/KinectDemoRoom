//------------------------------------------------------------------------------
// 
//     The Kinect for Windows APIs used here are preliminary and subject to change
// 
//------------------------------------------------------------------------------

#include "IKinectV2PluginPCH.h"
#include "KinectV2Classes.h"
#include "KinectPlayerController.h"
#include "KinectV2InputDevice.h"

#include "OneEuroFilter.h"

#define NumOfKinectBones 25

FEnableBodyJoystick UKinectFunctionLibrary::EnableBodyJoystickEvent;
FMapBodyCoordToScreenCoord UKinectFunctionLibrary::MapBodyCoordToScreenCoordEvent;
FOneEuroFilterSetEnableEvent UKinectFunctionLibrary::OneEuroFilterSetEnableEvent;
FOneEuroFilterSetFreqEvent UKinectFunctionLibrary::OneEuroFilterSetFreqEvent;
FOneEuroFilterSetMinCutoffEvent UKinectFunctionLibrary::OneEuroFilterSetMinCutoffEvent;
FOneEuroFilterSetBetaEvent UKinectFunctionLibrary::OneEuroFilterSetBetaEvent;
FOneEuroFilterSetDCutOffEvent UKinectFunctionLibrary::OneEuroFilterSetDCutOffEvent;
FOneEuroFilterSetFilterParamsEvent UKinectFunctionLibrary::OneEuroFilterSetFilterParamsEvent;
FGetKinectManegerEvent UKinectFunctionLibrary::GetKinectManagerEvent;
FGetKinectInputDevice UKinectFunctionLibrary::GetKinectInputDeviceEvent;
FStartSensorEvent UKinectFunctionLibrary::StartSensorEvent;
FShutdownSensorEvent UKinectFunctionLibrary::ShutdownSensorEvent;


TSharedPtr<KinectSkeletonOneEuroFilter> UKinectFunctionLibrary::KinectBoneFilter = TSharedPtr<KinectSkeletonOneEuroFilter>(new KinectSkeletonOneEuroFilter(30.f,1.5f,1.f,1.f));

TMap<TEnumAsByte<EJoint::Type>, TEnumAsByte<EJoint::Type>> UKinectFunctionLibrary::BoneMap;


UKinectFunctionLibrary::UKinectFunctionLibrary(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BoneMap.Add(EJoint::JointType_SpineBase, EJoint::JointType_SpineBase);
	BoneMap.Add(EJoint::JointType_SpineMid, EJoint::JointType_SpineBase);
	BoneMap.Add(EJoint::JointType_Neck, EJoint::JointType_SpineMid);
	BoneMap.Add(EJoint::JointType_Head, EJoint::JointType_Neck);
	BoneMap.Add(EJoint::JointType_ShoulderLeft, EJoint::JointType_SpineShoulder);//
	BoneMap.Add(EJoint::JointType_ElbowLeft, EJoint::JointType_ShoulderLeft);
	BoneMap.Add(EJoint::JointType_WristLeft, EJoint::JointType_ElbowLeft);
	BoneMap.Add(EJoint::JointType_HandLeft, EJoint::JointType_WristLeft);
	BoneMap.Add(EJoint::JointType_ShoulderRight, EJoint::JointType_SpineShoulder);//
	BoneMap.Add(EJoint::JointType_ElbowRight, EJoint::JointType_ShoulderRight);
	BoneMap.Add(EJoint::JointType_WristRight, EJoint::JointType_ElbowRight);
	BoneMap.Add(EJoint::JointType_HandRight, EJoint::JointType_WristRight);
	BoneMap.Add(EJoint::JointType_HipLeft, EJoint::JointType_SpineBase);
	BoneMap.Add(EJoint::JointType_KneeLeft, EJoint::JointType_HipLeft);
	BoneMap.Add(EJoint::JointType_AnkleLeft, EJoint::JointType_KneeLeft);
	BoneMap.Add(EJoint::JointType_FootLeft, EJoint::JointType_AnkleLeft);
	BoneMap.Add(EJoint::JointType_HipRight, EJoint::JointType_SpineBase);
	BoneMap.Add(EJoint::JointType_KneeRight, EJoint::JointType_HipRight);
	BoneMap.Add(EJoint::JointType_AnkleRight, EJoint::JointType_KneeRight);
	BoneMap.Add(EJoint::JointType_FootRight, EJoint::JointType_AnkleRight);
	BoneMap.Add(EJoint::JointType_SpineShoulder, EJoint::JointType_SpineMid);
	BoneMap.Add(EJoint::JointType_HandTipLeft, EJoint::JointType_HandLeft);
	BoneMap.Add(EJoint::JointType_ThumbLeft, EJoint::JointType_HandLeft);
	BoneMap.Add(EJoint::JointType_HandTipRight, EJoint::JointType_HandRight);
	BoneMap.Add(EJoint::JointType_ThumbRight, EJoint::JointType_HandRight);


}

FBody::FBody(IBody* pBody){

	KinectBones.AddZeroed(NumOfKinectBones);

	if (!pBody)
		return;

	BOOLEAN IsTracked;
	HRESULT hr = pBody->get_IsTracked(&IsTracked);

	if (FAILED(hr) || !IsTracked){

		bIsTracked = false;

		return;

	}

	TrackingId = 0;

	hr = pBody->get_TrackingId(&TrackingId);

	if (FAILED(hr)){

		bIsTracked = false;

		return;

	}


	Joint nJoints[NumOfKinectBones];
	hr = pBody->GetJoints(NumOfKinectBones, nJoints);

	if (FAILED(hr)){

		bIsTracked = false;
		return;
	}


	JointOrientation nJointOrientation[NumOfKinectBones];

	hr = pBody->GetJointOrientations(NumOfKinectBones, nJointOrientation);
	if (FAILED(hr)){

		bIsTracked = false;
		return;
	}

	for (size_t i = 0; i < NumOfKinectBones; i++) //first pass build global skeleton relations 
	{
		KinectBones[i] = FKinectBone(nJointOrientation[i], nJoints[i]);
		
	}

	for (auto Bone : KinectBones)
	{
		
		if (Bone.JointTypeEnd != EJoint::JointType_SpineBase)
			KinectBones[Bone.JointTypeStart].Children.AddUnique(Bone.JointTypeEnd);

		if (Bone.JointTypeEnd == EJoint::JointType_SpineBase 
			|| Bone.JointTypeEnd == EJoint::JointType_HandTipLeft
			|| Bone.JointTypeEnd == EJoint::JointType_HandTipRight
			|| Bone.JointTypeEnd == EJoint::JointType_Head
			|| Bone.JointTypeEnd == EJoint::JointType_FootLeft
			|| Bone.JointTypeEnd == EJoint::JointType_FootRight)
			continue;

		

		FRotationMatrix R(Bone.JointTransform.GetRotation().Rotator());

		KinectBones[Bone.JointTypeStart].Normal = R.GetScaledAxis(EAxis::X);
	}

	HandState nLeftHandState, nRightHandState;
	if (FAILED(pBody->get_HandLeftState(&nLeftHandState)) || FAILED(pBody->get_HandRightState(&nRightHandState))){
		bIsTracked = false;
		return;
	}


	PointF tempLean;
	pBody->get_Lean(&tempLean);

	Lean = FVector(tempLean.X, tempLean.Y,0);

	LeftHandState = (EHandState::Type)nLeftHandState;
	RightHandState = (EHandState::Type)nRightHandState;
	bIsTracked = true;

}

FBody::FBody() :bIsTracked(false){

	KinectBones.AddZeroed(NumOfKinectBones);


	TrackingId = 0;

}

FBodyFrame::FBodyFrame(){

	Bodies.AddZeroed(BODY_COUNT);
	
}

FBodyFrame::FBodyFrame(const FBodyFrame& bodyFrame){

	Bodies.AddZeroed(BODY_COUNT);
	for (auto i = 0; i < BODY_COUNT; ++i){
		this->Bodies[i] = bodyFrame.Bodies[i];
	}
	this->FloorPlane = bodyFrame.FloorPlane;

}

FBodyFrame::FBodyFrame(IBody** ppBodies, const Vector4& floorPlane){

	Bodies.AddZeroed(BODY_COUNT);

	for (auto i = 0; i < BODY_COUNT; ++i){
		Bodies[i] = FBody(ppBodies[i]);
	}


	FloorPlane = FPlane(FVector4(FRotator(0.f, 180.f, 0.f).RotateVector(FVector(-floorPlane.z, -floorPlane.x, floorPlane.y)), floorPlane.w));

}

FBodyFrame& FBodyFrame::operator=(const FBodyFrame& OtherBodyFrame)
{
	if (this->Bodies.Num() == 0)
	{
		Bodies.AddZeroed(BODY_COUNT);
	}
	
	for (auto i = 0; i < BODY_COUNT; ++i){
		this->Bodies[i] = OtherBodyFrame.Bodies[i];
	}
	this->FloorPlane = OtherBodyFrame.FloorPlane;

	return *this;
}

FBody::FBody(const FBody& body){


	KinectBones.AddZeroed(NumOfKinectBones);

	this->bIsTracked = body.bIsTracked;

	this->BodyTrackingState = body.BodyTrackingState;

	this->LeftHandState = body.LeftHandState;

	this->RightHandState = body.RightHandState;

	this->TrackingId = body.TrackingId;

	this->Lean = body.Lean;

	for (size_t i = 0; i < body.KinectBones.Num(); i++)
	{
		this->KinectBones[i] = body.KinectBones[i];
	}
}

FBody& FBody::operator=(const FBody & OtherBody)
{

	if (this->KinectBones.Num() == 0){

		KinectBones.AddZeroed(NumOfKinectBones);

	}
	this->bIsTracked = OtherBody.bIsTracked;

	this->BodyTrackingState = OtherBody.BodyTrackingState;

	this->LeftHandState = OtherBody.LeftHandState;

	this->RightHandState = OtherBody.RightHandState;

	this->TrackingId = OtherBody.TrackingId;

	this->Lean = FVector(OtherBody.Lean);

	for (size_t i = 0; i < OtherBody.KinectBones.Num(); i++)
	{

		this->KinectBones[i] = OtherBody.KinectBones[i];
	}

	return *this;
}

FKinectBone::FKinectBone(){}

FKinectBone::FKinectBone(const JointOrientation& jointOrientation, const Joint& joint)
{

	
	JointTypeStart = UKinectFunctionLibrary::BoneMap[(EJoint::Type)joint.JointType];
	JointTypeEnd = (EJoint::Type)joint.JointType;

	TrackingState = (ETrackingState::Type)joint.TrackingState;
	CameraSpacePoint = FVector(joint.Position.X, joint.Position.Y, joint.Position.Z);



	Orientation = FQuat(jointOrientation.Orientation.x,
		jointOrientation.Orientation.y,
		jointOrientation.Orientation.z,
		jointOrientation.Orientation.w);

	CameraSpacePoint = FVector(joint.Position.X, joint.Position.Y, joint.Position.Z);

	FVector UETranslation = FVector(-CameraSpacePoint.Z, -CameraSpacePoint.X, CameraSpacePoint.Y);
	FQuat UERotation = FQuat(-Orientation.Z, -Orientation.X, Orientation.Y, Orientation.W) *FRotator(0.f, 180.f, 0.f).Quaternion();

	JointTransform.SetRotation(UERotation);

	JointTransform.SetTranslation(UETranslation);

	MirroredJointTransform = JointTransform;

	MirroredJointTransform.NormalizeRotation();

	MirroredJointTransform.Mirror(EAxis::Y, EAxis::None);


}


FKinectBone& FKinectBone::operator=(const FKinectBone& Other)
{
	this->JointTypeStart = Other.JointTypeStart;

	this->JointTypeEnd = Other.JointTypeEnd;

	this->TrackingState = Other.TrackingState;

	this->CameraSpacePoint = Other.CameraSpacePoint;

	this->Orientation = Other.Orientation; 

	this->JointTransform = Other.JointTransform;

	this->MirroredJointTransform = Other.MirroredJointTransform;

	this->Normal = Other.Normal;

	this->Children = Other.Children;

	return *this;
}

FRotator UKinectFunctionLibrary::Vec4QuatToRotator(const FVector4& TheVec){

	return FRotator(FQuat(TheVec.X, TheVec.Y, TheVec.Z, TheVec.W));

}

FQuat	UKinectFunctionLibrary::RotatorToQuat(const FRotator& TheRotator){

	return FQuat(TheRotator);

}

void UKinectFunctionLibrary::BreakBodyFrame(const FBodyFrame& InBodyFrame, TArray<FBody> &Bodies){

	for (size_t i = 0; i < InBodyFrame.Bodies.Num(); i++)
		Bodies.Push(FBody(InBodyFrame.Bodies[i]));

}


void UKinectFunctionLibrary::BreakBody(const FBody& InBody, TArray<FTransform> &BoneTransforms, bool &IsTracked){
	IsTracked = InBody.bIsTracked;

	for (size_t i = 0; i <InBody.KinectBones.Num(); i++)
		BoneTransforms.Push(FTransform(InBody.KinectBones[i].JointTransform));

}

void UKinectFunctionLibrary::BreakKinectBone(const FKinectBone& InBody, TEnumAsByte<EJoint::Type> &JointTypeStart,
	TEnumAsByte<EJoint::Type>& JointTypeEnd,
	FQuat& Orientation, FVector& CameraSpacePoint, TEnumAsByte<ETrackingState::Type>& TrackingState){
	JointTypeStart = InBody.JointTypeStart;
	JointTypeEnd = InBody.JointTypeEnd;
	Orientation = InBody.Orientation;
	CameraSpacePoint = InBody.CameraSpacePoint;
	TrackingState = InBody.TrackingState;
}


FQuat UKinectFunctionLibrary::MakeQuat(float x, float y, float z, float w){

	return FQuat(x, y, z, w);

}

void UKinectFunctionLibrary::BreakQuat(const FQuat& InQuat, float& x, float& y, float& z, float& w){
	x = InQuat.X;
	y = InQuat.Y;
	z = InQuat.Z;
	w = InQuat.W;
}

FRotator UKinectFunctionLibrary::Conv_QuatToRotator(const FQuat& Quat){
	return Quat.Rotator();
}

FQuat UKinectFunctionLibrary::Conv_Vector4ToQuat(const FVector4& InVec){

	return FQuat(InVec.X, InVec.Y, InVec.Z, InVec.W);

}

void UKinectFunctionLibrary::BreakVector4(const FVector4& InVector, float& X, float& Y, float& Z, float& W){

	X = InVector.X;
	Y = InVector.Y;
	Z = InVector.Z;
	W = InVector.W;

}

FVector4 UKinectFunctionLibrary::MakeVector4(const float& X, const float& Y, const float& Z, const float& W){
	return FVector4(X, Y, Z, W);
}

FQuat UKinectFunctionLibrary::MultiplyQuat(const FQuat& A, const FQuat& B){
	return B*A;
}





void UKinectFunctionLibrary::QuatToAxisOutAngle(const FQuat& InQuat, FVector& OutAxis, float &OutAngle){

	FQuat rotation = InQuat;
	rotation.Normalize();
	float angle = 2.0f * (float)FMath::Acos(rotation.W);

	float s = (float)FMath::Sqrt(1.0f - (rotation.W * rotation.W));

	// If the angle is very small, the direction is not important - set a default here
	FVector axis = FVector(rotation.X, rotation.Y, rotation.Z);

	// perform calculation if proper angle
	if (s >= 0.001f)
	{
		float oneOverS = 1.0f / s;
		axis.X = rotation.X * oneOverS; // normalize axis
		axis.Y = rotation.Y * oneOverS;
		axis.Z = rotation.Z * oneOverS;
	}

	axis.Normalize();

	OutAxis = axis;

	OutAngle = angle;

}

float UKinectFunctionLibrary::DegreesToRadians(const float &InDegrees){
	return FMath::DegreesToRadians(InDegrees);
}

float UKinectFunctionLibrary::RadiansToDegrees(const float &InRadians){
	return FMath::RadiansToDegrees(InRadians);
}

TEnumAsByte<EJoint::Type> UKinectFunctionLibrary::GetBoneParent(const TEnumAsByte<EJoint::Type> &InBone){

	if (InBone < BoneMap.Num())
	{
		return BoneMap[InBone];
	}
	
	return TEnumAsByte<EJoint::Type>(EJoint::JointType_Count);
}

FVector UKinectFunctionLibrary::GetDirVec(const FVector& From, const FVector& To){

	FVector retVec = (To - From).GetSafeNormal();

	return FVector(-retVec.X, -retVec.Z, retVec.Y);

}

FTransform UKinectFunctionLibrary::InverseTransform(const FTransform& InTransform){
	return InTransform.Inverse();
}


FRotator UKinectFunctionLibrary::ConvertRotatorToLocal(const FRotator& WorldRotator, const FTransform& ParentWorldTransform)
{
	FVector Forward = ParentWorldTransform.InverseTransformVectorNoScale(FRotationMatrix(WorldRotator).GetScaledAxis(EAxis::X));
	FVector Right = ParentWorldTransform.InverseTransformVectorNoScale(FRotationMatrix(WorldRotator).GetScaledAxis(EAxis::Y));
	FVector Up = ParentWorldTransform.InverseTransformVectorNoScale(FRotationMatrix(WorldRotator).GetScaledAxis(EAxis::Z));

	FMatrix RotMatrix(Forward, Right, Up, FVector::ZeroVector);

	return RotMatrix.Rotator();
}

FTransform UKinectFunctionLibrary::MultiplyTransform(const FTransform& A, const FTransform& B){

	return A*B;

}


FQuat UKinectFunctionLibrary::InverseQuat(const FQuat& InQuat){
	return InQuat.Inverse();
}

void UKinectFunctionLibrary::EnableBodyJoystick(const bool& Enable){

	EnableBodyJoystickEvent.ExecuteIfBound(Enable);

}

FVector2D UKinectFunctionLibrary::ConvertBodyPointToScreenPoint(const FVector&BodyPoint, int32 ScreenSizeX, int32 ScreenSizeY)
{
	if (MapBodyCoordToScreenCoordEvent.IsBound()){
		return MapBodyCoordToScreenCoordEvent.Execute(BodyPoint, ScreenSizeX, ScreenSizeY);
	}

	return FVector2D(-1, -1);
}

void UKinectFunctionLibrary::OneEuroFilterSetEnable(bool Enable)
{
	OneEuroFilterSetEnableEvent.ExecuteIfBound(Enable);
}

void UKinectFunctionLibrary::OneEuroFilterSetFreq(float Freq)
{
	OneEuroFilterSetFreqEvent.ExecuteIfBound(Freq);
}

void UKinectFunctionLibrary::OneEuroFilterSetMinCutoff(float MinCutoff)
{
	OneEuroFilterSetMinCutoffEvent.ExecuteIfBound(MinCutoff);
}

void UKinectFunctionLibrary::OneEuroFilterSetBeta(float Beta)
{
	OneEuroFilterSetBetaEvent.ExecuteIfBound(Beta);
}

void UKinectFunctionLibrary::OneEuroFilterSetDCutOff(float DCutoff)
{
	OneEuroFilterSetDCutOffEvent.ExecuteIfBound(DCutoff);
}

void UKinectFunctionLibrary::OneEuroFilterSetFilterParams(float freq, float minCutoff, float beta, float dCutoff)
{
	KinectBoneFilter->SetFilterParams(freq, minCutoff, beta, dCutoff);
	OneEuroFilterSetFilterParamsEvent.ExecuteIfBound(freq,  minCutoff,  beta, dCutoff);
}

TArray<FTransform> UKinectFunctionLibrary::Filter(const TArray<FTransform>& KinectBoneTransforms, float timeStamp /*= UndefinedTime*/)
{
	return KinectBoneFilter->Filter(KinectBoneTransforms, timeStamp);
}

UKinectFunctionLibrary* UKinectFunctionLibrary::GetKinectFunctionLibraryInstance(bool& IsValid)
{
	IsValid = false;
	UKinectFunctionLibrary* DataInstance = Cast<UKinectFunctionLibrary>(GEngine->GameSingleton);

	if (!DataInstance) return NULL;
	if (!DataInstance->IsValidLowLevel()) return NULL;

	IsValid = true;
	return DataInstance;
}

UKinectEventManager* UKinectFunctionLibrary::GetKinectEventManager()
{
	if (GetKinectManagerEvent.IsBound())
	{
		return GetKinectManagerEvent.Execute();
	}
	
	return nullptr;
}


void UKinectFunctionLibrary::StartSensor()
{
	if (StartSensorEvent.IsBound())
	{
		StartSensorEvent.Execute();
	}
}


void UKinectFunctionLibrary::ShutdownSensor()
{
	if (ShutdownSensorEvent.IsBound())
	{
		ShutdownSensorEvent.Execute();
	}
}

TArray<FTransform> UKinectFunctionLibrary::MirrorKinectSkeleton(const FBody& BodyToMirror, float JointPosScale /*= 1.f*/)
{
	TArray<FTransform> RetArray;

	if (JointPosScale < 0.f)
		JointPosScale *= -1.f;

	if (BodyToMirror.bIsTracked)
	{

		for (const FKinectBone & Bone : BodyToMirror.KinectBones){

			auto MirroredTransform = Bone.JointTransform;

			MirroredTransform.ScaleTranslation(JointPosScale);

			MirroredTransform.NormalizeRotation();

			MirroredTransform.Mirror(EAxis::Y, EAxis::None);

			RetArray.Push(MirroredTransform);
		}

	}
	else
	{
		RetArray.AddZeroed(25);
	}

	return RetArray;
}

FKinectV2InputDevice* UKinectFunctionLibrary::GetKinectInputDevice(){

	if (GetKinectInputDeviceEvent.IsBound())
	{
		return GetKinectInputDeviceEvent.Execute();
	}

	return nullptr;

}



