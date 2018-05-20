#pragma once 

#include <SFML/Graphics.hpp>

namespace rat {

class Input;
class BattleScene;
class BattleSkill;
class Script;

class BattlePawn {
	using RenderTarget = sf::RenderTarget;
public:

// Constructors

	///
	BattlePawn(BattleScene& battleScene);

// Main

	///
	void update(float deltaTime);

	///
	void render(RenderTarget& canvas);

// Getters

	///
	const sf::Vector2f& getPosition() const;

	///
	float getColliderRadius() const;

	/// Check collision with point
	bool hitTest(const sf::Vector2f& point) const;
	
	/// (angle in format -pi..pi), 1:left, 0:right
	bool getDirection(float angle);

// Manipulations

	/// Set position with collision callculation
	void setPosition(const sf::Vector2f& position);
	void setPositionX(float x);
	void setPositionY(float y);

	/// Set position without collision callculation
	void setRawPosition(const sf::Vector2f& position);
	void setRawPositionX(float x);
	void setRawPositionY(float y);

	///
	void move(const sf::Vector2f& vector);

	///
	void moveRaw(const sf::Vector2f& vector);

	///
	float getAngleTo(const sf::Vector2f& pos);

	///
	float getAngleToPointer();

	/// 
	void moveInDirection(float angle, float distance);

	/// Load pawn from folder
	void loadPawn(const std::string& dirPath);


// Controller

	///
	void renderController(RenderTarget& canvas);

	///
	void updateController();

// Skills

	/// Add new skill for pawn
	BattleSkill* newSkill(const std::string& skillName);
	
	/// Create and use skill wihout name
	BattleSkill* newInstaSkill();

	/// Get skill by name
	BattleSkill* getSkill(const std::string& skillName);

	/// Use skill 
	BattleSkill* useSkill(BattleSkill* skill);

	/// Use skill by name
	BattleSkill* useSkill(const std::string& skillName);

// Visual

	///
	void setFrame(int frame);

	///
	void setFlip(bool flag);

// Scripts

	///
	static void initScript(Script& script);
	
	/// Run script for this pawn
	void runScript(const std::string& filePath);


private:

// Visual

	/// Get texture rect by frame
	sf::IntRect calculateTextureRect(int frame);

private:

// Main

	BattleScene& battleScene;
	sf::Vector2f pos{0,0};
	float colliderRadius = 20.f;

// Visual
	
	sf::Texture texture;
	sf::Vector2i frameSize;
	int frame = 0;
	int framesInRow = 1;
	bool flip = false;
	bool isTexture = false;

// Controller
	
	Input& input;

// Skills
	
	std::vector<std::unique_ptr<BattleSkill>> skills;
	std::vector<std::unique_ptr<BattleSkill>> activeSkills;
};

}