import sys

# adafruit IO
import adafruit_minimqtt.adafruit_minimqtt as MQTT
from adafruit_io.adafruit_io import IO_MQTT

sys.path.append("../lib/adafruit")
sys.path.append("../lib/adafruit/adafruit_io")
sys.path.append("../lib/adafruit/adafruit_minimqtt")

class aio():
    def __init__(self, soc, esp, secrets, init_block_name):
        self.soc = soc
        self.esp = esp
        self.secrets=secrets
        self.init_block_name=init_block_name

        # Initialize MQTT interface with the esp interface
        MQTT.set_socket(self.soc, self.esp)

        # Initialize a new MQTT Client object
        mqtt_client = MQTT.MQTT(
            broker="io.adafruit.com",
            username=self.secrets["aio_username"],
            password=self.secrets["aio_key"],
        )

        # Initialize an Adafruit IO MQTT Client
        io = IO_MQTT(mqtt_client)

        # Connect the callback methods defined above to Adafruit IO
        io.on_connect = self.connected
        io.on_disconnect = self.disconnected
        io.on_subscribe = self.subscribe
        io.on_unsubscribe = self.unsubscribe
        io.on_message = self.message

        # Connect to Adafruit IO
        print("Connecting to Adafruit IO...")
        io.connect()

        self.io=io

    def get_oi(self):
        return self.io

    def subscribe(self, client, userdata, topic, granted_qos):
        # This method is called when the client subscribes to a new feed.
        _ = (client, userdata)
        print("Subscribed to {0} with QOS level {1}".format(topic, granted_qos))

    def aio_url_hh_mm(self):
        username=self.secrets["aio_username"]
        password=self.secrets["aio_key"]
        TIME_URL = "http://io.adafruit.com/api/v2/%s/integrations/time/strftime?x-aio-key=%s" % (
            username, password)

        TIME_URL += "&fmt=%25H%25M"
        return TIME_URL

    def unsubscribe(self, client, userdata, topic, pid):
        _ = (client, userdata)
        # This method is called when the client unsubscribes from a feed.
        print("Unsubscribed from {0} with PID {1}".format(topic, pid))

    def connected(self, client):
        # Connected function will be called when the client is connected to Adafruit IO.
        # This is a good place to subscribe to feed changes.  The client parameter
        # passed to this function is the Adafruit IO MQTT client so you can make
        # calls against it easily.
        print("Connected to Adafruit IO")
        # Subscribe to changes on a feed named DemoFeed.
        client.subscribe(self.init_block_name)

    # pylint: disable=unused-argument
    def disconnected(self, client):
        # Disconnected function will be called when the client disconnects.
        print("Disconnected from Adafruit IO!")

    # pylint: disable=unused-argument
    def message(self, client, feed_id, payload):
        # Message function will be called when a subscribed feed has a new value.
        # The feed_id parameter identifies the feed, and the payload parameter has
        # the new value.
        print("Feed {0} received new value: {1}".format(feed_id, payload))
