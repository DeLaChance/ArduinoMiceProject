import pandas as pd
import numpy as np
import sys
import os

sheetsDictionary = {}

def readExcelFile(fileName):
    print("Reading Excel file " + fileName)
    with pd.ExcelFile(fileName) as excelFile:
        for sheet in excelFile.sheet_names:
            dataframe = pd.read_excel(fileName, sheet_name=sheet, header=None)
            sheetsDictionary[str(sheet)] = dataframe.transpose().to_numpy()


def writeSheetToFiles(sheetName, sheetData):
    if (sheetData.ndim == 0):
        return

    timeColumn = sheetData[0]
    timeTransformer = lambda t: t * 0.75
    transformedTimeColumn = timeTransformer(timeColumn)

    runIndex = 0
    for runColumn in sheetData[1:]:
        runIndex += 1
        writeRunToFiles(runIndex, sheetName, runColumn, transformedTimeColumn)


def writeRunToFiles(runIndex, sheetName, runColumn, transformedTimeColumn):
    
    index = 0    
    eventsToTimestamps = [[], [], [], []];
    for value in runColumn: 
        timeValue = transformedTimeColumn[index]
        eventsToTimestamps[value-1].append(timeValue)
        index += 1

    print(eventsToTimestamps)
        
    for event in range(1,5):        
        fileName = "output/" + sheetName + "_ses-1_run_" + str(runIndex) + "_" + str(event) + ".txt"
        print("Writing to file with fileName: " + fileName)

        with open(fileName, "w") as f:
            if (len(eventsToTimestamps[event-1]) == 0):
                f.write("0 0 1\n")
            else:
                for timestamp in eventsToTimestamps[event-1]: 
                    f.write(str(timestamp) + " 1 1\n")
               
            f.close();                    

    #print(countPerEvent)        

def makeOutputDirectory():
    try:
        os.makedirs("output")
    except OSError as e:
        print(e)

if __name__ == "__main__":

    makeOutputDirectory()

    fileName = sys.argv[1]
    readExcelFile(fileName)
    for sheetName in sheetsDictionary.keys():
        print("Processing sheet: " + sheetName)
        sheetData = sheetsDictionary[sheetName];
        writeSheetToFiles(sheetName, sheetData)