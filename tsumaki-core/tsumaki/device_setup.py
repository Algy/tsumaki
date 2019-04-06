import plaidml
import plaidml.settings

from dataclasses import dataclass
from typing import List, Optional

@dataclass
class Device:
    id: str
    description: str
    experimental: bool


    def assign(self, device):
        self.id = device.id
        self.description = device.description
        self.experimental = device.experimental



class DeviceSetup:
    def __init__(self, ctx=None):
        self.ctx = ctx or plaidml.Context()

    def find_device_by_id(self, id) -> Optional[Device]:
        for dev in self.available_devices:
            if dev.id == id:
                return dev
        return None


    @property
    def available_devices(self) -> List[Device]:
        plaidml.settings.experimental = False
        devices, _ = plaidml.devices(self.ctx, limit=100, return_all=True)
        std_device_ids = set(dev.id for dev in devices)

        plaidml.settings.experimental = True
        exp_devices, _ = plaidml.devices(self.ctx, limit=100, return_all=True)
        devices = [self._to_dev(dev, False) for dev in devices] + [self._to_dev(dev, True) for dev in exp_devices if not dev.id in std_device_ids]
        return devices

    def _to_dev(self, d, experimental):
        return Device(
            id=d.id.decode(),
            description=d.description.decode(),
            experimental=experimental,
        )

    @property
    def current_device(self) -> Optional[Device]:
        try:
            return self.find_device_by_id(plaidml.settings.device_ids[0])
        except IndexError:
            return None

    @current_device.setter
    def current_device(self, device):
        if id:
            plaidml.settings.device_ids = [device.id]
        else:
            plaidml.settings.device_ids = []

    def driver_help_message(self):
        return '''No OpenCL devices found. Check driver installation.
Read the helpful, easy driver installation instructions from our README:
http://github.com/plaidml/plaidml
'''

    def validate(self):
        with plaidml.open_first_device(self.ctx) as dev:
            matmul = plaidml.Function("function (B[X,Z], C[Z,Y]) -> (A) { A[x,y : X,Y] = +(B[x,z] * C[z,y]); }")
            shape = plaidml.Shape(self.ctx, plaidml.DType.FLOAT32, 3, 3)
            a = plaidml.Tensor(dev, shape)
            b = plaidml.Tensor(dev, shape)
            c = plaidml.Tensor(dev, shape)
            plaidml.run(self.ctx, matmul, inputs={"B": b, "C": c}, outputs={"A": a})

    def save(self):
        plaidml.settings.save(plaidml.settings.user_settings)

