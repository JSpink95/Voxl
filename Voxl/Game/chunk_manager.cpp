#include "Game/chunk_manager.h"
#include "Game/chunk.h"

#include <ranges>

namespace vxl
{
	namespace helper
	{
		template<typename TPred>
		void get_all_chunks_where(
			  std::unordered_map<glm::ivec3, chunk_t*>* source
			, std::vector<chunk_t*>* chunks
			, TPred predicate)
		{
			auto values = std::views::values(*source) | std::views::filter([predicate](chunk_t* chunk) -> bool { return predicate(chunk); });
			std::transform(values.begin(), values.end(), std::back_inserter(*chunks), [](chunk_t* chunk) -> auto { return chunk; });
		}

		glm::ivec3 voxel_to_chunk(const glm::ivec3& voxel)
		{
			glm::ivec3 adjusted = voxel;
			if (adjusted.x < 0) adjusted.x -= chunk_size;
			if (adjusted.y < 0) adjusted.y -= chunk_size;
			if (adjusted.z < 0) adjusted.z -= chunk_size;

			return adjusted / (s32)chunk_size;
		}
	}

	void initialise_chunk_manager(
		  chunk_manager_t* manager
		, noise_t* noise)
	{
		manager->noise = noise;
		manager->thread.worker = std::async(chunk_loader_thread, manager);
	}

	void cleanup_chunk_manager(
		chunk_manager_t* manager)
	{
		manager->thread.running = false;
		manager->thread.worker.get();

		for (auto [id, chunk] : manager->chunks)
		{
			cleanup_chunk(chunk);
			delete chunk;
		}

		manager->chunks.clear();
	}

	void create_chunk(
		  chunk_manager_t* manager
		, s32 x
		, s32 y
		, s32 z)
	{
		vxl::chunk_t* chunk = new vxl::chunk_t{};
		chunk->offset = glm::ivec3(x, y, z) * (s32)chunk_size;
		vxl::initialise_chunk(chunk, manager->noise);

		manager->chunks.emplace(glm::ivec3(x, y, z), chunk);
		// add to load queue
		[&manager, &chunk]() -> void
		{
			std::lock_guard<std::mutex> lock(manager->thread.loadQueueMutex);
			manager->thread.loadQueue.insert(manager->thread.loadQueue.begin(), chunk);
		}();
	}

	bool try_create_chunk(
		  chunk_manager_t* manager
		, s32 x
		, s32 y
		, s32 z)
	{
		if (auto it = manager->chunks.find(glm::ivec3(x, y, z)); it != manager->chunks.end())
		{
			return false;
		}

		create_chunk(manager, x, y, z);
		return true;
	}

	void load_queued_chunk(
		chunk_manager_t* manager)
	{
		chunk_t* chunk = [&manager]() -> chunk_t*
		{
			// take ownership of the queue
			std::lock_guard<std::mutex> lock(manager->thread.loadQueueMutex);

			// pop the front element
			chunk_t* front = manager->thread.loadQueue.front();
			manager->thread.loadQueue.erase(manager->thread.loadQueue.begin());

			return front;
		}();
		vxl::generate_chunk_data(chunk);
	}

	void chunk_loader_thread(
		chunk_manager_t* manager)
	{
		manager->thread.running = true;
		while (manager->thread.running)
		{
			if (!manager->thread.loadQueue.empty())
			{
				load_queued_chunk(manager);
			}
		}
	}

	void erase_chunk(
		  chunk_manager_t* manager
		, const glm::ivec3& id)
	{
		chunk_t* chunk = get_chunk(manager, id.x, id.y, id.z);

		{
			// remove it from the load queue if it is in there
			std::lock_guard<std::mutex> chunkLoadGuard(manager->thread.loadQueueMutex);
			if (auto it = std::find(manager->thread.loadQueue.begin(), manager->thread.loadQueue.end(), chunk); it != manager->thread.loadQueue.end())
			{
				manager->thread.loadQueue.erase(it);
			}
		}

		cleanup_chunk(chunk);
		delete chunk;

		manager->chunks.erase(id);
	}

	void add_new_chunks(
		  chunk_manager_t* manager
		, const glm::ivec3& minVisibleBounds
		, const glm::ivec3& maxVisibleBounds)
	{
		for (s32 x = minVisibleBounds.x; x <= maxVisibleBounds.x; ++x)
		{
			for (s32 y = glm::max(minVisibleBounds.y, 0); y <= maxVisibleBounds.y; ++y)
			{
				for (s32 z = minVisibleBounds.z; z <= maxVisibleBounds.z; ++z)
				{
					try_create_chunk(manager, x, y, z);
				}
			}
		}

	}

	void remove_out_of_bounds_chunks(
		  chunk_manager_t* manager
		, const glm::ivec3& minVisibleBounds
		, const glm::ivec3& maxVisibleBounds)
	{
		auto filteredChunks = std::views::filter(manager->chunks, [minVisibleBounds, maxVisibleBounds](std::pair<const glm::ivec3, chunk_t*>& chunks) -> bool
		{
			glm::ivec3 chunkId = chunks.first;
			chunk_t* chunk = chunks.second;

			const bool xOverlap = minVisibleBounds.x <= chunkId.x && chunkId.x <= maxVisibleBounds.x;
			const bool yOverlap = minVisibleBounds.y <= chunkId.y && chunkId.y <= maxVisibleBounds.y;
			const bool zOverlap = minVisibleBounds.z <= chunkId.z && chunkId.z <= maxVisibleBounds.z;
			return (!xOverlap || !yOverlap || !zOverlap) && !chunk->dirty && !chunk->generatingMeshData;
		});

		std::vector<glm::ivec3> chunkIds = {};
		std::transform(filteredChunks.begin(), filteredChunks.end(), std::back_inserter(chunkIds), [](std::pair<const glm::ivec3, chunk_t*>& pair) -> glm::ivec3
			{
				return pair.first;
			});

		for (const glm::ivec3& id: chunkIds)
		{
			erase_chunk(manager, id);
		}
	}

	void sort_chunk_queue(
		  chunk_manager_t* manager
		, const glm::vec3& camera)
	{
		std::lock_guard<std::mutex> guard(manager->thread.loadQueueMutex);
		std::sort(manager->thread.loadQueue.begin(), manager->thread.loadQueue.end(), [camera](chunk_t* a, chunk_t* b)
			{
				glm::vec3 aCentre = glm::vec3(a->offset + (s32)chunk_size / 2);
				glm::vec3 bCentre = glm::vec3(b->offset + (s32)chunk_size / 2);

				f32 aDistance = glm::distance(aCentre, camera);
				f32 bDistance = glm::distance(bCentre, camera);
				return aDistance < bDistance;
			});
	}

	void update_chunks(
		  chunk_manager_t* manager
		, const glm::vec3& camera)
	{
		constexpr s32 chunk_visible_distance = 4;

		glm::ivec3 minVisibleBounds = helper::voxel_to_chunk(glm::ivec3(camera) - glm::ivec3(chunk_visible_distance * chunk_size));
		glm::ivec3 maxVisibleBounds = helper::voxel_to_chunk(glm::ivec3(camera) + glm::ivec3(chunk_visible_distance * chunk_size));

		add_new_chunks(manager, minVisibleBounds, maxVisibleBounds);
		remove_out_of_bounds_chunks(manager, minVisibleBounds, maxVisibleBounds);
		sort_chunk_queue(manager, camera);
	}

	void get_all_chunks(
		  chunk_manager_t* manager
		, std::vector<chunk_t*>* chunks)
	{
		helper::get_all_chunks_where(&manager->chunks, chunks, [](chunk_t* chunk) -> bool
			{
				return chunk != nullptr;
			});
	}

	void get_dirty_chunks(
		  chunk_manager_t* manager
		, std::vector<chunk_t*>* chunks)
	{
		helper::get_all_chunks_where(&manager->chunks, chunks, [](chunk_t* chunk) -> bool
			{
				return chunk != nullptr && chunk->dirty;
			});
	}

	vxl::chunk_t* get_chunk(
		  chunk_manager_t* manager
		, s32 x
		, s32 y
		, s32 z)
	{
		if (auto it = manager->chunks.find(glm::ivec3(x, y, z)); it != manager->chunks.end())
		{
			return it->second;
		}

		return nullptr;
	}
}