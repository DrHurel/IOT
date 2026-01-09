FROM eclipse-mosquitto:2

# Copy custom configuration
COPY mosquitto.conf /mosquitto/config/mosquitto.conf

# Expose MQTT ports
EXPOSE 1883
EXPOSE 9001

# Create directories for persistence
RUN mkdir -p /mosquitto/data /mosquitto/log

CMD ["mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
