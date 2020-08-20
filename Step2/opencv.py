from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np

camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 480))

time.sleep(0.1)

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
   
    image = frame.array
    #cv2.imshow('color',image)
    image_gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    #cv2.imshow('gray',image_gray)
    
    ret,image_binary = cv2.threshold(image_gray,100,255,cv2.THRESH_BINARY)
    #cv2.imshow("binary",image_binary)
    
  #  img_result = cv2.bitwise_and(image,image,mask=image_binary)
   # cv2.imshow('result',img_result)
    image_canny = cv2.Canny(image_gray,100,200)
    cv2.imshow('canny',image_canny)
    
    image_lines = cv2.HoughLines(image_canny,1,np.pi/180,150)
    for line in image_lines:
        rho,theta = line[0]
        cos = np.cos(theta)
        sin = np.sin(theta)
        x0 = cos*rho
        y0 = sin*rho
        x1 = int(x0 + 1000 *(-sin))
        y1 = int(x0 + 1000 *(cos))
        x2 = int(x0 - 1000 *(-sin))
        y2 = int(x0 - 1000 *(cos))
        cv2.line(image,(x1,y1),(x2,y2),(0,0,255),1)
    
    cv2.imshow('line',image)
    
    key = cv2.waitKey(2) & 0xFF
    rawCapture.truncate(0)
    
    if key == ord('q'):
        break
        
    
cv2.destroyAllWindows()
