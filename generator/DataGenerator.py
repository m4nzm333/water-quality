# ==========
# GENERATOR DATA (Jangan ditampilkan)
# ==========

from datetime import datetime, timedelta
from time import sleep
from random import randint, uniform
import mysql.connector
from threading import Thread

# Init Generator
timeStart = "2022-07-28 16:00:00.000000"
timeEnd = "2022-07-28 17:00:00.000000"
timeStartObj = datetime.strptime(timeStart, "%Y-%m-%d %H:%M:%S.%f")
timeEndObj = datetime.strptime(timeEnd, "%Y-%m-%d %H:%M:%S.%f")

arrayAllData = []


def generateData(idAlat, phMin, phMax, turbidityMin, turbidityMax):

    timeCurrent = timeStartObj
    while timeEndObj > timeCurrent:
        randomDelay = randint(5000000, 6000000)
        randomPh = round(uniform(phMin, phMax), 2)
        randomTurbidity = round(uniform(turbidityMin, turbidityMax), 0)
        timeCurrent = timeCurrent + timedelta(microseconds=randomDelay)
        timeCurrentString = timeCurrent.strftime("%Y-%m-%d %H:%M:%S.%f")
        arrayData = [
            idAlat,
            "{:.2f}".format(round(randomPh, 3)),
            str(int(randomTurbidity)),
            timeCurrentString,
        ]
        # Add to File
        generatedFile = open("./generatedData-{idAlat}.csv".format(idAlat=idAlat), "a")
        stringData = ";".join(arrayData)
        generatedFile.write(stringData)
        generatedFile.write("\n")
        generatedFile.close()
        # Push to array Data
        arrayAllData.append(
            {
                "idAlat": idAlat,
                "ph": randomPh,
                "kekeruhan": randomTurbidity,
                "waktu": timeCurrentString,
            }
        )


generateData("A1", 7.88, 8.24, 50, 200)
generateData("A2", 7.94, 8.15, 150, 250)
generateData("A3", 7.75, 7.98, 100, 200)
