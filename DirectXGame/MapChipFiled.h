#include "base/DirectXCommon.h"
#include "input/Input.h"
#include <vector>
#include <string>
#include <assert.h>
#include "math/Vector3.h"

enum class MapChipType {
	kBlank, // ��
	kBlock, // �u���b�N
	kEnemy,
};
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// �}�b�v�`�b�v�t�B�[���h
/// </summary>
class MapChipFiled {

public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};
	// �͈͋�`
	struct Rect {
		float left;   // ���[
		float right;  // �E�[
		float bottom; // ���[
		float top;    // ��[
	};

public:
	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }

	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	// 1�u���b�N�̃T�C�Y
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// �u���b�N�̌�
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};