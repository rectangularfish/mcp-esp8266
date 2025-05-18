from flask import Flask
from flask_mqtt import Mqtt
import os
import time

app = Flask(__name__)

# MQTT broker configuration
app.config['MQTT_BROKER_URL'] = 'broker.hivemq.com'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = ''
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_KEEPALIVE'] = 60

mqtt = Mqtt(app)

# Distance tracking
latest_distance = None
SIGNIFICANT_CHANGE_CM = 20  # Change threshold

def press_alt_tab():
    print("Triggering Alt+Tab!")
    os.system("sudo /usr/bin/ydotool key 125:1 3:1 3:0 125:0")

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    if rc == 0:
        print('Connected to MQTT broker')
        mqtt.subscribe('nikolaus/jamhacks/sensor/distance')
    else:
        print(f'Failed to connect, return code={rc}')

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    global latest_distance
    try:
        distance = float(message.payload.decode())
        print(f"MQTT message: {distance} cm")

        if latest_distance is not None:
            change = abs(distance - latest_distance)
            print(f"Change: {change} cm")
            if change >= SIGNIFICANT_CHANGE_CM:
                press_alt_tab()
        else:
            print("Initial distance recorded.")

        latest_distance = distance

    except ValueError:
        print("Invalid payload received:", message.payload.decode())
        time.sleep(20)

@app.route('/')
def index():
    return f"<h1>MQTT active</h1><p>Last distance: {latest_distance if latest_distance is not None else 'N/A'} cm</p>"

if __name__ == '__main__':
    app.run(debug=True, use_reloader=False, host='0.0.0.0', port=5000)
