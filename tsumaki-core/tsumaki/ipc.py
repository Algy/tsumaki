
import os
import struct
import logging

from google.protobuf.message import Error as MessageError
from DetectPerson_pb2 import DetectPersonRequest, DetectPersonResponse
from Heartbeat_pb2 import HeartbeatRequest, HeartbeatResponse
from ErrorResponse_pb2 import ErrorResponse

RequestDef_v1 = {
    0: (HeartbeatRequest, HeartbeatResponse),
    1: (DetectPersonRequest, DetectPersonResponse),
}

class IPCError(Exception):
    pass

class IPCFormatError(IPCError):
    pass

class IPCApiError(IPCError):
    pass

class IPCTransportClosedError(IPCError):
    pass

class IPCTransport:
    header_sig = b"@(^o^)=@"
    trailer_sig = b"@=(^o^)@"

    version = 1
    def __init__(self, f, block_size=1024 * 1024):
        self.f = f
        # 1MB buffer
        self.block_size = block_size


    def close(self):
        self.f.close()

    def read_frame(self):
        header_buf = self._read_all(17, "header")
        try:
            (hdsigval,
             version,
             frame_type_c,
             method,
             body_length) = (
                struct.unpack("<8sHcHI", header_buf)
            )

            if hdsigval != self.header_sig:
                raise IPCFormatError("Header signature is invalid")
            if version != self.version:
                raise IPCFormatError(f"Version {version} is not supported")
            frame_type = None
            if frame_type_c == b'Q':
                frame_type = "request"
            elif frame_type_c == b'P':
                frame_type = "response"
            elif frame_type_c == b'E':
                frame_type = "error"
            else:
                raise IPCFormatError(f"Frame type {frame_type_c} not understood")

            if method not in RequestDef_v1:
                raise IPCFormatError(f"Method {method} not supported")
            req_frame_cls, resp_frame_cls = RequestDef_v1[method]

            if frame_type == 'request':
                cls = req_frame_cls
            elif frame_type == 'response':
                cls = resp_frame_cls
            elif frame_type == 'error':
                cls = ErrorResponse
            else:
                raise RuntimeError("Unreachable")

            body = self._read_all(body_length, "body")
            trsigval = self._read_all(8, "trailer")
            if trsigval != self.trailer_sig:
                raise IPCFormatError("Trailer signature is invalid")

            frame = cls()
            frame.ParseFromString(body)
            return frame, frame_type
        except struct.error as exc:
            raise IPCFormatError(str(exc))
        except MessageError as exc:
            raise IPCFormatError(str(exc))

    def write_frame(self, frame, frame_type):
        try:
            if frame_type == 'request':
                frame_type_c = b"Q"
            elif frame_type == 'response':
                frame_type_c = b"P"
            elif frame_type == 'error':
                frame_type_c = b'E'
            else:
                raise IPCError(f"Unsupported frame type {frame_type}")


            method = 0
            if frame_type != 'error':
                for cand_method, (req_frame_cls, resp_frame_cls) in RequestDef_v1.items():
                    if frame_type == 'request' and isinstance(frame, req_frame_cls) or frame_type == 'response' and isinstance(frame, resp_frame_cls):
                        method = cand_method
                        break
                else:
                    raise IPCError(f"Unrecognized {frame_type} frame {frame!r}")
            elif not isinstance(frame, ErrorResponse):
                raise IPCError(f"Frame should be of type ErrorResponse, but got {frame!r}")
                method = 0

            body_buf = frame.SerializeToString()
            body_length = len(body_buf)
            header_buf = struct.pack("<8sHcHI", self.header_sig, self.version, frame_type_c, method, body_length)
            trailer_buf = self.trailer_sig

            self._write_all(header_buf, "header")
            self._write_all(body_buf, "body")
            self._write_all(trailer_buf, "trailer")
        except struct.error as exc:
            raise IPCFormatError(str(exc))
        except MessageError as exc:
            raise IPCFormatError(str(exc))


    def _read_all(self, length, tag):
        block_size = self.block_size
        remaining_bytes = length
        result = bytearray()

        while remaining_bytes > 0:
            nbytes_to_read = min(remaining_bytes, block_size)
            received_buf = self.f.read(nbytes_to_read)
            result.extend(received_buf)
            remaining_bytes -= len(received_buf)
            if not received_buf and remaining_bytes > 0:
                raise IPCTransportClosedError(
                    f"Received truncated {tag}. "
                    f"Expected {length} bytes, but got "
                    f"{len(received_buf)} bytes"
                )
        return bytes(result)


    def _write_all(self, content, tag):
        if not isinstance(content, bytes):
            raise TypeError("Expected bytes")
        block_size = self.block_size
        remaining_bytes = len(content)
        while remaining_bytes > 0:
            nbytes_to_write = min(remaining_bytes, block_size)
            start_index = len(content) - remaining_bytes
            buf = content[start_index:start_index + nbytes_to_write]
            nbytes_written = self.f.write(buf)
            if nbytes_written == 0:
                raise IPCFormatError(f"Failed to write {tag} to file")
            remaining_bytes -= nbytes_written


class IPCApiServer:
    handlers = {}

    def __init__(self, transport, logger=None):
        self.transport = transport
        self.logger = logger or logging.getLogger("IPCApiServer")
        self.request_handlers = {}

    def send_response(self, frame):
        self.transport.write_frame(frame, "request")

    def send_error(self, code, msg):
        self.transport.write_frame(
            ErrorResponse(code=code, msg=msg),
            "error",
        )

    @classmethod
    def route(cls, frame_cls):
        def wrapper(fn):
            cls.handlers[frame_cls] = fn
            return fn
        return wrapper

    def run(self, once=False):
        flag = True
        while flag:
            if once:
                flag = False
            try:
                frame, frame_type = self.transport.read_frame()
                if frame_type != 'request':
                    self.send_error(400, "Bad Format")
                    raise IPCFormatError(f"Got a {frame_type} frame from server")
                try:
                    handler = self.handlers[type(frame)]
                except KeyError:
                    raise IPCApiServer(f"handler for {frame} not found")
                response_frame = handler(self, frame)
                self.transport.write_frame(response_frame, "response")
            except IPCTransportClosedError as exc:
                raise
            except Exception as exc:
                self.send_error(500, f"Internal Server Error: {exc!r}")
                self.logger.exception("error raised on ipc hanlder")

