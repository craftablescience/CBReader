#pragma once

#include "cbreader/types.h"

#include <vector>
#include <string>

class BufferStream;

constexpr std::uint32_t MAX_SOUND_EMITTERS = 16;

struct Header
{
	// Length prefixed string
	std::string header;

	/** Defines whether or not the RMesh file has any triggers embedded within the file. */
	bool hasTriggerBox = false;

	/** NTF Mod specific property. */
	bool hasNoColl = false;
};

struct Texture
{
	std::byte blendType;

	std::string textureName;
};

struct Vertex
{
	Vector3 vertex;
	Vector2 uv;

	float unk1{};
	float unk2{};

	std::byte r{};
	std::byte g{};
	std::byte b{};
};

struct Surface
{
	Texture textures[2];

	std::vector<Vertex> vertices;

	std::vector<Triangle> triangles;
};

struct Mesh
{
	std::vector<Surface> surfaces;

	std::string name;
};

/**
* @class Entity
* 
* Root entity class embedded in RMesh files
*/
struct Entity
{
	std::string className;

	/** This is read per entity. It would be ideal to read it in Entity, but EntityModel is special in that it has the model file name THEN the position. */
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	virtual void Read(BufferStream& stream, std::string _className);
};

struct EntityScreen : public Entity
{
	std::string imgPath;

	void Read(BufferStream& stream, std::string _className) override;
};

struct EntityWaypoint : public Entity
{
	void Read(BufferStream& stream, std::string _className) override;
};

struct EntityLight : public Entity
{
	/** SCP:CB calculates this as range/2000.0 in the RMesh code*/
	float range = 0.f;

	/** Space delimited color string. Color is 3 ints, and multiplied by intensity when reading. */
	std::string color;

	/** SCP:CB calculates this as Min(intensity*0.8,1)*/
	float intensity = 0.f;

	void Read(BufferStream& stream, std::string _className) override;
};

struct EntitySpotLight : public EntityLight
{
	std::string angles;

	std::uint32_t innerConeAngle = 0;
	std::uint32_t outerConeAngle = 0;

	void Read(BufferStream& stream, std::string _className) override;
};

struct EntitySoundEmitter : public Entity
{
	void Read(BufferStream& stream, std::string _className) override;
};

struct EntityPlayerStart : public Entity
{
	std::string angles;

	void Read(BufferStream& stream, std::string _className) override;
};

struct EntityModel : public Entity
{
	std::string path;

	void Read(BufferStream& stream, std::string _className) override;
};

/**
* @class RMesh
* RMesh is the main room mesh format in SCP: Containment Breach
* 
* @todo Maybe move Read to the constructor instead
*/
struct RMesh
{
	Header header;

	Mesh drawnMesh;
	Mesh collisionMesh;

	std::vector<Mesh> triggerBoxes;

	std::vector<Entity> entities;

	/**
	* Reads the specified RMesh file
	* 
	* @param path The file path you wish to open
	* 
	* @returns Whether the RMesh file was read correctly.
	*/
	bool Read(const std::string& path);

	static Mesh ReadDrawnMesh(BufferStream& stream);
	static Mesh ReadCollisionMesh(BufferStream& stream);
};