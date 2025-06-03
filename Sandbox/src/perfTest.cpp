#include "perfTest.h"
#include <imgui/imgui.h>
#include <chrono>
#include <random>
#include <algorithm>

// 顶点着色器源码
const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex;

layout (std140) uniform CameraMatrices {
    mat4 u_ViewProjection;
};

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
}
)";

// 片段着色器源码
const char* fragmentShaderSrc = R"(
#version 330 core
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[16];

out vec4 FragColor;

void main()
{
    // 纯色四边形
    if (v_TexIndex < 0.0) {
        FragColor = v_Color;
    } 
    // 纹理四边形
    else {
        int index = int(v_TexIndex);
        FragColor = texture(u_Textures[index], v_TexCoord) * v_Color;
    }
}
)";

perfTest::perfTest()
	: Layer("perfTest"), m_CameraController(1280.0f / 720.0f)
{
	// 初始化GPU时间查询对象
	glGenQueries(1, &m_BatchTimeQuery);
	glGenQueries(1, &m_TraditionalTimeQuery);
	glGenQueries(MAX_BATCHES, m_BatchTimeQueries);

	// 预生成索引数据
	m_Indices.resize(6 * MAX_QUADS);
	for (uint32_t i = 0, offset = 0; i < MAX_QUADS; i++) {
		m_Indices[i * 6 + 0] = offset + 0;
		m_Indices[i * 6 + 1] = offset + 1;
		m_Indices[i * 6 + 2] = offset + 2;
		m_Indices[i * 6 + 3] = offset + 2;
		m_Indices[i * 6 + 4] = offset + 3;
		m_Indices[i * 6 + 5] = offset + 0;
		offset += 4;
	}

	// 初始化自动测试状态
	m_AutoTestRunning = false;
	m_AutoTestCurrentSize = AUTO_TEST_MIN_SIZE;
	m_AutoTestSamples = 0;
}

GLuint perfTest::CreateTexture(int width, int height, const uint32_t* data)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 使用带 Mipmap 的过滤防止锯齿
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// 生成 Mipmap 以消除锯齿
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint perfTest::CreateCheckerTexture()
{
	const int width = 64, height = 64;
	uint32_t* pixels = new uint32_t[width * height];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			bool isBlack = ((x / 8) % 2) ^ ((y / 8) % 2);
			pixels[y * width + x] = isBlack ? 0xFF000000 : 0xFFFFFFFF;
		}
	}

	GLuint texture = CreateTexture(width, height, pixels);
	delete[] pixels;

	return texture;
}

void perfTest::OnAttach()
{
	BC_PROFILE_FUNCTION();
	InitShader();
	InitBuffers();

	// 启用深度测试以避免闪烁
	glDisable(GL_DEPTH_TEST);

	// 启用混合以支持半透明
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 创建纹理
	uint32_t whitePixel = 0xFFFFFFFF;
	m_WhiteTexture = CreateTexture(1, 1, &whitePixel);
	m_CheckerTexture = CreateCheckerTexture();
}

void perfTest::OnDetach()
{
	BC_PROFILE_FUNCTION();
	glDeleteProgram(m_ShaderProgram);
	glDeleteBuffers(1, &m_UniformBuffer);

	// 删除纹理
	glDeleteTextures(1, &m_WhiteTexture);
	glDeleteTextures(1, &m_CheckerTexture);

	// 删除缓冲区
	delete[] m_BatchVertexBufferBase;

	glDeleteVertexArrays(1, &m_BatchVAO);
	glDeleteBuffers(1, &m_BatchVBO);
	glDeleteBuffers(1, &m_BatchIBO);

	glDeleteVertexArrays(1, &m_TraditionalVAO);
	glDeleteBuffers(1, &m_TraditionalVBO);

	// 删除查询对象
	glDeleteQueries(1, &m_BatchTimeQuery);
	glDeleteQueries(1, &m_TraditionalTimeQuery);
	glDeleteQueries(MAX_BATCHES, m_BatchTimeQueries);
}

void perfTest::InitShader()
{
	// 创建着色器程序
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);

	// 检查顶点着色器编译错误
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		BC_CORE_ERROR("Vertex shader compilation failed: {0}", infoLog);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);

	// 检查片段着色器编译错误
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		BC_CORE_ERROR("Fragment shader compilation failed: {0}", infoLog);
	}

	// 链接着色器程序
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram, vertexShader);
	glAttachShader(m_ShaderProgram, fragmentShader);
	glLinkProgram(m_ShaderProgram);

	// 检查链接错误
	glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
		BC_CORE_ERROR("Shader program linking failed: {0}", infoLog);
	}

	// 删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 设置纹理采样器
	glUseProgram(m_ShaderProgram);
	GLint loc = glGetUniformLocation(m_ShaderProgram, "u_Textures");
	int samplers[MAX_TEXTURE_SLOTS];
	for (int i = 0; i < MAX_TEXTURE_SLOTS; i++)
		samplers[i] = i;
	glUniform1iv(loc, MAX_TEXTURE_SLOTS, samplers);
	glUseProgram(0);
}

void perfTest::InitBuffers()
{
	// 创建统一缓冲区
	glGenBuffers(1, &m_UniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UniformBuffer);

	// ======== 批处理顶点缓冲区 ========
	glGenVertexArrays(1, &m_BatchVAO);
	glGenBuffers(1, &m_BatchVBO);
	glGenBuffers(1, &m_BatchIBO);

	glBindVertexArray(m_BatchVAO);

	// 顶点缓冲区
	glBindBuffer(GL_ARRAY_BUFFER, m_BatchVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4 * MAX_QUADS, nullptr, GL_DYNAMIC_DRAW);

	// 预分配内存
	m_BatchVertexBufferBase = new Vertex[MAX_QUADS * 4];
	m_BatchVertexBufferPtr = m_BatchVertexBufferBase;

	// 位置属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

	// 颜色属性
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

	// 纹理坐标
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

	// 纹理索引
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));

	// 索引缓冲区
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BatchIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), m_Indices.data(), GL_STATIC_DRAW);

	// ======== 传统立即模式缓冲区 ========
	glGenVertexArrays(1, &m_TraditionalVAO);
	glGenBuffers(1, &m_TraditionalVBO);

	glBindVertexArray(m_TraditionalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TraditionalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4 * MAX_QUADS, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));

	glBindVertexArray(0);

	// 初始化批处理状态
	StartBatch();
}

void perfTest::UpdateCameraAspectRatio()
{
	auto& window = Bacon::Application::Get().GetWindow();
	uint32_t width = window.GetWidth();
	uint32_t height = window.GetHeight();

	if (width != m_WindowWidth || height != m_WindowHeight) {
		m_WindowWidth = width;
		m_WindowHeight = height;
		m_CameraController.OnResize(static_cast<float>(width), static_cast<float>(height));
	}
}

void perfTest::StartBatch()
{
	m_BatchQuadCount = 0;
	m_BatchVertexBufferPtr = m_BatchVertexBufferBase;
	m_TextureSlotIndex = 0;
}

void perfTest::AddQuadToBatch(const Quad& quad)
{
	// 获取实际纹理ID
	GLuint actualTexture = 0;
	if (quad.TextureID != -1) {
		actualTexture = (quad.TextureID == 0) ? m_WhiteTexture : m_CheckerTexture;
	}

	float texIndex = -1.0f;
	if (quad.TextureID != -1) {
		// 查找纹理是否已存在
		bool found = false;
		for (uint32_t i = 0; i < m_TextureSlotIndex; i++) {
			if (m_TextureSlots[i] == actualTexture) {
				texIndex = static_cast<float>(i);
				found = true;
				break;
			}
		}

		// 添加新纹理
		if (!found) {
			if (m_TextureSlotIndex >= MAX_TEXTURE_SLOTS) {
				// 纹理槽已满，先提交当前批次
				FlushBatch();
				StartBatch();

				// 重新尝试添加纹理
				found = false;
				for (uint32_t i = 0; i < m_TextureSlotIndex; i++) {
					if (m_TextureSlots[i] == actualTexture) {
						texIndex = static_cast<float>(i);
						found = true;
						break;
					}
				}

				// 如果仍然没找到，添加新纹理
				if (!found) {
					texIndex = static_cast<float>(m_TextureSlotIndex);
					m_TextureSlots[m_TextureSlotIndex] = actualTexture;
					m_TextureSlotIndex++;
				}
			}
			else {
				texIndex = static_cast<float>(m_TextureSlotIndex);
				m_TextureSlots[m_TextureSlotIndex] = actualTexture;
				m_TextureSlotIndex++;
			}
		}
	}

	const float halfX = quad.Size.x * 0.5f;
	const float halfY = quad.Size.y * 0.5f;

	// 顶点数据
	Vertex* v = m_BatchVertexBufferPtr;

	// 位置
	v[0].Position = { quad.Position.x - halfX, quad.Position.y - halfY, quad.Position.z };
	v[1].Position = { quad.Position.x + halfX, quad.Position.y - halfY, quad.Position.z };
	v[2].Position = { quad.Position.x + halfX, quad.Position.y + halfY, quad.Position.z };
	v[3].Position = { quad.Position.x - halfX, quad.Position.y + halfY, quad.Position.z };

	// 颜色
	v[0].Color = quad.Color;
	v[1].Color = quad.Color;
	v[2].Color = quad.Color;
	v[3].Color = quad.Color;

	// 纹理坐标
	v[0].TexCoord = { 0.0f, 0.0f };
	v[1].TexCoord = { 1.0f, 0.0f };
	v[2].TexCoord = { 1.0f, 1.0f };
	v[3].TexCoord = { 0.0f, 1.0f };

	// 纹理索引
	v[0].TexIndex = texIndex;
	v[1].TexIndex = texIndex;
	v[2].TexIndex = texIndex;
	v[3].TexIndex = texIndex;

	m_BatchVertexBufferPtr += 4;
	m_BatchQuadCount++;
}

void perfTest::FlushBatch()
{
	if (m_BatchQuadCount == 0)
		return;

	// 测量上传时间
	auto uploadStart = std::chrono::high_resolution_clock::now();

	// 上传顶点数据
	size_t dataSize = (uint8_t*)m_BatchVertexBufferPtr - (uint8_t*)m_BatchVertexBufferBase;
	glBindBuffer(GL_ARRAY_BUFFER, m_BatchVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_BatchVertexBufferBase);

	auto uploadEnd = std::chrono::high_resolution_clock::now();
	m_BatchFlushTime += std::chrono::duration<float, std::milli>(uploadEnd - uploadStart).count();

	// 开始当前批次的GPU时间查询
	if (m_CurrentBatchIndex < MAX_BATCHES) {
		glBeginQuery(GL_TIME_ELAPSED, m_BatchTimeQueries[m_CurrentBatchIndex]);
	}

	// 关键修复：绑定着色器程序
	glUseProgram(m_ShaderProgram);

	// 绑定纹理
	for (uint32_t i = 0; i < m_TextureSlotIndex; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_TextureSlots[i]);
	}

	// 绘制
	glBindVertexArray(m_BatchVAO);
	glDrawElements(GL_TRIANGLES, m_BatchQuadCount * 6, GL_UNSIGNED_INT, 0);

	// 结束当前批次的GPU时间查询
	if (m_CurrentBatchIndex < MAX_BATCHES) {
		glEndQuery(GL_TIME_ELAPSED);
	}

	m_BatchFlushCount++;
	m_CurrentBatchIndex++;
}

void perfTest::StartAutoTest()
{
	m_AutoTestRunning = true;
	m_AutoTestCurrentSize = AUTO_TEST_MIN_SIZE;
	m_AutoTestSamples = 0;
	m_AutoTestResults.clear();

	// 重置累计数据
	m_AutoTestBatchAddTime = 0.0f;
	m_AutoTestBatchFlushTime = 0.0f;
	m_AutoTestBatchGPUTime = 0.0f;
	m_AutoTestTraditionalGenTime = 0.0f;
	m_AutoTestTraditionalUploadTime = 0.0f;
	m_AutoTestTraditionalGPUTime = 0.0f;
	m_AutoTestBatchFlushCount = 0;
	m_AutoTestTraditionalDrawCalls = 0;

	BC_CORE_INFO("AutoTest started");
}

void perfTest::StopAutoTest()
{
	if (m_AutoTestRunning) {
		m_AutoTestRunning = false;
		SaveTestResults();
		BC_CORE_INFO("AutoTest stopped and results saved");
	}
}

void perfTest::UpdateAutoTest()
{
	if (!m_AutoTestRunning)
		return;

	// 累计当前尺寸的性能数据
	m_AutoTestBatchAddTime += m_BatchAddTime;
	m_AutoTestBatchFlushTime += m_BatchFlushTime;
	m_AutoTestBatchGPUTime += m_BatchGPUTime;
	m_AutoTestTraditionalGenTime += m_TraditionalGenTime;
	m_AutoTestTraditionalUploadTime += m_TraditionalUploadTime;
	m_AutoTestTraditionalGPUTime += m_TraditionalGPUTime;
	m_AutoTestBatchFlushCount += m_BatchFlushCount;
	m_AutoTestTraditionalDrawCalls += (m_GridSize * m_GridSize);

	m_AutoTestSamples++;

	// 如果达到采样次数，保存结果并前进到下一个尺寸
	if (m_AutoTestSamples >= AUTO_TEST_SAMPLES_PER_SIZE) {
		// 计算平均值
		AutoTestResult result;
		result.GridSize = m_AutoTestCurrentSize;
		result.BatchAddTime = m_AutoTestBatchAddTime / AUTO_TEST_SAMPLES_PER_SIZE;
		result.BatchFlushTime = m_AutoTestBatchFlushTime / AUTO_TEST_SAMPLES_PER_SIZE;
		result.BatchGPUTime = m_AutoTestBatchGPUTime / AUTO_TEST_SAMPLES_PER_SIZE;
		result.BatchTotalTime = result.BatchAddTime + result.BatchFlushTime + result.BatchGPUTime;
		result.TraditionalGenTime = m_AutoTestTraditionalGenTime / AUTO_TEST_SAMPLES_PER_SIZE;
		result.TraditionalUploadTime = m_AutoTestTraditionalUploadTime / AUTO_TEST_SAMPLES_PER_SIZE;
		result.TraditionalGPUTime = m_AutoTestTraditionalGPUTime / AUTO_TEST_SAMPLES_PER_SIZE;
		result.TraditionalTotalTime = result.TraditionalGenTime + result.TraditionalUploadTime + result.TraditionalGPUTime;
		result.Ratio = result.TraditionalTotalTime / result.BatchTotalTime;
		result.BatchDrawCalls = static_cast<float>(m_AutoTestBatchFlushCount) / AUTO_TEST_SAMPLES_PER_SIZE;
		result.TraditionalDrawCalls = static_cast<float>(m_AutoTestTraditionalDrawCalls) / AUTO_TEST_SAMPLES_PER_SIZE;

		m_AutoTestResults.push_back(result);

		// 重置累计数据
		m_AutoTestBatchAddTime = 0.0f;
		m_AutoTestBatchFlushTime = 0.0f;
		m_AutoTestBatchGPUTime = 0.0f;
		m_AutoTestTraditionalGenTime = 0.0f;
		m_AutoTestTraditionalUploadTime = 0.0f;
		m_AutoTestTraditionalGPUTime = 0.0f;
		m_AutoTestBatchFlushCount = 0;
		m_AutoTestTraditionalDrawCalls = 0;

		// 前进到下一个尺寸
		m_AutoTestCurrentSize += AUTO_TEST_SIZE_STEP;
		m_AutoTestSamples = 0;

		// 如果超出范围，停止测试
		if (m_AutoTestCurrentSize > AUTO_TEST_MAX_SIZE) {
			StopAutoTest();
		}
	}
}

void perfTest::SaveTestResults()
{
	std::ofstream file("perf_test_results.csv");
	if (!file.is_open()) {
		BC_CORE_ERROR("Failed to open perf_test_results.csv for writing");
		return;
	}

	// 写入CSV头部
	file << "GridSize,"
		<< "BatchAddTime,BatchFlushTime,BatchGPUTime,BatchTotalTime,"
		<< "TraditionalGenTime,TraditionalUploadTime,TraditionalGPUTime,TraditionalTotalTime,"
		<< "Ratio,BatchDrawCalls,TraditionalDrawCalls\n";

	// 写入数据
	for (const auto& result : m_AutoTestResults) {
		file << result.GridSize << ","
			<< result.BatchAddTime << "," << result.BatchFlushTime << "," << result.BatchGPUTime << "," << result.BatchTotalTime << ","
			<< result.TraditionalGenTime << "," << result.TraditionalUploadTime << "," << result.TraditionalGPUTime << "," << result.TraditionalTotalTime << ","
			<< result.Ratio << ","
			<< result.BatchDrawCalls << "," << result.TraditionalDrawCalls << "\n";
	}

	file.close();

	// 添加成功消息
	BC_CORE_INFO("Saved {} test results to perf_test_results.csv", m_AutoTestResults.size());
	BC_CORE_INFO("File location: {}", std::filesystem::absolute("perf_test_results.csv").string());
}

void perfTest::OnUpdate(Bacon::Timestep ts)
{
	BC_PROFILE_FUNCTION();
	auto frameStart = std::chrono::high_resolution_clock::now();
	
	// 更新自动测试状态
	if (m_AutoTestRunning) {
		// 设置当前测试尺寸
		m_GridSize = m_AutoTestCurrentSize;
	}

	// 重置批处理计数器
	m_BatchFlushCount = 0;
	m_BatchAddTime = 0.0f;
	m_BatchFlushTime = 0.0f;
	m_CurrentBatchIndex = 0;

	// 更新相机宽高比
	UpdateCameraAspectRatio();

	// 更新相机
	m_CameraController.OnUpdate(ts);

	// 设置视口
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);

	// 清除缓冲区
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 更新统一缓冲区
	glm::mat4 vp = m_CameraController.GetCamera().GetViewProjectionMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(vp));

	// 准备四边形数据
	const int totalQuads = m_GridSize * m_GridSize;
	const float halfSize = (m_GridSize - 1) * m_Spacing * 0.5f;

	// 使用微小深度偏移避免闪烁
	float zBase = 0.0f;
	const float zIncrement = 0.0001f;

	// 批处理四边形（左侧）
	std::vector<Quad> batchQuads;
	batchQuads.reserve(totalQuads);

	// 传统模式四边形（右侧）
	std::vector<Quad> traditionalQuads;
	traditionalQuads.reserve(totalQuads);

	// 随机数生成器（用于纹理分配）
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	for (int y = 0; y < m_GridSize; y++) {
		for (int x = 0; x < m_GridSize; x++) {
			glm::vec4 color = {
				static_cast<float>(x) / m_GridSize,
				0.4f,
				static_cast<float>(y) / m_GridSize,
				0.7f  // 注意：alpha值为0.7
			};

			// 随机决定是否使用纹理
			int textureID = -1;
			if (dis(gen) < m_TextureRatio) {
				textureID = (x + y) % 2; // 0或1，表示两种纹理
			}

			// 添加微小深度偏移避免闪烁
			float z = zBase;
			zBase += zIncrement;

			// 批处理四边形（左侧）
			float batchX = x * m_Spacing - halfSize - 6.0f;
			float batchY = y * m_Spacing - halfSize;
			batchQuads.push_back({
				{ batchX, batchY, z },
				{ m_QuadSize, m_QuadSize },
				color,
				textureID
				});

			// 传统模式四边形（右侧）
			float traditionalX = x * m_Spacing - halfSize + 6.0f;
			float traditionalY = y * m_Spacing - halfSize;
			traditionalQuads.push_back({
				{ traditionalX, traditionalY, z },
				{ m_QuadSize, m_QuadSize },
				color,
				textureID
				});
		}
	}

	// ======== 批处理渲染 ========
	auto batchAddStart = std::chrono::high_resolution_clock::now();

	StartBatch();
	for (const auto& quad : batchQuads) {
		bool needNewBatch = false;

		// 情况1：四边形数量达到上限
		if (m_BatchQuadCount >= MAX_QUADS) {
			needNewBatch = true;
		}
		// 情况2：纹理四边形但纹理槽已满
		else if (quad.TextureID != -1 && m_TextureSlotIndex >= MAX_TEXTURE_SLOTS) {
			needNewBatch = true;
		}

		if (needNewBatch) {
			// 提交当前批次
			auto batchAddEnd = std::chrono::high_resolution_clock::now();
			m_BatchAddTime += std::chrono::duration<float, std::milli>(batchAddEnd - batchAddStart).count();

			FlushBatch();
			StartBatch();

			batchAddStart = std::chrono::high_resolution_clock::now();
		}

		AddQuadToBatch(quad);
	}
	auto batchAddEnd = std::chrono::high_resolution_clock::now();
	m_BatchAddTime += std::chrono::duration<float, std::milli>(batchAddEnd - batchAddStart).count();

	// 提交最终批次
	FlushBatch();
	auto batchEnd = std::chrono::high_resolution_clock::now();

	// 获取所有批次的GPU时间
	m_BatchGPUTime = 0.0f;
	int numBatchesToRead = (m_CurrentBatchIndex < MAX_BATCHES) ? m_CurrentBatchIndex : MAX_BATCHES;
	for (int i = 0; i < numBatchesToRead; i++) {
		GLuint64 batchGpuTime = 0;
		GLint available = 0;
		while (!available) {
			glGetQueryObjectiv(m_BatchTimeQueries[i], GL_QUERY_RESULT_AVAILABLE, &available);
		}
		glGetQueryObjectui64v(m_BatchTimeQueries[i], GL_QUERY_RESULT, &batchGpuTime);
		m_BatchGPUTime += static_cast<float>(batchGpuTime) / 1000000.0f; // 纳秒转毫秒
	}

	// ======== 传统立即模式渲染 ========
	auto tradGenStart = std::chrono::high_resolution_clock::now();

	// 重置传统模式上传时间
	m_TraditionalUploadTime = 0.0f;

	glUseProgram(m_ShaderProgram);
	glBindVertexArray(m_TraditionalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TraditionalVBO);

	// 开始传统模式GPU时间查询
	glBeginQuery(GL_TIME_ELAPSED, m_TraditionalTimeQuery);

	// 当前绑定的纹理
	GLuint currentTexture = 0;
	bool textureBound = false;

	for (const auto& quad : traditionalQuads) {
		// 生成顶点数据
		const float halfX = quad.Size.x * 0.5f;
		const float halfY = quad.Size.y * 0.5f;

		Vertex vertices[4];

		// 位置
		vertices[0].Position = { quad.Position.x - halfX, quad.Position.y - halfY, quad.Position.z };
		vertices[1].Position = { quad.Position.x + halfX, quad.Position.y - halfY, quad.Position.z };
		vertices[2].Position = { quad.Position.x + halfX, quad.Position.y + halfY, quad.Position.z };
		vertices[3].Position = { quad.Position.x - halfX, quad.Position.y + halfY, quad.Position.z };

		// 颜色
		vertices[0].Color = quad.Color;
		vertices[1].Color = quad.Color;
		vertices[2].Color = quad.Color;
		vertices[3].Color = quad.Color;

		// 纹理坐标
		vertices[0].TexCoord = { 0.0f, 0.0f };
		vertices[1].TexCoord = { 1.0f, 0.0f };
		vertices[2].TexCoord = { 1.0f, 1.0f };
		vertices[3].TexCoord = { 0.0f, 1.0f };

		// 纹理索引
		if (quad.TextureID != -1) {
			GLuint texture = quad.TextureID == 0 ? m_WhiteTexture : m_CheckerTexture;

			// 优化：仅在纹理变化时绑定
			if (!textureBound || texture != currentTexture) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				currentTexture = texture;
				textureBound = true;
			}

			vertices[0].TexIndex = 0.0f;
			vertices[1].TexIndex = 0.0f;
			vertices[2].TexIndex = 0.0f;
			vertices[3].TexIndex = 0.0f;
		}
		else {
			// 确保纯色四边形正确设置纹理索引
			textureBound = false;
			vertices[0].TexIndex = -1.0f;
			vertices[1].TexIndex = -1.0f;
			vertices[2].TexIndex = -1.0f;
			vertices[3].TexIndex = -1.0f;
		}

		// 测量上传时间
		auto uploadStart = std::chrono::high_resolution_clock::now();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		auto uploadEnd = std::chrono::high_resolution_clock::now();
		m_TraditionalUploadTime += std::chrono::duration<float, std::milli>(uploadEnd - uploadStart).count();

		// 绘制
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	// 结束传统模式GPU时间查询
	glEndQuery(GL_TIME_ELAPSED);

	auto tradGenEnd = std::chrono::high_resolution_clock::now();
	m_TraditionalGenTime = std::chrono::duration<float, std::milli>(tradGenEnd - tradGenStart).count();

	// 获取传统模式GPU时间
	GLuint64 tradGpuTime = 0;
	glGetQueryObjectui64v(m_TraditionalTimeQuery, GL_QUERY_RESULT, &tradGpuTime);
	m_TraditionalGPUTime = static_cast<float>(tradGpuTime) / 1000000.0f;

	// ======== 帧结束处理 ========
	auto frameEnd = std::chrono::high_resolution_clock::now();
	m_FrameTime = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();
	m_FPS = 1000.0f / m_FrameTime;

	// 更新自动测试
	if (m_AutoTestRunning) {
		UpdateAutoTest();
	}

	// 重置OpenGL状态 - 确保不影响ImGui渲染
	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ======== 累计性能指标 ========
	m_AccumBatchAddTime += m_BatchAddTime;
	m_AccumBatchFlushTime += m_BatchFlushTime;
	m_AccumBatchGPUTime += m_BatchGPUTime;
	m_AccumTraditionalGenTime += m_TraditionalGenTime;
	m_AccumTraditionalUploadTime += m_TraditionalUploadTime;
	m_AccumTraditionalGPUTime += m_TraditionalGPUTime;
	m_AccumFrameTime += m_FrameTime;

	// 累计绘制调用
	m_AccumBatchFlushCount += m_BatchFlushCount;
	m_AccumTraditionalDrawCalls += (m_GridSize * m_GridSize);

	// 累计纹理槽使用情况
	m_AccumTextureSlotsUsed += m_TextureSlotIndex;

	// 增加帧计数器
	m_AccumFrames++;

	// 自动重置机制：每1000帧重置累计值
	if (m_AccumFrames >= AUTO_RESET_FRAME_COUNT) {
		m_AccumBatchAddTime = 0.0f;
		m_AccumBatchFlushTime = 0.0f;
		m_AccumBatchGPUTime = 0.0f;
		m_AccumTraditionalGenTime = 0.0f;
		m_AccumTraditionalUploadTime = 0.0f;
		m_AccumTraditionalGPUTime = 0.0f;
		m_AccumFrameTime = 0.0f;
		m_AccumBatchFlushCount = 0;
		m_AccumTraditionalDrawCalls = 0;
		m_AccumTextureSlotsUsed = 0.0f;
		m_AccumFrames = 0;
	}
}

void perfTest::OnImGuiRender()
{
	BC_PROFILE_FUNCTION();

	// ======== 控制面板 ========
	ImGui::Begin("PerfTest Controls");

	// 自动测试控制
	ImGui::Separator();
	ImGui::Text("Auto Test:");
	if (ImGui::Button(m_AutoTestRunning ? "Stop Auto Test" : "Start Auto Test")) {
		if (m_AutoTestRunning) {
			StopAutoTest();
		}
		else {
			StartAutoTest();
		}
	}

	if (m_AutoTestRunning) {
		ImGui::Text("Testing: GridSize = %d (%d/%d samples)",
			m_AutoTestCurrentSize,
			m_AutoTestSamples,
			AUTO_TEST_SAMPLES_PER_SIZE);
		ImGui::Text("Progress: %d/%d",
			m_AutoTestCurrentSize - AUTO_TEST_MIN_SIZE,
			AUTO_TEST_MAX_SIZE - AUTO_TEST_MIN_SIZE);
		ImGui::ProgressBar(static_cast<float>(m_AutoTestCurrentSize - AUTO_TEST_MIN_SIZE) /
			(AUTO_TEST_MAX_SIZE - AUTO_TEST_MIN_SIZE));
	}
	else if (!m_AutoTestResults.empty()) {
		ImGui::Text("Auto Test Complete: %d sizes tested", m_AutoTestResults.size());
		if (ImGui::Button("Save Results to CSV")) {
			SaveTestResults();
		}
	}

	// 控制参数
	ImGui::Text("Test Parameters:");
	ImGui::SliderInt("Grid Size", &m_GridSize, 1, 500);
	ImGui::SliderFloat("Quad Size", &m_QuadSize, 0.1f, 1.0f);
	ImGui::SliderFloat("Spacing", &m_Spacing, 0.1f, 2.0f);
	ImGui::SliderFloat("Texture Ratio", &m_TextureRatio, 0.0f, 1.0f);

	// 统计信息
	ImGui::Separator();
	ImGui::Text("Render Stats:");
	ImGui::Text("  Quads: %d", m_GridSize * m_GridSize);
	ImGui::Text("  Vertices: %d", m_GridSize * m_GridSize * 4);
	ImGui::Text("  Indices: %d", m_GridSize * m_GridSize * 6);
	ImGui::Text("  Batch Count: %d", m_BatchFlushCount);

	// 重置按钮
	ImGui::Separator();
	if (ImGui::Button("Reset Averages")) {
		m_AccumBatchAddTime = 0.0f;
		m_AccumBatchFlushTime = 0.0f;
		m_AccumBatchGPUTime = 0.0f;
		m_AccumTraditionalGenTime = 0.0f;
		m_AccumTraditionalUploadTime = 0.0f;
		m_AccumTraditionalGPUTime = 0.0f;
		m_AccumFrameTime = 0.0f;
		m_AccumBatchFlushCount = 0;
		m_AccumTraditionalDrawCalls = 0;
		m_AccumTextureSlotsUsed = 0.0f;
		m_AccumFrames = 0;
	}
	ImGui::SameLine();
	ImGui::Text("Auto reset every %d frames", AUTO_RESET_FRAME_COUNT);

	ImGui::End();

	// ======== 性能数据面板 ========
	ImGui::Begin("PerfTest Metrics");

	// 当前帧概览
	ImGui::Text("Frame: %.1f FPS (%.2f ms)", m_FPS, m_FrameTime);

	// 使用标签页组织数据
	if (ImGui::BeginTabBar("PerformanceTabs")) {
		// 当前帧标签页
		if (ImGui::BeginTabItem("Current Frame")) {
			// 批处理渲染性能
			ImGui::Separator();
			ImGui::Text("Batch Rendering:");
			ImGui::Columns(2, "batch_columns", true);
			ImGui::Text("Draw Calls: %d", m_BatchFlushCount);
			ImGui::Text("Texture Slots: %d/%d", m_TextureSlotIndex, MAX_TEXTURE_SLOTS);
			ImGui::NextColumn();
			ImGui::Text("Add Time: %.3f ms", m_BatchAddTime);
			ImGui::Text("Flush Time: %.3f ms", m_BatchFlushTime);
			ImGui::Text("GPU Time: %.3f ms", m_BatchGPUTime);
			ImGui::Columns(1);

			// 传统渲染性能
			ImGui::Separator();
			ImGui::Text("Traditional Rendering:");
			ImGui::Columns(2, "traditional_columns", true);
			ImGui::Text("Draw Calls: %d", m_GridSize * m_GridSize);
			ImGui::NextColumn();
			ImGui::Text("Gen Time: %.3f ms", m_TraditionalGenTime);
			ImGui::Text("Upload Time: %.3f ms", m_TraditionalUploadTime);
			ImGui::Text("GPU Time: %.3f ms", m_TraditionalGPUTime);
			ImGui::Columns(1);

			// 性能对比
			ImGui::Separator();
			ImGui::Text("Performance Ratio:");
			float batchTotal = m_BatchAddTime + m_BatchFlushTime + m_BatchGPUTime;
			float tradTotal = m_TraditionalGenTime + m_TraditionalUploadTime + m_TraditionalGPUTime;
			ImGui::Text("  Batch Total: %.3f ms", batchTotal);
			ImGui::Text("  Traditional Total: %.3f ms", tradTotal);
			ImGui::Text("  Ratio: %.2fx", tradTotal / batchTotal);

			ImGui::EndTabItem();
		}

		// 平均数据标签页
		if (ImGui::BeginTabItem("Averages")) {
			if (m_AccumFrames > 0) {
				// 计算平均值
				float avgBatchAddTime = m_AccumBatchAddTime / m_AccumFrames;
				float avgBatchFlushTime = m_AccumBatchFlushTime / m_AccumFrames;
				float avgBatchGPUTime = m_AccumBatchGPUTime / m_AccumFrames;
				float avgBatchDrawCalls = static_cast<float>(m_AccumBatchFlushCount) / m_AccumFrames;
				float avgTextureSlotsUsed = m_AccumTextureSlotsUsed / m_AccumFrames;

				float avgTraditionalGenTime = m_AccumTraditionalGenTime / m_AccumFrames;
				float avgTraditionalUploadTime = m_AccumTraditionalUploadTime / m_AccumFrames;
				float avgTraditionalGPUTime = m_AccumTraditionalGPUTime / m_AccumFrames;
				float avgTraditionalDrawCalls = static_cast<float>(m_AccumTraditionalDrawCalls) / m_AccumFrames;

				float avgFrameTime = m_AccumFrameTime / m_AccumFrames;
				float avgFPS = 1000.0f / avgFrameTime;

				// 帧信息
				ImGui::Text("Frames: %d", m_AccumFrames);
				ImGui::Text("Avg Frame: %.1f FPS (%.2f ms)", avgFPS, avgFrameTime);

				// 批处理平均性能
				ImGui::Separator();
				ImGui::Text("Batch Rendering (Avg):");
				ImGui::Columns(2, "batch_avg_columns", true);
				ImGui::Text("Draw Calls: %.1f", avgBatchDrawCalls);
				ImGui::Text("Texture Slots: %.1f/%d", avgTextureSlotsUsed, MAX_TEXTURE_SLOTS);
				ImGui::NextColumn();
				ImGui::Text("Add Time: %.3f ms", avgBatchAddTime);
				ImGui::Text("Flush Time: %.3f ms", avgBatchFlushTime);
				ImGui::Text("GPU Time: %.3f ms", avgBatchGPUTime);
				ImGui::Columns(1);

				// 传统平均性能
				ImGui::Separator();
				ImGui::Text("Traditional Rendering (Avg):");
				ImGui::Columns(2, "trad_avg_columns", true);
				ImGui::Text("Draw Calls: %.1f", avgTraditionalDrawCalls);
				ImGui::NextColumn();
				ImGui::Text("Gen Time: %.3f ms", avgTraditionalGenTime);
				ImGui::Text("Upload Time: %.3f ms", avgTraditionalUploadTime);
				ImGui::Text("GPU Time: %.3f ms", avgTraditionalGPUTime);
				ImGui::Columns(1);

				// 平均性能对比
				ImGui::Separator();
				ImGui::Text("Performance Ratio (Avg):");
				float avgBatchTotal = avgBatchAddTime + avgBatchFlushTime + avgBatchGPUTime;
				float avgTradTotal = avgTraditionalGenTime + avgTraditionalUploadTime + avgTraditionalGPUTime;
				ImGui::Text("  Batch Total: %.3f ms", avgBatchTotal);
				ImGui::Text("  Traditional Total: %.3f ms", avgTradTotal);
				ImGui::Text("  Ratio: %.2fx", avgTradTotal / avgBatchTotal);
			}
			else {
				ImGui::Text("No data accumulated yet.");
			}

			ImGui::EndTabItem();
		}

		// 详细数据标签页
		if (ImGui::BeginTabItem("Detailed Metrics")) {
			// 当前帧详细数据
			ImGui::Text("Current Frame Detailed Metrics:");

			ImGui::Separator();
			ImGui::Text("Batch Rendering:");
			ImGui::Text("  Add Time: %.3f ms", m_BatchAddTime);
			ImGui::Text("  Flush Time: %.3f ms", m_BatchFlushTime);
			ImGui::Text("  GPU Time: %.3f ms", m_BatchGPUTime);
			ImGui::Text("  Batches: %d", m_BatchFlushCount);

			ImGui::Separator();
			ImGui::Text("Traditional Rendering:");
			ImGui::Text("  Gen Time: %.3f ms", m_TraditionalGenTime);
			ImGui::Text("  Upload Time: %.3f ms", m_TraditionalUploadTime);
			ImGui::Text("  GPU Time: %.3f ms", m_TraditionalGPUTime);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	// ======== 自动测试结果标签页 ========
	if (ImGui::BeginTabBar("PerformanceTabs")) {

		// 添加自动测试结果标签页
		if (ImGui::BeginTabItem("Auto Test Results")) {
			if (!m_AutoTestResults.empty()) {
				// 显示统计摘要
				ImGui::Text("Test Summary:");
				ImGui::Text("  Tested Sizes: %d", m_AutoTestResults.size());
				ImGui::Text("  Min Ratio: %.2fx",
					std::min_element(m_AutoTestResults.begin(), m_AutoTestResults.end(),
						[](const AutoTestResult& a, const AutoTestResult& b) {
							return a.Ratio < b.Ratio;
						})->Ratio);
				ImGui::Text("  Max Ratio: %.2fx",
					std::max_element(m_AutoTestResults.begin(), m_AutoTestResults.end(),
						[](const AutoTestResult& a, const AutoTestResult& b) {
							return a.Ratio < b.Ratio;
						})->Ratio);

				// 保存按钮
				if (ImGui::Button("Save Results to CSV")) {
					SaveTestResults();
				}
				ImGui::SameLine();
				ImGui::Text("Saved to perf_test_results.csv");

				// 显示数据表
				if (ImGui::TreeNode("Raw Data")) {
					if (ImGui::BeginTable("AutoTestResults", 12, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
						ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
						ImGui::TableSetupColumn("Size");
						ImGui::TableSetupColumn("Batch Add");
						ImGui::TableSetupColumn("Batch Flush");
						ImGui::TableSetupColumn("Batch GPU");
						ImGui::TableSetupColumn("Batch Total");
						ImGui::TableSetupColumn("Trad Gen");
						ImGui::TableSetupColumn("Trad Upload");
						ImGui::TableSetupColumn("Trad GPU");
						ImGui::TableSetupColumn("Trad Total");
						ImGui::TableSetupColumn("Ratio");
						ImGui::TableSetupColumn("Batch Calls");
						ImGui::TableSetupColumn("Trad Calls");
						ImGui::TableHeadersRow();

						for (const auto& result : m_AutoTestResults) {
							ImGui::TableNextRow();
							ImGui::TableNextColumn(); ImGui::Text("%d", result.GridSize);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.BatchAddTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.BatchFlushTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.BatchGPUTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.BatchTotalTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.TraditionalGenTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.TraditionalUploadTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.TraditionalGPUTime);
							ImGui::TableNextColumn(); ImGui::Text("%.3f", result.TraditionalTotalTime);
							ImGui::TableNextColumn(); ImGui::Text("%.2fx", result.Ratio);
							ImGui::TableNextColumn(); ImGui::Text("%.1f", result.BatchDrawCalls);
							ImGui::TableNextColumn(); ImGui::Text("%.1f", result.TraditionalDrawCalls);
						}

						ImGui::EndTable();
					}
					ImGui::TreePop();
				}
			}
			else {
				ImGui::Text("No auto test results available. Start auto test to collect data.");
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void perfTest::OnEvent(Bacon::Event& e)
{
	m_CameraController.OnEvent(e);

	// 处理窗口大小变化事件
	Bacon::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Bacon::WindowResizeEvent>([this](Bacon::WindowResizeEvent& event) {
		m_WindowWidth = event.GetWidth();
		m_WindowHeight = event.GetHeight();
		m_CameraController.OnResize(static_cast<float>(m_WindowWidth), static_cast<float>(m_WindowHeight));
		return false;
		});
}
