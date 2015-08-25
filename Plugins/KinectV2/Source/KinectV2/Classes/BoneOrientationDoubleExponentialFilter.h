#pragma once

#include "BoneOrientationDoubleExponentialFilter.generated.h"


namespace EJoint { enum Type; }

USTRUCT(BlueprintType)
struct KINECTV2_API FFilterDoubleExponentialData
{

	
	GENERATED_USTRUCT_BODY()

public:


	FQuat RawBoneOrientation;

	FQuat FilteredBoneOrientation;

	FQuat Trend;

	uint32 FrameCount;

};


USTRUCT(BlueprintType)
struct KINECTV2_API FTransformSmoothParameters
{


	GENERATED_USTRUCT_BODY();

public:

	FTransformSmoothParameters& operator=(const FTransformSmoothParameters& Other);

public:

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Parameters)
	float Correction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Parameters)
	float JitterRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Parameters)
	float MaxDeviationRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Parameters)
	float Prediction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Parameters)
	float Smoothing;

};


UCLASS(BlueprintType)
class KINECTV2_API UBoneOrientationDoubleExponentialFilter : public UObject
{

	GENERATED_UCLASS_BODY()

public:

	void Init();

	void Init(float SmoothingValue, float CorrectionValue, float PredictionValue, float JitterRadiusValue, float MmaxDeviationRadiusValue);

	void Init(const FTransformSmoothParameters& SmoothingParameters);

	void Reset();


	UFUNCTION(BlueprintCallable, Category = "Kinect|Filters")
	void UpdateFilter(struct FBody InBody, struct FBody& OutBody);

protected:

	void FilterJoint(struct FBody& Body, EJoint::Type Jt, const FTransformSmoothParameters& TransformSmoothParameters);




private:


	bool IsInit;

	TArray<FFilterDoubleExponentialData> History;

	FTransformSmoothParameters SmoothParameters;

};