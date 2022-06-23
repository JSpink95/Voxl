#pragma once
#include <core_types.h>

// get a hash for glm::ivec3
#include <glm\gtx\hash.hpp>

#include <future>
#include <vector>
#include <unordered_map>

namespace vxl
{
	struct noise_t;
	struct chunk_t;

	struct chunk_manager_thread_t
	{
		// thread
		std::future<void> worker;
		bool running = false;

		// load queue
		std::mutex loadQueueMutex;
		std::vector<vxl::chunk_t*> loadQueue;
	};

	struct chunk_manager_t
	{
		noise_t* noise = nullptr;
		std::unordered_map<glm::ivec3, vxl::chunk_t*> chunks;

		chunk_manager_thread_t thread;
	};

	void initialise_chunk_manager(
		  chunk_manager_t* manager
		, noise_t* noise);

	void cleanup_chunk_manager(
		chunk_manager_t* manager);

	void create_chunk(
		  chunk_manager_t* manager
		, s32 x
		, s32 y
		, s32 z);

	void chunk_loader_thread(
		chunk_manager_t* manager);

	void update_chunks(
		chunk_manager_t* manager
		, const glm::vec3& camera);

	void get_all_chunks(
		chunk_manager_t* manager
		, std::vector<chunk_t*>* chunks);

	void get_dirty_chunks(
		  chunk_manager_t* manager
		, std::vector<chunk_t*>* chunks);

	vxl::chunk_t* get_chunk(
		  chunk_manager_t* manager
		, s32 x
		, s32 y
		, s32 z);
}