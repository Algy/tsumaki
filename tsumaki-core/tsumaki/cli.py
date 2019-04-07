import click
import os

from .api import TsumakiApiServer, build_model
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


@cli.command()
def self_check():
    import numpy as np
    img = np.ndarray((200, 200, 3), dtype=np.uint8)
    model = build_model("incubator", "mobilenetv2", "0.0.1", 256)
    model.predict(img)


@cli.command()
def plaidml_setup():
    from plaidml.plaidml_setup import main
    from plaidml import settings
    print(settings.user_settings)
    main()


@cli.command()
def shell():
    pass

@cli.command()
def bench():
    import numpy as np
    import time
    img = np.ndarray((480, 640, 3), dtype=np.uint8)
    model = build_model("incubator", "mobilenetv2", "0.0.1", 256)
    # Burn in
    model.predict(img)

    times = []
    for _ in range(3):
        t1 = time.time()
        model.predict(img)
        t2 = time.time()
        times.append(t2 - t1)

    batch_times = []
    arr = [img for _ in range(101)]

    # Burn in
    model.predict_multi(arr)
    for _ in range(3):
        t1 = time.time()
        model.predict_multi(arr)
        t2 = time.time()
        batch_times.append(t2 - t1)
    mu, std = np.mean(times), np.std(times)
    batch_mu, batch_std = np.mean(batch_times), np.std(batch_times)

    diff_mu = (batch_mu - mu) / 100
    diff_std = np.sqrt(batch_std ** 2 + std ** 2) / 100
    print(f"Single Sample Time: {mu:.3f}(std: {std:.3f})")
    print(f"Batch(N=101) Sample Time: {batch_mu:.3f}(std: {batch_std:.3f})")
    print(f"Estimated Overhead: {diff_mu:.3f}(std: {diff_std:.3f})")

