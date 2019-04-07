import numpy as np

import plaidml
from plaidml.keras import install_backend
install_backend()


from .ipc import IPCApiServer
from .ipc import (HeartbeatRequest, HeartbeatResponse,
                  DetectPersonRequest, DetectPersonResponse,
                  SetupDeviceRequest, SetupDeviceResponse)
from .ipc import ErrorResponse

from .device_setup import DeviceSetup, Device



class TsumakiApiServer(IPCApiServer):
    handlers = {}

@TsumakiApiServer.route(HeartbeatRequest)
def heartbeat(server, request_frame):
    hello = request_frame.hello
    if hello:
        return HeartbeatResponse(hello=request_frame.hello)
    else:
        return ErrorResponse(code=333, msg="hello string can't be empty")



models = {}
def build_model(branch, name, version, neural_dim):
    from tsumaki.model_branch.incubator.mobilenetv2 import Model
    if (name, neural_dim) not in models:
        models[name, neural_dim] = Model(neural_dim )
    return models[name, neural_dim]

counter = 0
@TsumakiApiServer.route(DetectPersonRequest)
def detect_person(server, request_frame):
    global counter
    import numpy as np

    from tsumaki.model_branch.incubator.mobilenetv2 import Model
    width, height = request_frame.image.width, request_frame.image.height
    image = np.frombuffer(request_frame.image.data, dtype=np.uint8)
    image = image.reshape(height, width, 4)
    image = image[:, :, [0, 1, 2]] # to rgb

    neural_param = request_frame.neural_param 
    neural_dim = neural_param.dimension
    name, branch, version = neural_param.name, neural_param.branch, neural_param.version


    if branch != 'incubator':
        return ErrorResponse(code=404, msg=f"Unspported branch {branch}") 
    if name == 'mobilenetv2':
        model = build_model(branch, name, version, neural_dim)
    elif name == 'xception':
        raise NotImplementedError("TODO")
    else:
        return ErrorResponse(code=404, msg=f"Unspported neural net {name}")

    import time
    t1 = time.time()
    mask = model.predict(image)
    t2 = time.time()
    if counter % 30 == 0:
        print("DT:", t2 - t1)
    counter += 1
    resp = DetectPersonResponse()
    resp.mask.data = mask.tobytes()
    resp.mask.width = width
    resp.mask.height = height
    return resp

@TsumakiApiServer.route(SetupDeviceRequest)
def setup_device_api(server, request_frame):
    def assign(a, b):
        a.id = b.id
        a.description = b.description
        a.experimental = b.experimental
    device_setup = DeviceSetup()
    target_device_id = request_frame.target_device_id
    available_devices = device_setup.available_devices
    if target_device_id:
        target_device = device_setup.find_device_by_id(target_device_id)
        if not target_device:
            return ErrorResponse(code=404, msg=f"Unspported device {target_device_id}")
        device_setup.current_device = target_device
        device_setup.save()
    current_device = device_setup.current_device
    response = SetupDeviceResponse()
    if current_device:
        assign(response.current_device, current_device)


    for device in available_devices:
        assign(response.available_devices.add(), device)
    return response
