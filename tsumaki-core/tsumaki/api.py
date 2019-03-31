import numpy as np

import plaidml
from plaidml.keras import install_backend
install_backend()


from .ipc import IPCApiServer
from .ipc import (HeartbeatRequest, HeartbeatResponse,
                  DetectPersonRequest, DetectPersonResponse)
from .ipc import ErrorResponse



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

@TsumakiApiServer.route(DetectPersonRequest)
def detect_person(server, request_frame):
    import numpy as np

    from tsumaki.model_branch.incubator.mobilenetv2 import Model
    from scipy.misc import imresize
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
        models[name, neural_dim] = model = Model(neural_dim)
    elif name == 'xception':
        raise NotImplementedError("TODO")
    else:
        return ErrorResponse(code=404, msg=f"Unspported neural net {name}")
    mask = model.predict(image)
    resp = DetectPersonResponse()
    resp.mask.data = mask.tobytes()
    resp.mask.width = width
    resp.mask.height = height
    return resp

