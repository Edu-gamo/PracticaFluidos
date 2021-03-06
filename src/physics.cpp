#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

void PhysicsInit();

vector<vec3> waveDir;
vector<float> waveAmp;
vector<float> waveFrec;

vec3 force = vec3(0.0f, -9.8f, 0.0f);
vec3 spherePos, spherePostPos, sphereVel, spherePostVel;
float sphereMass;

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
		ImGui::Begin("");
		if (ImGui::Button("Resetear Simulacion")) { PhysicsInit(); }
		ImGui::End();
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void AddWave(vec3 dir, float ampli, float frec) {
	waveDir.push_back(dir);
	waveAmp.push_back(ampli);
	waveFrec.push_back(frec);
}

void RemoveWaves() {
	waveDir.clear();
	waveAmp.clear();
	waveFrec.clear();
}

void PhysicsInit() {
	//TODO

	time = 0.0f;

	spherePos = vec3(0.0f, 9.0f, 0.0f);
	sphereVel = vec3(0.0f);
	sphereMass = 1.0f;

	//Reset vectors
	RemoveWaves();

	//A�adir Holas
	AddWave(vec3(1.5f, 0.0f, 1.0f), 0.2f, 5.0f);
	AddWave(vec3(1.0f, 0.0f, 1.5f), 0.1f, 2.0f);
	AddWave(vec3(-1.0f, 0.0f, 1.5f), 0.15f, 1.0f);

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

	//Calcular posicion esfera
	spherePostPos.x = spherePos.x + dt * sphereVel.x;
	spherePostPos.y = spherePos.y + dt * sphereVel.y;
	spherePostPos.z = spherePos.z + dt * sphereVel.z;

	float sphereBottom = (spherePos.y - 1.0f);
	float Vsub = 3.0f - sphereBottom;
	if (Vsub > 8.0f) Vsub = 8.0f;
	float buoyancy = 1.0f * -force.y * Vsub * 1.0f;

	if (sphereBottom < 3.0f) {
		//Calcular velocidad esfera
		spherePostVel.x = sphereVel.x + dt * (force.x / sphereMass);
		spherePostVel.y = sphereVel.y + dt * (buoyancy + force.y / sphereMass);
		spherePostVel.z = sphereVel.z + dt * (force.z / sphereMass);
	} else {
		//Calcular velocidad esfera
		spherePostVel.x = sphereVel.x + dt * (force.x / sphereMass);
		spherePostVel.y = sphereVel.y + dt * (force.y / sphereMass);
		spherePostVel.z = sphereVel.z + dt * (force.z / sphereMass);
	}

	//Waves movement
	for (int i = 0; i < maxPart; i++) {
		vec3 sumaHola;
		float sumaHolaY = 0.0f;
		for (int j = 0; j < waveDir.size(); j++) {
			sumaHola += (normalize(waveDir[j]))*waveAmp[j] * sin(dot(waveDir[j], particles[i].initPos) - waveFrec[j] * time);
			sumaHolaY += waveAmp[j] * cos(dot(waveDir[j], particles[i].initPos) - waveFrec[j] *time);
		}
		particles[i].position = particles[i].initPos - sumaHola;
		particles[i].position.y = sumaHolaY + 3.0f;
	}



	//Simple waves movement
	/*for (int i = 0; i < maxPart; i++) {
		particles[i].position = particles[i].initPos - (normalize(k))*amplitud*sin(dot(k, particles[i].initPos) - frec*time);
		particles[i].position.y = amplitud*cos(dot(k, particles[i].initPos) - frec*time);
		particles[i].position.y += 5.0f;
	}*/

	spherePos = spherePostPos;
	sphereVel = spherePostVel;

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
	RemoveWaves();
	delete[] particles;
}