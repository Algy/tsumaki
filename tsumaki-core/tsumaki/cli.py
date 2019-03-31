import click
import os

from .api import TsumakiApiServer
from .transport import run_unix_socket_server, run_tcp_socket_server

@click.group()
def cli():
    pass


@cli.command()
@click.option('--socket', help='The name of unix socket')
@click.option('--host', default="localhost", help='tcp host')
@click.option('--port', type=int, default=1125, help='tcp host')
@click.option('--block-size', default=4096 * 1024) # 4M
@click.option('--concurrency', default=1)
def run(socket, host, port, block_size, concurrency):
    if socket:
        run_unix_socket_server(TsumakiApiServer, socket, block_size, concurrency)
    else:
        run_tcp_socket_server(TsumakiApiServer, host, port, block_size, concurrency)
