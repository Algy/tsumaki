# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: RGBAImage.proto

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
  name='RGBAImage.proto',
  package='tsumaki',
  syntax='proto3',
  serialized_pb=_b('\n\x0fRGBAImage.proto\x12\x07tsumaki\"8\n\tRGBAImage\x12\r\n\x05width\x18\x01 \x01(\x05\x12\x0e\n\x06height\x18\x02 \x01(\x05\x12\x0c\n\x04\x64\x61ta\x18\x03 \x01(\x0c\x62\x06proto3')
)
_sym_db.RegisterFileDescriptor(DESCRIPTOR)




_RGBAIMAGE = _descriptor.Descriptor(
  name='RGBAImage',
  full_name='tsumaki.RGBAImage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='width', full_name='tsumaki.RGBAImage.width', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='height', full_name='tsumaki.RGBAImage.height', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='data', full_name='tsumaki.RGBAImage.data', index=2,
      number=3, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
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
  serialized_end=84,
)

DESCRIPTOR.message_types_by_name['RGBAImage'] = _RGBAIMAGE

RGBAImage = _reflection.GeneratedProtocolMessageType('RGBAImage', (_message.Message,), dict(
  DESCRIPTOR = _RGBAIMAGE,
  __module__ = 'RGBAImage_pb2'
  # @@protoc_insertion_point(class_scope:tsumaki.RGBAImage)
  ))
_sym_db.RegisterMessage(RGBAImage)


# @@protoc_insertion_point(module_scope)