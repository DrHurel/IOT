# Firmware port
usbipd unbind --busid 4-10
usbipd bind --busid 4-10
usbipd attach --wsl --busid 4-10 

# Bluetooth Adapter
usbipd unbind --busid 4-17
usbipd bind --busid 4-17
usbipd attach --wsl --busid 4-17

