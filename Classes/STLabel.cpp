#include "STLabel.h"
#include "AppDelegate.h"

STLabel* STLabel::create()
{
	auto ret = new (std::nothrow) STLabel();

	if (ret)
	{
		ret->autorelease();
		ret->setScale(1.0f);
	}

	return ret;
}



STLabel* STLabel::createWithTTF(const std::string& text, const std::string& fontFile, float fontSize, const Size& dimensions /* = Size::ZERO */, TextHAlignment hAlignment /* = TextHAlignment::LEFT */, TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
	auto ret = new (std::nothrow) STLabel(hAlignment, vAlignment);

	if (ret && FileUtils::getInstance()->isFileExist(fontFile))
	{
		TTFConfig ttfConfig(fontFile.c_str(), fontSize, GlyphCollection::DYNAMIC);
		if (ret->setTTFConfig(ttfConfig))
		{
			ret->setDimensions(dimensions.width, dimensions.height);
			ret->setString(text);

			ret->autorelease();
			ret->setScale(1.0f);
			return ret;
		}
	}

	delete ret;
	return nullptr;
}

STLabel* STLabel::createWithTTF(const TTFConfig& ttfConfig, const std::string& text, TextHAlignment hAlignment /* = TextHAlignment::CENTER */, int maxLineWidth /* = 0 */)
{

	auto ret = new (std::nothrow) STLabel(hAlignment);

	if (ret && FileUtils::getInstance()->isFileExist(ttfConfig.fontFilePath) && ret->setTTFConfig(ttfConfig))
	{
		ret->setMaxLineWidth(maxLineWidth);
		ret->setString(text);
		ret->autorelease();
		ret->setScale(1.0f);

		return ret;
	}

	delete ret;
	return nullptr;
}

bool STLabel::setTTFConfig(const TTFConfig & ttfConfig)
{
	auto temp = TTFConfig(ttfConfig);
	temp.fontSize /= AppDelegate::resolutionRatio;
	return Label::setTTFConfig(temp);
}

void STLabel::setScaleX(float scaleX)
{
	Node::setScaleX(scaleX * AppDelegate::resolutionRatio);
}

float STLabel::getScaleX() const
{
	return Node::getScaleX() / AppDelegate::resolutionRatio;
}

void STLabel::setScaleY(float scaleY)
{
	Node::setScaleY(scaleY * AppDelegate::resolutionRatio);
}

float STLabel::getScaleY() const
{
	return Node::getScaleY() / AppDelegate::resolutionRatio;
}

void STLabel::setScaleZ(float scaleZ)
{
	Node::setScaleZ(scaleZ * AppDelegate::resolutionRatio);
}

float STLabel::getScaleZ() const
{
	return Node::getScaleZ() / AppDelegate::resolutionRatio;
}

void STLabel::setScale(float scale)
{
	Node::setScale(scale * AppDelegate::resolutionRatio);
}

float STLabel::getScale() const
{
	return Node::getScale() / AppDelegate::resolutionRatio;
}

void STLabel::setScale(float scaleX, float scaleY)
{
	Node::setScale(scaleX * AppDelegate::resolutionRatio, scaleY * AppDelegate::resolutionRatio);
}

STLabel::STLabel(TextHAlignment hAlignment /*= TextHAlignment::LEFT*/,
	TextVAlignment vAlignment /*= TextVAlignment::TOP*/) {
	Label::Label(hAlignment, vAlignment);
}
STLabel::~STLabel()
{
}
