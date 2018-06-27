#include "BattleScene.hpp"

#include <cmath>
#include <fstream>
#include <experimental/filesystem>

#include <SFML/Graphics.hpp>

#include <json.hpp>
using Json = nlohmann::json;

#include "Szczur/Utility/Logger.hpp"

#include "BattleProvider.hpp"

#include <glad/glad.h>

namespace rat {

// ========== Constructors ========== 

BattleScene::BattleScene() {
	LOG_INFO("Initializing BattleScene module");
	dumpPawn.reset(new BattlePawn(*this));
	initScript();
	init();

	// Test
	// loadBattle("Assets/Battles/battle_1.json");

	// auto& window = getModule<Window>().getWindow();
	// fieldSize = sf::Vector2f(900, 500);
	// fieldPos.x = (window.getSize().x-fieldSize.x)/2.f;
	// fieldPos.y = window.getSize().y-fieldSize.y-100;
	// BattlePawn* pawn = addPawn("Assets/Pawns/karion");
	// changePawn(pawn);
	// changeSkill("Dash and hit");

	LOG_INFO("Module BattleScene initialized");
}

BattleScene::~BattleScene() {
	LOG_INFO("Module BattleScene destructed");
}

// ========== Main ========== 

void BattleScene::init() {
	auto& window = getModule<Window>();
	window.pushGLStates();
	canvas.create(window.getWindow().getSize().x, window.getWindow().getSize().y);
	canvas.clear({0,0,0,0});
	window.popGLStates();
}

void BattleScene::update(float deltaTime) {
	if(!battleRunning) return;

	// Test
	if(InputManager& input = getModule<Input>().getManager(); input.isPressed(Mouse::Right)) {
		addPawn("Assets/Pawns/slime", sf::Vector2f(input.getMousePosition()));
	}

	for(auto& obj : skillsInQueue) {
		obj->init();
	}
	for(auto& obj : triggersInQueue) {
		obj->init();
	}

	updateSkills(deltaTime);
	updateTriggers(deltaTime);

	for(auto itr = pawns.begin(); itr != pawns.end();) {
		(*itr)->setCollisionStatus(false);
		(*itr)->update(deltaTime);
		if((*itr)->getHp() == 0) {
			for(auto& obj : activeSkills) {
				if(obj->getPawn()==itr->get()) {
					obj->setPawn(dumpPawn.get());
				}
			}
			for(auto& obj : activeTriggers) {
				if(obj->getCaster()==itr->get()) {
					obj->setCaster(dumpPawn.get());
				}
				obj->replaceTargets(itr->get(), dumpPawn.get());
			}
			if(itr->get() == controlledPawn) {
				controlledPawn = nullptr;
				controlledSkill= nullptr;
			}
			pawns.erase(itr);
		}
		else {
			++itr;
		}
	}
	
	updateController();
}

void BattleScene::render() {	
	if(!battleRunning) return;
	auto& window = getModule<Window>();
	window.pushGLStates();
	canvas.clear();
	
	// Battle field
	sf::RectangleShape battleField(fieldSize);
	battleField.setPosition(fieldPos);
	battleField.setFillColor({0,0,0,0});
	battleField.setOutlineThickness(2);
	battleField.setOutlineColor({255,0,150,255});
	canvas.draw(battleField);

	// Triggers
	for(auto& obj : activeTriggers) {
		obj->render(canvas);
	}

	// Pawns
	for(auto& obj : pawns) {
		if(obj.get()!=controlledPawn) {
			obj->renderController(canvas, false);
		}
		else {			
			obj->renderController(canvas, true);
		}
	}
	for(auto& obj : pawns) {
		obj->render(canvas);
	}

	// Controller
	renderController();

	canvas.display();
	window.draw(sf::Sprite(canvas.getTexture()));
	window.popGLStates();
}

void BattleScene::input(sf::Event& event) {
	if(!battleRunning) return;
}

// ========== Battle ========== 

void BattleScene::changeBattleFieldSize(const sf::Vector2f& size) {
	auto& window = getModule<Window>().getWindow();
	fieldSize = size;
	fieldPos.x = (window.getSize().x-fieldSize.x)/2.f;
	fieldPos.y = window.getSize().y-fieldSize.y-100;
}

BattlePawn* BattleScene::addPawn(const std::string& dirPath, const sf::Vector2f& position) {
	BattlePawn* pawn =  pawns.emplace_back(new BattlePawn(*this)).get();
	pawn->loadPawn(dirPath);
	pawn->setPosition(position);
	return pawn;
}

void BattleScene::changePawn(BattlePawn* pawn) {
	controlledPawn = pawn;
	controlledSkill = nullptr;
}

void BattleScene::changeSkill(const std::string& skillName) {
	if(controlledPawn != nullptr) {
		controlledSkill = controlledPawn->getSkill(skillName);
	}
}

void BattleScene::changeSkill(int index) {
	if(controlledPawn != nullptr) {
		if(index<0) index = 0;
		auto& skills = controlledPawn->getUsableSkills();
		if(index<skills.size()) {
			controlledSkill = skills[index].first;
			return;
		}
		controlledSkill = nullptr;
	}
}

void BattleScene::loadBattle(const std::string& configPath) {
	namespace fs = std::experimental::filesystem;

	closeBattle();

	Json json;
	std::ifstream(configPath) >> json;
	if(!json["script"].is_null()) {
		std::string dir = fs::path(configPath).parent_path().string();
		getModule<Script>().scriptFile(dir+"/"+json["script"].get<std::string>());
	}

	changeBattleFieldSize({json["size"][0].get<float>(), json["size"][1].get<float>()});

	// Player
	changePawn(addPawn(json["player"][0].get<std::string>(), sf::Vector2f(json["player"][1].get<float>()+fieldPos.x, json["player"][2].get<float>()+fieldPos.y)));

	// Enemies
	for(auto& obj : json["enemies"]) {
		addPawn(obj[0].get<std::string>(),{obj[1].get<float>()+fieldPos.x, obj[2].get<float>()+fieldPos.y});
	}

	battleRunning = true;
}

void BattleScene::closeBattle() {
	pawns.clear();
	controlledPawn = nullptr;
	controlledSkill = nullptr;
	battleRunning = false;
}

void BattleScene::fixPosition(BattlePawn& pawn) {
	if(pawn.getPosition().x-pawn.getColliderRadius()<fieldPos.x) {
		pawn.setRawPositionX(fieldPos.x+pawn.getColliderRadius());
	}
	else if(pawn.getPosition().x+pawn.getColliderRadius()>fieldPos.x+fieldSize.x) {
		pawn.setRawPositionX(fieldPos.x+fieldSize.x-pawn.getColliderRadius());
	}
	if(pawn.getPosition().y-pawn.getColliderRadius()<fieldPos.y) {
		pawn.setRawPositionY(fieldPos.y+pawn.getColliderRadius());
	}
	else if(pawn.getPosition().y+pawn.getColliderRadius()>fieldPos.y+fieldSize.y) {
		pawn.setRawPositionY(fieldPos.y+fieldSize.y-pawn.getColliderRadius());
	}

	bool checkAllAgain = false;
	int tries = 10;
	do {
		checkAllAgain = false;
		for(auto& obj : pawns) {
			if(obj.get() == &pawn) { 
				continue;
			}

			// Distance
			const sf::Vector2f& v1 = obj->getPosition();
			const sf::Vector2f& v2 = pawn.getPosition();
			float dx = v1.x-v2.x;
			float dy = v1.y-v2.y; 
			float dis = std::sqrt(dx*dx+dy*dy);

			// Angle
			float ang = std::atan2(v2.y-v1.y, v2.x-v1.x);
			if(tries<0) {
				break;
			}

			// Move
			if(float d = obj->getColliderRadius()+pawn.getColliderRadius()-dis; d>=0) {
				pawn.moveRaw({std::cos(ang)*d, std::sin(ang)*d});
				checkAllAgain = true;
				pawn.setCollisionStatus(true);
				obj->setCollisionStatus(true);
			}
		}
		--tries;
	} 
	while(checkAllAgain);
}	

// ========== Skills ========== 

std::vector<std::unique_ptr<BattlePawn>>& BattleScene::getPawns() {
	return pawns;
}

// ========== Skills ========== 

void BattleScene::updateSkills(float deltaTime) {
	for(auto& obj : activeSkills) {
		obj->update(deltaTime);
	}
	for(auto itr = activeSkills.begin(); itr<activeSkills.end(); ) {
		if((*itr)->isFinished()) {
			activeSkills.erase(itr);
		}
		else {
			++itr;
		}
	}
	if(skillsInQueue.empty()) return;
	for(auto itr = skillsInQueue.begin(); itr != skillsInQueue.end();) {
		if((*itr)->isInited()) {
			activeSkills.emplace_back(std::move(*itr));
			skillsInQueue.erase(itr);
		}
		else {
			++itr;
		}
	}
	
	// activeSkills.insert(activeSkills.end(),
	// 	std::make_move_iterator(skillsInQueue.begin()), 
	// 	std::make_move_iterator(skillsInQueue.end())
	// );
	// skillsInQueue.clear();
}

void BattleScene::updateTriggers(float deltaTime) {	
	for(auto& obj : activeTriggers) {
		obj->update(deltaTime);
	}

	for(auto itr = activeTriggers.begin(); itr<activeTriggers.end(); ) {
		if((*itr)->isFinished()) {
			activeTriggers.erase(itr);
		}
		else {
			++itr;
		}
	}

	if(triggersInQueue.empty()) return;
	for(auto itr = triggersInQueue.begin(); itr != triggersInQueue.end();) {
		if((*itr)->isInited()) {
			activeTriggers.emplace_back(std::move(*itr));
			triggersInQueue.erase(itr);
		}
		else {
			++itr;
		}
	}

	// if(triggersInQueue.empty()) return;
	// activeTriggers.insert(activeTriggers.end(),
	// 	std::make_move_iterator(triggersInQueue.begin()), 
	// 	std::make_move_iterator(triggersInQueue.end())
	// );
	// triggersInQueue.clear();
}

BattleSkill* BattleScene::useSkill(BattleSkill* skill) {
	if(skill == nullptr) return nullptr;
	// BattleSkill* ret = skillsInQueue.emplace_back(new BattleSkill(*skill)).get();
	// ret->init();
	// return ret;
	return skillsInQueue.emplace_back(new BattleSkill(*skill)).get();
}

BattleTrigger* BattleScene::addTrigger(BattleTrigger* trigger) {
	if(trigger == nullptr) return nullptr;
	return triggersInQueue.emplace_back(trigger).get();
}

// ========== Controller ========== 

void BattleScene::updateController() {

	if(getModule<Input>().getManager().isPressed(Keyboard::Num1)) {
		controlledSkill = controlledPawn->getUsableSkills()[0].first;
	}
	else if(getModule<Input>().getManager().isPressed(Keyboard::Num2)) {
		controlledSkill = controlledPawn->getUsableSkills()[1].first;
	}
	else if(getModule<Input>().getManager().isPressed(Keyboard::Q)) {
		changePawn(pawns[0].get());
		changeSkill(0);
	}
	else if(getModule<Input>().getManager().isPressed(Keyboard::E)) {
		changePawn(pawns[1].get());
		changeSkill(0);
	}

	BattlePawn* selectedPawn = nullptr;
	auto& input = getModule<Input>().getManager();
	for(auto itr = pawns.rbegin(); itr<pawns.rend(); ++itr) {
		if((*itr)->hitTest(sf::Vector2f(input.getMousePosition()))) {
			selectedPawn = itr->get();
			break;
		}
	}

	if(controlledSkill != nullptr) {
		controlledSkill->updateController(selectedPawn);
	}
}

void BattleScene::renderController() {
	if(controlledPawn != nullptr) {
		sf::RectangleShape shape({80, 80});
		shape.setOutlineColor({255,0,150,150});
		shape.setOutlineThickness(2);
		shape.setFillColor({100,0,50,100});
		auto& skills = controlledPawn->getSkills();
		int i = 0;
		for(auto& obj : controlledPawn->getUsableSkills()) {
			sf::Sprite icon(obj.first->getPawn()->getIconSprite(obj.second));
			icon.setPosition(300+i*90,50);
			shape.setPosition(icon.getPosition());
			if(obj.first != controlledSkill) {			
				canvas.draw(shape);
				canvas.draw(icon);
			}
			else {
				icon.setColor({255,255,255,255});
				shape.setOutlineColor({255,0,150,255});	
				shape.setFillColor({100,0,50,150});				
				canvas.draw(shape);
				canvas.draw(icon);
				shape.setFillColor({100,0,50,100});				
				shape.setOutlineColor({255,0,150,150});	
				icon.setColor({255,255,255,150});	
			}
			++i;
		}
		// shape.setOutlineColor({255,0,150,255});
	}
	if(controlledSkill != nullptr) {
		controlledSkill->renderController(canvas);
	}
}

// ========== Scripts ========== 

void BattleScene::initScript() {
	auto& script = getModule<Script>();
	auto module = script.newModule("BattleScene");
	script.initClasses<BattlePawn, BattleSkill, BattleTrigger, BattleProvider>(); 

	// TEMPORARY	
	auto moduleUtility = script.newModule("Utility");
	auto classClock = script.newClass<sf::Clock>("Clock", "Utility");
	classClock.set("new", sol::constructors<sf::Clock()>());
	classClock.set("restart", &sf::Clock::restart);
	classClock.set("elapsed", [](sf::Clock& clock){return clock.getElapsedTime().asSeconds();});
	classClock.init();

	auto moduleInput = script.newModule("Input");
	moduleInput.set_function("getMousePosition", [&](){return sf::Vector2f(getModule<Input>().getManager().getMousePosition());});

	auto moduleMath = script.get().get<sol::table>("Math");
	moduleMath.set_function("distance", [](const sf::Vector2f& a, const sf::Vector2f& b) {
		sf::Vector2f d = a-b;
		return float(std::sqrt(d.x*d.x+d.y*d.y));
	});
	moduleMath.set_function("angle", [](const sf::Vector2f& from, const sf::Vector2f& to) {
		sf::Vector2f d = to-from;
		return float(std::atan2(d.y, d.x));
	});
	moduleMath.set_function("move", [](sf::Vector2f pos, float angle, float distance) {
		pos.x += std::cos(angle)*distance;
		pos.y += std::sin(angle)*distance;
		return pos;
	});
}

}