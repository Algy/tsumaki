import pytest
import socket

from threading import Thread
from tsumaki import ipc, transport

def test_transport_basic(transport_pair):
    hello = "abcdefghijk" * 1024
    server_transport, client_transport = transport_pair
    def server():
        frame, frame_type = server_transport.read_frame()
        assert isinstance(frame, ipc.HeartbeatRequest)
        assert frame_type == 'request'
        assert frame.hello == hello
        server_transport.write_frame(ipc.HeartbeatResponse(hello=hello), "response")


    def client():
        req = ipc.HeartbeatRequest(hello=hello)
        client_transport.write_frame(req, "request")
        received_frame, frame_type = client_transport.read_frame()
        assert isinstance(received_frame, ipc.HeartbeatResponse)
        assert frame_type == 'response'
        assert received_frame.hello == hello

    
    th1 = Thread(target=server)
    th2 = Thread(target=client)

    th1.start()
    th2.start()
    th2.join()
    th1.join()


def test_transport_error(transport_pair):
    server_transport, client_transport = transport_pair
    def server():
        try:
            server_transport.read_frame()
            server_transport.write_frame(ipc.ErrorResponse(code=999, msg="msg"), "error")
        except Exception as exc:
            print(str(exc))
            raise


    def client():
        try:
            req = ipc.HeartbeatRequest(hello="abc")
            client_transport.write_frame(req, "request")
            received_frame, frame_type = client_transport.read_frame()
            assert isinstance(received_frame, ipc.ErrorResponse)
            assert frame_type == 'error'
            assert received_frame.code == 999
            assert received_frame.msg == 'msg'
        except Exception as exc:
            print(str(exc))
            raise


    th1 = Thread(target=server)
    th2 = Thread(target=client)

    th1.start()
    th2.start()
    th2.join()
    th1.join()

