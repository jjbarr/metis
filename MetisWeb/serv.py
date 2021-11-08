from flask import Flask, request, send_file, jsonify

# from db import Database
import serial
import json

ENABLE_SELECTION = True
HOT_DATABASE_RELOAD = False


# PORT = '/dev/ttyACM0'
PORT = "/dev/tty.usbmodem146101"
try:
    sio = serial.Serial(PORT, 9600)
except serial.serialutil.SerialException as err:
    input("!!!UNABLE TO CONNECT TO MetisHub, it enter to continue in test mode!!!:")
    sio = type(  # a dumby object
        "SIOZombie",
        (object,),
        dict(
            __getattr__=lambda self, *_, **__: self,
            __call__=lambda self, *_, **__: None,
        ),
    )()


# db = Database("database.json")
app = Flask(__name__)


@app.route("/")
def mainpage():
    return send_file("static/index.html")


@app.route("/static/<path:file>")
def static_route(file):
    return send_from_directory("static", name)


@app.route("/reset_leds", methods=["POST"])
def reset_leds():

    print("resetting the leds")
    sio.write("x\n".encode())
    print("sent")
    return ("", 200)


@app.route("/change_led", methods=["POST"])
def change_led():

    selected = request.json["selected_bypass"]
    row = selected = request.json["row"]
    col = request.json["col"]
    shelf_id = request.json["shelf_id"]
    name = request.json["name"]
    print(f"{name=}, {selected=} {request.json['selected_bypass']=}")
    msg = f"{'s' if request.json['selected_bypass'] else 'c'}|{shelf_id}|{row}|{col}\n"  # |{name}\n"

    print(f"sending {msg!r} ...")
    sio.write(msg.encode())
    print("sent")
    return ("", 200)


with app.app_context():
    if HOT_DATABASE_RELOAD:

        def _load_db():
            with open("database.json") as file:
                return jsonify(json.load(file))

    else:

        with open("database.json") as file:
            jsondb = jsonify(json.load(file))

        def _load_db():
            return jsondb


@app.route("/get_db")
def get_db():
    return _load_db()


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
