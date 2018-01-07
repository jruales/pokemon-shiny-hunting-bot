'''
Authors: Antonella Ruales and Joaquín Ruales

This Python program runs in the host computer and communicates with the Arduino.
The main objective of this program is to wait for instructions from the arduino, and once the message is received from the Arduino, capture a picture with the webcam and use OpenCV to detect if the Rowlet is shiny or not. Then, the program notifies the Arduino if it's shiny to determine if to soft reset the 3DS.
'''

import datetime
import time

import serial
import cv2
import numpy as np

WEBCAM_NUMBER = 2
ARDUINO_PORT = 'COM3'
ARDUINO_BAUD_RATE = 9600
HUMAN_CLASSIFICATION = False

# coordinates of the pixel in the middle of Rowlet's wing, to be used to determine if the Rowlet is shiny (blue) or not (orange)
TEST_PIXEL_X = 315
TEST_PIXEL_Y = 230

# coordinates of a background pixel. Used to determine if the camera took a picture of the correct screen. This background pixel should always be blue if the camera is looking at the right screen.
BACKGROUND_PIXEL_X = 200
BACKGROUND_PIXEL_Y = 250

def isItShiny():
    while True:
        # initialize camera (we need to do this every time because the camera has a frame buffer and the first frame it gives us is at the time of initialization rather than the time of capture)
        cap = cv2.VideoCapture(WEBCAM_NUMBER)
        
        # set camera gain (to have stable lighting)
        cap.set(15, -2.0)
        
        # capture webcam image
        ret, frame = cap.read()
        
        # release control of camera
        cap.release()
        
        # blur the image a little bit to avoid camera noise or compression artifacts from messing with the algorithm
        frame = cv2.GaussianBlur(frame, (11,11), 0)
        
        # Convert BGR to HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # define range of what constitutes "blue" in HSV
        lower_blue = np.array([60,20,50])
        upper_blue = np.array([120,255,255])

        # Threshold the HSV image to get a black and white image that is white (255) where the original image is blue and white (0) if not.
        mask = cv2.inRange(hsv, lower_blue, upper_blue)

        # Apply a bitwise-AND between the mask and the original image
        res = cv2.bitwise_and(frame,frame, mask= mask)
        
        # create a datetime string to be appended to saved image filenames
        currentDateTimeString = datetime.datetime.today().strftime('%Y-%m-%dT%H-%M-%S')
        
        print('img/evidence/' + currentDateTimeString + '_frame.png')
        
        # Paint test pixel red before saving the image to be able to debug problems after the fact
        frame[TEST_PIXEL_Y, TEST_PIXEL_X][2] = 255
        frame[TEST_PIXEL_Y, TEST_PIXEL_X][1] = 0
        frame[TEST_PIXEL_Y, TEST_PIXEL_X][0] = 0
        
        # Paint background pixel yellow
        frame[BACKGROUND_PIXEL_Y, BACKGROUND_PIXEL_X][2] = 255
        frame[BACKGROUND_PIXEL_Y, BACKGROUND_PIXEL_X][1] = 255
        frame[BACKGROUND_PIXEL_Y, BACKGROUND_PIXEL_X][0] = 0
        
        # Save webcam capture and classification mask
        cv2.imwrite('img/evidence/' + currentDateTimeString + '_frame.png', frame)
        cv2.imwrite('img/evidence/' + currentDateTimeString + '_mask.png', mask)
        cv2.imwrite('img/evidence/' + currentDateTimeString + '_res.png', res)
        
        if HUMAN_CLASSIFICATION:
            isShiny = input('Is this Pokemon shiny? [y/n]')
            return isShiny == 'y'
        else:
            # if the background isn't blue, we retry
            if mask[BACKGROUND_PIXEL_Y, BACKGROUND_PIXEL_X] == 0:
                print('ERROR: The background isn\'t blue! we will take another webcam picture and try again after 5 minutes')
                # sleep for 5 minutes before trying again to avoid having too many saved images
                time.sleep(5*60)
                continue
            
            # only if the test pixel (on Rowlet's wing) is blue we consider the Rowlet shiny
            if mask[TEST_PIXEL_Y, TEST_PIXEL_X] == 0:
                return False
            else:
                return True

if __name__ == '__main__':
    ser = serial.Serial(ARDUINO_PORT, ARDUINO_BAUD_RATE)
    while True:
        # Empty line
        print()

        # Wait for message from Arduino
        #message = b'Command: checkIfShiny\r\n'
        message = ser.readline()
        print("Message from Arduino: ")
        print(message)
        
        if message == b'Command: checkIfShiny\r\n':
            if isItShiny():
                print('Shiny!')
                ser.write(b'y')
            else:
                print('Not shiny')
                ser.write(b'n')
        else:
            '''
            Do nothing
            '''
