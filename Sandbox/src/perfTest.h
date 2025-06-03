#pragma once

#include "Bacon.h"
#include <imgui/imgui.h>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <glad/glad.h>
#include <random>

class perfTest : public Bacon::Layer
{
public:
	perfTest();
	virtual ~perfTest() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Bacon::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Bacon::Event& e) override;

private:
	struct Vertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct Quad {
		glm::vec3 Position;
		glm::vec2 Size;
		glm::vec4 Color;
		int TextureID = -1; // -1表示纯色四边形，否则为纹理ID
	};

	void InitShader();
	void InitBuffers();
	void UpdateCameraAspectRatio();
	void FlushBatch();
	void StartBatch();
	void AddQuadToBatch(const Quad& quad);
	GLuint CreateTexture(int width, int height, const uint32_t* data);
	GLuint CreateCheckerTexture();

	void StartAutoTest();
	void StopAutoTest();
	void UpdateAutoTest();
	void SaveTestResults();

private:
	Bacon::OrthographicCameraController m_CameraController;

	// 测试参数
	int m_GridSize = 100;
	float m_QuadSize = 0.2f;
	float m_Spacing = 0.3f;
	float m_TextureRatio = 0.3f; // 使用纹理的四边形比例

	// 性能计时器
	float m_BatchAddTime = 0.0f;      // 批处理添加四边形的时间
	float m_BatchFlushTime = 0.0f;    // 批处理提交时间
	float m_BatchGPUTime = 0.0f;      // 批处理GPU时间
	uint32_t m_BatchFlushCount = 0;   // 批处理提交次数

	float m_TraditionalGenTime = 0.0f;    // 传统模式生成时间
	float m_TraditionalUploadTime = 0.0f; // 传统模式上传时间
	float m_TraditionalGPUTime = 0.0f;    // 传统模式GPU时间

	float m_FrameTime = 0.0f;
	float m_FPS = 0.0f;

	// 累计性能数据
	float m_AccumBatchAddTime = 0.0f;
	float m_AccumBatchFlushTime = 0.0f;
	float m_AccumBatchGPUTime = 0.0f;
	float m_AccumTraditionalGenTime = 0.0f;
	float m_AccumTraditionalUploadTime = 0.0f;
	float m_AccumTraditionalGPUTime = 0.0f;
	float m_AccumFrameTime = 0.0f;

	// 累计调用次数
	uint32_t m_AccumBatchFlushCount = 0;
	uint32_t m_AccumTraditionalDrawCalls = 0;

	// 帧计数器
	uint32_t m_AccumFrames = 0;

	// 累计纹理槽使用情况
	float m_AccumTextureSlotsUsed = 0.0f;

	// 自动重置计数器
	static constexpr uint32_t AUTO_RESET_FRAME_COUNT = 1000; // 每1000帧自动重置累计值

	// OpenGL 对象
	GLuint m_ShaderProgram = 0;
	GLuint m_UniformBuffer = 0;
	GLuint m_BatchVAO = 0;
	GLuint m_BatchVBO = 0;
	GLuint m_BatchIBO = 0;
	GLuint m_TraditionalVAO = 0;
	GLuint m_TraditionalVBO = 0;

	// 批处理参数
	static constexpr uint32_t MAX_QUADS = 10000;
	static constexpr uint32_t MAX_TEXTURE_SLOTS = 16;
	static constexpr uint32_t MAX_BATCHES = 10; // 最大批次数量

	Vertex* m_BatchVertexBufferBase = nullptr;
	Vertex* m_BatchVertexBufferPtr = nullptr;
	uint32_t m_BatchQuadCount = 0;

	std::array<GLuint, MAX_TEXTURE_SLOTS> m_TextureSlots;
	uint32_t m_TextureSlotIndex = 0;
	GLuint m_WhiteTexture = 0;
	GLuint m_CheckerTexture = 0;

	// GPU 时间查询对象
	GLuint m_BatchTimeQuery = 0;
	GLuint m_TraditionalTimeQuery = 0;
	GLuint m_BatchTimeQueries[MAX_BATCHES] = { 0 };
	int m_CurrentBatchIndex = 0;

	// 索引数据缓存
	std::vector<uint32_t> m_Indices;

	// 窗口尺寸缓存
	uint32_t m_WindowWidth = 1280;
	uint32_t m_WindowHeight = 720;

	// ========== 自动测试功能 ==========
	bool m_AutoTestRunning = false;
	int m_AutoTestCurrentSize = 1;
	int m_AutoTestSamples = 0;
	static constexpr int AUTO_TEST_SAMPLES_PER_SIZE = 20; // 每个尺寸采样次数
	static constexpr int AUTO_TEST_MIN_SIZE = 1;
	static constexpr int AUTO_TEST_MAX_SIZE = 500;
	static constexpr int AUTO_TEST_SIZE_STEP = 5; // 尺寸增加步长

	// 当前尺寸的累计数据
	float m_AutoTestBatchAddTime = 0.0f;
	float m_AutoTestBatchFlushTime = 0.0f;
	float m_AutoTestBatchGPUTime = 0.0f;
	float m_AutoTestTraditionalGenTime = 0.0f;
	float m_AutoTestTraditionalUploadTime = 0.0f;
	float m_AutoTestTraditionalGPUTime = 0.0f;
	uint32_t m_AutoTestBatchFlushCount = 0;
	uint32_t m_AutoTestTraditionalDrawCalls = 0;

	// 测试结果
	struct AutoTestResult {
		int GridSize;
		float BatchAddTime;
		float BatchFlushTime;
		float BatchGPUTime;
		float BatchTotalTime;
		float TraditionalGenTime;
		float TraditionalUploadTime;
		float TraditionalGPUTime;
		float TraditionalTotalTime;
		float Ratio; // 传统模式总时间 / 批处理总时间
		float BatchDrawCalls;
		float TraditionalDrawCalls;
	};
	std::vector<AutoTestResult> m_AutoTestResults;
};
