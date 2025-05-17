from flask import Flask 
import pyautogui
import threading
import os


app = Flask(__name__)

@app.route('/')
def index():
    return "test"

@app.route('/altTab')
def altTab():
    # going to add code for alt tab
    threading.Thread(target=press_alt_tab).start()
    return "alt+tab pressed"

def press_alt_tab():
    print("pressing super + 2")
    # 125 = super | 3 = #2
    os.system("sudo /usr/bin/ydotool key 125:1 3:1 3:0 125:0")

app.run(host='0.0.0.0')
