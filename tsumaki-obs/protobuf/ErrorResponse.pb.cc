// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ErrorResponse.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ErrorResponse.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace tsumaki {

namespace {

const ::google::protobuf::Descriptor* ErrorResponse_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ErrorResponse_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_ErrorResponse_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AssignDesc_ErrorResponse_2eproto() {
  protobuf_AddDesc_ErrorResponse_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ErrorResponse.proto");
  GOOGLE_CHECK(file != NULL);
  ErrorResponse_descriptor_ = file->message_type(0);
  static const int ErrorResponse_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ErrorResponse, code_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ErrorResponse, msg_),
  };
  ErrorResponse_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      ErrorResponse_descriptor_,
      ErrorResponse::default_instance_,
      ErrorResponse_offsets_,
      -1,
      -1,
      -1,
      sizeof(ErrorResponse),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ErrorResponse, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ErrorResponse, _is_default_instance_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ErrorResponse_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      ErrorResponse_descriptor_, &ErrorResponse::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_ErrorResponse_2eproto() {
  delete ErrorResponse::default_instance_;
  delete ErrorResponse_reflection_;
}

void protobuf_AddDesc_ErrorResponse_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AddDesc_ErrorResponse_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\023ErrorResponse.proto\022\007tsumaki\"*\n\rErrorR"
    "esponse\022\014\n\004code\030\001 \001(\005\022\013\n\003msg\030\002 \001(\tB\002H\001b\006"
    "proto3", 86);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ErrorResponse.proto", &protobuf_RegisterTypes);
  ErrorResponse::default_instance_ = new ErrorResponse();
  ErrorResponse::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ErrorResponse_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ErrorResponse_2eproto {
  StaticDescriptorInitializer_ErrorResponse_2eproto() {
    protobuf_AddDesc_ErrorResponse_2eproto();
  }
} static_descriptor_initializer_ErrorResponse_2eproto_;

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int ErrorResponse::kCodeFieldNumber;
const int ErrorResponse::kMsgFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

ErrorResponse::ErrorResponse()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:tsumaki.ErrorResponse)
}

void ErrorResponse::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

ErrorResponse::ErrorResponse(const ErrorResponse& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:tsumaki.ErrorResponse)
}

void ErrorResponse::SharedCtor() {
    _is_default_instance_ = false;
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  code_ = 0;
  msg_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

ErrorResponse::~ErrorResponse() {
  // @@protoc_insertion_point(destructor:tsumaki.ErrorResponse)
  SharedDtor();
}

void ErrorResponse::SharedDtor() {
  msg_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != default_instance_) {
  }
}

void ErrorResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ErrorResponse::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ErrorResponse_descriptor_;
}

const ErrorResponse& ErrorResponse::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ErrorResponse_2eproto();
  return *default_instance_;
}

ErrorResponse* ErrorResponse::default_instance_ = NULL;

ErrorResponse* ErrorResponse::New(::google::protobuf::Arena* arena) const {
  ErrorResponse* n = new ErrorResponse;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void ErrorResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:tsumaki.ErrorResponse)
  code_ = 0;
  msg_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool ErrorResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tsumaki.ErrorResponse)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 code = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &code_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_msg;
        break;
      }

      // optional string msg = 2;
      case 2: {
        if (tag == 18) {
         parse_msg:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_msg()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->msg().data(), this->msg().length(),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tsumaki.ErrorResponse.msg"));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:tsumaki.ErrorResponse)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tsumaki.ErrorResponse)
  return false;
#undef DO_
}

void ErrorResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tsumaki.ErrorResponse)
  // optional int32 code = 1;
  if (this->code() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->code(), output);
  }

  // optional string msg = 2;
  if (this->msg().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->msg().data(), this->msg().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tsumaki.ErrorResponse.msg");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->msg(), output);
  }

  // @@protoc_insertion_point(serialize_end:tsumaki.ErrorResponse)
}

::google::protobuf::uint8* ErrorResponse::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:tsumaki.ErrorResponse)
  // optional int32 code = 1;
  if (this->code() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->code(), target);
  }

  // optional string msg = 2;
  if (this->msg().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->msg().data(), this->msg().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tsumaki.ErrorResponse.msg");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->msg(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:tsumaki.ErrorResponse)
  return target;
}

int ErrorResponse::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:tsumaki.ErrorResponse)
  int total_size = 0;

  // optional int32 code = 1;
  if (this->code() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->code());
  }

  // optional string msg = 2;
  if (this->msg().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->msg());
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ErrorResponse::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tsumaki.ErrorResponse)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const ErrorResponse* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const ErrorResponse>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tsumaki.ErrorResponse)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tsumaki.ErrorResponse)
    MergeFrom(*source);
  }
}

void ErrorResponse::MergeFrom(const ErrorResponse& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tsumaki.ErrorResponse)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  if (from.code() != 0) {
    set_code(from.code());
  }
  if (from.msg().size() > 0) {

    msg_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.msg_);
  }
}

void ErrorResponse::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tsumaki.ErrorResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ErrorResponse::CopyFrom(const ErrorResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tsumaki.ErrorResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ErrorResponse::IsInitialized() const {

  return true;
}

void ErrorResponse::Swap(ErrorResponse* other) {
  if (other == this) return;
  InternalSwap(other);
}
void ErrorResponse::InternalSwap(ErrorResponse* other) {
  std::swap(code_, other->code_);
  msg_.Swap(&other->msg_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata ErrorResponse::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ErrorResponse_descriptor_;
  metadata.reflection = ErrorResponse_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// ErrorResponse

// optional int32 code = 1;
void ErrorResponse::clear_code() {
  code_ = 0;
}
 ::google::protobuf::int32 ErrorResponse::code() const {
  // @@protoc_insertion_point(field_get:tsumaki.ErrorResponse.code)
  return code_;
}
 void ErrorResponse::set_code(::google::protobuf::int32 value) {
  
  code_ = value;
  // @@protoc_insertion_point(field_set:tsumaki.ErrorResponse.code)
}

// optional string msg = 2;
void ErrorResponse::clear_msg() {
  msg_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 const ::std::string& ErrorResponse::msg() const {
  // @@protoc_insertion_point(field_get:tsumaki.ErrorResponse.msg)
  return msg_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void ErrorResponse::set_msg(const ::std::string& value) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tsumaki.ErrorResponse.msg)
}
 void ErrorResponse::set_msg(const char* value) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tsumaki.ErrorResponse.msg)
}
 void ErrorResponse::set_msg(const char* value, size_t size) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tsumaki.ErrorResponse.msg)
}
 ::std::string* ErrorResponse::mutable_msg() {
  
  // @@protoc_insertion_point(field_mutable:tsumaki.ErrorResponse.msg)
  return msg_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 ::std::string* ErrorResponse::release_msg() {
  // @@protoc_insertion_point(field_release:tsumaki.ErrorResponse.msg)
  
  return msg_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void ErrorResponse::set_allocated_msg(::std::string* msg) {
  if (msg != NULL) {
    
  } else {
    
  }
  msg_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), msg);
  // @@protoc_insertion_point(field_set_allocated:tsumaki.ErrorResponse.msg)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace tsumaki

// @@protoc_insertion_point(global_scope)
