#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

class ExpWithParent : public Expression {
protected:
    bool isParent = false;

public:
    bool IsParent() const {
        return isParent;
    }

    void SetIsParent() {
        isParent = true;
    }
};

class EValue : public ExpWithParent {
    int value;
public:
    EValue(int value) : value(value) {}

    int Evaluate() const override {
        return value;
    }

    std::string ToString() const override {
        if (isParent) {
            return "(" + std::to_string(value) + ")";
        }
        else {
            return std::to_string(value);
        }
    }
};

class ESum : public ExpWithParent {
    ExpressionPtr left, right;
public:
    ESum(ExpressionPtr left, ExpressionPtr right) : left(move(left)), right(move(right)) {}

    int Evaluate() const override {
        return left->Evaluate() + right->Evaluate();
    }

    std::string ToString() const override {
        if (isParent) {
            return "(" + left->ToString() + "+" + right->ToString() + ")";
        }
        else {
            return left->ToString() + "+" + right->ToString();
        }
    }
};

class EProduct : public ExpWithParent {
    ExpressionPtr left, right;
public:
    EProduct(ExpressionPtr left, ExpressionPtr right) : left(move(left)), right(move(right)) {}

    int Evaluate() const override {
        return left->Evaluate() * right->Evaluate();
    }

    std::string ToString() const override {
        if (isParent) {
            return "(" + left->ToString() + "*" + right->ToString() + ")";
        }
        else {
            return left->ToString() + "*" + right->ToString();
        }
    }
};

ExpressionPtr Value(int value) {
    return make_unique<EValue>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
    dynamic_cast<ExpWithParent*>(left.get())->SetIsParent();
    dynamic_cast<ExpWithParent*>(right.get())->SetIsParent();
    return make_unique<ESum>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
    dynamic_cast<ExpWithParent*>(left.get())->SetIsParent();
    dynamic_cast<ExpWithParent*>(right.get())->SetIsParent();
    return make_unique<EProduct>(move(left), move(right));
}

string Print(const Expression* e) {
    if (!e) {
        return "Null expression provided";
    }
    stringstream output;
    output << e->ToString() << " = " << e->Evaluate();
    return output.str();
}

void Test() {
    ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
    ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

    ExpressionPtr e2 = Sum(move(e1), Value(5));
    ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

    ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, Test);
    return 0;
}