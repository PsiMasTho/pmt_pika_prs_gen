#pragma once

#define PMT_FW_DECL_NS_CLASS(ns_name_, class_name_) \
  namespace ns_name_ {                              \
  class class_name_;                                \
  }

#define PMT_FW_DECL_CLASS(class_name_) class class_name_
