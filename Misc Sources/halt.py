import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setup(25,GPIO.OUT) 
GPIO.setup(24,GPIO.OUT) 
p=GPIO.PWM(24,1200)  
q=GPIO.PWM(25,1100)
p.start(81)
q.start(80)
p.ChangeDutyCycle(81)
q.ChangeDutyCycle(80)
time.sleep(5)
GPIO.cleanup()
