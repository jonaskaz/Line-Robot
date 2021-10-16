from flask import Flask, render_template, request
import serial
 

app = Flask(__name__)
PORT = '/dev/ttyACM0'
BAUD = 9600

CONST_MAP = {
    "speed": 0,
    "error_offset": 1,
    "kP": 2,
    "kI": 3,
    "kD": 4,
}
 

@app.route('/', methods = ["POST", "GET"])
def home():
    if request.method == 'POST':
        data = request.form
        send_updates(data)
    return render_template("dash.html")


def send_updates(data):
    ser = serial.Serial(PORT, BAUD)
    for k, v in data.items():
        if v:
            message = f"{CONST_MAP[k]}:{v},"
            ser.write(message.encode('utf-8'))
    ser.close() 

 
if __name__ == '__main__':
    app.run()