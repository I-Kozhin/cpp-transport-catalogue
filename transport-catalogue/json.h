#pragma once

/**
 *@file json.h
 *@brief This file contains the declaration of the JSON library, including classes for parsing and working with JSON data.
 */

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    /**
     * @class ParsingError
     * @brief Custom exception class for JSON parsing errors.
     */
    class ParsingError : public std::runtime_error {
        public:
            using runtime_error::runtime_error;
    };

    /**
     * @class Node
     * @brief Represents a JSON node, which can hold various types of values.
     */
    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
        public:
            using variant::variant;
            using Value = variant;

            /**
             * @brief Checks if the node holds an integer value.
             * @return True if the node holds an integer value, false otherwise.
             */
            bool IsInt() const {
                return std::holds_alternative<int>(*this);
            }

            /**
             * @brief Retrieves the integer value held by the node.
             * @note Throws a logic_error if the node does not hold an integer value.
             * @return The integer value.
             */
            int AsInt() const {
                using namespace std::literals;
                if (!IsInt()) {
                    throw std::logic_error("Not an int"s);
                }
                return std::get<int>(*this);
            }

            /**
             * @brief Checks if the node holds a pure double value (not derived from an integer).
             * @return True if the node holds a pure double value, false otherwise.
             */
            bool IsPureDouble() const {
                return std::holds_alternative<double>(*this);
            }

            /**
             * @brief Checks if the node holds a double value (may be derived from an integer).
             * @return True if the node holds a double value, false otherwise.
             */
            bool IsDouble() const {
                return IsInt() || IsPureDouble();
            }
            
            /**
             * @brief Retrieves the double value held by the node.
             * @note Throws a logic_error if the node does not hold a double value.
             * @return The double value.
             */
            double AsDouble() const {
                using namespace std::literals;
                if (!IsDouble()) {
                    throw std::logic_error("Not a double"s);
                }
                return IsPureDouble() ? std::get<double>(*this) : AsInt();
            }

            /**
             * @brief Checks if the node holds a boolean value.
             * @return True if the node holds a boolean value, false otherwise.
             */
            bool IsBool() const {
                return std::holds_alternative<bool>(*this);
            }

            /**
             * @brief Retrieves the boolean value held by the node.
             * @note Throws a logic_error if the node does not hold a boolean value.
             * @return The boolean value.
             */
            bool AsBool() const {
                using namespace std::literals;
                if (!IsBool()) {
                    throw std::logic_error("Not a bool"s);
                }

                return std::get<bool>(*this);
            }

            /**
             * @brief Checks if the node is null.
             * @return True if the node is null, false otherwise.
             */
            bool IsNull() const {
                return std::holds_alternative<std::nullptr_t>(*this);
            }

            /**
             * @brief Checks if the node holds an array value.
             * @return True if the node holds an array value, false otherwise.
             */
            bool IsArray() const {
                return std::holds_alternative<Array>(*this);
            }

            /**
             * @brief Retrieves the array value held by the node.
             * @note Throws a logic_error if the node does not hold an array value.
             * @return The array value.
             */
            const Array& AsArray() const {
                using namespace std::literals;
                if (!IsArray()) {
                    throw std::logic_error("Not an array"s);
                }

                return std::get<Array>(*this);
            }

            /**
             * @brief Retrieves the array value held by the node (non-const version).
             * @note Throws a logic_error if the node does not hold an array value.
             * @return The array value.
             */
            Array& AsArray() {
                using namespace std::literals;
                if (!IsArray()) {
                    throw std::logic_error("Not an array"s);
                }

                return std::get<Array>(*this);
            }

            /**
             * @brief Checks if the node holds a string value.
             * @return True if the node holds a string value, false otherwise.
             */
            bool IsString() const {
                return std::holds_alternative<std::string>(*this);
            }

            /**
             * @brief Retrieves the string value held by the node.
             * @note Throws a logic_error if the node does not hold a string value.
             * @return The string value.
             */
            const std::string& AsString() const {
                using namespace std::literals;
                if (!IsString()) {
                    throw std::logic_error("Not a string"s);
                }

                return std::get<std::string>(*this);
            }

            /**
             * @brief Checks if the node holds a dictionary (object) value.
             * @return True if the node holds a dictionary value, false otherwise.
             */
            bool IsDict() const {
                return std::holds_alternative<Dict>(*this);
            }

            /**
             * @brief Retrieves the dictionary (object) value held by the node.
             * @note Throws a logic_error if the node does not hold a dictionary value.
             * @return The dictionary value.
             */
            const Dict& AsDict() const {
                using namespace std::literals;
                if (!IsDict()) {
                    throw std::logic_error("Not a dict"s);
                }

                return std::get<Dict>(*this);
            }

            /**
             * @brief Retrieves the dictionary (object) value held by the node (non-const version).
             * @note Throws a logic_error if the node does not hold a dictionary value.
             * @return The dictionary value.
             */
            Dict& AsDict() {
                using namespace std::literals;
                if (!IsDict()) {
                    throw std::logic_error("Not a dict"s);
                }

                return std::get<Dict>(*this);
            }

            /**
             * @brief Checks if two nodes are equal.
             * @param rhs The node to compare with.
             * @return True if the nodes are equal, false otherwise.
             */
            bool operator==(const Node& rhs) const {
                return GetValue() == rhs.GetValue();
            }

            /**
             * @brief Retrieves the underlying value of the node.
             * @return The value of the node.
             */
            const Value& GetValue() const {
                return *this;
            }
    };

    /**
     * @brief Checks if two Node objects are not equal.
     * @param lhs The left-hand side Node.
     * @param rhs The right-hand side Node.
     * @return True if the nodes are not equal, false otherwise.
     */
    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @class Document
     * @brief Represents a JSON document.
     */
    class Document {
        public:
            explicit Document(Node root)
                : root_(std::move(root)) {
            }

            const Node& GetRoot() const {
                return root_;
            }

        private:
            Node root_;
    };

    /**
     * @brief Checks if two Document objects are equal.
     * @param lhs The left-hand side Document.
     * @param rhs The right-hand side Document.
     * @return True if the documents are equal, false otherwise.
     */
    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    /**
     * @brief Checks if two Document objects are not equal.
     * @param lhs The left-hand side Document.
     * @param rhs The right-hand side Document.
     * @return True if the documents are not equal, false otherwise.
     */
    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Loads a JSON document from the input stream.
     * @param input The input stream.
     * @return The loaded JSON document.
     * @throws ParsingError if there is an error while parsing the JSON.
     */
    Document Load(std::istream& input);

    /**
     * @brief Prints a JSON document to the output stream.
     * @param doc The JSON document to print.
     * @param output The output stream.
     */
    void Print(const Document& doc, std::ostream& output);

}  // namespace json