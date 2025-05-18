from typing import Any
import httpx
from mcp.server.fastmcp import FastMCP
import paho.mqtt.publish as publish





# Create MCP server
mcp = FastMCP("mqtt")

# MQTT settings
BROKER = "broker.hivemq.com"
TOPIC = "nikolaus/jamhacks/sensor/distance"

@mcp.tool()
async def stop_sensor() -> str:
    """Send a 'STOP' message to the sensor system over MQTT."""
    try:
        publish.single(topic=TOPIC, payload="STOP", hostname=BROKER)
        return "Published payload: STOP"
    except Exception as e:
        return f"Failed to publish: {str(e)}"

if __name__ == "__main__":
    mcp.run(transport="stdio")
