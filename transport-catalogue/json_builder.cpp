/**
 * @file json_builder.cpp
 * @brief Implementation of the Builder class for constructing JSON documents.
 */

#include "json_builder.h"

namespace json {

    Builder::Builder() {
    }

    Node Builder::Build() const {
        if (on_top()) {
            return root_;
        } else {
            throw std::logic_error("Returning an incomplete document");
        }
    }


    Builder::BaseContext Builder::Value(const Node & value) {
        if (in_array()) {
            nodes_stack_.back()->AsArray().push_back(value);
        } else if (in_dict()) {
            if (current_key_) {
                nodes_stack_.back()->AsDict()[current_key_.value()] = value;
                current_key_.reset();
            } else {
                throw std::logic_error("Inserting a value with no key");
            }
        } else if (on_top()) {
            root_ = value;
        } else {
            throw std::logic_error("Trying to put value while neither on top nor in array/dict");
        }
        return BaseContext(this);
    }


    Builder::DictValueContext Builder::Key(const std::string &key) {
        if (in_dict()) {
            current_key_ = key;
        } else {
            throw std::logic_error("Adding a key while not in dictionary");
        }
        return DictValueContext(this);
    }

    Builder::DictItemContext Builder::StartDict() {
        start_container(Dict());
        return DictItemContext(this);
    }

    Builder::BaseContext Builder::EndDict() {
        if (in_dict()) {
            nodes_stack_.pop_back();
        } else {
            throw std::logic_error("Ending a dictionary while not in dictionary");
        }
        return BaseContext(this);
    }

    Builder::ArrayItemContext Builder::StartArray() {
        start_container(Array());
        return ArrayItemContext(this);
    }

    Builder::BaseContext Builder::EndArray() {
        if (in_array()) {
            nodes_stack_.pop_back();
        } else {
            throw std::logic_error("Ending an array while not in dictionary");
        }
        return BaseContext(this);
    }

    void Builder::start_container(Node && container) {
        if (in_array()) {
            nodes_stack_.back()->AsArray().push_back(container);
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        } else if (in_dict()) {
            if (current_key_) {
                nodes_stack_.back()->AsDict()[current_key_.value()] = container;
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_.value()]);
                current_key_.reset();
            } else {
                throw std::logic_error("Inserting a value with no key");
            }
        } else if (on_top()) {
            root_ = container;
            nodes_stack_.push_back(&root_);
        } else {
            throw std::logic_error("Starting an array/dict while neither on top nor in array/dict");
        }
    }

    bool Builder::on_top() const {
        return nodes_stack_.empty();
    }

    bool Builder::in_array() const {
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsArray()) {
                return true;
            }
        }
        return false;
    }

    bool Builder::in_dict() const {
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsDict()) {
                return true;
            }
        }
        return false;
    }

    Node Builder::BaseContext::Build() {
        return builder_->Build();
    }

    Builder::DictValueContext Builder::BaseContext::Key(const std::string &key)
    {
        return builder_->Key(key);
    }

    Builder::DictItemContext Builder::BaseContext::StartDict()
    {
        return builder_->StartDict();
    }

    Builder::BaseContext Builder::BaseContext::EndDict()
    {
        return builder_->EndDict();
    }

    Builder::ArrayItemContext Builder::BaseContext::StartArray()
    {
        return builder_->StartArray();
    }

    Builder::BaseContext Builder::BaseContext::EndArray()
    {
        return builder_->EndArray();
    }


    Builder::BaseContext Builder::BaseContext::Value(const Node & value) {
        return builder_->Value(value);
    }

    Builder::DictItemContext Builder::DictValueContext::Value(const Node & value) {
        builder_->Value(value);
        return DictItemContext(builder_);
    }

    Builder::ArrayItemContext Builder::ArrayItemContext::Value(const Node & value) {
        builder_->Value(value);
        return ArrayItemContext(builder_);
    }
}