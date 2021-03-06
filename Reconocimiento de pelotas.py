import cv2
import numpy as np
import urllib.request
import requests
url='http://192.168.4.2/cam-hi.jpg' #url de la cámara ESP32 Cam
resx=800 #resolución de cámara
x=0
verdeBajo = np.array([40,45,107],np.uint8)   #Frontera inferior de color verde 
verdeAlto = np.array([60,255,255],np.uint8)  #Frontera superior de color verde
while True:
  
  img_resp=urllib.request.urlopen(url)
  imgnp=np.array(bytearray(img_resp.read()),dtype=np.uint8)
  frame = cv2.imdecode(imgnp,-1)
  frameHSV = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
  mask = cv2.inRange(frameHSV,verdeBajo,verdeAlto)
  contornos,_ = cv2.findContours(mask, cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_SIMPLE)
    #cv2.drawContours(frame, contornos, -1, (255,0,0), 3)
  for c in contornos:
    area = cv2.contourArea(c)
    if area > 300:
      M = cv2.moments(c)
      if (M["m00"]==0): M["m00"]=1 
      x = int(M["m10"]/M["m00"])
      y = int(M['m01']/M['m00'])
      cv2.circle(frame, (x,y), 7, (0,255,0), -1)
      font = cv2.FONT_HERSHEY_SIMPLEX
      cv2.putText(frame, '{},{}'.format(x,y),(x+10,y), font, 0.75,(0,255,0),1,cv2.LINE_AA)
      nuevoContorno = cv2.convexHull(c)
      cv2.drawContours(frame, [nuevoContorno], 0, (255,0,0), 3)
    #cv2.imshow('maskAzul',mask)
  cv2.imshow('ESP32 Cam',frame)
  if x > resx/2+10:
    res=requests.post("http://192.168.4.1/right")
  else: 
    if 0 < x < resx/2-10:
      r=requests.post("http://192.168.4.1/left")
    else: 
      if resx/2-10 < x < resx/2+10: 
        r=requests.post("http://192.168.4.1/center")
      else:
        r=requests.post("http://192.168.4.1/null") 
  x=0
  if cv2.waitKey(1) & 0xFF == ord('s'):
    break
cap.release()
cv2.destroyAllWindows()
