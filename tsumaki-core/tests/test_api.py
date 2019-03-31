import pytest
import socket
import os

from threading import Thread
from tsumaki import api

data_path = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "data"
)


def test_api_server_heartbeat(transport_pair):
    server_transport, client_transport = transport_pair
    def server():
        try:
            server = api.TsumakiApiServer(server_transport)
            server.run(once=True)
        except Exception as exc:
            print(str(exc))



    Thread(target=server, daemon=True).start()

    client_transport.write_frame(
        api.HeartbeatRequest(hello="hello"),
        "request"
    )
    frame, frame_type = client_transport.read_frame()
    assert isinstance(frame, api.HeartbeatResponse)
    assert frame_type == 'response'
    assert frame.hello == 'hello'


def test_detect_person_api(transport_pair):
    import numpy as np
    from PIL import Image

    server_transport, client_transport = transport_pair
    def server():
        try:
            server = api.TsumakiApiServer(server_transport)
            server.run(once=True)
        except Exception as exc:
            print(str(exc))


    Thread(target=server, daemon=True).start()

    with Image.open(os.path.join(data_path, "person.png")) as person, \
            Image.open(os.path.join(data_path, "person_mask.png")) as person_mask:
        person_rgb = np.array(person)
        person_mask = np.array(person_mask) > 0

    person_rgba = np.concatenate([person_rgb, np.ones(person_rgb.shape[:2] + (1, ), np.uint8) * 255], axis=2)
    rgba_bytes = person_rgba.tobytes(order='C')
    height, width = person_rgba.shape[:2]

    request = api.DetectPersonRequest()
    request.image.width = width
    request.image.height = height
    request.image.data = rgba_bytes
    request.base_dimension = 640
    request.neural_param.branch = "incubator"
    request.neural_param.name = "mobilenetv2"
    request.neural_param.version = "0.0.1"
    request.neural_param.dimension = 256

    assert len(request.image.data) == request.image.width * request.image.height * 4

    client_transport.write_frame(request, "request")
    frame, frame_type = client_transport.read_frame()
    assert isinstance(frame, api.DetectPersonResponse)
    assert frame_type == 'response'
    mask = np.frombuffer(frame.mask.data, dtype=np.uint8).reshape(height, width)
    pred_mask = mask > 127

    iou = ((pred_mask & person_mask).sum() / (pred_mask | person_mask).sum())
    assert iou >= 0.6

