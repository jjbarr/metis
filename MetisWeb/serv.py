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

#PORT = '/dev/ttyACM0'
#sio = serial.Serial(PORT, 9600)

db = Database('database.json')

app = Flask(__name__)

@app.route('/')
def mainpage():
    return send_file('static/index.html')

@app.route('/static/<path:file>')
def static_route(file):
    return send_from_directory('static', name)

@app.route('/reset_leds', methods=['POST'])
def reset_leds():
    print('reset the leds')
    return ('',200)

@app.route('/change_led', methods=['POST'])
def change_led():
    print(f"led shelf: {request.json['shelf_id']} row: {request.json['row']} col: {request.json['col']} turned {'on' if request.json['selected'] else 'off'}")
    return ('',200)

@app.route('/get_db')
def get_db():
    return jsonify(db.data)

if __name__ == '__main__':
    app.run()
