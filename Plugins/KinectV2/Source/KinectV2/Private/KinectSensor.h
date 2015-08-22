//------------------------------------------------------------------------------
// 
//     The Kinect for Windows APIs used here are preliminary and subject to change
// 
//------------------------------------------------------------------------------
#pragma once
#include "Core.h"
#include "KinectFunctionLibrary.h"
#include "AllowWindowsPlatformTypes.h"
#include "Kinect.h"
#include "Kinect.Face.h"

#include "COMPointer.h"


/*
#include <DirectXMath.h>
#include <queue>

namespace Sample
{
	typedef struct _TRANSFORM_SMOOTH_PARAMETERS
	{
		float   fSmoothing;             // [0..1], lower values closer to raw data
		float   fCorrection;            // [0..1], lower values slower to correct towards the raw data
		float   fPrediction;            // [0..n], the number of frames to predict into the future
		float   fJitterRadius;          // The radius in meters for jitter reduction
		float   fMaxDeviationRadius;    // The maximum radius in meters that filtered positions are allowed to deviate from raw data
	} TRANSFORM_SMOOTH_PARAMETERS;

	// Holt Double Exponential Smoothing filter
	class FilterDoubleExponentialData
	{
	public:
		FVector m_vRawPosition;
		FVector m_vFilteredPosition;
		FVector m_vTrend;
		uint32    m_dwFrameCount;
	};

	class FilterDoubleExponential
	{
	public:
		FilterDoubleExponential() { Init(); }
		~FilterDoubleExponential() { Shutdown(); }

		void Init(float fSmoothing = 0.25f, float fCorrection = 0.25f, float fPrediction = 0.25f, float fJitterRadius = 0.03f, float fMaxDeviationRadius = 0.05f)
		{
			Reset(fSmoothing, fCorrection, fPrediction, fJitterRadius, fMaxDeviationRadius);
		}

		void Shutdown()
		{
		}

		void Reset(float fSmoothing = 0.25f, float fCorrection = 0.25f, float fPrediction = 0.25f, float fJitterRadius = 0.03f, float fMaxDeviationRadius = 0.05f)
		{
			assert(m_pFilteredJoints);
			assert(m_pHistory);

			m_fMaxDeviationRadius = fMaxDeviationRadius; // Size of the max prediction radius Can snap back to noisy data when too high
			m_fSmoothing = fSmoothing;                   // How much smothing will occur.  Will lag when too high
			m_fCorrection = fCorrection;                 // How much to correct back from prediction.  Can make things springy
			m_fPrediction = fPrediction;                 // Amount of prediction into the future to use. Can over shoot when too high
			m_fJitterRadius = fJitterRadius;             // Size of the radius where jitter is removed. Can do too much smoothing when too high

			memset(m_pFilteredJoints, 0, sizeof(FVector) * JointType_Count);
			memset(m_pHistory, 0, sizeof(FilterDoubleExponentialData) * JointType_Count);
		}

		void Update(IBody* const pBody);
		void Update(Joint joints[]);

		inline const FVector* GetFilteredJoints() const { return &m_pFilteredJoints[0]; }

	private:
		FVector m_pFilteredJoints[JointType_Count];
		FilterDoubleExponentialData m_pHistory[JointType_Count];
		float m_fSmoothing;
		float m_fCorrection;
		float m_fPrediction;
		float m_fJitterRadius;
		float m_fMaxDeviationRadius;

		void Update(Joint joints[], uint32 JointID, TRANSFORM_SMOOTH_PARAMETERS smoothingParams);
	};
}

*/
/**********************************************************************************************//**
 * A kinect sensor.
 *
 * @author	Leon Rosengarten
 **************************************************************************************************/

class KINECTV2_API FKinectSensor : public FRunnable
{

	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;
	static const int        cInfraredWidth = 512;
	static const int        cInfraredHeight = 424;

public:

	friend class UVisualGestureImporterFactory;

	friend class UVisualGestureDataBase;

	friend class UVisualGestureDetector;

	/**********************************************************************************************//**
	 * Gets the get.
	 *
	 * @author	Leon Rosengarten
	 * @return	A KinectSensor&amp;
	 **************************************************************************************************/

	//static FKinectSensor& Get();


	/**********************************************************************************************//**
	 * Default constructor.
	 *
	 * @author	Leon Rosengarten
	 **************************************************************************************************/

	FKinectSensor();

	/**********************************************************************************************//**
	 * Destructor.
	 *
	 * @author	Leon Rosengarten
	 **************************************************************************************************/

	virtual ~FKinectSensor();

	/**********************************************************************************************//**
	 * Initializes this object.
	 *
	 * @author	Leon Rosengarten
	 * @return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool Init() override;

	/**********************************************************************************************//**
	 * Gets the run.
	 *
	 * @author	Leon Rosengarten
	 * @return	An uint32.
	 **************************************************************************************************/

	virtual uint32 Run() override;

	/**********************************************************************************************//**
	 * Exits this object.
	 *
	 * @author	Leon Rosengarten
	 **************************************************************************************************/

	virtual void Exit() override;

	/**********************************************************************************************//**
	 * Stops this object.
	 *
	 * @author	Leon Rosengarten
	 **************************************************************************************************/

	virtual void Stop() override;

	void StartSensor();

	void ShutDownSensor();

	/**********************************************************************************************//**
	 * Gets body frame.
	 *
	 * @author	Leon Rosengarten
	 * @return	The body frame.
	 **************************************************************************************************/

	virtual bool GetBodyFrame(FBodyFrame& OutBodyFrame);

	/**********************************************************************************************//**
	 * Updates the color texture described by pTexture.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pTexture	If non-null, the texture.
	 **************************************************************************************************/

	virtual void UpdateColorTexture(UTexture2D* pTexture);

	/**********************************************************************************************//**
	 * Updates the infrared texture described by pTexture.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pTexture	If non-null, the texture.
	 **************************************************************************************************/

	virtual void UpdateInfraredTexture(UTexture2D* pTexture);

	/**********************************************************************************************//**
	 * Updates the depth frame texture described by pTexture.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pTexture	If non-null, the texture.
	 **************************************************************************************************/

	virtual void UpdateDepthFrameTexture(UTexture2D* pTexture);

	void UpdateBodyIndexTexture(UTexture2D* pTexture);

	/**************************************************************************************************
	 * Body to screen.
	 *
	 * @author	Leon Rosengarten
	 * @param	bodyPoint	The body point.
	 * @param	width	 	The width.
	 * @param	height   	The height.
	 *
	 * @return	A FVector2D.
	 **************************************************************************************************/

	virtual FVector2D BodyToScreen(const FVector& bodyPoint, int32 width, int32 height);


	bool IsRunning();

private:




	/**********************************************************************************************//**
	 * Process the body frame described by pArgs.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pArgs	If non-null, the arguments.
	 **************************************************************************************************/

	void ProcessBodyFrame(IBodyFrameArrivedEventArgs* pArgs);

	/**********************************************************************************************//**
	 * Process the color frame described by pArgs.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pArgs	If non-null, the arguments.
	 **************************************************************************************************/

	void ProcessColorFrame(IColorFrameArrivedEventArgs* pArgs);

	/**********************************************************************************************//**
	 * Process the infrared frame described by pArgs.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pArgs	If non-null, the arguments.
	 **************************************************************************************************/

	void ProcessInfraredFrame(IInfraredFrameArrivedEventArgs* pArgs);

	/**********************************************************************************************//**
	 * Process the depth frame described by pArgs.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pArgs	If non-null, the arguments.
	 **************************************************************************************************/

	void ProcessDepthFrame(IDepthFrameArrivedEventArgs* pArgs);

	//************************************
	// Method:    ProcessBodyIndex
	// FullName:  FKinectSensor::ProcessBodyIndex
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: IBodyIndexFrameArrivedEventArgs * pArgs
	//************************************
	void ProcessBodyIndex(IBodyIndexFrameArrivedEventArgs* pArgs);


	void ConvertBodyIndexData(const TArray<uint8> BodyIndexBufferData, RGBQUAD* pBodyIndexRGBX);

	/**********************************************************************************************//**
	 * Convert infrared data.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pInfraredBuffer	If non-null, buffer for infrared data.
	 * @param [in,out]	pInfraredRGBX  	If non-null, the infrared rgbx.
	 **************************************************************************************************/

	void ConvertInfraredData(uint16* pInfraredBuffer, RGBQUAD* pInfraredRGBX);

	/**********************************************************************************************//**
	 * Convert depth data.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pDepthBuffer	If non-null, buffer for depth data.
	 * @param [in,out]	pDepthRGBX  	If non-null, the depth rgbx.
	 * @param	minDepth				The minimum depth.
	 * @param	maxDepth				The maximum depth.
	 **************************************************************************************************/

	void ConvertDepthData(uint16* pDepthBuffer, RGBQUAD* pDepthRGBX, USHORT minDepth, USHORT maxDepth);

	/**********************************************************************************************//**
	 * Updates the texture.
	 *
	 * @author	Leon Rosengarten
	 * @param [in,out]	pTexture	If non-null, the texture.
	 * @param	pData				The data.
	 * @param	SizeX				The size x coordinate.
	 * @param	SizeY				The size y coordinate.
	 **************************************************************************************************/

	void UpdateTexture(UTexture2D* pTexture, const RGBQUAD* pData, uint32 SizeX, uint32 SizeY);

private:


	TComPtr<IKinectSensor>			m_pKinectSensor; ///< The kinect sensor

	TComPtr<IBodyFrameReader>		m_pBodyFrameReader;   ///< The body frame reader

	TComPtr<IColorFrameReader>		m_pColorFrameReader; ///< The color frame reader

	TComPtr<IDepthFrameReader>		m_pDepthFrameReader; ///< The depth frame reader

	TComPtr<IInfraredFrameReader>	m_pInfraredFrameReader;   ///< The infrared frame reader

	TComPtr<IBodyIndexFrameReader>	m_pBodyIndexFrameReader;	///< The body index frame reader

	TComPtr<IAudioBeamFrameReader>  m_pAudioBeamFrameReader;	///< The audio beam frame reader

	TComPtr<ICoordinateMapper>		m_pCoordinateMapper; ///< The coordinate mapper

	TComPtr<IKinectCoreWindow>		m_pCoreWindow;

	TComPtr<IFaceFrameSource>		m_pFaceFrameSources[BODY_COUNT];

	TComPtr<IFaceFrameReader>		m_pFaceFrameReaders[BODY_COUNT];




private:


	FCriticalSection		mBodyCriticalSection;  ///< The body critical section

	FCriticalSection		mColorCriticalSection; ///< The color critical section

	FCriticalSection		mInfraredCriticalSection;  ///< The infrared critical section

	FCriticalSection		mDepthCriticalSection; ///< The depth critical section

	FCriticalSection		mBodyIndexCriticalSection;

private:


	WAITABLE_HANDLE			BodyEventHandle;	///< Handle of the body event

	WAITABLE_HANDLE			ColorEventHandle;   ///< Handle of the color event

	WAITABLE_HANDLE			DepthEventHandle;   ///< Handle of the depth event

	WAITABLE_HANDLE			InfraredEventHandle;	///< Handle of the infrared event

	WAITABLE_HANDLE			BodyIndexEventHandle;   ///< Handle of the body index event

	WAITABLE_HANDLE			TrackingIdLostEventHandle;

	WAITABLE_HANDLE			AudioBeamEventHandle;   ///< Handle of the audio beam event

	WAITABLE_HANDLE			PointerEnteredEventHandle;

	WAITABLE_HANDLE			PointerExitedEventHandle;

	WAITABLE_HANDLE			PointerMovedEventHandle;

	WAITABLE_HANDLE			FaceEventHandle;



private:


	bool					bStop;

	FBodyFrame				BodyFrame;

	RGBQUAD*				m_pColorFrameRGBX;

	RGBQUAD*				m_pInfraredFrameRGBX;

	RGBQUAD*				m_pDepthFrameRGBX;

	RGBQUAD*				m_pBodyIndexFrameRGBX;

	FRunnableThread*		pKinectThread; ///< The kinect thread

	bool					m_bNewBodyFrame = false;

	bool					m_bNewColorFrame = false;

	bool					m_bNewDepthFrame = false;

	bool					m_bNewInfraredFrame = false;

	bool					m_bNewBodyIndexFrame = false;

	BYTE					m_ucBodyIndexFrame[cInfraredWidth*cInfraredHeight];


};

#include "HideWindowsPlatformTypes.h"