#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <memory>
#include <iomanip>
using namespace std;

class Figure {
public:
	Figure(const string& n, const double p, const double a) :name(n), perimeter(p), area(a) {}

	virtual string Name() = 0;
	virtual double Perimeter() = 0;
	virtual double Area() = 0;

protected:
	const string name;
	const double perimeter;
	const double area;
};

class Triangle : public Figure {
public:
	Triangle(const double a, const double b, const double c) :Figure("TRIANGLE", a + b + c, sqrt((a + b + c) / 2.0 * ((a + b + c) / 2.0 - a) * ((a + b + c) / 2.0 - b) * ((a + b + c) / 2.0 - c))), a(a), b(b), c(c) {}

	string Name() override {
		return name;
	}
	double Perimeter() override {
		return perimeter;
	}
	double Area() override {
		return area;
	}
private:
	const double a;
	const double b;
	const double c;
};

class Rect : public Figure {
public:
	Rect(const double w, const double h) :Figure("RECT", 2 * (w + h), w* h), w(w), h(h) {}

	string Name() override {
		return name;
	}
	double Perimeter() override {
		return perimeter;
	}
	double Area() override {
		return area;
	}

private:
	const double w;
	const double h;
};

class Circle : public Figure {
public:
	Circle(const double r) : Figure("CIRCLE", 2.0 * 3.14 * r, 3.14 * r * r), r(r) {}

	string Name() override {
		return name;
	}
	double Perimeter() override {
		return perimeter;
	}
	double Area() override {
		return area;
	}

private:
	const double r;
};

shared_ptr<Figure> CreateFigure(istringstream& is) {
	string Name;
	is >> Name;
	shared_ptr<Figure>to_return;
	if (Name == "RECT") {
		int a, b;
		is >> a >> b;
		to_return = make_shared<Rect>(a, b);
	}
	else if (Name == "CIRCLE") {
		int r;
		is >> r;
		to_return = make_shared<Circle>(r);
	}
	else if (Name == "TRIANGLE") {
		int a, b, c;
		is >> a >> b >> c;
		to_return = make_shared<Triangle>(a, b, c);
	}
	return to_return;
}

int main() {
	vector<shared_ptr<Figure>> figures;
	for (string line; getline(cin, line); ) {
		istringstream is(line);

		string command;
		is >> command;
		if (command == "ADD") {
			is >> ws;
			figures.push_back(CreateFigure(is));
		}
		else if (command == "PRINT") {
			for (const auto& current_figure : figures) {
				cout << fixed << setprecision(3)
					<< current_figure->Name() << " "
					<< current_figure->Perimeter() << " "
					<< current_figure->Area() << endl;
			}
		}
	}
	return 0;
}