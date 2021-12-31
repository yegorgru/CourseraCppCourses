#include "geo2d.h"
#include "game_object.h"

#include <vector>
#include <memory>

using namespace std;

class Unit : public GameObject {
public:
    explicit Unit(geo2d::Point position);

    geo2d::Point GetPosition() const;

    bool Collide(const GameObject& that) const override;
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;

private:
    geo2d::Point m_Point;
};

class Building : public GameObject {
public:
    explicit Building(geo2d::Rectangle geometry);

    geo2d::Rectangle GetPosition() const;

    bool Collide(const GameObject& that) const;
    bool CollideWith(const Unit& that) const;
    bool CollideWith(const Building& that) const;
    bool CollideWith(const Tower& that) const;
    bool CollideWith(const Fence& that) const;
private:
    geo2d::Rectangle m_Rectangle;
};

class Tower : public GameObject {
public:
    explicit Tower(geo2d::Circle geometry);

    geo2d::Circle GetPosition()  const;

    bool Collide(const GameObject& that) const override;
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
private:
    geo2d::Circle m_Circle;
};

class Fence : public GameObject {
public:
    explicit Fence(geo2d::Segment geometry);
    geo2d::Segment GetPosition()  const;
    bool Collide(const GameObject& that) const override;
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
private:
    geo2d::Segment m_Segment;
};

// Р РµР°Р»РёР·СѓР№С‚Рµ С„СѓРЅРєС†РёСЋ Collide РёР· С„Р°Р№Р»Р° GameObject.h

bool Collide(const GameObject& first, const GameObject& second) {
    return first.Collide(second);
}

Unit::Unit(geo2d::Point position) :
    m_Point(position) {}

geo2d::Point Unit::GetPosition() const {
    return m_Point;
}

bool Unit::Collide(const GameObject& that) const{
    return that.CollideWith(*this);
}
bool Unit::CollideWith(const Unit& that) const {
    return geo2d::Collide(that.GetPosition(), m_Point);
}
bool Unit::CollideWith(const Building& that) const {
    return geo2d::Collide(that.GetPosition(), m_Point);
}
bool Unit::CollideWith(const Tower& that) const {
    return geo2d::Collide(that.GetPosition(), m_Point);
}
bool Unit::CollideWith(const Fence& that) const {
    return geo2d::Collide(that.GetPosition(), m_Point);
}

Building::Building(geo2d::Rectangle geometry) :
    m_Rectangle(geometry) {}

geo2d::Rectangle Building::GetPosition() const {
    return m_Rectangle;
}

bool Building::Collide(const GameObject& that) const{
    return that.CollideWith(*this);
}
bool Building::CollideWith(const Unit& that) const{
    return geo2d::Collide(that.GetPosition(), m_Rectangle);
}
bool Building::CollideWith(const Building& that) const {
    return geo2d::Collide(that.GetPosition(), m_Rectangle);
}
bool Building::CollideWith(const Tower& that) const{
    return geo2d::Collide(that.GetPosition(), m_Rectangle);
}
bool Building::CollideWith(const Fence& that) const {
    return geo2d::Collide(that.GetPosition(), m_Rectangle);
}

Tower::Tower(geo2d::Circle geometry) :
    m_Circle(geometry) {}

geo2d::Circle Tower::GetPosition()  const {
    return m_Circle;
}

bool Tower::Collide(const GameObject& that) const {
    return that.CollideWith(*this);
}
bool Tower::CollideWith(const Unit& that) const {
    return geo2d::Collide(that.GetPosition(), m_Circle);
}
bool Tower::CollideWith(const Building& that) const {
    return geo2d::Collide(that.GetPosition(), m_Circle);
}
bool Tower::CollideWith(const Tower& that) const {
    return geo2d::Collide(that.GetPosition(), m_Circle);
}
bool Tower::CollideWith(const Fence& that) const {
    return geo2d::Collide(that.GetPosition(), m_Circle);
}

Fence::Fence(geo2d::Segment geometry) :
    m_Segment(geometry) {}

geo2d::Segment Fence::GetPosition()  const {
    return m_Segment;
}
bool Fence::Collide(const GameObject& that) const {
    return that.CollideWith(*this);
}
bool Fence::CollideWith(const Unit& that) const {
    return geo2d::Collide(that.GetPosition(), m_Segment);
}
bool Fence::CollideWith(const Building& that) const {
    return geo2d::Collide(that.GetPosition(), m_Segment);
}
bool Fence::CollideWith(const Tower& that) const{
    return geo2d::Collide(that.GetPosition(), m_Segment);
}
bool Fence::CollideWith(const Fence& that) const{
    return geo2d::Collide(that.GetPosition(), m_Segment);
}