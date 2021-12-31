#include "Common.h"

using namespace std;

class Rectangle : public IShape {
    std::unique_ptr<IShape> Clone() const override {
        std::unique_ptr<IShape> cloned = MakeShape(ShapeType::Rectangle);
        cloned->SetPosition(mPosition);
        cloned->SetSize(mSize);
        cloned->SetTexture(mTexture);
        return cloned;
    }

    void SetPosition(Point point) override {
        mPosition = point;
    }

    Point GetPosition() const override {
        return mPosition;
    }

    void SetSize(Size size) override {
        mSize = size;
    }

    Size GetSize() const override {
        return mSize;
    }

    void SetTexture(std::shared_ptr<ITexture> texture) override {
        mTexture = texture;
    }

    ITexture* GetTexture() const override {
        return mTexture.get();
    }

    void Draw(Image& image) const override {
        for (int i = 0; i < mSize.height; ++i) {
            for (int j = 0; j < mSize.width; ++j) {
                if (mPosition.y + i < image.size() && mPosition.x + j < image.at(mPosition.y + i).size()) {
                    if (mTexture && i < mTexture->GetSize().height && j < mTexture->GetSize().width) {
                        image.at(mPosition.y + i).at(mPosition.x + j) = mTexture->GetImage().at(i).at(j);
                    }
                    else {
                        image.at(mPosition.y + i).at(mPosition.x + j) = '.';
                    }
                }
            }
        }
    }

private:
    Size mSize;
    Point mPosition;
    std::shared_ptr<ITexture> mTexture;
};

class Ellipse : public IShape {
    std::unique_ptr<IShape> Clone() const override {
        std::unique_ptr<IShape> cloned = MakeShape(ShapeType::Ellipse);
        cloned->SetPosition(mPosition);
        cloned->SetSize(mSize);
        cloned->SetTexture(mTexture);
        return cloned;
    }

    void SetPosition(Point point) override {
        mPosition = point;
    }

    Point GetPosition() const override {
        return mPosition;
    }

    void SetSize(Size size) override {
        mSize = size;
    }

    Size GetSize() const override {
        return mSize;
    }

    void SetTexture(std::shared_ptr<ITexture> texture) override {
        mTexture = texture;
    }

    ITexture* GetTexture() const override {
        return mTexture.get();
    }

    void Draw(Image& image) const override {
        for (int i = 0; i < mSize.height; ++i) {
            for (int j = 0; j < mSize.width; ++j) {
                if (IsPointInEllipse({ j, i }, mSize) && mPosition.y + i < image.size() &&
                    mPosition.x + j < image.at(mPosition.y + i).size())
                {
                    if (mTexture && i < mTexture->GetSize().height && j < mTexture->GetSize().width) {
                        image.at(mPosition.y + i).at(mPosition.x + j) = mTexture->GetImage().at(i).at(j);
                    }
                    else {
                        image.at(mPosition.y + i).at(mPosition.x + j) = '.';
                    }
                }
            }
        }
    }

private:
    Size mSize;
    Point mPosition;
    std::shared_ptr<ITexture> mTexture;
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
    switch (shape_type) {
    case ShapeType::Rectangle:
        return std::make_unique<Rectangle>();
    case ShapeType::Ellipse:
        return std::make_unique<Ellipse>();
    }
}