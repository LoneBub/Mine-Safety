import paho.mqtt.client as mqtt
import time
import json

node_sentry = ['temp', 'humidity', 'gas']
node_helmet = ['temp', 'humidity', 'gas', 'x','y','z']
node_bot = ['temp', 'humidity', 'gas']



def json_data(file_str, node ,sensor_values):
    dict_from_list =dict(zip(node,sensor_values))
    with open(file_str,'w') as json_file:
        json.dump(dict_from_list, json_file,indent=4)
        
        
def on_connect(client, userdata, flags, rc):
   global flag_connected
   flag_connected = 1
   client_subscriptions(client)
   print("Connected to MQTT server")

def on_disconnect(client, userdata, rc):
   global flag_connected
   flag_connected = 0
   print("Disconnected from MQTT server")
   
# sentry callback functions 
sensor_list = []
sensor_vals = len(node_sentry)
count = 0

def callback_esp32_sensor_node_sentry(client, userdata, msg):
    sensor_data= msg.payload.decode('utf-8')
    print('Sentry data: ',sensor_data)
    
    sensor_list.append(sensor_data)
    global count
    count += 1
    
    if count >= sensor_vals:
        json_data('sentry.json',node_sentry,sensor_list)
        sensor_list.clear()
        count=0
         
# helmet callback function 
sensor_list = []
sensor_vals = len(node_helmet)
count = 0

def callback_esp32_sensor_node_helmet(client, userdata, msg):
    sensor_data= msg.payload.decode('utf-8')
    print('Helmet data: ',sensor_data)
    
    sensor_list.append(sensor_data)
    global count
    count += 1
    
    if count >= sensor_vals:
        json_data('helmet.json',node_helmet,sensor_list)
        sensor_list.clear()
        count=0

# exploration bot callback function
sensor_list = []
sensor_vals = len(node_bot)
count = 0

def callback_esp32_sensor_node_bot(client, userdata, msg):
    sensor_data= msg.payload.decode('utf-8')
    print('Exploration Bot data: ',sensor_data)
    
    sensor_list.append(sensor_data)
    global count
    count += 1
    
    if count >= sensor_vals:
        json_data('Bot.json',node_bot,sensor_list)
        sensor_list.clear()
        count=0

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

def callback_rpi_broadcast(client, userdata, msg):
    print('RPi Broadcast message:  ', str(msg.payload.decode('utf-8')))

def client_subscriptions(client):
    client.subscribe("esp32/#")
    client.subscribe("rpi/broadcast")

client = mqtt.Client("rpi_client1") #this should be a unique name
flag_connected = 0

client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.message_callback_add('esp32/sensor/sentry', callback_esp32_sensor_node_sentry)
client.message_callback_add('esp32/sensor/helmet', callback_esp32_sensor_node_helmet)
client.message_callback_add('esp32/sensor/bot', callback_esp32_sensor_node_bot)
client.message_callback_add('rpi/broadcast', callback_rpi_broadcast)
client.connect('127.0.0.1',1883)
# start a new thread
client.loop_start()
client_subscriptions(client)
print("......client setup complete............")


while True:
    time.sleep(4)
    if (flag_connected != 1):
        print("trying to connect MQTT server..")
        
