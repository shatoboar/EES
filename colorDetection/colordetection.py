import cap as cap
import numpy as np
import cv2
import sys

# Source for most of the code + basis from Tristan's branch
# https://www.geeksforgeeks.org/multiple-color-detection-in-real-time-using-python-opencv/
img = cv2.imread(sys.argv[1])
hsvFrame = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

# set color range red and define mask
red_lower = np.array([136, 87, 111], np.uint8)
red_upper = np.array([180, 255, 255], np.uint8)
red_mask = cv2.inRange(hsvFrame, red_lower, red_upper)

# set color range green and define mask
green_lower = np.array([36, 25, 25], np.uint8)
green_upper = np.array([76, 255, 255], np.uint8)
green_mask = cv2.inRange(hsvFrame, green_lower, green_upper)

# set color range blue and define mask
blue_lower = np.array([94, 80, 2], np.uint8)
blue_upper = np.array([114, 255, 255], np.uint8)
blue_mask = cv2.inRange(hsvFrame, blue_lower, blue_upper)

# set color range yellow and define mask
yellow_lower = np.array([15, 100, 100], np.uint8)
yellow_upper = np.array([30, 255, 255], np.uint8)
yellow_mask = cv2.inRange(hsvFrame, yellow_lower, yellow_upper)

kernal = np.ones((5, 5), "uint8")

# for red color
red_mask = cv2.dilate(red_mask, kernal)
res_red = cv2.bitwise_and(img, img, mask=red_mask)

# for green colour
green_mask = cv2.dilate(green_mask, kernal)
res_green = cv2.bitwise_and(img, img, mask=green_mask)

# For blue color
blue_mask = cv2.dilate(blue_mask, kernal)
res_blue = cv2.bitwise_and(img, img, mask=blue_mask)

# For yellow color
yellow_mask = cv2.dilate(yellow_mask, kernal)
res_yellow = cv2.bitwise_and(img, img, mask=yellow_mask)


def box_creater(contour, r, g, b, name):
    x, y, w, h = cv2.boundingRect(contour)
    cv2.rectangle(img, (x, y),
                  (x + w, y + h),
                  (r, g, b), 2)

    cv2.putText(img, name, (x, y),
                cv2.FONT_HERSHEY_SIMPLEX,
                2.0, (0, 0, 0))


# show biggest box of each color
def show_all_boxes():
    box_creater(redCounter, 0, 0, 255, "Red Colour")
    box_creater(greenCounter, 0, 255, 0, "Green Colour")
    box_creater(blueCounter, 255, 0, 0, "Blue Colour")
    box_creater(yellowCounter, 255, 255, 255, "Yellow Colour")


# create contour to track color (mask)
def detect_max_color(mask):
    largestArea = 0
    largestCounter = None
    contours, hierarchy = cv2.findContours(mask,
                                           cv2.RETR_TREE,
                                           cv2.CHAIN_APPROX_SIMPLE)

    for pic, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if (area > 5000):
            if largestArea < area:
                largestArea = area
                largestCounter = contour

    return largestArea, largestCounter


largestRedArea, redCounter = detect_max_color(red_mask)
largestGreenArea, greenCounter = detect_max_color(green_mask)
largestBlueArea, blueCounter = detect_max_color(blue_mask)
largestYellowArea, yellowCounter = detect_max_color(yellow_mask)
show_all_boxes()

cv2.namedWindow("Test", cv2.WINDOW_NORMAL)  # automatically adjust size of display
cv2.imshow("Test", img)
if cv2.waitKey(10000) & 0xFF == ord('q'):  # press q to close or after 10 seconds
    cap.release()
    cv2.destroyAllWindows()
    # break