/**
 *@file json.cpp
 *@brief This file contains the implementation of the JSON library, including classes for parsing and working with JSON data.
 */
#include "json.h"

#include <iterator>

namespace json {

    namespace {
        using namespace std::literals;

        /**
         * @brief Loads a JSON node from the input stream.
         * @param input The input stream.
         * @return The loaded JSON node.
         * @throws ParsingError if there is an error while parsing the JSON.
         */
        Node LoadNode(std::istream& input);

        /**
         * @brief Loads a JSON string from the input stream.
         * @param input The input stream.
         * @return The loaded JSON string.
         * @throws ParsingError if there is an error while parsing the string.
         */
        Node LoadString(std::istream& input);

        /**
         * @brief Loads a JSON literal from the input stream.
         * @param input The input stream.
         * @return The loaded JSON literal.
         * @throws ParsingError if there is an error while parsing the literal.
         */
        std::string LoadLiteral(std::istream& input) {
            std::string s;
            while (std::isalpha(input.peek())) {
                s.push_back(static_cast<char>(input.get()));
            }
            return s;
        }

        /**
         * @brief Loads a JSON array from the input stream.
         * @param input The input stream.
         * @return The loaded JSON array.
         * @throws ParsingError if there is an error while parsing the array.
         */
        Node LoadArray(std::istream& input) {
            std::vector<Node> result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input) {
                throw ParsingError("Array parsing error"s);
            }
            return Node(std::move(result));
        }

        /**
         * @brief Loads a JSON dictionary (object) from the input stream.
         * @param input The input stream.
         * @return The loaded JSON dictionary.
         * @throws ParsingError if there is an error while parsing the dictionary.
         */
        Node LoadDict(std::istream& input) {
            Dict dict;

            for (char c; input >> c && c != '}';) {
                if (c == '"') {
                    std::string key = LoadString(input).AsString();
                    if (input >> c && c == ':') {
                        if (dict.find(key) != dict.end()) {
                            throw ParsingError("Duplicate key '"s + key + "' have been found");
                        }
                        dict.emplace(std::move(key), LoadNode(input));
                    }
                    else {
                        throw ParsingError(": is expected but '"s + c + "' has been found"s);
                    }
                }
                else if (c != ',') {
                    throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
                }
            }
            if (!input) {
                throw ParsingError("Dictionary parsing error"s);
            }
            return Node(std::move(dict));
        }

        /**
         * @brief Loads a JSON string from the input stream.
         * @param input The input stream.
         * @return The loaded JSON string.
         * @throws ParsingError if there is an error while parsing the string.
         */
        Node LoadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(std::move(s));
        }

        /**
         * @brief Loads a JSON boolean value from the input stream.
         * @param input The input stream.
         * @return The loaded JSON boolean value.
         * @throws ParsingError if there is an error while parsing the boolean value.
         */
        Node LoadBool(std::istream& input) {
            const auto s = LoadLiteral(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Failed to parse '"s + s + "' as bool"s);
            }
        }

        /**
         * @brief Loads a JSON null value from the input stream.
         * @param input The input stream.
         * @return The loaded JSON null value.
         * @throws ParsingError if there is an error while parsing the null value.
         */
        Node LoadNull(std::istream& input) {
            if (auto literal = LoadLiteral(input); literal == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Failed to parse '"s + literal + "' as null"s);
            }
        }

        /**
         * @brief Loads a JSON number from the input stream.
         * @param input The input stream.
         * @return The loaded JSON number.
         * @throws ParsingError if there is an error while parsing the number.
         */
        Node LoadNumber(std::istream& input) {
            std::string parsed_num;

            // Reads the next character from the input stream and appends it to parsed_num.
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Reads one or more digits from the input stream and appends them to parsed_num.
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Parses the integer part of the number.
            // No other digits can follow in JSON after 0.
            if (input.peek() == '0') {
                read_char();
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Parses the decimal part of the number.
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Parses the exponential part of the number.
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // First, try to convert the parsed_num string to an int.
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // the code below will attempt to convert the string to a double.
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        /**
         * @brief Parses a JSON node from the input stream. 
         * This function reads the next character from the input stream and determines the type of JSON node to parse.
         * It calls the corresponding parsing function based on the character read, such as LoadArray, LoadDict, LoadString,
         * LoadBool, LoadNull, or LoadNumber.
         * @param input The input stream to read characters from.
         * @return The parsed JSON node as a Node object.
         * @throws ParsingError if there is an unexpected end of file or an error while parsing the node.
         */
        Node LoadNode(std::istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Unexpected EOF"s);
            }
            switch (c) {
            case '[':
                return LoadArray(input);
            case '{':
                return LoadDict(input);
            case '"':
                return LoadString(input);
            case 't':
                [[fallthrough]];
            case 'f':
                input.putback(c);
                return LoadBool(input);
            case 'n':
                input.putback(c);
                return LoadNull(input);
            default:
                input.putback(c);
                return LoadNumber(input);
            }
        }

        /**
         * @brief Represents the print context for formatting JSON output.
        */
        struct PrintContext {
            std::ostream& output;   /*< The output stream to write the formatted JSON. */
            int indent_step = 4;    /*< The number of spaces for each indentation level. */
            int indent = 0;         /*< The current indentation level. */

            /**
             * @brief Prints the appropriate number of spaces for the current indentation level.
             */
            void PrintIndent() const {
                for (int i = 0; i < indent; ++i) {
                    output.put(' ');
                }
            }
            
            /**
             * @brief Creates a new PrintContext with increased indentation level.
             * @return The new PrintContext object with increased indentation level. 
             */
            PrintContext Indented() const {
                return { output, indent_step, indent_step + indent };
            }
        };

        /**
         * @brief Prints a JSON node to the output stream.
         * @param value The JSON node to print.
         * @param ctx The print context.
         */
        void PrintNode(const Node& value, const PrintContext& ctx);

        /**
         * @brief Prints a JSON value to the output stream.
         * @tparam Value The type of the value.
         * @param value The JSON value to print.
         * @param ctx The print context.
         */
        template <typename Value>
        void PrintValue(const Value& value, const PrintContext& ctx) {
            ctx.output << value;
        }

        /**
         * @brief Prints a JSON string to the output stream.
         * @param value The JSON string to print.
         * @param output The output stream.
         */
        void PrintString(const std::string& value, std::ostream& output) {
            output.put('"');
            for (const char c : value) {
                switch (c) {
                case '\r':
                    output << "\\r"sv;
                    break;
                case '\n':
                    output << "\\n"sv;
                    break;
                case '"':
                    [[fallthrough]];
                case '\\':
                    output.put('\\');
                    [[fallthrough]];
                default:
                    output.put(c);
                    break;
                }
            }
            output.put('"');
        }

        template <>
        void PrintValue<std::string>(const std::string& value, const PrintContext& ctx) {
            PrintString(value, ctx.output);
        }

        template <>
        void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctx) {
            ctx.output << "null"sv;
        }

        /**
         * @brief Prints a boolean value to the output stream using the provided print context.
         * @param value The boolean value to print.
         * @param ctx The print context that controls the formatting of the output.
         */
        template <>
        void PrintValue<bool>(const bool& value, const PrintContext& ctx) {
            ctx.output << (value ? "true"sv : "false"sv);
        }

        template <>
        void PrintValue<Array>(const Array& nodes, const PrintContext& ctx) {
            std::ostream& output = ctx.output;
            output << "[\n"sv;
            bool first = true;
            auto inner_ctx = ctx.Indented();
            for (const Node& node : nodes) {
                if (first) {
                    first = false;
                }
                else {
                    output << ",\n"sv;
                }
                inner_ctx.PrintIndent();
                PrintNode(node, inner_ctx);
            }
            output.put('\n');
            ctx.PrintIndent();
            output.put(']');
        }

        template <>
        void PrintValue<Dict>(const Dict& nodes, const PrintContext& ctx) {
            std::ostream& output = ctx.output;
            output << "{\n"sv;
            bool first = true;
            auto inner_ctx = ctx.Indented();
            for (const auto& [key, node] : nodes) {
                if (first) {
                    first = false;
                }
                else {
                    output << ",\n"sv;
                }
                inner_ctx.PrintIndent();
                PrintString(key, ctx.output);
                output << ": "sv;
                PrintNode(node, inner_ctx);
            }
            output.put('\n');
            ctx.PrintIndent();
            output.put('}');
        }

        void PrintNode(const Node& node, const PrintContext& ctx) {
            std::visit(
                [&ctx](const auto& value) {
                    PrintValue(value, ctx);
                },
                node.GetValue());
        }

    }  // namespace

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), PrintContext{ output });
    }

}  // namespace json