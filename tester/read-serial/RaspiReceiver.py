import mysql.connector


mydb = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    port="3306",
    database="waterquality"
)

mycursor = mydb.cursor()
mycursor.execute("CREATE TABLE IF NOT EXISTS ph ( id_data INT AUTO_INCREMENT PRIMARY KEY, id_alat VARCHAR(3), waktu TIMESTAMP DEFAULT CURRENT_TIMESTAMP, nilai FLOAT);")
mycursor.execute("CREATE TABLE IF NOT EXISTS kekeruhan ( id_data INT AUTO_INCREMENT PRIMARY KEY, , id_alat VARCHAR(3), waktu TIMESTAMP DEFAULT CURRENT_TIMESTAMP, nilai INT);")

turbidityValue = 19

mycursor.execute("INSERT INTO kekeruhan (nilai) VALUES ({turbidityValue})".format(turbidityValue = turbidityValue))
mydb.commit()

mycursor.close()
mydb.close()