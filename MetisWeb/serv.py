from flask import Flask, request, send_file, jsonify


db = [
    {
        'name': "part name",
        'shelf_id': 11283,
        'row': 0,
        'col': 0,
    },
    {
        'name': "another part",
        'shelf_id': 1111,
        'row': 1,
        'col': 1,
    },
    {
        'name': "yet another part",
        'shelf_id': 1111,
        'row': 2,
        'col': 1,
    }
]

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
    return jsonify(db)

if __name__ == '__main__':
    app.run()
