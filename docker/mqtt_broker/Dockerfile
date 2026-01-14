FROM eclipse-mosquitto:2

# Copy custom configuration
COPY mosquitto.conf /mosquitto/config/mosquitto.conf

# Copy authentication files (password and ACL)
# Note: For production, generate proper password hashes with setup_auth.sh
COPY passwd /mosquitto/config/passwd
COPY acl /mosquitto/config/acl

# Expose MQTT ports
# 1883 - Standard MQTT
# 9001 - WebSocket for web clients
EXPOSE 1883
EXPOSE 9001

# Create directories for persistence
RUN mkdir -p /mosquitto/data /mosquitto/log && \
    chown -R mosquitto:mosquitto /mosquitto

CMD ["mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
