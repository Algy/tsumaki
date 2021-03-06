// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: MaskImage.proto

#ifndef PROTOBUF_MaskImage_2eproto__INCLUDED
#define PROTOBUF_MaskImage_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace tsumaki {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_MaskImage_2eproto();
void protobuf_AssignDesc_MaskImage_2eproto();
void protobuf_ShutdownFile_MaskImage_2eproto();

class MaskImage;

// ===================================================================

class MaskImage : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tsumaki.MaskImage) */ {
 public:
  MaskImage();
  virtual ~MaskImage();

  MaskImage(const MaskImage& from);

  inline MaskImage& operator=(const MaskImage& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MaskImage& default_instance();

  void Swap(MaskImage* other);

  // implements Message ----------------------------------------------

  inline MaskImage* New() const { return New(NULL); }

  MaskImage* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MaskImage& from);
  void MergeFrom(const MaskImage& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(MaskImage* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 width = 1;
  void clear_width();
  static const int kWidthFieldNumber = 1;
  ::google::protobuf::int32 width() const;
  void set_width(::google::protobuf::int32 value);

  // optional int32 height = 2;
  void clear_height();
  static const int kHeightFieldNumber = 2;
  ::google::protobuf::int32 height() const;
  void set_height(::google::protobuf::int32 value);

  // optional bytes data = 3;
  void clear_data();
  static const int kDataFieldNumber = 3;
  const ::std::string& data() const;
  void set_data(const ::std::string& value);
  void set_data(const char* value);
  void set_data(const void* value, size_t size);
  ::std::string* mutable_data();
  ::std::string* release_data();
  void set_allocated_data(::std::string* data);

  // @@protoc_insertion_point(class_scope:tsumaki.MaskImage)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::int32 width_;
  ::google::protobuf::int32 height_;
  ::google::protobuf::internal::ArenaStringPtr data_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_MaskImage_2eproto();
  friend void protobuf_AssignDesc_MaskImage_2eproto();
  friend void protobuf_ShutdownFile_MaskImage_2eproto();

  void InitAsDefaultInstance();
  static MaskImage* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// MaskImage

// optional int32 width = 1;
inline void MaskImage::clear_width() {
  width_ = 0;
}
inline ::google::protobuf::int32 MaskImage::width() const {
  // @@protoc_insertion_point(field_get:tsumaki.MaskImage.width)
  return width_;
}
inline void MaskImage::set_width(::google::protobuf::int32 value) {
  
  width_ = value;
  // @@protoc_insertion_point(field_set:tsumaki.MaskImage.width)
}

// optional int32 height = 2;
inline void MaskImage::clear_height() {
  height_ = 0;
}
inline ::google::protobuf::int32 MaskImage::height() const {
  // @@protoc_insertion_point(field_get:tsumaki.MaskImage.height)
  return height_;
}
inline void MaskImage::set_height(::google::protobuf::int32 value) {
  
  height_ = value;
  // @@protoc_insertion_point(field_set:tsumaki.MaskImage.height)
}

// optional bytes data = 3;
inline void MaskImage::clear_data() {
  data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& MaskImage::data() const {
  // @@protoc_insertion_point(field_get:tsumaki.MaskImage.data)
  return data_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void MaskImage::set_data(const ::std::string& value) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tsumaki.MaskImage.data)
}
inline void MaskImage::set_data(const char* value) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tsumaki.MaskImage.data)
}
inline void MaskImage::set_data(const void* value, size_t size) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tsumaki.MaskImage.data)
}
inline ::std::string* MaskImage::mutable_data() {
  
  // @@protoc_insertion_point(field_mutable:tsumaki.MaskImage.data)
  return data_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* MaskImage::release_data() {
  // @@protoc_insertion_point(field_release:tsumaki.MaskImage.data)
  
  return data_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void MaskImage::set_allocated_data(::std::string* data) {
  if (data != NULL) {
    
  } else {
    
  }
  data_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), data);
  // @@protoc_insertion_point(field_set_allocated:tsumaki.MaskImage.data)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace tsumaki

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_MaskImage_2eproto__INCLUDED
