import serial
from time import sleep
import datetime
import os
import sys

# Open port with baud rate
ser = serial.Serial("/dev/ttyS0", 9600)


def main():
    print("Program Running At")
    print(str(datetime.datetime.now().strftime("%Y-%m-%d")))
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
            print('Nilah ph invalid')
            continue

        # check jika bukan int
        turbidityVal = dataFilter[1]
        turbidityVal = int(phVal)
        if turbidityVal == False:
            print('Nilah Turbidity invalid')
            continue


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('Interrupted')
        try:
            ser.close()
            sys.exit(0)
        except SystemExit:
            os._exit(0)
