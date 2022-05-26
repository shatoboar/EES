import numpy as np
import cv2 
import sys

#Source for most of the code 
#https://www.geeksforgeeks.org/multiple-color-detection-in-real-time-using-python-opencv/
img = cv2.imread(sys.argv[1])

hsvFrame = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

#set color range red and define mask
red_lower = np.array([136, 87, 111], np.uint8)
red_upper = np.array([180, 255, 255], np.uint8)
red_mask = cv2.inRange(hsvFrame, red_lower, red_upper)

#set color range green and define mask
green_lower = np.array([25, 52, 72], np.uint8)
green_upper = np.array([102, 255, 255], np.uint8)
green_mask = cv2.inRange(hsvFrame, green_lower, green_upper)

#set color range blue and define mask
blue_lower = np.array([94, 80, 2], np.uint8)
blue_upper = np.array([120, 255, 255], np.uint8)
blue_mask = cv2.inRange(hsvFrame, blue_lower, blue_upper)

#set color range yellow and define mask
yellow_lower = np.array([180, 180, 0], np.uint8)
yellow_upper = np.array([255, 255, 180], np.uint8)
yellow_mask = cv2.inRange(hsvFrame, yellow_lower, yellow_upper)

kernal = np.ones((5, 5), "uint8")

#for red color
red_mask = cv2.dilate(red_mask, kernal)
res_red = cv2.bitwise_and(img, img, mask = red_mask)

#for green colour
green_mask = cv2.dilate(green_mask, kernal)
res_green = cv2.bitwise_and(img, img,
                                mask = green_mask)

 # For blue color
blue_mask = cv2.dilate(blue_mask, kernal)
res_blue = cv2.bitwise_and(img, img,
                               mask = blue_mask)

#For yellow color 
yellow_mask = cv2.dilate(yellow_mask, kernal)
res_yellow = cv2.bitwise_and(img, img, mask=yellow_mask)
    
#create contour to track red color 
contours, hierarchy = cv2.findContours(red_mask,
                                           cv2.RETR_TREE,
                                           cv2.CHAIN_APPROX_SIMPLE)
      
for pic, contour in enumerate(contours):
    area = cv2.contourArea(contour)
    if(area > 300):
        x, y, w, h = cv2.boundingRect(contour)
        img = cv2.rectangle(img, (x, y), 
                                    (x + w, y + h), 
                                    (0, 0, 255), 2)
              
        cv2.putText(img, "Red Colour", (x, y),
                    cv2.FONT_HERSHEY_SIMPLEX, 2.0                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                    (0, 0, 0))
        
#create contour to track green color
contours, hierarchy = cv2.findContours(green_mask,
                                        cv2.RETR_TREE,
                                        cv2.CHAIN_APPROX_SIMPLE)
      
for pic, contour in enumerate(contours):
    area = cv2.contourArea(contour)
    if(area > 300):
        x, y, w, h = cv2.boundingRect(contour)
        imageFrame = cv2.rectangle(img, (x, y), 
                                    (x + w, y + h),
                                    (0, 255, 0), 2)
              
        cv2.putText(img, "Green Colour", (x, y),
                    cv2.FONT_HERSHEY_SIMPLEX, 
                    2.0, (0, 0, 0))
        
#create contour to track blue color
contours, hierarchy = cv2.findContours(blue_mask,
                                        cv2.RETR_TREE,
                                        cv2.CHAIN_APPROX_SIMPLE)
for pic, contour in enumerate(contours):
    area = cv2.contourArea(contour)
    if(area > 300):
        x, y, w, h = cv2.boundingRect(contour)
        imageFrame = cv2.rectangle(img, (x, y),
                                    (x + w, y + h),
                                    (255, 0, 0), 2)
              
        cv2.putText(img, "Blue Colour", (x, y),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    2.0, (0, 0, 0))

#create contour to track yellow color
contours, hierarchy = cv2.findContours(yellow_mask, 
                                       cv2.RETR_TREE, 
                                       cv2.CHAIN_APPROX_SIMPLE)
for pic, contour in enumerate(contours):
    area = cv2.contourArea(contour)
    if(area>300):
        x, y, w, h = cv2.boundingRect(contour)
        imageFrame = cv2.rectangle(img, (x, y),
                                   (x + w, y + h),
                                   (255, 255, 255), 2)
        
        cv2.putText(img, "Yellow Colour", (x, y),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    2.0, (0, 0, 0))

    
cv2.namedWindow("Test", cv2.WINDOW_NORMAL)#automatically adjust size of display
cv2.imshow("Test", img)
if cv2.waitKey(10000) & 0xFF == ord('q'): #press q to close or after 10 seconds
    cap.release()
    cv2.destroyAllWindows()
    #break
