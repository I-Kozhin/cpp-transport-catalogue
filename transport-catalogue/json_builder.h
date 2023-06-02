#pragma once

/**
 * @file json_builder.h
 * @brief This file contains the declaration of the JSON Builder class and its related context classes.
 */

#include <vector>
#include <optional>
#include "json.h"

namespace json {

    /**
     * @class Builder
     * @brief The Builder class provides a fluent interface for constructing JSON documents.
     */
    class Builder {
        public:
            class BaseContext;
            class DictValueContext;
            class DictItemContext;
            class ArrayItemContext;

        public:

            /**
             * @class BaseContext
             * @brief The BaseContext class represents the base context for building JSON documents.
             */
            class BaseContext {
                public:

                    /**
                     * @brief Constructs a BaseContext object with the specified Builder.
                     * @param builder A pointer to the Builder object.
                     */
                    explicit BaseContext(Builder * builder) { builder_ = builder; }

                    /**
                     * @brief Builds and returns the JSON document.
                     * @return The built JSON document.
                     */
                    Node Build();
                    
                    /**
                     * @brief Adds a value to the JSON document.
                     * @param value The value to add.
                     * @return The BaseContext object for chaining method calls.
                     */
                    BaseContext Value(const json::Node & value);

                    /**
                     * @brief Adds a key to the JSON document.
                     * @param key The key to add.
                     * @return The DictValueContext object for continuing dictionary value construction.
                     */
                    DictValueContext Key(const std::string & key);

                    /**
                     * @brief Starts building a dictionary.
                     * @return The DictItemContext object for continuing dictionary item construction.
                     */
                    DictItemContext StartDict();

                    /**
                     * @brief Ends the construction of a dictionary.
                     * @return The BaseContext object for chaining method calls.
                     */                    
                    BaseContext EndDict();

                    /**
                     * @brief Starts building an array.
                     * @return The ArrayItemContext object for continuing array item construction.
                     */                    
                    ArrayItemContext StartArray();

                    /**
                     * @brief Ends the construction of an array.
                     * @return The BaseContext object for chaining method calls.
                     */                    
                    BaseContext EndArray();

                protected:
                    Builder * builder_;     /**< The pointer to the Builder object. */
            };

            /**
             * @class DictValueContext
             * @brief The DictValueContext class represents the context for building dictionary values.
             */
            class DictValueContext: public BaseContext {
                public:

                    /**
                     * @brief Constructs a DictValueContext object with the specified Builder.
                     * @param builder A pointer to the Builder object.
                     */                
                    explicit DictValueContext(Builder * builder): BaseContext(builder) {}

                    /**
                     * @brief Builds and returns the JSON document.
                     * @return The built JSON document.
                     */
                    Node Build() = delete;

                    /**
                     * @brief Adds a value to the JSON document.
                     * @param value The value to add.
                     * @return The DictItemContext object for continuing dictionary item construction.
                     */
                    DictItemContext Value(const json::Node & value);

                    DictValueContext Key(const std::string & key) = delete;
                    BaseContext EndArray() = delete;
                    BaseContext EndDict() = delete;
            };

            /**
             * @class DictItemContext
             * @brief The DictItemContext class represents the context for building dictionary items.
             */
            class DictItemContext: public BaseContext {
                public:

                    /**
                     * @brief Constructs a DictItemContext object with the specified Builder.
                     * @param builder A pointer to the Builder object.
                     */
                    explicit DictItemContext(Builder * builder): BaseContext(builder) {}

                    /**
                     * @brief Builds and returns the JSON document.
                     * @return The built JSON document.
                     */
                    Node Build() = delete;

                    BaseContext Value(const json::Node & value) = delete;
                    DictItemContext &StartDict() = delete;
                    ArrayItemContext &StartArray() = delete;
                    BaseContext EndArray() = delete;
            };

            /**
             * @class ArrayItemContext
             * @brief The ArrayItemContext class represents the context for building array items.
             */
            class ArrayItemContext: public BaseContext {
                public:

                    /**
                     * @brief Constructs an ArrayItemContext object with the specified Builder.
                     * @param builder A pointer to the Builder object.
                     */                
                    explicit ArrayItemContext(Builder * builder): BaseContext(builder) {}

                    /**
                     * @brief Builds and returns the JSON document.
                     * @return The built JSON document.
                     */
                    Node Build() = delete;

                    /**
                     * @brief Adds a value to the JSON document.
                     * @param value The value to add.
                     * @return The ArrayItemContext object for continuing array item construction.
                     */
                    ArrayItemContext Value(const json::Node & value);

                    DictValueContext Key(const std::string & key) = delete;
                    BaseContext EndDict() = delete;
            };
        public:
            Builder();

            /**
             * @brief Builds and returns the JSON document.
             * @return The built JSON document.
             */            
            Node Build() const;

            /**
             * @brief Adds a value to the JSON document.
             * @param value The value to add.
             * @return The BaseContext object for chaining method calls.
             */
            BaseContext Value(const json::Node & value);

            /**
             * @brief Adds a key to the JSON document.
             * @param key The key to add.
             * @return The DictValueContext object for continuing dictionary value construction.
             */            
            DictValueContext Key(const std::string & key);

            /**
             * @brief Starts building a dictionary.
             * @return The DictItemContext object for continuing dictionary item construction.
             */            
            DictItemContext StartDict();

            /**
             * @brief Ends the construction of a dictionary.
             * @return The BaseContext object for chaining method calls.
             */            
            BaseContext EndDict();

            /**
             * @brief Starts building an array.
             * @return The ArrayItemContext object for continuing array item construction.
             */
            ArrayItemContext StartArray();

            /**
             * @brief Ends the construction of an array.
             * @return The BaseContext object for chaining method calls.
             */
            BaseContext EndArray();

        private:

            /**
             * @brief Starts the construction of a container.
             * @param container The container to start.
             */
            void start_container(Node && container);

            /**
             * @brief Checks if the builder is on top of the stack.
             * @return `true` if the builder is on top, `false` otherwise.
             */
            bool on_top() const;

            /**
             * @brief Checks if the builder is in an array.
             * @return `true` if the builder is in an array, `false` otherwise.
             */            
            bool in_array() const;

            /**
             * @brief Checks if the builder is in a dictionary.
             * @return `true` if the builder is in a dictionary, `false` otherwise.
             */            
            bool in_dict() const;

            Node root_;                                     /**< The root node of the JSON document. */
            std::vector<Node*> nodes_stack_;                /**< The stack of nodes being constructed. */
            std::optional<std::string> current_key_;        /**< The current key being used in dictionary construction. */
    };
}  // namespace json