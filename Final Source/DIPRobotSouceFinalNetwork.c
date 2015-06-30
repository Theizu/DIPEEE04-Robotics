
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <XnCppWrapper.h>
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
char data[2000];
int countertime=0;
///////////////////////NETWORKING/////////////////////////////////////////////
void error(char *msg) {
    perror(msg);
    exit(0);
}
int getData( int sockfd ) {
  char buffer[32];
  int n;

  if ( (n = read(sockfd,buffer,31) ) < 0 )
       error( const_cast<char *>( "ERROR reading from socket") );
  buffer[n] = '\0';
  return atoi( buffer );
}
//void getData (int sockfd) 
//{
//Receive a reply from the server
//char reply[2000];
//if( recv(sockfd, reply , 2000 , 0) < 0)
//{
//puts("recv failed");
//}
//puts("Reply received\n");
//puts(reply);
//reply=data;
//}


////////////////////////////////////////////////////////////////////////////////


XnBool fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}
int system(const char *command);
int main(int argc, char *argv[])
{
///////////////////NETWORK INIT///////////////////////
 int sockfd, portno = 51717, n;
    char serverIp[] = "192.168.0.100";
    struct sockaddr_in serv_addr;
    struct hostent *server;
	char *message ;
	char server_reply[2000];
   int reply;
   
//////////////////////////////////////////////////////
 int haltmany=0;
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
XnDepthPixel FarL;
XnDepthPixel FarR;
XnDepthPixel EdgeL;
XnDepthPixel EdgeR;


int halttime=0;
int lefttime=0;
int righttime=0;

	const char *fn = NULL;

	nRetVal = xnEnumerationErrorsAllocate(&pErrors);
	CHECK_RC(nRetVal, "Allocate errors object");

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
	
	////////////////////////////////////NETWORKING CONNECTING///////////////////////
	if (argc < 3) {
      // error( const_cast<char *>( "usage myClient2 hostname port\n" ) );
      printf( "contacting %s on port %d\n", serverIp, portno );
      // exit(0);
    }
	if ( ( sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
        error( const_cast<char *>( "ERROR opening socket") );

    if ( ( server = gethostbyname( serverIp ) ) == NULL ) 
        error( const_cast<char *>("ERROR, no such host\n") );
    
    bzero( (char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error( const_cast<char *>( "ERROR connecting") );

		
	/////////////////////////////////////////////////////////////////////////////////
	softPwmWrite(24,61);   //"Pause Before Starting"
    softPwmWrite(25,61);
	delay(700);
	
	/////////////////////////////////////////////////////////////////////////////////
	
	while (!xnOSWasKeyboardHit())
	//while (!xnOSWasKeyboardHit())
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
        FarL = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 1];
        FarR = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 159];
        EdgeL = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 25]; 
        EdgeR = pDepthMap[pDepthMD->pMap->Res.X * pDepthMD->pMap->Res.Y/2 + 121];

		
        printf("Frame %d Middle point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, middlePoint);
        printf("Frame %d Right point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, FarR);
        printf("Frame %d Left point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, FarL);
        printf("Frame %d Edge Left point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, EdgeL);
        printf("Frame %d Edge Right point is: %u\n", pDepthMD->pMap->pOutput->nFrameID, EdgeR);
       
//message = middlepoint+"|"+FarR;
//if( send(sockfd , message , strlen(message) , 0) < 0)
//{
//puts("Send failed");
//}
//puts("Data Send\n");

//Receive a reply from the server
//if( recv(sockfd, server_reply , 2000 , 0) < 0)
//{
//puts("recv failed");
//}
//puts("Reply received\n");
//puts(server_reply);



/////////////////////////////////ALGO CODE STARTS HERE///////////////////////////




if((middlePoint==0)&&(EdgeL==0)&&(EdgeR==0))
{
//////////////////////////////////////////////////////////////////////////////////
if(halttime<30){
//halt
softPwmWrite(24,61);
softPwmWrite(25,61);
halttime=halttime+1;
printf("Collision Imminent: NOTHING\n");
message = "@Halt|";
if( send(sockfd , message , strlen(message) , 0) < 0)
{
puts("Send failed");
}
puts("Data Send\n");


}
else{
if (countertime==0){
halttime=0;
softPwmWrite(24,0);  
softPwmWrite(25,100); 
printf("STUCKED SO LONG, TURNING Left\n"); 
countertime=countertime+1;
message = "@StuckedLeft|";
if( send(sockfd , message , strlen(message) , 0) < 0)
{
puts("Send failed");
}
puts("Data Send\n");

delay(290);
countertime=countertime+1;
}
else{
halttime=0;
softPwmWrite(24,0);  
softPwmWrite(25,100); 
printf("STUCKED SO LONG, TURNING 180\n"); 
countertime=countertime+1;
message = "@Turn180|";
if( send(sockfd , message , strlen(message) , 0) < 0)
{
puts("Send failed");
}
puts("Data Send\n");

delay(1000);
countertime=0;


}
}
}
//////////////////////////////////////////////////////////////////////////////////

else if((EdgeL!=0)&&(EdgeR==0))
//right (left obsticle)
{


halttime=0;

//Front

softPwmWrite(24,67);   //100 (4/6)   //0    //left
softPwmWrite(25,55);   //52     //61         //right
delay(130);

printf("Turning Right : Obstacle on the Left\n");

message = "@Right|";
if( send(sockfd , message , strlen(message) , 0) < 0)
{
puts("Send failed");
}
puts("Data Send\n");


}
else if((EdgeL==0)&&(EdgeR!=0))
//left  (right obsticle)
{

halttime=0;
//front

softPwmWrite(24,55);
softPwmWrite(25,67);
delay(125);
printf("Turning Left : Obstacle on the Right\n");

message = "@Left|";
if( send(sockfd , message , strlen(message) , 0) < 0)
{
puts("Send failed");
}
puts("Data Send\n");



}


else
//((middlePoint!=0)&&(EdgeL!=0)&&(EdgeR!=0))
{
//forward
halttime=0;
lefttime=0;
righttime=0;
countertime=0;
//front
softPwmWrite(24,67);
softPwmWrite(25,67);

printf("GOGOGOGO Lets Goooooo!\n");

message = "@Front|";
if( send(sockfd , message , strlen(message) , 0) < 0)
{
puts("Send failed");
}
puts("Data Send\n");


}




/////////////////////////////CODE ENDS HERE/////////////////////////
	
	
	
	}
	xnFreeDepthMetaData(pDepthMD);
	xnProductionNodeRelease(hDepth);
	xnProductionNodeRelease(hScriptNode);
	xnContextRelease(pContext);
	close( sockfd );
	return 0;
}
