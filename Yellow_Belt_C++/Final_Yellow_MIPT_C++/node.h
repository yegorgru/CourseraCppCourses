#pragma once

#include <string>

#include "token.h"
#include "date.h"

class Node {
public:
	virtual bool Evaluate(const Date& date, const std::string& event) const = 0;
};

class DateComparisonNode : public Node {
public:
	DateComparisonNode(const Comparison& cmp, const Date& d);

	bool Evaluate(const Date& date, const std::string& event) const override;
private:
	Comparison comparison;
	Date date;
};

class EventComparisonNode : public Node {
public:
	EventComparisonNode(const Comparison& cmp, const std::string& ev);

	bool Evaluate(const Date& date, const std::string& event) const override;
private:
	Comparison comparison;
	std::string event;
};

class EmptyNode : public Node{
	bool Evaluate(const Date& date, const std::string& event) const override;
};


class LogicalOperationNode : public Node {
public:
	LogicalOperationNode(const LogicalOperation& op, std::shared_ptr<Node>l, std::shared_ptr<Node>r);

	bool Evaluate(const Date& date, const std::string& event) const override;
private:
	LogicalOperation operation;
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> right;
};