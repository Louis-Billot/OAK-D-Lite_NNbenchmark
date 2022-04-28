import depthai as dai

device_infos = dai.Device.getAllAvailableDevices()

if len(device_infos) == 0: raise RuntimeError("No devices found!")
else: print("Found", len(device_infos), "devices")
queues = []

for device_info in device_infos:
    print(device_info.getMxId())