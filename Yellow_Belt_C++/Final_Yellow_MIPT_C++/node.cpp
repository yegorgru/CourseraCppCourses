#include "node.h"

DateComparisonNode::DateComparisonNode(const Comparison& cmp, const Date& d)
	:comparison(cmp), date(d){}

bool DateComparisonNode::Evaluate(const Date& date, const std::string& event) const {
	if (comparison == Comparison::Less) {
		return date < this->date;
	}
	else if (comparison == Comparison::LessOrEqual) {
		return date <= this->date;
	}
	else if (comparison == Comparison::Greater) {
		return date > this->date;
	}
	else if (comparison == Comparison::GreaterOrEqual) {
		return date >= this->date;
	}
	else if (comparison == Comparison::Equal) {
		return date == this->date;
	}
	else if (comparison == Comparison::NotEqual) {
		return date != this->date;
	}
	return false;
}



EventComparisonNode::EventComparisonNode(const Comparison& cmp, const std::string& ev)
	:comparison(cmp), event(ev) {}

bool EventComparisonNode::Evaluate(const Date& date, const std::string& event) const {
	if (comparison == Comparison::Less) {
		return event < this->event;
	}
	else if (comparison == Comparison::LessOrEqual) {
		return event <= this->event;
	}
	else if (comparison == Comparison::Greater) {
		return event > this->event;
	}
	else if (comparison == Comparison::GreaterOrEqual) {
		return event >= this->event;
	}
	else if (comparison == Comparison::Equal) {
		return event == this->event;
	}
	else if (comparison == Comparison::NotEqual) {
		return event != this->event;
	}
	return false;
}


LogicalOperationNode::LogicalOperationNode(const LogicalOperation& op, std::shared_ptr<Node> l, std::shared_ptr<Node> r)
{
	operation = op;
	left = l;
	right = r;
}

bool LogicalOperationNode::Evaluate(const Date& date, const std::string& event) const
{
	if (operation == LogicalOperation::And) {
		return left->Evaluate(date, event) && right->Evaluate(date, event);
	}
	else {
		return left->Evaluate(date, event) || right->Evaluate(date, event);
	}
}

bool EmptyNode::Evaluate(const Date& date, const std::string& event) const
{
	return true;
}