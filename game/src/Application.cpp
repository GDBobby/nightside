
#include "nightside/Application.hpp"
#include "nightside/core/Finder.hpp"
#include "nightside/shader/Shader.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>
#include <string>
#include <cmath>

bool KeyState::Update() {
	if (sf::Keyboard::isKeyPressed(key)) {
		bool const justPressed = state == KeyState::Up;
		state = KeyState::Down;
		return justPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
	} else {
		state = KeyState::Up;
	}
	return false;
}

namespace nightside {

Application::Application() {
	window.setFramerateLimit(144);
	auto dimensions{sf::Vector2u{1280, 720}};
	auto mode{sf::VideoMode(dimensions)};
	auto title{"Nightside Projects"};
	window.create(mode, title, sf::Style::Default & ~sf::Style::Resize);
	if (!ImGui::SFML::Init(window)) { std::cout << "Failed to initialize window.\n"; }
}

void DrawPointLightControl(sf::Glsl::Vec2 pos, LightShader& lightShader, PointLight& pointLight) {

	if (ImGui::Begin("point light controls")) {
		ImGui::Text("Light to be added");
		ImGui::Text("mouse pos - %.2f:%.2f", pos.x, pos.y);
		if (ImGui::Button("clear point lights")) { lightShader.ClearPointLights(); }
		ImGui::Separator();

		ImGui::DragFloat("radius##p", &pointLight.radius, 0.1f, 0.1f, 1000.f);
		ImGui::DragFloat("luminence##p", &pointLight.luminosity, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("attenuation constant##p", &pointLight.attenuation_constant, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("attenuation linear##p", &pointLight.attenuation_linear, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("attenuation quadratic##p", &pointLight.attenuation_quadratic, 0.1f, 0.f, 100.f);

		std::string existingPLStr = "existing point light";
		std::string existingPLExt = "##p";
		ImGui::Separator();
		ImGui::Text("current point light count %d", lightShader.currentPointLight);
		for (int i = 0; i < lightShader.currentPointLight; i++) {
			existingPLStr = "PL ";
			existingPLStr += std::to_string(i);
			existingPLExt = "##p";
			if (ImGui::TreeNode((existingPLStr + existingPLExt).c_str())) {
				existingPLExt += std::to_string(i);

				existingPLStr = "position";
				ImGui::DragFloat2((existingPLStr + existingPLExt).c_str(), reinterpret_cast<float*>(&lightShader.pointlightPosition[i]), 1.f, -10000.f, 10000.f);
				existingPLStr = "radius";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightRadius[i], 0.1f, 0.1f, 1000.f);
				existingPLStr = "luminence";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightLuminosity[i], 0.1f, 0.f, 100.f);

				existingPLStr = "attenuation constant";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightAttenuation_constant[i], 0.1f, 0.f, 100.f);
				existingPLStr = "attenuation linear";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightAttenuation_linear[i], 0.1f, 0.f, 100.f);
				existingPLStr = "attenuation quadratic";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightAttenuation_quadratic[i], 0.1f, 0.f, 100.f);
				existingPLStr = "distance scaling";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightDistanceScaling[i], 0.1f, 0.f, 100.f);
				existingPLStr = "distance flat";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.pointlightDistanceFlat[i], 0.1f, 0.f, 100.f);
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

float CartesianToPolar(sf::Vector2f direction) { return std::atan2(direction.y, direction.x); }
sf::Vector2f PolarToCartesian(float theta) {
	return { std::cos(theta), std::sin(theta) };
}

void DrawSpotLightControl(sf::Glsl::Vec2 pos, LightShader& lightShader, SpotLight& spotlight) {
	if (ImGui::Begin("spot light controls")) {
		ImGui::Text("Light to be added");
		ImGui::Text("mouse pos - %.2f:%.2f", pos.x, pos.y);

		if (ImGui::Button("clear spot lights")) { lightShader.ClearSpotLights();
		}
		ImGui::Separator();

		float basePolar = CartesianToPolar(spotlight.direction);
		ImGui::DragFloat("direction##s", &basePolar, 0.1f, -10.f, 10.f);
		spotlight.direction = PolarToCartesian(basePolar);
		ImGui::DragFloat("radius##s", &spotlight.radius, 0.1f, 0.1f, 1000.f);
		ImGui::DragFloat("luminence##s", &spotlight.luminosity, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("attenuation constant##s", &spotlight.attenuation_constant, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("attenuation linear##s", &spotlight.attenuation_linear, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("attenuation quadratic##s", &spotlight.attenuation_quadratic, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("cutoff", &spotlight.cutoffAngle, 0.1f, 0.1f, 1000.f);
		ImGui::DragFloat("outer cutoff", &spotlight.outerCutoffAngle, 0.1f, 0.1f, 1000.f);

		std::string existingPLStr = "existing spot light";
		std::string existingPLExt = "##s";
		ImGui::Separator();
		ImGui::Text("current spot light count %d", lightShader.currentSpotLight);
		for (int i = 0; i < lightShader.currentSpotLight; i++) {
			existingPLStr = "SL ";
			existingPLStr += std::to_string(i);
			existingPLExt = "##s";
			if (ImGui::TreeNode((existingPLStr + existingPLExt).c_str())) {
				existingPLExt += std::to_string(i);

				existingPLStr = "position";
				ImGui::DragFloat2((existingPLStr + existingPLExt).c_str(), reinterpret_cast<float*>(&lightShader.spotlightPosition[i]), 1.f, -10000.f, 10000.f);
				existingPLStr = "direction";

				float localPolar = CartesianToPolar(lightShader.spotlightDirection[i]);
				ImGui::Text("direction debuggging : %.2f:%.2f", lightShader.spotlightDirection[i].x, lightShader.spotlightDirection[i].y);
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &localPolar, 0.1f, -10.f, 10.f);
				lightShader.spotlightDirection[i] = PolarToCartesian(localPolar);
				existingPLStr = "radius";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightRadius[i], 0.1f, 0.1f, 1000.f);
				existingPLStr = "luminence";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightLuminosity[i], 0.1f, 0.f, 100.f);

				existingPLStr = "attenuation constant";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightAttenuation_constant[i], 0.1f, 0.f, 100.f);
				existingPLStr = "attenuation linear";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightAttenuation_linear[i], 0.1f, 0.f, 100.f);
				existingPLStr = "attenuation quadratic";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightAttenuation_quadratic[i], 0.1f, 0.f, 100.f);

				
				existingPLStr = "cutoff";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlight_cutoff[i], 0.1f, 0.1f, 1000.f);
				existingPLStr = "outer cutoff";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlight_outerCutoff[i], 0.1f, 0.1f, 1000.f);
				existingPLStr = "distance scaling";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightDistanceScaling[i], 0.1f, 0.f, 100.f);
				existingPLStr = "distance flat";
				ImGui::DragFloat((existingPLStr + existingPLExt).c_str(), &lightShader.spotlightDistanceFlat[i], 0.1f, 0.f, 100.f);

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}


void Application::run() {
	constexpr static float size_v{64.f};
	constexpr static sf::Vector2<float> dimensions_v{size_v, size_v};
	static auto f_window_size_v{sf::Vector2f{window.getSize()}};
	static auto f_center_v{sf::Vector2f{window.getSize() / 2u}};
	static auto clock{sf::Clock()};
	clock.start();

	// game variables
	Finder finder{};
	//Shader darken{"lighten", "pioneer", finder};
	sf::Texture m_subject;
	if (!m_subject.loadFromFile(finder.get_resource_path() / fs::path{"images/pioneer.png"})) { std::cout << "Failed to load image.\n"; }

	sf::Sprite subject_sprite0{m_subject};
	subject_sprite0.setPosition(sf::Vector2f(100.f, 200.f));

	sf::Sprite subject_sprite1{m_subject};
	subject_sprite0.setPosition(sf::Vector2f(400.f, 200.f));

	Palette palette{"pioneer", finder};
	LightShader lightShader{finder};

	PointLight pointLight{};
	pointLight.luminosity = 1.f;
	//pointLight.position; thisll be set when added, based on the mouse position
	pointLight.radius = 100.f;
	pointLight.attenuation_constant = 0.f;
	pointLight.attenuation_linear = 1.f;
	pointLight.attenuation_quadratic = 0.f;
	pointLight.distanceScaling = 1.f;

	SpotLight spotLight{};
	spotLight.luminosity = 1.f;
	spotLight.radius = 100.f;
	spotLight.attenuation_constant = 0.f;
	spotLight.attenuation_linear = 1.f;
	spotLight.attenuation_quadratic = 0.f;
	spotLight.cutoffAngle = 3.14f / 2.f; //90 degrees
	spotLight.outerCutoffAngle = 3.14f / 1.8f; //slightly more than 90 degrees
	spotLight.distanceScaling = 1.f;

	

	KeyState addPointLightKey{};
	KeyState addSpotLightKey{};
	addPointLightKey.key = sf::Keyboard::Key::P;
	addSpotLightKey.key = sf::Keyboard::Key::S;

	while (window.isOpen()) {
		while (std::optional const event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) { window.close(); }
			ImGui::SFML::ProcessEvent(window, *event);
		}

		// update
		ImGui::SFML::Update(window, clock.getElapsedTime());
		auto& io = ImGui::GetIO();
		auto pos = sf::Glsl::Vec2{io.MousePos.x, window.getSize().y - io.MousePos.y};

		if (ImGui::Begin("sprite control")) { 
			
			sf::Vector2f pos0 = subject_sprite0.getPosition();
			ImGui::DragFloat2("sprite0 pos", reinterpret_cast<float*>(&pos0), 1.f, 0.f, 10000.f);
			subject_sprite0.setPosition(pos0);
			sf::Vector2f pos1 = subject_sprite1.getPosition();
			ImGui::DragFloat2("sprite1 pos", reinterpret_cast<float*>(&pos1), 1.f, 0.f, 10000.f);
			subject_sprite1.setPosition(pos1);
		}
		ImGui::End();

		DrawPointLightControl(pos, lightShader, pointLight);
		DrawSpotLightControl(pos, lightShader, spotLight);


		bool isInsideWindow = (pos.x >= 0 && pos.x < window.getSize().x && pos.y >= 0 && pos.y < window.getSize().y);
		if (addPointLightKey.Update()) {
			pointLight.position = pos;
			if (window.hasFocus() && isInsideWindow) {
				printf("adding point light\n");
				lightShader.AddPointLight(pointLight);
			}
		}
		if (addSpotLightKey.Update()) {
			spotLight.position = pos; 
			if (window.hasFocus() && isInsideWindow) {
				printf("adding spot light\n");
				lightShader.AddSpotLight(spotLight);
			}
		}


		// render
		window.clear();

		sf::RectangleShape backdrop{};
		backdrop.setFillColor(sf::Color{16, 8, 28});
		backdrop.setSize(f_window_size_v);
		backdrop.setOrigin(f_window_size_v / 2.f);
		backdrop.setPosition(f_center_v);
		window.draw(backdrop);

		lightShader.Finalize();
		lightShader.Submit(window, palette, subject_sprite0);
		lightShader.Submit(window, palette, subject_sprite1);

		ImGui::SFML::Render(window);
		window.display();
	}
	shutdown();
}

void Application::shutdown() { ImGui::SFML::Shutdown(); }

} // namespace nightside