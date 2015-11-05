// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_ATBUSPROTO_ATBUS_PROTOCOL_H_
#define FLATBUFFERS_GENERATED_ATBUSPROTO_ATBUS_PROTOCOL_H_

#include "flatbuffers/flatbuffers.h"


namespace atbus {
namespace protocol {

struct data_transform;
struct channel_data;
struct node_data;
struct node_tree;
struct msg_body;
struct msg_head;
struct msg;

enum CMD {
  CMD_CMD_INVALID = 0,
  CMD_CMD_DATA_TRANSFORM_REQ = 1,
  CMD_CMD_DATA_TRANSFORM_RSP = 2,
  CMD_CMD_NODE_SYNC_REQ = 33,
  CMD_CMD_NODE_SYNC_RSP = 34,
  CMD_CMD_NODE_ADD_REQ = 35,
  CMD_CMD_NODE_ADD_RSP = 36,
  CMD_CMD_NODE_MOD_REQ = 37,
  CMD_CMD_NODE_MOD_RSP = 38,
  CMD_CMD_NODE_DEL_REQ = 39,
  CMD_CMD_NODE_DEL_RSP = 40
};

inline const char **EnumNamesCMD() {
  static const char *names[] = { "CMD_INVALID", "CMD_DATA_TRANSFORM_REQ", "CMD_DATA_TRANSFORM_RSP", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "CMD_NODE_SYNC_REQ", "CMD_NODE_SYNC_RSP", "CMD_NODE_ADD_REQ", "CMD_NODE_ADD_RSP", "CMD_NODE_MOD_REQ", "CMD_NODE_MOD_RSP", "CMD_NODE_DEL_REQ", "CMD_NODE_DEL_RSP", nullptr };
  return names;
}

inline const char *EnumNameCMD(CMD e) { return EnumNamesCMD()[static_cast<int>(e)]; }

MANUALLY_ALIGNED_STRUCT(1) data_transform FLATBUFFERS_FINAL_CLASS {
 private:

 public:
  data_transform()
    :  { }

};
STRUCT_END(data_transform, 0);

MANUALLY_ALIGNED_STRUCT(4) channel_data FLATBUFFERS_FINAL_CLASS {
 private:
  uint32_t priority_;

 public:
  channel_data(uint32_t priority)
    : priority_(flatbuffers::EndianScalar(priority)) { }

  uint32_t priority() const { return flatbuffers::EndianScalar(priority_); }
};
STRUCT_END(channel_data, 4);

struct node_data FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  uint64_t bus_id() const { return GetField<uint64_t>(4, 0); }
  uint8_t overwrite() const { return GetField<uint8_t>(6, 0); }
  uint8_t has_global_tree() const { return GetField<uint8_t>(8, 0); }
  uint8_t children_id_mask() const { return GetField<uint8_t>(10, 0); }
  const flatbuffers::Vector<flatbuffers::Offset<node_data>> *children() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<node_data>> *>(12); }
  const flatbuffers::Vector<const channel_data *> *channels() const { return GetPointer<const flatbuffers::Vector<const channel_data *> *>(14); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, 4 /* bus_id */) &&
           VerifyField<uint8_t>(verifier, 6 /* overwrite */) &&
           VerifyField<uint8_t>(verifier, 8 /* has_global_tree */) &&
           VerifyField<uint8_t>(verifier, 10 /* children_id_mask */) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 12 /* children */) &&
           verifier.Verify(children()) &&
           verifier.VerifyVectorOfTables(children()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 14 /* channels */) &&
           verifier.Verify(channels()) &&
           verifier.EndTable();
  }
};

struct node_dataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_bus_id(uint64_t bus_id) { fbb_.AddElement<uint64_t>(4, bus_id, 0); }
  void add_overwrite(uint8_t overwrite) { fbb_.AddElement<uint8_t>(6, overwrite, 0); }
  void add_has_global_tree(uint8_t has_global_tree) { fbb_.AddElement<uint8_t>(8, has_global_tree, 0); }
  void add_children_id_mask(uint8_t children_id_mask) { fbb_.AddElement<uint8_t>(10, children_id_mask, 0); }
  void add_children(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<node_data>>> children) { fbb_.AddOffset(12, children); }
  void add_channels(flatbuffers::Offset<flatbuffers::Vector<const channel_data *>> channels) { fbb_.AddOffset(14, channels); }
  node_dataBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  node_dataBuilder &operator=(const node_dataBuilder &);
  flatbuffers::Offset<node_data> Finish() {
    auto o = flatbuffers::Offset<node_data>(fbb_.EndTable(start_, 6));
    return o;
  }
};

inline flatbuffers::Offset<node_data> Createnode_data(flatbuffers::FlatBufferBuilder &_fbb,
   uint64_t bus_id = 0,
   uint8_t overwrite = 0,
   uint8_t has_global_tree = 0,
   uint8_t children_id_mask = 0,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<node_data>>> children = 0,
   flatbuffers::Offset<flatbuffers::Vector<const channel_data *>> channels = 0) {
  node_dataBuilder builder_(_fbb);
  builder_.add_bus_id(bus_id);
  builder_.add_channels(channels);
  builder_.add_children(children);
  builder_.add_children_id_mask(children_id_mask);
  builder_.add_has_global_tree(has_global_tree);
  builder_.add_overwrite(overwrite);
  return builder_.Finish();
}

struct node_tree FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const flatbuffers::Vector<flatbuffers::Offset<node_data>> *nodes() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<node_data>> *>(4); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* nodes */) &&
           verifier.Verify(nodes()) &&
           verifier.VerifyVectorOfTables(nodes()) &&
           verifier.EndTable();
  }
};

struct node_treeBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_nodes(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<node_data>>> nodes) { fbb_.AddOffset(4, nodes); }
  node_treeBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  node_treeBuilder &operator=(const node_treeBuilder &);
  flatbuffers::Offset<node_tree> Finish() {
    auto o = flatbuffers::Offset<node_tree>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<node_tree> Createnode_tree(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<node_data>>> nodes = 0) {
  node_treeBuilder builder_(_fbb);
  builder_.add_nodes(nodes);
  return builder_.Finish();
}

struct msg_body FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const data_transform *transfer() const { return GetStruct<const data_transform *>(4); }
  const node_tree *nodes() const { return GetPointer<const node_tree *>(6); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<data_transform>(verifier, 4 /* transfer */) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* nodes */) &&
           verifier.VerifyTable(nodes()) &&
           verifier.EndTable();
  }
};

struct msg_bodyBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_transfer(const data_transform *transfer) { fbb_.AddStruct(4, transfer); }
  void add_nodes(flatbuffers::Offset<node_tree> nodes) { fbb_.AddOffset(6, nodes); }
  msg_bodyBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  msg_bodyBuilder &operator=(const msg_bodyBuilder &);
  flatbuffers::Offset<msg_body> Finish() {
    auto o = flatbuffers::Offset<msg_body>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<msg_body> Createmsg_body(flatbuffers::FlatBufferBuilder &_fbb,
   const data_transform *transfer = 0,
   flatbuffers::Offset<node_tree> nodes = 0) {
  msg_bodyBuilder builder_(_fbb);
  builder_.add_nodes(nodes);
  builder_.add_transfer(transfer);
  return builder_.Finish();
}

struct msg_head FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  CMD cmd() const { return static_cast<CMD>(GetField<int8_t>(4, 0)); }
  int32_t type() const { return GetField<int32_t>(6, 0); }
  int32_t ret() const { return GetField<int32_t>(8, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, 4 /* cmd */) &&
           VerifyField<int32_t>(verifier, 6 /* type */) &&
           VerifyField<int32_t>(verifier, 8 /* ret */) &&
           verifier.EndTable();
  }
};

struct msg_headBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_cmd(CMD cmd) { fbb_.AddElement<int8_t>(4, static_cast<int8_t>(cmd), 0); }
  void add_type(int32_t type) { fbb_.AddElement<int32_t>(6, type, 0); }
  void add_ret(int32_t ret) { fbb_.AddElement<int32_t>(8, ret, 0); }
  msg_headBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  msg_headBuilder &operator=(const msg_headBuilder &);
  flatbuffers::Offset<msg_head> Finish() {
    auto o = flatbuffers::Offset<msg_head>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<msg_head> Createmsg_head(flatbuffers::FlatBufferBuilder &_fbb,
   CMD cmd = CMD_CMD_INVALID,
   int32_t type = 0,
   int32_t ret = 0) {
  msg_headBuilder builder_(_fbb);
  builder_.add_ret(ret);
  builder_.add_type(type);
  builder_.add_cmd(cmd);
  return builder_.Finish();
}

struct msg FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const msg_head *head() const { return GetPointer<const msg_head *>(4); }
  const msg_body *body() const { return GetPointer<const msg_body *>(6); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* head */) &&
           verifier.VerifyTable(head()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* body */) &&
           verifier.VerifyTable(body()) &&
           verifier.EndTable();
  }
};

struct msgBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_head(flatbuffers::Offset<msg_head> head) { fbb_.AddOffset(4, head); }
  void add_body(flatbuffers::Offset<msg_body> body) { fbb_.AddOffset(6, body); }
  msgBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  msgBuilder &operator=(const msgBuilder &);
  flatbuffers::Offset<msg> Finish() {
    auto o = flatbuffers::Offset<msg>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<msg> Createmsg(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<msg_head> head = 0,
   flatbuffers::Offset<msg_body> body = 0) {
  msgBuilder builder_(_fbb);
  builder_.add_body(body);
  builder_.add_head(head);
  return builder_.Finish();
}

inline const atbus::protocol::msg *Getmsg(const void *buf) { return flatbuffers::GetRoot<atbus::protocol::msg>(buf); }

inline bool VerifymsgBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<atbus::protocol::msg>(); }

inline void FinishmsgBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<atbus::protocol::msg> root) { fbb.Finish(root); }

}  // namespace protocol
}  // namespace atbus

#endif  // FLATBUFFERS_GENERATED_ATBUSPROTO_ATBUS_PROTOCOL_H_