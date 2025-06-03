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
		int TextureID = -1; // -1��ʾ��ɫ�ı��Σ�����Ϊ����ID
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

	// ���Բ���
	int m_GridSize = 100;
	float m_QuadSize = 0.2f;
	float m_Spacing = 0.3f;
	float m_TextureRatio = 0.3f; // ʹ��������ı��α���

	// ���ܼ�ʱ��
	float m_BatchAddTime = 0.0f;      // ����������ı��ε�ʱ��
	float m_BatchFlushTime = 0.0f;    // �������ύʱ��
	float m_BatchGPUTime = 0.0f;      // ������GPUʱ��
	uint32_t m_BatchFlushCount = 0;   // �������ύ����

	float m_TraditionalGenTime = 0.0f;    // ��ͳģʽ����ʱ��
	float m_TraditionalUploadTime = 0.0f; // ��ͳģʽ�ϴ�ʱ��
	float m_TraditionalGPUTime = 0.0f;    // ��ͳģʽGPUʱ��

	float m_FrameTime = 0.0f;
	float m_FPS = 0.0f;

	// �ۼ���������
	float m_AccumBatchAddTime = 0.0f;
	float m_AccumBatchFlushTime = 0.0f;
	float m_AccumBatchGPUTime = 0.0f;
	float m_AccumTraditionalGenTime = 0.0f;
	float m_AccumTraditionalUploadTime = 0.0f;
	float m_AccumTraditionalGPUTime = 0.0f;
	float m_AccumFrameTime = 0.0f;

	// �ۼƵ��ô���
	uint32_t m_AccumBatchFlushCount = 0;
	uint32_t m_AccumTraditionalDrawCalls = 0;

	// ֡������
	uint32_t m_AccumFrames = 0;

	// �ۼ������ʹ�����
	float m_AccumTextureSlotsUsed = 0.0f;

	// �Զ����ü�����
	static constexpr uint32_t AUTO_RESET_FRAME_COUNT = 1000; // ÿ1000֡�Զ������ۼ�ֵ

	// OpenGL ����
	GLuint m_ShaderProgram = 0;
	GLuint m_UniformBuffer = 0;
	GLuint m_BatchVAO = 0;
	GLuint m_BatchVBO = 0;
	GLuint m_BatchIBO = 0;
	GLuint m_TraditionalVAO = 0;
	GLuint m_TraditionalVBO = 0;

	// ���������
	static constexpr uint32_t MAX_QUADS = 10000;
	static constexpr uint32_t MAX_TEXTURE_SLOTS = 16;
	static constexpr uint32_t MAX_BATCHES = 10; // �����������

	Vertex* m_BatchVertexBufferBase = nullptr;
	Vertex* m_BatchVertexBufferPtr = nullptr;
	uint32_t m_BatchQuadCount = 0;

	std::array<GLuint, MAX_TEXTURE_SLOTS> m_TextureSlots;
	uint32_t m_TextureSlotIndex = 0;
	GLuint m_WhiteTexture = 0;
	GLuint m_CheckerTexture = 0;

	// GPU ʱ���ѯ����
	GLuint m_BatchTimeQuery = 0;
	GLuint m_TraditionalTimeQuery = 0;
	GLuint m_BatchTimeQueries[MAX_BATCHES] = { 0 };
	int m_CurrentBatchIndex = 0;

	// �������ݻ���
	std::vector<uint32_t> m_Indices;

	// ���ڳߴ绺��
	uint32_t m_WindowWidth = 1280;
	uint32_t m_WindowHeight = 720;

	// ========== �Զ����Թ��� ==========
	bool m_AutoTestRunning = false;
	int m_AutoTestCurrentSize = 1;
	int m_AutoTestSamples = 0;
	static constexpr int AUTO_TEST_SAMPLES_PER_SIZE = 20; // ÿ���ߴ��������
	static constexpr int AUTO_TEST_MIN_SIZE = 1;
	static constexpr int AUTO_TEST_MAX_SIZE = 500;
	static constexpr int AUTO_TEST_SIZE_STEP = 5; // �ߴ����Ӳ���

	// ��ǰ�ߴ���ۼ�����
	float m_AutoTestBatchAddTime = 0.0f;
	float m_AutoTestBatchFlushTime = 0.0f;
	float m_AutoTestBatchGPUTime = 0.0f;
	float m_AutoTestTraditionalGenTime = 0.0f;
	float m_AutoTestTraditionalUploadTime = 0.0f;
	float m_AutoTestTraditionalGPUTime = 0.0f;
	uint32_t m_AutoTestBatchFlushCount = 0;
	uint32_t m_AutoTestTraditionalDrawCalls = 0;

	// ���Խ��
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
		float Ratio; // ��ͳģʽ��ʱ�� / ��������ʱ��
		float BatchDrawCalls;
		float TraditionalDrawCalls;
	};
	std::vector<AutoTestResult> m_AutoTestResults;
};
