import serial
from time import sleep
import datetime
import os
import sys
import mysql.connector

# Open port with baud rate
ser = serial.Serial("/dev/ttyS0", 9600)

mydb = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    port="3306",
    database="waterquality"
)
mycursor = mydb.cursor()
mycursor.execute("CREATE TABLE IF NOT EXISTS ph ( id_data INT AUTO_INCREMENT PRIMARY KEY, id_alat VARCHAR(3), waktu TIMESTAMP DEFAULT CURRENT_TIMESTAMP, nilai FLOAT);")
mycursor.execute("CREATE TABLE IF NOT EXISTS kekeruhan ( id_data INT AUTO_INCREMENT PRIMARY KEY, id_alat VARCHAR(3), waktu TIMESTAMP DEFAULT CURRENT_TIMESTAMP, nilai INT);")
mycursor.close()


def main():
    print("Program Running At")
    print(str(datetime.datetime.now()))
    print(mydb)
    while True:
        # Bikin folder untuk datalog
        if not os.path.exists('datalog'):
            os.makedirs('datalog')
        # Bikin folder untuk tahun sekarang
        thisYear = str(datetime.datetime.now().year)
        if not os.path.exists('datalog/{year}'.format(year=thisYear)):
            os.makedirs('datalog/{year}'.format(year=thisYear))
        # Bikin file untuk datalog
        thisDate = str(datetime.datetime.now().strftime("%Y-%m-%d"))
        theFile = open(
            "datalog/{year}/{date}".format(year=thisYear, date=thisDate), "a")
        # Ambil data waktu sekarang
        thisTime = str(datetime.datetime.now())

        received_data = ser.read()  # read serial port
        sleep(0.03)
        data_left = ser.inWaiting()  # check for remaining byte
        received_data += ser.read(data_left)
        print(received_data)  # print received data
        ser.write(received_data)

        # Masukkan ke log
        theFile.write("{data};{time}\n".format(
            data=received_data, time=thisTime))
        theFile.close()

        # Filter Data
        # Check jika tidak 3 bagian
        dataFilter = str(received_data).split(';')
        if len(dataFilter) != 3:
            print('!!! Jumlah Alat tidak lengkap')
            continue
        # Cek jika id Alat bukan mulai dari A
        idAlat = dataFilter[0]
        if idAlat[0] != 'A':
            print('!!! Id Alat tidak ditemukan')
            continue

        # check jika bukan float
        phVal = dataFilter[1]
        phVal = float(phVal)
        if phVal == False:
            print('Nilai ph invalid')
            continue
        # cek jika tidak sesuai dengan range
        if phVal <= 0 or phVal >= 14:
            print('Nilai ph invalid')
            continue

        # check jika bukan int
        turbidityVal = dataFilter[1]
        turbidityVal = int(phVal)
        if turbidityVal == False:
            print('Nilai Turbidity invalid')
            continue
        # cek jika tidak sesuai dengan range
        if turbidityVal < 0 or turbidityVal > 3000:
            print('Nilai ph invalid')
            continue
            
        # insert to DB
        
        mycursor = mydb.cursor()
        mycursor.execute("INSERT INTO ph (id_alat, nilai) VALUES ('{idAlat}', {value})".format(idAlat = idAlat, value = phVal))
        mycursor.execute("INSERT INTO kekeruhan (id_alat, nilai) VALUES ('{idAlat}', {value})".format(idAlat = idAlat, value = turbidityVal))
        mydb.commit()
        mycursor.close()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('Interrupted')
        try:
            mydb.close()
            ser.close()
            sys.exit(0)
        except SystemExit:
            os._exit(0)
