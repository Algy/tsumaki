# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: Heartbeat.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='Heartbeat.proto',
  package='tsumaki',
  syntax='proto3',
  serialized_pb=_b('\n\x0fHeartbeat.proto\x12\x07tsumaki\"!\n\x10HeartbeatRequest\x12\r\n\x05hello\x18\x01 \x01(\t\"\"\n\x11HeartbeatResponse\x12\r\n\x05hello\x18\x01 \x01(\t2V\n\x10HeartbeatService\x12\x42\n\tHeartbeat\x12\x19.tsumaki.HeartbeatRequest\x1a\x1a.tsumaki.HeartbeatResponseb\x06proto3')
)
_sym_db.RegisterFileDescriptor(DESCRIPTOR)




_HEARTBEATREQUEST = _descriptor.Descriptor(
  name='HeartbeatRequest',
  full_name='tsumaki.HeartbeatRequest',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='hello', full_name='tsumaki.HeartbeatRequest.hello', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=28,
  serialized_end=61,
)


_HEARTBEATRESPONSE = _descriptor.Descriptor(
  name='HeartbeatResponse',
  full_name='tsumaki.HeartbeatResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='hello', full_name='tsumaki.HeartbeatResponse.hello', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=63,
  serialized_end=97,
)

DESCRIPTOR.message_types_by_name['HeartbeatRequest'] = _HEARTBEATREQUEST
DESCRIPTOR.message_types_by_name['HeartbeatResponse'] = _HEARTBEATRESPONSE

HeartbeatRequest = _reflection.GeneratedProtocolMessageType('HeartbeatRequest', (_message.Message,), dict(
  DESCRIPTOR = _HEARTBEATREQUEST,
  __module__ = 'Heartbeat_pb2'
  # @@protoc_insertion_point(class_scope:tsumaki.HeartbeatRequest)
  ))
_sym_db.RegisterMessage(HeartbeatRequest)

HeartbeatResponse = _reflection.GeneratedProtocolMessageType('HeartbeatResponse', (_message.Message,), dict(
  DESCRIPTOR = _HEARTBEATRESPONSE,
  __module__ = 'Heartbeat_pb2'
  # @@protoc_insertion_point(class_scope:tsumaki.HeartbeatResponse)
  ))
_sym_db.RegisterMessage(HeartbeatResponse)


# @@protoc_insertion_point(module_scope)
