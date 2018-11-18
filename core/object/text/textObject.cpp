#include "textObject.h"

TextObj::~TextObj() {
	if (this->textObj != NULL) {
		delete this->textObj;
	}

	if (this->text != NULL) {
		free(this->text);
	}
}

TextObj::TextObj(const char* name, vector* pos, const char* text, const char* font, unsigned int fontSize, unsigned short z, bool visible) 
: Object(name, pos, NULL, NULL, z, visible) {
	Log::inf(LOG_OBJ, "===== CREATING TEXT: '%s' =====", text);
	
	Log::dbg(LOG_OBJ, "-- Fetch Font: '%s'", font);
	this->font = AssetMgr::get()->getFont(font);
	if (this->font == NULL) {
		int len = 27 + strlen(font);
		char msg[len];
		memset(msg, 0, len);
		snprintf(msg, len, "Fail To Find Font: '%s' !!!", font);

		Log::dbg(LOG_OBJ, "%s", msg);
		throw(new Exception(LOG_OBJ, msg));
	}

	this->text = Str(text);
	this->size = fontSize;

	Log::dbg(LOG_OBJ, "-- Create Texture");
	this->textObj = new sf::Text();
	//Log::err(LOG_OBJ, "-- ADDR: %p", this->textObj);

	Log::dbg(LOG_OBJ, "-- Set Font");
	this->textObj->setFont(*(this->font));

	std::string str(this->text);
	Log::dbg(LOG_OBJ, "-- Set Text");
	this->textObj->setString(str);

	Log::dbg(LOG_OBJ, "-- Set Size: %u", fontSize);
	this->textObj->setCharacterSize(fontSize);

	// this->setTexture(texture);


	// Log::dbg(LOG_OBJ, "-- Gen Clip: %u| %u", texture->getSize().x, texture->getSize().y);
	// IntRect* clip = new IntRect(0,0,texture->getSize().x, texture->getSize().y);

	// Log::dbg(LOG_OBJ, "-- Apply Clip");
	// this->setClip(clip, true);

}

TextObj::TextObj(const char* name, vector* pos, const char* text, const char* font, unsigned int fontSize, unsigned short z, bool visible, sf::Color color)
: TextObj(name,pos,text,font,fontSize,z,visible) {
	this->color = color;
	this->textObj->setFillColor(this->color);
}

void TextObj::draw(RenderWindow* window, bool grav) {
	if (!this->isEnabled() || !this->checkCameraDistance()) {
		Log::dbg(LOG_OBJ, "-- Skip Render: %s", this->getName());
		return;
	}

	bool b = this->lock("Draw");


	vector pos = this->getPosition();
	this->textObj->setPosition(pos.x, pos.y);
	window->draw(*(this->textObj));
	
	Node* n = NULL;
	while ((n = listIterate(this->childs, n)) != NULL) {
		if (n->value == NULL) {
			continue;
		}

		Object* child = (Object*) n->value;
		child->draw(window, grav);
	}

	this->unlock("Draw", b);
}