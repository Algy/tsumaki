import os
import stat
import sys
import errno
import socket

from contextlib import contextmanager
from .ipc import IPCTransport, IPCTransportClosedError
from .api import TsumakiApiServer

class SocketWrapperFile:
    def __init__(self, socket):
        self.socket = socket

    def read(self, n):
        return self.socket.recv(n)

    def write(self, content):
        try:
            return self.socket.send(content)
        except IOError as e:
            # Broken pipe
            if e.errno == errno.EPIPE:
                raise IPCTransportClosedError
            else:
                raise

    
    def close(self):
        self.socket.close()


def _unix_socket_receiver(server_sock, server_builder, block_size, flag_box):
    while flag_box[0]:
        client_sock, _ = server_sock.accept()
        print("ASDF")
        try:
            transport = IPCTransport(SocketWrapperFile(client_sock), block_size=block_size)
            api_server = server_builder(transport)
            api_server.run()
        except IPCTransportClosedError:
            pass
        finally:
            client_sock.close()

def _run_socket_server(server_builder, server_sock, block_size, concurrency):
    from threading import Thread
    flag_box = [True]

    threads = [
        Thread(
            target=_unix_socket_receiver,
            args=(server_sock, server_builder, block_size, flag_box),
            daemon=True
        )
        for _ in range(concurrency)
    ]

    for th in threads: th.start()
    try:
        for th in threads: th.join()
    finally:
        server_sock.close()
        flag_box[0] = False


def run_unix_socket_server(server_builder, name, block_size, concurrency=1):
    server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        mode = os.stat(name).st_mode
        is_socket = stat.S_ISSOCK(mode)
        if is_socket:
            os.unlink(name)
    except FileNotFoundError:
        pass

    server_sock.bind(name)
    sys.stderr.write(f"Running server on unix socket {name}\n")
    server_sock.listen(100)
    return _run_socket_server(server_builder, server_sock, block_size, concurrency)


def run_tcp_socket_server(server_builder, host, port, block_size, concurrency):
    server_sock = socket.socket()
    server_sock.bind((host, port))
    server_sock.listen(100)
    sys.stderr.write(f"Running server on {host}:{port}\n")
    return _run_socket_server(server_builder, server_sock, block_size, concurrency)

