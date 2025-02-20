#include "nightside/shader/LightShader.hpp"
#include "nightside/core/Finder.hpp"

#include <imgui.h>

#include <iostream>

//its important this exactly matches the value in shaders/light_combined.frag
#define MAX_POINT_LIGHTS 5

namespace nightside {

LightShader::LightShader(Finder& finder) {
	if (!sf::Shader::isAvailable()) { std::cout << "Shaders are not available.\n"; }
	auto vert {finder.get_resource_path() / fs::path{"shaders/point_light.vert"}};
	auto frag{finder.get_resource_path() / fs::path{"shaders/light_combined.frag"}};
	if (!m_shader.loadFromFile(vert, frag)) { 
		std::cout << "Failed to load shader " + frag.string() << ".\n"; 
	}
}
void LightShader::AddPointLight(float key, sf::Vector2f position, int luminosity, float radius) {
	if (currentPointLight >= (MAX_POINT_LIGHTS - 1)) { 
		printf("already have maximum lights\n");
		return;
	}

	pointLightKeys.push_back(key);
	pointlightPosition.push_back(position);
	pointlightLuminosity.push_back(luminosity);
	pointlightRadius.push_back(radius);


	currentPointLight++;
}
void LightShader::AddSpotLight(float key, sf::Vector2f vertex0, sf::Vector2f vertex1, sf::Vector2f vertex2) {
	if (currentSpotLight >= (MAX_POINT_LIGHTS - 1)) {
		printf("already have maximum lights\n");
		return;
	}
	spotlightKey.push_back(key);
	spotlight_vertex0.push_back(vertex0);
	spotlight_vertex1.push_back(vertex1);
	spotlight_vertex2.push_back(vertex2);

	currentSpotLight++;
}

void LightShader::Finalize() { 
	//m_shader.setUniform("pointlight_key", pointLightKeys.data()); 
	m_shader.setUniform("pointlight_count", currentPointLight + 1);
	m_shader.setUniformArray("pointlight_key", pointLightKeys.data(), pointLightKeys.size());
	m_shader.setUniformArray("pointlight_position", pointlightPosition.data(), pointlightPosition.size());

	//setUniform should be called as little as possible. SFML doesn't support int arrays. if the CPU performance is an issue, and GPU performance isnt, swap luminosity to a float (in both the C++ code and the GLSL code)
	for (int i = 0; i < currentPointLight; i++) { 
		m_shader.setUniform(("pointlight_luminosity[" + std::to_string(i) + "]").c_str(), pointlightLuminosity[i]); 
	}
	for (int i = currentPointLight; i < MAX_POINT_LIGHTS; i++) { 
		m_shader.setUniform(("pointlight_luminosity[" + std::to_string(i) + "]").c_str(), 0); 
	}

	m_shader.setUniformArray("pointlight_radius", pointlightRadius.data(), pointlightRadius.size());

	m_shader.setUniform("spotlight_count", currentSpotLight + 1);
	m_shader.setUniformArray("spotlight_key", spotlightKey.data(), spotlightKey.size());
	m_shader.setUniformArray("spotlight_vertex0", spotlight_vertex0.data(), spotlight_vertex0.size());
	m_shader.setUniformArray("spotlight_vertex1", spotlight_vertex1.data(), spotlight_vertex1.size());
	m_shader.setUniformArray("spotlight_vertex2", spotlight_vertex2.data(), spotlight_vertex2.size());
}

void LightShader::Submit(sf::RenderWindow& win, Palette& palette, sf::Texture const& tex, sf::Vector2f offset) {
	palette.set_position({400.f, 400.f});
	m_shader.setUniform("palette_size", static_cast<int>(palette.get_size()));
	m_shader.setUniform("palette", palette.get_texture().getTexture());
	m_shader.setUniform("texture", tex);

	sf::Sprite palette_sprite{palette.get_texture().getTexture()};
	palette_sprite.setScale({64.f, 64.f});

	sf::Sprite tile{tex};
	tile.setPosition(palette.get_position() - sf::Vector2f{tile.getLocalBounds().size} + offset);

	win.draw(tile, &m_shader);

}

void LightShader::ClearPointLights() {
	currentPointLight = 0;
	pointLightKeys.clear();
	pointlightPosition.clear();
	pointlightLuminosity.clear();
	pointlightRadius.clear();
}

void LightShader::ClearSpotLights() {
	currentSpotLight = 0;
	spotlightKey.clear();
	spotlight_vertex0.clear();
	spotlight_vertex1.clear();
	spotlight_vertex2.clear();
}

} // namespace nightside