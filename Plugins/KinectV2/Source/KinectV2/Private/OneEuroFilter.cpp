
#include "IKinectV2PluginPCH.h"
#include "OneEuroFilter.h"

float OneEuroFilter::alpha(float cutoff)
{
	float te = 1.0 / freq;
	float tau = 1.0 / (2 * PI *cutoff);
	return 1.0 / (1.0 + tau / te);
}

void OneEuroFilter::setFrequency(float f)
{
	if (f <= 0)
		freq = KINDA_SMALL_NUMBER;
	else
		freq = f;
}

void OneEuroFilter::setMinCutoff(float mc)
{
	if (mc <= 0)
		mincutoff = KINDA_SMALL_NUMBER;
	else
		mincutoff = mc;
}

void OneEuroFilter::setBeta(float b)
{
	beta_ = b;
}

void OneEuroFilter::setDerivateCutoff(float dc)
{
	if (dc <= 0)
		dcutoff = KINDA_SMALL_NUMBER;
	else
		dcutoff = dc;
}

OneEuroFilter::OneEuroFilter(float freq, float mincutoff /*= 1.0*/, float beta_ /*= 0.0*/, float dcutoff /*= 1.0*/)
{
	setFrequency(freq);
	setMinCutoff(mincutoff);
	setBeta(beta_);
	setDerivateCutoff(dcutoff);
	x = TSharedPtr<LowPassFilter>(new LowPassFilter(alpha(mincutoff)));
	dx = TSharedPtr<LowPassFilter>(new LowPassFilter(alpha(dcutoff)));
	lasttime = UndefinedTime;
}

float OneEuroFilter::filter(float value, TimeStamp timestamp /*= UndefinedTime*/)
{
	// update the sampling frequency based on timestamps
	if (lasttime != UndefinedTime && timestamp != UndefinedTime)
		freq = 1.0 / (timestamp - lasttime);
	lasttime = timestamp;
	// estimate the current variation per second 
	float dvalue = x->hasLastRawValue() ? (value - x->lastRawValue())*freq : 0.0; // FIXME: 0.0 or value?
	float edvalue = dx->filterWithAlpha(dvalue, alpha(dcutoff));
	// use it to update the cutoff frequency
	float cutoff = mincutoff + beta_*fabs(edvalue);
	// filter the given value
	return x->filterWithAlpha(value, alpha(cutoff));
}

OneEuroFilter::~OneEuroFilter(void)
{

}

void LowPassFilter::setAlpha(float alpha)
{
	if (alpha <= 0.0 || alpha > 1.0)
		a = KINDA_SMALL_NUMBER;
	else
		a = alpha;
}

float LowPassFilter::filter(float value)
{
	float result;
	if (initialized)
		result = a*value + (1.0 - a)*s;
	else {
		result = value;
		initialized = true;
	}
	y = value;
	s = result;
	return result;
}

float LowPassFilter::filterWithAlpha(float value, float alpha)
{
	setAlpha(alpha);
	return filter(value);
}

float LowPassFilter::lastRawValue(void)
{
	return y;
}

LowPassFilter::LowPassFilter(float alpha, float initval /*= 0.0*/)
{
	y = s = initval;
	setAlpha(alpha);
	initialized = false;
}

KienctBoneQuatOneEuroFilter::KienctBoneQuatOneEuroFilter(float freq, float minCutoff, float beta, float dCutoff)
{
	for (size_t i = 0; i < 4; i++)
	{
		QutaFilter.Push(TSharedPtr<OneEuroFilter>(new OneEuroFilter(freq, minCutoff, beta, dCutoff)));
	}
}

FQuat KienctBoneQuatOneEuroFilter::Filter(const FQuat& Value, TimeStamp timeStamp /*= UndefinedTime*/)
{


	
	FQuat FilterdQuat = FQuat(QutaFilter[0]->filter(Value.X, timeStamp), QutaFilter[1]->filter(Value.Y, timeStamp), QutaFilter[2]->filter(Value.Z, timeStamp), QutaFilter[3]->filter(Value.W, timeStamp));
	FilterdQuat.Normalize();
	return FilterdQuat;

}

KinectSkeletonOneEuroFilter::KinectSkeletonOneEuroFilter(float freq, float minCutoff, float beta, float dCutoff)
{
	for (auto i = 0; i < 25;++i)
	{

		BoneFilters.Push(TSharedPtr<KienctBoneQuatOneEuroFilter>(new KienctBoneQuatOneEuroFilter(freq, minCutoff, beta, dCutoff)));
	}
}

TArray<FTransform> KinectSkeletonOneEuroFilter::Filter(const TArray<FTransform>& KinectBoneTransforms, TimeStamp timeStamp /*= UndefinedTime*/)
{

	TArray<FTransform> FilterdTransforms;
	for (auto i = 0; i < KinectBoneTransforms.Num();++i)
	{
		FilterdTransforms.Push(FTransform(BoneFilters[i]->Filter(KinectBoneTransforms[i].GetRotation(),timeStamp), KinectBoneTransforms[i].GetLocation(), KinectBoneTransforms[i].GetScale3D()));
	}

	return FilterdTransforms;
}

void KinectSkeletonOneEuroFilter::SetAlpha(float alpha)
{

}

void KinectSkeletonOneEuroFilter::SetFreq(float freq)
{

}

void KinectSkeletonOneEuroFilter::SetMinCutoff(float minCutoff)
{

}

void KinectSkeletonOneEuroFilter::SetBeta(float beta)
{

}

void KinectSkeletonOneEuroFilter::setDCutoff(float dCutoff)
{

}

void KinectSkeletonOneEuroFilter::SetFilterParams(float freq, float minCutoff, float beta, float dCutoff)
{
	for (auto i = 0; i < BoneFilters.Num(); ++i)
	{

		BoneFilters[i] = TSharedPtr<KienctBoneQuatOneEuroFilter>(new KienctBoneQuatOneEuroFilter(freq, minCutoff, beta, dCutoff));
	}
}
