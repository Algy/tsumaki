import pytest
import socket

from threading import Thread
from tsumaki import api


def test_transport_basic(transport_pair):
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

