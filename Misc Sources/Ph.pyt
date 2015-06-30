import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setup(25,GPIO.OUT)
GPIO.setup(24,GPIO.OUT)
p=GPIO.PWM(25,50)
q=GPIO.PWM(24,50)
p.start(50)
q.start(50)
p.ChangeDutyCycle(77)
q.ChangeDutyCycle(77)

