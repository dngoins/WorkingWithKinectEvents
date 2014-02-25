#pragma once
/*
This is based on preliminary software and/or hardware, subject to change
*/

#include "resource.h"
#include "common.h"
#include <Kinect.h>
#include <memory>
#include <algorithm>

using namespace std; 

struct KinectEvents
{
public:
	std::unique_ptr<IKinectSensor> pKinect;
	std::unique_ptr<IInfraredFrameSource>  pSource;
	std::unique_ptr<UINT16*> pInfraredData;
	std::unique_ptr<IInfraredFrameReader> pReader;
	WAITABLE_HANDLE hIREvent;
	UINT mLengthInPixels;
	bool mIsStarted;
	std::unique_ptr<IMultiSourceFrameReader> pMultiSourceFrameReader;
	WAITABLE_HANDLE hMSEvent;

	KinectEvents() : pKinect(nullptr),
		pSource(nullptr), 
		pInfraredData(nullptr),
		pReader(nullptr),
		hIREvent(NULL),
		mLengthInPixels(0),
		mIsStarted(false),
		pMultiSourceFrameReader(nullptr),
		hMSEvent(NULL)
	{
		TRACE(L"KinectEvents Constructed");
		
		//Initialize Kinect
		IKinectSensor * pSensor = pKinect.get();
		HRESULT hr = GetDefaultKinectSensor(&pSensor);
		if (SUCCEEDED(hr))
		{
			TRACE(L"Default Kinect Retreived - HR: %d", hr);

			//we have a kinect sensor
			pKinect.reset(pSensor);
			KinectStatus status;
			hr = pKinect->get_Status(&status);

			TRACE(L"Kinect is valid device - status: %d\n", status);
		}

		//if (SUCCEEDED(hr))
		//{
		//	IMultiSourceFrameReader * preader = nullptr;
		//	hr = pKinect->OpenMultiSourceFrameReader(
		//		FrameSourceTypes::FrameSourceTypes_Infrared ,
		//		&preader);

		//	if (SUCCEEDED(hr))	{
		//		TRACE(L"Multi source frame reader retrieved");
		//		pMultiSourceFrameReader.reset(preader);
		//	}
		//}
		//
	}

	~KinectEvents()
	{
		TRACE(L"KinectEvents Destructed");
		
		if (hIREvent)
		{
			TRACE(L"Handle %d - being released...", hIREvent);
			HRESULT hr = pReader->UnsubscribeFrameArrived(hIREvent);
			if (SUCCEEDED(hr))	
				TRACE(L"Handle to InfraredFrame Event Successfully Released");
			else
				TRACE(L"Handle to InfraredFrame Event Not Released");
		}
		
		hIREvent = NULL;
		TRACE(L"Handle to InfraredFrame set to NULL");

		if (hMSEvent)
		{
			TRACE(L"Handle %d - being released...", hMSEvent);
			HRESULT hr = pMultiSourceFrameReader->UnsubscribeMultiSourceFrameArrived(hMSEvent);
			if (SUCCEEDED(hr))
				TRACE(L"Handle to MultiSource Frame Event Successfully Released");
			else
				TRACE(L"Handle to MultiSource Frame Event Not Released");
		}

		hMSEvent = NULL;
		TRACE(L"Handle to MultiSource Frame Event set to NULL");

		pReader.release();
		pReader = nullptr;
		TRACE(L"InfraredFrame Reader Released");

		pInfraredData.release();
		pInfraredData = nullptr;
		TRACE(L"InfraredFrame Data buffer Released");


		pSource.release();
		pSource = nullptr;
		TRACE(L"InfraredFrameSource Released");

		pMultiSourceFrameReader.release();
		pMultiSourceFrameReader = nullptr;
		TRACE(L"Multi Source Frame Reader Released");

		
		if (pKinect)
		{
			HRESULT hr = pKinect->Close();
			TRACE(L"Closing Kinect - HR: %d", hr);

			HR(hr);

			TRACE(L"HR : %d", hr);
			pKinect.release();
			pKinect = nullptr;			
			TRACE(L"Kinect resources released.");
		}

	}

	void Start()
	{		

		ASSERT(pKinect);
		if (!mIsStarted)
		{
			
			ICoordinateMapper * m_pCoordinateMapper = nullptr;
			HRESULT hr = pKinect->get_CoordinateMapper(&m_pCoordinateMapper);
				TRACE(L"Retrieved CoordinateMapper- HR: %d", hr);
			
			IBodyFrameSource* pBodyFrameSource = nullptr;			

			if (SUCCEEDED(hr))
			{
				hr = pKinect->get_BodyFrameSource(&pBodyFrameSource);
				TRACE(L"Retrieved Body Frame Source - HR: %d", hr);
			}
			IBodyFrameReader * pBodyFrameReader = nullptr;
			if (SUCCEEDED(hr))
			{
				hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
				TRACE(L"Opened Kinect Reader - HR: %d", hr);
			}

			

			IInfraredFrameSource * pIRSource = nullptr;
			if (SUCCEEDED(hr))
			{
				hr = pKinect->get_InfraredFrameSource(&pIRSource);
				TRACE(L"Retrieved IR Frame Source - HR: %d", hr);
			}

			if (SUCCEEDED(hr)){
				TRACE(L"Kinect has not started yet... Opening");
				hr = pKinect->Open();
				TRACE(L"Opened Kinect - HR: %d", hr);
			}

			//if (SUCCEEDED(hr)){
			//	hr = pMultiSourceFrameReader->SubscribeMultiSourceFrameArrived(&hMSEvent);
			//	TRACE(L"Subscribed to MultisourceFrameEvent - HR: %d - EventID: %d", hr, hMSEvent);
			//}

			//IMultiSourceFrame * pMsFrame = nullptr;
			//if (SUCCEEDED(hr))
			//{
			//	hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMsFrame);
			//	TRACE(L"Retreived latest IR FRAME - HR: %d", hr);
			//}

			//IInfraredFrameReference * pIrRef = nullptr;
			//if (SUCCEEDED(hr))
			//{
			//	hr = pMsFrame->get_InfraredFrameReference(&pIrRef);
			//	TRACE(L"Retreived IR FRAME reference - HR: %d", hr);
			//}
			//pMsFrame->Release();

			//IInfraredFrame * pIrFrame = nullptr;
			//if (SUCCEEDED(hr))
			//{				
			//	hr = pIrRef->AcquireFrame(&pIrFrame);
			//	TRACE(L"Retreived IR FRAME  - HR: %d", hr);
			//}
			//pIrRef->Release();

			////Allocate a buffer
			IFrameDescription * pIRFrameDesc = nullptr;
			//if (SUCCEEDED(hr))
			//{
			//	hr = pIrFrame->get_FrameDescription(&pIRFrameDesc);
			//	TRACE(L"Retreived IR FRAME Description - HR: %d", hr);
			//}

			//IInfraredFrameSource * pIRSource = nullptr;
			
			if (SUCCEEDED(hr)){
//				hr = pIrFrame->get_InfraredFrameSource(&pIRSource);
				pSource.reset(pIRSource);
				hr = pIRSource->get_FrameDescription(&pIRFrameDesc);
				TRACE(L"Retreived IR FRAME Source - HR: %d", hr);

			}
			//pIrFrame->Release();

			UINT lengthInPixels = 0;
			if (SUCCEEDED(hr)){

			//	pSource.reset(pIRSource);
				hr = pIRFrameDesc->get_LengthInPixels(&lengthInPixels);
				TRACE(L"Retreived IR FRAME Description Pixel Length", hr);
			}


			auto ret = pIRFrameDesc->Release();
			TRACE(L"IR FrameDescription Released %d", ret);

			IInfraredFrameReader * pIRReader = nullptr;
			if (SUCCEEDED(hr)){
				TRACE(L"Length In Pixels: %d", lengthInPixels);
				mLengthInPixels = lengthInPixels;
				pInfraredData = make_unique<UINT16*>(new UINT16[lengthInPixels]);

				
				hr = pSource->OpenReader(&pIRReader);
				TRACE(L"Opened IR Reader");
			}

			if (SUCCEEDED(hr)){

				pReader.reset(pIRReader);
				hr = pReader->SubscribeFrameArrived(&hIREvent);
				TRACE(L"Reader Accessed Successfully");
				TRACE(L"Subscribe to Frame Arrived Event call - HR: %d", hr);

			}
			//pIRReader->Release();

			if (SUCCEEDED(hr)){
				TRACE(L"Successfully Subscribed to Frame Arrived EventID: %d", (UINT)hIREvent);
			}

			//pReader->Release();
			//pSource->Release();
			//pMultiSourceFrameReader->Release();

			mIsStarted = true;
		}
	}

	void InfraredFrameArrived(IInfraredFrameArrivedEventArgs* pArgs)
	{
		TRACE(L"IR Framed event arrived");
		ASSERT(pArgs);
		IInfraredFrameReference * pFrameRef = nullptr;

		HRESULT hr = pArgs->get_FrameReference(&pFrameRef);


		if (SUCCEEDED(hr)){
			//we have a frame reference
			//Now Acquire the frame
			TRACE(L"We have a frame reference - HR: %d", hr);
			bool processFrameValid = false;
			IInfraredFrame* pFrame = nullptr;
			TIMESPAN relativeTime = 0;
			hr = pFrameRef->AcquireFrame(&pFrame);


			if (SUCCEEDED(hr)){

				TRACE(L"We have acquired a frame - HR : %d", hr);

				//Now copy the frames data to the buffer
				hr = pFrame->CopyFrameDataToArray(mLengthInPixels, *pInfraredData);
				
				if (SUCCEEDED(hr)){
					TRACE(L"We have successfully copied ir frame data to buffer");
					processFrameValid = true;
					hr = pFrame->get_RelativeTime(&relativeTime);
					TRACE(L"Relative Time: - HR: %d\t Time: %d", hr, relativeTime);

				}
				auto ret = pFrame->Release();
				TRACE(L"IR Frame released: %d", ret);
			}

			auto ret = pFrameRef->Release();
			TRACE(L"IR Frame Reference released: %d", ret);

			
			if (processFrameValid)
				ProcessFrame(mLengthInPixels, *pInfraredData, relativeTime);

		}


	}

	void ProcessFrame(UINT length, UINT16 * pBuffer, TIMESPAN relativeTime)
	{
		TRACE(L"Process Frame Called.\nBufferLength: %d\n\tTimeSpan: %d", length, relativeTime);

	}
};

void StartKinect();