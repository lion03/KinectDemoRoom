/* -*- coding: utf-8 -*-
*
* OneEuroFilter.cc -
*
* Author: Nicolas Roussel (nicolas.roussel@inria.fr)
*
*/

#pragma once

#include "UnrealMath.h"
#include "SharedPointer.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <ctime>

typedef float TimeStamp; // in seconds

static const TimeStamp UndefinedTime = -1.0;

// -----------------------------------------------------------------

class LowPassFilter {

	float y, a, s;
	bool initialized;

	

public:

	void setAlpha(float alpha);
	
	LowPassFilter(float alpha, float initval = 0.0);

	float filter(float value);

	float filterWithAlpha(float value, float alpha);

	bool hasLastRawValue(void) {
		return initialized;
	}

	float lastRawValue(void);

};

// -----------------------------------------------------------------

class OneEuroFilter {

	float freq;

	float mincutoff;
	float beta_;
	float dcutoff;
	TSharedPtr<LowPassFilter> x;
	TSharedPtr<LowPassFilter> dx;
	TimeStamp lasttime;



public:

	float alpha(float cutoff);

	void setFrequency(float f);

	void setMinCutoff(float mc);

	void setBeta(float b);

	void setDerivateCutoff(float dc);
	
	OneEuroFilter(float freq, float mincutoff = 1.0, float beta_ = 0.0, float dcutoff = 1.0);

	float filter(float value, TimeStamp timestamp = UndefinedTime);

	float Freq() const { return freq; }

	float Mincutoff() const { return mincutoff; }

	float Beta() const { return beta_; }

	float Dcutoff() const { return dcutoff; }

	~OneEuroFilter(void);

};

class KienctBoneQuatOneEuroFilter {

public:

	KienctBoneQuatOneEuroFilter(float freq, float minCutoff, float beta, float dCutoff);

	FQuat Filter(const FQuat& Value, TimeStamp timeStamp = UndefinedTime);

//private:

	TArray<TSharedPtr<OneEuroFilter>> QutaFilter;


};

class KinectSkeletonOneEuroFilter{

public:

	KinectSkeletonOneEuroFilter(float freq, float minCutoff, float beta, float dCutoff);

	void SetAlpha(float alpha);

	void SetFreq(float freq);

	void SetMinCutoff(float minCutoff);

	void SetBeta(float beta);

	void setDCutoff(float dCutoff);

	void SetFilterParams(float freq, float minCutoff, float beta, float dCutoff);


	TArray<FTransform> Filter(const TArray<FTransform>& KinectBoneTransforms, TimeStamp timeStamp = UndefinedTime);

private:

	TArray<TSharedPtr<KienctBoneQuatOneEuroFilter>> BoneFilters;

};