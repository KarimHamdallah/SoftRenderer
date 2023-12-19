#include "mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <iostream>

namespace
{
	const unsigned int ImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_SortByPType |
		aiProcess_PreTransformVertices |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes |
		aiProcess_Debone |
		aiProcess_ValidateDataStructure;
}

struct LogStream : public Assimp::LogStream
{
	static void Initialize()
	{
		if (Assimp::DefaultLogger::isNullLogger())
		{
			Assimp::DefaultLogger::create("ARE Mesh Loader", Assimp::Logger::NORMAL);
			Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
		}
	}

	void write(const char* message) override
	{
		std::cout << "Assimp error:" << message << "\n";
	}
};

namespace Soft
{

	Mesh::Mesh(const std::filesystem::path& path)
	{
		LogStream::Initialize();

		std::cout << "Loading mesh: " << path.string() << "\n";

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), ImportFlags);
		if (!scene || !scene->HasMeshes())
			std::cout << "Failed to load mesh file: " << path.string() << "\n";

		aiMesh* mesh = scene->mMeshes[0];

		//ASSERT(mesh->HasPositions(), "Meshes require positions.");
		//ASSERT(mesh->HasNormals(), "Meshes require normals.");

		m_Vertices.reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < m_Vertices.capacity(); i++)
		{
			NormaledVertexEffect::vertex vertex;
			vertex.x = mesh->mVertices[i].x;
			vertex.y = mesh->mVertices[i].y;
			vertex.z = mesh->mVertices[i].z;

			vertex.nx = mesh->mNormals[i].x;
			vertex.ny = mesh->mNormals[i].y;
			vertex.nz = mesh->mNormals[i].z;

			/*
			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}
			*/

			
			if (mesh->HasTextureCoords(0))
			{
				vertex.u = mesh->mTextureCoords[0][i].x;
				vertex.v = mesh->mTextureCoords[0][i].y;
			}

			m_Vertices.push_back(vertex);
		}


		// Extract indices from model
		m_Indices.reserve(mesh->mNumFaces);
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			m_Indices.push_back(mesh->mFaces[i].mIndices[0]);
			m_Indices.push_back(mesh->mFaces[i].mIndices[1]);
			m_Indices.push_back(mesh->mFaces[i].mIndices[2]);
		}
	}
}
