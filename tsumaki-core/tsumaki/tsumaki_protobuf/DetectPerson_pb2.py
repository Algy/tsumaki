# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: DetectPerson.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import RGBAImage_pb2 as RGBAImage__pb2
import MaskImage_pb2 as MaskImage__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='DetectPerson.proto',
  package='tsumaki',
  syntax='proto3',
  serialized_pb=_b('\n\x12\x44\x65tectPerson.proto\x12\x07tsumaki\x1a\x0fRGBAImage.proto\x1a\x0fMaskImage.proto\"\xe9\x01\n\x13\x44\x65tectPersonRequest\x12!\n\x05image\x18\x01 \x01(\x0b\x32\x12.tsumaki.RGBAImage\x12\x16\n\x0e\x62\x61se_dimension\x18\x02 \x01(\r\x12\x42\n\x0cneural_param\x18\x03 \x01(\x0b\x32,.tsumaki.DetectPersonRequest.NeuralParameter\x1aS\n\x0fNeuralParameter\x12\x0e\n\x06\x62ranch\x18\x01 \x01(\t\x12\x0c\n\x04name\x18\x02 \x01(\t\x12\x0f\n\x07version\x18\x03 \x01(\t\x12\x11\n\tdimension\x18\x04 \x01(\r\"8\n\x14\x44\x65tectPersonResponse\x12 \n\x04mask\x18\x01 \x01(\x0b\x32\x12.tsumaki.MaskImage2b\n\x13\x44\x65tectPersonService\x12K\n\x0c\x44\x65tectPerson\x12\x1c.tsumaki.DetectPersonRequest\x1a\x1d.tsumaki.DetectPersonResponseB\x02H\x01\x62\x06proto3')
  ,
  dependencies=[RGBAImage__pb2.DESCRIPTOR,MaskImage__pb2.DESCRIPTOR,])
_sym_db.RegisterFileDescriptor(DESCRIPTOR)




_DETECTPERSONREQUEST_NEURALPARAMETER = _descriptor.Descriptor(
  name='NeuralParameter',
  full_name='tsumaki.DetectPersonRequest.NeuralParameter',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='branch', full_name='tsumaki.DetectPersonRequest.NeuralParameter.branch', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='name', full_name='tsumaki.DetectPersonRequest.NeuralParameter.name', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='version', full_name='tsumaki.DetectPersonRequest.NeuralParameter.version', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='dimension', full_name='tsumaki.DetectPersonRequest.NeuralParameter.dimension', index=3,
      number=4, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=216,
  serialized_end=299,
)

_DETECTPERSONREQUEST = _descriptor.Descriptor(
  name='DetectPersonRequest',
  full_name='tsumaki.DetectPersonRequest',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='image', full_name='tsumaki.DetectPersonRequest.image', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='base_dimension', full_name='tsumaki.DetectPersonRequest.base_dimension', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='neural_param', full_name='tsumaki.DetectPersonRequest.neural_param', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_DETECTPERSONREQUEST_NEURALPARAMETER, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=66,
  serialized_end=299,
)


_DETECTPERSONRESPONSE = _descriptor.Descriptor(
  name='DetectPersonResponse',
  full_name='tsumaki.DetectPersonResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='mask', full_name='tsumaki.DetectPersonResponse.mask', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
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
  serialized_start=301,
  serialized_end=357,
)

_DETECTPERSONREQUEST_NEURALPARAMETER.containing_type = _DETECTPERSONREQUEST
_DETECTPERSONREQUEST.fields_by_name['image'].message_type = RGBAImage__pb2._RGBAIMAGE
_DETECTPERSONREQUEST.fields_by_name['neural_param'].message_type = _DETECTPERSONREQUEST_NEURALPARAMETER
_DETECTPERSONRESPONSE.fields_by_name['mask'].message_type = MaskImage__pb2._MASKIMAGE
DESCRIPTOR.message_types_by_name['DetectPersonRequest'] = _DETECTPERSONREQUEST
DESCRIPTOR.message_types_by_name['DetectPersonResponse'] = _DETECTPERSONRESPONSE

DetectPersonRequest = _reflection.GeneratedProtocolMessageType('DetectPersonRequest', (_message.Message,), dict(

  NeuralParameter = _reflection.GeneratedProtocolMessageType('NeuralParameter', (_message.Message,), dict(
    DESCRIPTOR = _DETECTPERSONREQUEST_NEURALPARAMETER,
    __module__ = 'DetectPerson_pb2'
    # @@protoc_insertion_point(class_scope:tsumaki.DetectPersonRequest.NeuralParameter)
    ))
  ,
  DESCRIPTOR = _DETECTPERSONREQUEST,
  __module__ = 'DetectPerson_pb2'
  # @@protoc_insertion_point(class_scope:tsumaki.DetectPersonRequest)
  ))
_sym_db.RegisterMessage(DetectPersonRequest)
_sym_db.RegisterMessage(DetectPersonRequest.NeuralParameter)

DetectPersonResponse = _reflection.GeneratedProtocolMessageType('DetectPersonResponse', (_message.Message,), dict(
  DESCRIPTOR = _DETECTPERSONRESPONSE,
  __module__ = 'DetectPerson_pb2'
  # @@protoc_insertion_point(class_scope:tsumaki.DetectPersonResponse)
  ))
_sym_db.RegisterMessage(DetectPersonResponse)


DESCRIPTOR.has_options = True
DESCRIPTOR._options = _descriptor._ParseOptions(descriptor_pb2.FileOptions(), _b('H\001'))
# @@protoc_insertion_point(module_scope)
