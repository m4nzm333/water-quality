from flask import Flask
from flask import request
from flask import abort
import mysql.connector
from flask import json

app = Flask(__name__)


@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"


@app.route("/getByDate")
def getByDate():
    queryDate = request.args.get('date')
    sensor = request.args.get('sensor')
    idAlat = request.args.get('id')
    if queryDate is None or sensor is None or idAlat is None:
        abort(400, 'Missing Parameter')

    mydb = mysql.connector.connect(
        host="localhost",
        user="root",
        password="",
        port="3306",
        database="waterquality"
    )
    mycursor = mydb.cursor(dictionary=True)

    mycursor.execute("SELECT * FROM {sensor} WHERE waktu LIKE '{date}%' AND id_alat = '{idAlat}' ORDER BY waktu ASC".format(
        sensor=sensor, date=queryDate, idAlat=idAlat))
    myresult = mycursor.fetchall()

    mycursor.close()
    mydb.close()

    print(queryDate)
    return app.response_class(
        response=json.dumps(myresult),
        status=200,
        mimetype='application/json'
    )


@app.route("/getLastRow")
def getLasRow():
    sensor = request.args.get('sensor')
    idAlat = request.args.get('id')
    if None or sensor is None or idAlat is None:
        abort(400, 'Missing Parameter')

    mydb = mysql.connector.connect(
        host="localhost",
        user="root",
        password="",
        port="3306",
        database="waterquality"
    )
    mycursor = mydb.cursor(dictionary=True)

    print("SELECT * FROM {sensor} WHERE id_alat = '{idAlat}' ORDER BY waktu DESC".format(sensor=sensor, idAlat=idAlat))
    mycursor.execute("SELECT * FROM {sensor} WHERE id_alat = '{idAlat}' ORDER BY waktu DESC".format(sensor=sensor, idAlat=idAlat))
    myresult = mycursor.fetchone()

    # mycursor.close()
    # mydb.close()
    
    if mydb:
        mydb.close()

    return app.response_class(
        response=json.dumps(myresult),
        status=200,
        mimetype='application/json'
    )


app.run(port=8080,  debug=True)
