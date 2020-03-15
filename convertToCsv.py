#================================================================
#Authors: Sjors in 't Veld and Alifia Rachman
#This program is made to work with sketch_works_with_printed_values.ino
#================================================================

import serial
import time
import datetime
import os.path

ser = serial.Serial('COM3', 9600, timeout = 2)
now = datetime.datetime.now()
SavedData = b''
X = 0

Document = input("Name of the document?") +".csv"
while os.path.isfile(Document) == True:
    print("This document already exist do you want to add to it?")
    YesOrNo = input("Y / N")
    if YesOrNo.lower() == 'n':
        Document = input("Name of the document?") + ".csv"
    elif YesOrNo.lower() == 'y':
        print("Oke")
        break
    else:
        print("Invalid answer")


while True:
    StartData = ser.read()
    if StartData == b'\r':
        X = 0
    if StartData == b'\b':
        Data = ser.read()
        while Data != b'\n':
            SavedData = SavedData + Data
            Data = ser.read()
        if Data == b'\n':
            print(SavedData)
            with open(Document, "a") as F:
                StrSavedData = str(SavedData)
                StrSavedData = StrSavedData.replace("b'", "", 1)
                StrSavedData = StrSavedData.replace("\\r", "", 1)
                StrSavedData = StrSavedData.replace("'", "", 1)
                if X == 0:
                    F.write("\nValues at: ")
                    F.write(now.strftime("%Y-%m-%d %H:%M:%S"))
                    F.write("\n")
                    F.write("Time in millisecs,Red LED value,Yellow LED value,Green LED value,Button 2 value \n")
                    X = 1
                F.write("   ")
                F.write(StrSavedData)
                F.write("\n")
            SavedData = b''
