import numpy as np

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


@TsumakiApiServer.route(DetectPersonRequest)
def detect_person(server, request_frame):
    raise NotImplementedError("Todo")

