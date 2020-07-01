#include "ArmatureDisplayData.hpp"

#include <chrono>
#include <filesystem>
 
#include "Szczur/Modules/DragonBones/SF3DFactory.hpp"
#include "Szczur/Utility/Logger.hpp"

#include "Szczur/Modules/FileSystem/DragDrop.hpp"

using std::chrono::system_clock;

namespace rat
{
 
ArmatureDisplayData::ArmatureDisplayData(const std::string& path)
    : _folderPath(path)
{
	_name = std::filesystem::path(path).filename().string();

	checkForReload();
	load();
	_needReload = false;
}
    
ArmatureDisplayData::~ArmatureDisplayData() 
{
	unload();
}

void ArmatureDisplayData::load()
{
	LOG_INFO("Loading armature data: '", _name, "'...");

	auto dbFactory = dragonBones::SF3DFactory::get();

	_texture = new sf3d::Texture;
	_texture->loadFromFile(_folderPath + _textureFilePath);

	dbFactory->loadDragonBonesData(_folderPath  + _skeFilePath, _name);
	dbFactory->loadTextureAtlasData(_folderPath + _textureAtlasFilePath, _texture, _name);
}

void ArmatureDisplayData::unload()
{
	LOG_INFO("Removing armature data: '", _name, "'...");

	auto dbFactory = dragonBones::SF3DFactory::get();

	dbFactory->removeDragonBonesData(_name);
	dbFactory->removeTextureAtlasData(_name);
	delete _texture;
}

void ArmatureDisplayData::reload()
{
	checkForReload();

	if (_needReload)
	{
		unload();
		load();

		_needReload = false;
	}
}

template <typename TP>
std::time_t to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
		+ system_clock::now());
	return system_clock::to_time_t(sctp);
}

void ArmatureDisplayData::checkForReload()
{
	namespace fs = std::filesystem;

#ifndef PSYCHOX // Przepraszam :C 
	//auto tt = fs::last_write_time(_folderPath + _skeFilePath);
	auto lastSkeFileUpdate			= to_time_t(fs::last_write_time(_folderPath + _skeFilePath));
	auto lastTextureAtlasFileUpdate = to_time_t(fs::last_write_time(_folderPath + _textureAtlasFilePath));
	auto lastTextureFileUpdate		= to_time_t(fs::last_write_time(_folderPath + _textureFilePath));
	
	if (lastSkeFileUpdate			!= _lastSkeFileUpdate ||
		lastTextureAtlasFileUpdate	!= _lastTextureAtlasFileUpdate ||
		lastTextureFileUpdate		!= _lastTextureFileUpdate)
	{
		_lastSkeFileUpdate = lastSkeFileUpdate;
		_lastTextureAtlasFileUpdate = lastTextureAtlasFileUpdate;
		_lastTextureFileUpdate = lastTextureFileUpdate;
		_needReload = true;
	}
#else
	_needReload = true;
#endif
}
 
}
 
