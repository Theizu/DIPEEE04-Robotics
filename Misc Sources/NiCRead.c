
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <errno.h>
//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"
#define SAMPLE_XML_PATH_LOCAL "SamplesConfig.xml"
#define RANGE		100
//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
		return rc;													\
	}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnBool fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}
int system(const char *command);
int main()
{

    wiringPiSetupGpio()  ;
	softPwmCreate (24,0,RANGE);
    softPwmCreate (25,0,RANGE);
	XnStatus nRetVal = XN_STATUS_OK;
	XnContext* pContext;
	XnNodeHandle hScriptNode;
	XnEnumerationErrors* pErrors;
	XnNodeHandle hDepth;
	XnDepthMetaData* pDepthMD;
	const XnDepthPixel* pDepthMap;
	XnDepthPixel middlePoint;
XnDepthPixel FourPoint;
XnDepthPixel EightPoint;
XnDepthPixel EdgeL;
XnDepthPixel EdgeR;
int countexit = 30;


	const char *fn = NULL;

	nRetVal = xnEnumerationErrorsAllocate(&pErrors);
	CHECK_RC(nRetVal, "Allocate errors object");
//system("sudo python -c'import RPi.GPIO as GPIO;import time;GPIO.setmode(GPIO.BCM);GPIO.setup(25,GPIO.OUT);GPIO.setup(24,GPIO.OUT);p=GPIO.PWM(24,1200);q=GPIO.PWM(25,1100);p.start(81);q.start(80);time.sleep(5);'");

	if	(fileExists(SAMPLE_XML_PATH)) fn = SAMPLE_XML_PATH;
	else if (fileExists(SAMPLE_XML_PATH_LOCAL)) fn = SAMPLE_XML_PATH_LOCAL;
	else {
		printf("Could not find '%s' nor '%s'. Aborting.\n" , SAMPLE_XML_PATH, SAMPLE_XML_PATH_LOCAL);
		return XN_STATUS_ERROR;
	}
	printf("Reading config from: '%s'\n", fn);
	nRetVal = xnInitFromXmlFileEx(fn, &pContext, pErrors, &hScriptNode);
	if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		xnEnumerationErrorsToString(pErrors, strError, 1024);
		printf("%s\n", strError);
		xnEnumerationErrorsFree(pErrors);
		return (nRetVal);
	}
	else if (nRetVal != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(nRetVal));
		xnEnumerationErrorsFree(pErrors);
		return (nRetVal);
	}

	xnEnumerationErrorsFree(pErrors);

	nRetVal = xnFindExistingRefNodeByType(pContext, XN_NODE_TYPE_DEPTH, &hDepth);
	CHECK_RC(nRetVal, "Find depth generator");

	pDepthMD = xnAllocateDepthMetaData();
	
	while (!xnOSWasKeyboardHit())
	{
		nRetVal = xnWaitOneUpdateAll(pContext, hDepth);
		if (nRetVal != XN_STATUS_OK)
		{
			printf("UpdateData failed: %s\n", xnGetStatusString(nRetVal));
			continue;
		}

		xnGetDepthMetaData(hDepth, pDepthMD);
		pDepthMap = pDepthMD->pData;
		middlePoint = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + pDepthMD->pMap->Res.X/2];
        FourPoint = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 1];
        EightPoint = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 159];
        EdgeL = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 40]; 
        EdgeR = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 159];
        printf("Frame %d Middle point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, middlePoint);
        printf("Frame %d Right point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, FourPoint);
        printf("Frame %d Left point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, EightPoint);
        printf("Frame %d Edge Left point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, EdgeL);
        printf("Frame %d Edge Right point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, EdgeR);

/////////////////////////////////ALGO CODE STARTS HERE///////////////////////////
//if((middlePoint==0)&&(EdgeL==0)&&(EdgeR==0))
//{

//halt

/*
softPwmWrite(24,61);
softPwmWrite(25,80);

softPwmWrite(24,0);
softPwmWrite(25,61);

*/

//printf("Collision Imminent: NOTHING\n");

//}

if((middlePoint!=0)&&(EdgeL!=0)&&(EdgeR!=0))
{
//forward

softPwmWrite(24,61);
softPwmWrite(25,100);
printf("GOGOGOGO Lets Goooooo!\n");
}
if((EdgeL!=0)&&(EdgeR==0))
//if(((middlePoint!=0)&&(EdgeL!=0)&&(EdgeR==0))||((middlePoint==0)&&(EdgeL!=0)&&(EdgeR==0)))
//right (left obsticle)
{

softPwmWrite(24,0);
softPwmWrite(25,61);

printf("Turning Right : Obstacle on the Left\n");
//delay(250);

}

if((EdgeL==0)&&(EdgeR!=0))
//if(((middlePoint!=0)&&(EdgeL==0)&&(EdgeR!=0))||((middlePoint==0)&&(EdgeL==0)&&(EdgeR!=0)))
//left  (right obsticle)
{

softPwmWrite(24,100);
softPwmWrite(25,61);

printf("Turning Left : Obstacle on the Right\n");
//delay(200);

}

/////////////////////////////CODE ENDS HERE/////////////////////////
/*
//left
softPwmWrite(24,100);
softPwmWrite(25,61);
//right
softPwmWrite(24,0);
softPwmWrite(25,61);
//forward
softPwmWrite(24,61);
softPwmWrite(25,100);
//reverse
softPwmWrite(24,61);
softPwmWrite(25,0);
*/
	}

	xnFreeDepthMetaData(pDepthMD);

	xnProductionNodeRelease(hDepth);
	xnProductionNodeRelease(hScriptNode);
	xnContextRelease(pContext);

	return 0;
}
