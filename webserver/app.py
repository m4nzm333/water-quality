"""
    Program WEB untuk menampilkan data ke Website menggunakan Flask
"""

from flask import Flask, request, abort, json, render_template
import mysql.connector

app = Flask(__name__, static_url_path="", static_folder="static")

# Halaman index
@app.route("/")
def hello_world():
    return render_template("index.html")


# Mengambil data berdasarkan tanggal
@app.route("/getByDate")
def getByDate():
    queryDate = request.args.get("date")
    sensor = request.args.get("sensor")
    idAlat = request.args.get("id")
    if queryDate is None or sensor is None or idAlat is None:
        abort(400, "Missing Parameter")

    mydb = mysql.connector.connect(
        host="localhost",
        user="root",
        password="password",
        port="3306",
        database="waterquality",
    )
    mycursor = mydb.cursor(dictionary=True)

    mycursor.execute(
        "SELECT * FROM {sensor} WHERE waktu LIKE '{date}%' AND id_alat = '{idAlat}' ORDER BY waktu ASC".format(
            sensor=sensor, date=queryDate, idAlat=idAlat
        )
    )
    myresult = mycursor.fetchall()

    returnResult = []
    for x in myresult:
        x["waktu"] = str(x["waktu"])
        returnResult.append(x)

    try:
        mycursor.close()
        mydb.close()
    except:
        None

    return app.response_class(
        response=json.dumps(returnResult), status=200, mimetype="application/json"
    )


# Mengambil data terakhir
@app.route("/getLastRow")
def getLasRow():
    sensor = request.args.get("sensor")
    idAlat = request.args.get("id")
    lastDate = request.args.get("last")
    if None or sensor is None or idAlat is None or lastDate is None:
        abort(400, "Missing Parameter")

    mydb = mysql.connector.connect(
        host="localhost",
        user="root",
        password="password",
        port="3306",
        database="waterquality",
    )
    mycursor = mydb.cursor(dictionary=True)

    mycursor.execute(
        "SELECT * FROM {sensor} WHERE id_alat = '{idAlat}' AND waktu > '{lastDate}' ORDER BY waktu ASC".format(
            sensor=sensor, idAlat=idAlat, lastDate=lastDate
        )
    )
    myresult = mycursor.fetchall()

    returnResult = []
    for x in myresult:
        x["waktu"] = str(x["waktu"])
        returnResult.append(x)

    try:
        mycursor.close()
        mydb.close()
    except:
        None

    return app.response_class(
        response=json.dumps(myresult), status=200, mimetype="application/json"
    )


# Menjalankan program Flask
app.run(host="0.0.0.0", port=8080, debug=True, load_dotenv=True)
