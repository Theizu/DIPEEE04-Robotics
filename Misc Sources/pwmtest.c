
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100

int main ()
{

wiringPiSetupGpio()  ;

softPwmCreate (24,0,RANGE);
softPwmCreate (25,0,RANGE);
printf("init");

softPwmWrite(24,61);
softPwmWrite(25,79);
printf("61,79");
delay(7000);

softPwmWrite(24,61);
softPwmWrite(25,80);
printf("61,80");

delay(4000);

softPwmWrite(24,61);
softPwmWrite(25,81);

printf("61,81");
delay(5000);

softPwmWrite(24,61);
softPwmWrite(25,82);
printf("61,82");

delay(4000);
softPwmWrite(24,61);
softPwmWrite(25,83);
printf("61,83");

delay(2000);



softPwmWrite(24,61);
softPwmWrite(25,84);
printf("61,84");

delay(5000);
/*
softPwmWrite(24,60);
softPwmWrite(25,60);

delay(2000);

softPwmWrite(24,70);
softPwmWrite(25,70);

delay(2000);

softPwmWrite(24,80);
softPwmWrite(25,80);

delay(2000);
softPwmWrite(24,90);
softPwmWrite(25,90);

delay(2000);

softPwmWrite(24,100);
softPwmWrite(25,100);

delay(2000);





*/




}
