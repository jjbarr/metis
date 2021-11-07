from flask import Flask, request, send_file, jsonify
from db import Database
import serial

## SERIAL:
## drop whitespace
## 'x' clear all racks.
## 'c[0-9A-F]{8}' clear specified rack.
## 's[0-9A-F]{8}[0-9A-F]{4}' set specified slot.
## 'u[0-9A-F]{8}[0-9A-F]{4}' clear specified slot.
## 'ei' enumerate ids: send back identifiers for all connected racks.
##      Respond with [0-9A-F]{8}*X

# PORT = '/dev/ttyACM0'
PORT = "/dev/tty.usbmodem146101"
sio = serial.Serial(PORT, 9600)

db = Database("database.json")

app = Flask(__name__)


@app.route("/")
def mainpage():
    return send_file("static/index.html")


@app.route("/static/<path:file>")
def static_route(file):
    return send_from_directory("static", name)


@app.route("/reset_leds", methods=["POST"])
def reset_leds():
    sio.write(b"x")
    print("reset the leds")
    return ("", 200)


@app.route("/change_led", methods=["POST"])
def change_led():

    selected = request.json["selected"]
    row = selected = request.json["row"]
    col = request.json["col"]
    shelf_id = request.json["shelf_id"]
    name = request.json["name"]
    print(f"{selected=}")
    msg = f"{'s' if selected else 'c'}|{shelf_id}|{row}|{col}\n"  # |{name}\n"

    print(f"sending {msg!r} ...")
    sio.write(msg.encode())
    print("sent")
    return ("", 200)


@app.route("/get_db")
def get_db():
    print("runningget_db")
    return jsonify(db.data)


if __name__ == "__main__":
    app.run()
