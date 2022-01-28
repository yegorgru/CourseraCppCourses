#pragma once

#include <variant>
#include <map>
#include <string>
#include <vector>
#include <iostream>

namespace Json {

    class Node : std::variant<std::vector<Node>,
        std::map<std::string, Node>,
        double,
        std::string,
        bool> {
    public:
        using variant::variant;

        const auto& AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }
        const auto& AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }
        double AsDouble() const {
            return std::get<double>(*this);
        }
        const auto& AsString() const {
            return std::get<std::string>(*this);
        }
        const auto& AsBoolean() const {
            return std::get<bool>(*this);
        }
    };

    class Document {
    public:
        explicit Document(Node&& root) : root(std::move(root)) { }

        const Node& GetRoot() const {
            return root;
        }

    private:
        Node root;
    };

    Document Load(std::istream& input);

    Node LoadNode(std::istream& input);

    Node LoadArray(std::istream& input) {
        std::vector<Node> result;

        for (char c; input >> c && c != ']'; ) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadOther(std::istream& input) {
        if (input.peek() == 't') {
            input.ignore(4);
            return Node(true);
        }
        else if (input.peek() == 'f') {
            input.ignore(5);
            return Node(false);
        }
        double result;
        std::cin >> result;
        return Node(result);
    }

    Node LoadString(std::istream& input) {
        std::string line;
        std::getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadDict(std::istream& input) {
        std::map<std::string, Node> result;

        for (char c; input >> c && c != '}'; ) {
            if (c == ',') {
                input >> c;
            }

            std::string key = LoadString(input).AsString();
            input >> c;
            result.emplace(std::move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(std::istream& input) {
        char c;
        input >> c;

        if (c == '[') {
            return LoadArray(input);
        }
        else if (c == '{') {
            return LoadDict(input);
        }
        else if (c == '"') {
            return LoadString(input);
        }
        else {
            input.putback(c);
            return LoadOther(input);
        }
    }

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }
}