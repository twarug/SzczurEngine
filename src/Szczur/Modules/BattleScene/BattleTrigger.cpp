#include "BattleTrigger.hpp"

#include "BattlePawn.hpp"
#include "BattleScene.hpp"

namespace rat {

// ========== Main ========== 

BattleTrigger::BattleTrigger(BattlePawn* caster) 
	: caster(caster), scene(caster->getScene()) {
	shape.setRadius(radius);
	shape.setOrigin(radius, radius);
	shape.setFillColor({40,120,180,180});
	shape.setOutlineColor({60,150,220,180});
	shape.setOutlineThickness(-4);
	shape.setPosition(position);
}

// ========== Getters ========== 

bool BattleTrigger::isFinished() const {
	return killed;
}

BattlePawn* BattleTrigger::getCaster() {
	return caster;
}

const sf::Vector2f& BattleTrigger::getPosition() const {
	return position;
}

bool BattleTrigger::isInited() const {
	return inited;
}

// ========== Main ========== 

void BattleTrigger::render(sf::RenderTarget& canvas) {

	if(targets.empty()) {
		shape.setFillColor({40,120,180});
		shape.setOutlineColor({60,150,220});
	}
	else {		
		shape.setFillColor({180,120,40});
		shape.setOutlineColor({220,150,60});
	}

	sf::Color color = shape.getOutlineColor();
	color.a = 180.f*(1-clock.getElapsedTime().asSeconds()/duration);
	shape.setOutlineColor(color);
	if(float elapsed = clock.getElapsedTime().asSeconds()<=duration*0.5f) {
		color = shape.getFillColor();
		color.a = 180.f*(1-clock.getElapsedTime().asSeconds()/(duration*0.5f));
		shape.setFillColor(color);
	}
	else {
		shape.setFillColor({0,0,0,0});
	}
	canvas.draw(shape);
}


void BattleTrigger::update(float deltaTime) {
	if(clock.getElapsedTime().asSeconds()>duration) {
		killed = true;
	}
	if(!targets.empty() && onUpdate.valid()) {
		onUpdate(this, deltaTime);
	}
}


void BattleTrigger::init() {
	clock.restart();
	updateTargets();
	if(onInit.valid() && !targets.empty()) {
		onInit(this);
	}
	inited = true;
}

// ========== Manipulations ========== 


void BattleTrigger::setRadius(float radius) {
	this->radius = radius;
	shape.setRadius(radius);
	shape.setOrigin(radius, radius);
}


void BattleTrigger::setPosition(float x, float y) {
	position.x = x;
	position.y = y;
	shape.setPosition(position);
}


void BattleTrigger::setPosition(const sf::Vector2f& pos) {
	position = pos;
	shape.setPosition(position);
}


void BattleTrigger::setDuration(float time) {
	if(time<0.001) time = 0.001;
	duration = time;
}

void BattleTrigger::setCaster(BattlePawn* caster) {
	this->caster = caster;
}

// ========== Targeting ========== 

void BattleTrigger::updateTargets() {
	if(nearestOnlyType) {
		targetNearest();
	}
}

void BattleTrigger::targetNearest() {

	const auto& pawns = scene->getPawns();

	float min = 100000000;
	BattlePawn* target = nullptr;

	float dis;
	for(auto& obj : pawns) {
		dis = getDistanceTo(obj->getPosition())-obj->getColliderRadius();
		if(min>dis && obj.get() != caster) {
			min = dis;
			target = obj.get();
		}
	}

	if(target != nullptr) {
		if(min<=radius) {
			targets.emplace_back(target);
		}
	}
}

void BattleTrigger::nearestOnly() {
	nearestOnlyType = true;
}

BattlePawn* BattleTrigger::getTarget() {
	if(targets.empty()) return nullptr;
	return targets.front();
}


void BattleTrigger::removeFromTargets(BattlePawn* pawn) {
	for(auto itr = targets.begin(); itr != targets.end();) {
		if(*itr == pawn) {
			targets.erase(itr);
		}
		else {
			++itr;
		}
	}
}

void BattleTrigger::replaceTargets(BattlePawn* pawn, BattlePawn* newPawn) {
	for(auto itr = targets.begin(); itr != targets.end(); ++itr) {
		if(*itr == pawn) {
			*itr = newPawn;
		}
	}
}

// ========== Math ========== 

float BattleTrigger::getSemiDistanceTo(const sf::Vector2f& pos) {
	sf::Vector2f d = position-pos;
	return d.x*d.x+d.y*d.y;
}


float BattleTrigger::getDistanceTo(const sf::Vector2f& pos) {
	sf::Vector2f d = position-pos;
	return std::sqrt(d.x*d.x+d.y*d.y);
}

// ========== Scripts ========== 


void BattleTrigger::setVariable(std::string key, sol::stack_object object) {
	auto itr = data.find(key);
	if(itr == data.end()) {
		data.insert(itr, {std::move(key), std::move(object)});
	}
	else {
		itr->second = sol::object(std::move(object));
	}
}

sol::object BattleTrigger::getVariable(const std::string& key) {
	auto itr = data.find(key);
	if(itr == data.end()) {
		return sol::lua_nil;
	}
	return itr->second;
}

BattleTrigger::initScript(Script& script) {
	auto object = script.newClass<BattleTrigger>("BattleTrigger", "BattleScene");
	object.set("getCaster", &BattleTrigger::getCaster);
	object.set("setRadius", &BattleTrigger::setRadius);
	object.set("nearestOnly", &BattleTrigger::nearestOnly);
	object.set("getPosition", &BattleTrigger::getPosition);
	object.set("getTarget", &BattleTrigger::getTarget);
	object.set("getCaster", &BattleTrigger::getCaster);
	object.set("setDuration", &BattleTrigger::setDuration);
	object.setOverload("setPosition", 
		sol::resolve<void(float,float)>(&BattleTrigger::setPosition),
		sol::resolve<void(const sf::Vector2f&)>(&BattleTrigger::setPosition)
	);
	object.set("onInit", &BattleTrigger::onInit);
	object.set("onUpdate", &BattleTrigger::onUpdate);
	object.set(sol::meta_function::index, &BattleTrigger::getVariable);
	object.set(sol::meta_function::new_index, &BattleTrigger::setVariable);
	object.init();
}

}