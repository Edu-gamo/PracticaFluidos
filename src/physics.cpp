#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

vector<vec3> waveDir;
vector<float> waveAmp;
vector<float> waveFrec;

float time = 0.0f;

struct particle {
	vec3 position;
	vec3 initPos;

	int row, col;

	float mass = 1.0f;
};

int maxPart = 14 * 18;
particle *particles = new particle[14 * 18];
float partDist = 0.5f;

namespace ClothMesh {
	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}

namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

bool show_test_window = false;
void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	//TODO

	time = 0.0f;

	//Wave 1
	waveDir.push_back(vec3(1.5f, 0.0f, 1.0f));
	waveAmp.push_back(0.2f);
	waveFrec.push_back(5.0f);

	//Wave 2
	waveDir.push_back(vec3(1.0f, 0.0f, 1.5f));
	waveAmp.push_back(0.1f);
	waveFrec.push_back(2.0f);

	//Wave 3
	waveDir.push_back(vec3(-1.0f, 0.0f, 1.5f));
	waveAmp.push_back(0.3f);
	waveFrec.push_back(1.0f);

	float initX = -3.0;
	float initZ = -4.5;
	int part = 0;
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			particles[part].initPos = vec3(initX + partDist*j, 0.0f, initZ + partDist*i);
			particles[part].position = particles[part].initPos;
			particles[part].row = j;
			particles[part].col = i;
			part++;
		}
	}
}
void PhysicsUpdate(float dt) {
	//TODO

	time += dt;

	for (int i = 0; i < maxPart; i++) {
		vec3 sumaHola;
		float sumaHolaY = 0.0f;
		for (int j = 0; j < waveDir.size(); j++) {
			sumaHola += (normalize(waveDir[j]))*waveAmp[j] * sin(dot(waveDir[j], particles[i].initPos) - waveFrec[j] * time);
			sumaHolaY += waveAmp[j] * cos(dot(waveDir[j], particles[i].initPos) - waveFrec[j] *time);
		}
		particles[i].position = particles[i].initPos - sumaHola;
		particles[i].position.y = sumaHolaY;
		particles[i].position.y += 5.0f;
	}

	/*for (int i = 0; i < maxPart; i++) {
		particles[i].position = particles[i].initPos - (normalize(k))*amplitud*sin(dot(k, particles[i].initPos) - frec*time);
		particles[i].position.y = amplitud*cos(dot(k, particles[i].initPos) - frec*time);
		particles[i].position.y += 5.0f;
	}*/

	float *partVerts = new float[(14 * 18) * 3];
	for (int i = 0; i < (14 * 18); ++i) {
		partVerts[i * 3 + 0] = particles[i].position.x;
		partVerts[i * 3 + 1] = particles[i].position.y;
		partVerts[i * 3 + 2] = particles[i].position.z;
	}
	LilSpheres::updateParticles(0, (14 * 18), partVerts);
	ClothMesh::updateClothMesh(partVerts);
	delete[] partVerts;
}
void PhysicsCleanup() {
	//TODO
}