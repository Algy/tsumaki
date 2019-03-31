import pytest
import socket

from tsumaki import ipc, transport


@pytest.fixture
def transport_pair():
    socket_pair = socket.socketpair()
    try:
        server_transport = ipc.IPCTransport(
            transport.SocketWrapperFile(socket_pair[0]),
            block_size=11,
        )
        client_transport = ipc.IPCTransport(
            transport.SocketWrapperFile(socket_pair[1]),
            block_size=7,
        )
        yield server_transport, client_transport
    finally:
        socket_pair[0].close()
        socket_pair[1].close()

