# ==========
# GENERATOR DATA (Jangan ditampilkan)
# ==========

from datetime import datetime, timedelta
from time import sleep
from random import randint, uniform
import mysql.connector
from threading import Thread

# Init Generator
timeStart = "2022-09-28 16:00:00.000000"
timeEnd = "2022-09-28 17:00:00.000000"
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
                "kekeruhan": 0,
                "waktu": timeCurrentString,
            }
        )


if __name__ == "__main__":

    thread1 = Thread(target=generateData, args=("A1", 7.88, 8.24, 50, 200))
    thread2 = Thread(target=generateData, args=("A2", 7.94, 8.15, 150, 250))
    thread3 = Thread(target=generateData, args=("A3", 7.75, 7.98, 100, 200))
    thread1.start()
    thread2.start()
    thread3.start()
    thread1.join()
    thread2.join()
    thread3.join()

    sleep(5)

    # SQL Create Table
    mydb = mysql.connector.connect(
        host="localhost",
        user="root",
        password="password",
        port="3306",
        database="waterquality",
    )
    mycursor = mydb.cursor()
    mycursor.execute(
        "CREATE TABLE IF NOT EXISTS ph ( id_data INT AUTO_INCREMENT PRIMARY KEY, id_alat VARCHAR(3), waktu TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6), nilai FLOAT);"
    )
    mycursor.execute(
        "CREATE TABLE IF NOT EXISTS kekeruhan ( id_data INT AUTO_INCREMENT PRIMARY KEY, id_alat VARCHAR(3), waktu TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6), nilai INT);"
    )
    mycursor.close()
    # SQL Generated Data to DB
    arrayAllData = sorted(arrayAllData, key=lambda d: d["waktu"])
    for rowData in arrayAllData:
        mycursor = mydb.cursor()
        mycursor.execute(
            "INSERT INTO ph (id_alat, nilai, waktu) VALUES ('{idAlat}', {value}, '{waktu}')".format(
                idAlat=rowData["idAlat"], value=rowData["ph"], waktu=rowData["waktu"]
            )
        )
        mycursor.execute(
            "INSERT INTO kekeruhan (id_alat, nilai, waktu) VALUES ('{idAlat}', {value}, '{waktu}')".format(
                idAlat=rowData["idAlat"], value=0, waktu=rowData["waktu"]
            )
        )
        mydb.commit()
        mycursor.close()

    print("thread finished...exiting")
