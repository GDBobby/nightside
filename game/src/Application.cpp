
#include "nightside/Application.hpp"
#include "nightside/core/Finder.hpp"
#include "nightside/shader/Shader.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>
#include <string>

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
	//Shader point_light{"point_light", "pioneer", finder};
	//Shader point_light_2{"point_light", "pioneer", finder};

	Palette palette{"pioneer", finder};
	LightShader lightShader{finder};

	PointLight pointLight{};
	pointLight.luminosity = 5.f;
	//pointLight.position; thisll be set when added, based on the mouse position
	pointLight.radius = 5.f;
	pointLight.attenuation_constant = 0.f; //need imgui to fine tune these
	pointLight.attenuation_linear = 1.f;
	pointLight.attenuation_quadratic = 0.f;

	SpotLight spotLight{};
	spotLight.key = 0;
	spotLight.size = 4;
	spotLight.vertices[0] = {0.0f, 0.0f};
	spotLight.vertices[1] = {1200.f, 0.0f};
	spotLight.vertices[2] = {0.0f, 1200.f};

	

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

		//printf("key val? : %d\n", sf::Keyboard::isKeyPressed(addPointLightKey.key));

		if (ImGui::Begin("point light controls")) { ImGui::Text("Light to be added");
			ImGui::Text("mouse pos - %.2f:%.2f", pos.x, pos.y);
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
					ImGui::DragFloat2((existingPLStr + existingPLExt).c_str(), reinterpret_cast<float*>(&lightShader.pointlightPosition[i]), 0.1f, -10000.f, 10000.f);
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
					ImGui::TreePop();
				}
			}

		}
		ImGui::End();

		if (addPointLightKey.Update()) {
			pointLight.position = pos;
			printf("adding point light\n");
			lightShader.AddPointLight(pointLight);
		}
		if (addSpotLightKey.Update()) {
			for (int i = 0; i < 3; i++) { 
				spotLight.vertices[i] += pos; 
			}
			printf("adding spot light\n");
			lightShader.AddSpotLight(spotLight);
		}


		//darken.update(window, clock, -5.f, {});
		//point_light_2.update(window, clock, 0.f, {200.f, 300.f});
		//point_light.update(window, clock, 0.f, pos);
		lightShader.Finalize();

		// render
		window.clear();

		sf::RectangleShape backdrop{};
		backdrop.setFillColor(sf::Color{16, 8, 28});
		backdrop.setSize(f_window_size_v);
		backdrop.setOrigin(f_window_size_v / 2.f);
		backdrop.setPosition(f_center_v);
		window.draw(backdrop);

		//darken.render(window, sf::Vector2f{0.f, 0.f});
		lightShader.Submit(window, palette, m_subject);
		//point_light_2.render(window, sf::Vector2f{0.f, 0.f});
		//point_light.render(window, sf::Vector2f{0.f, 0.f});

		ImGui::SFML::Render(window);
		window.display();
	}
	shutdown();
}

void Application::shutdown() { ImGui::SFML::Shutdown(); }

} // namespace nightside