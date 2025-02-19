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

void LightShader::SetTextures(Palette& palette, sf::Texture const& tex) {
	palette.set_position({400.f, 400.f});
	m_shader.setUniform("u_size", static_cast<float>(palette.get_size()));
	m_shader.setUniform("palette", palette.get_texture().getTexture());
	m_shader.setUniform("texture", tex); 
}

void LightShader::AddPointLight(float key, sf::Vector2f position, int luminosity, float radius) {
	pointLightKeys.push_back(key);
	pointlightPosition.push_back(position);
	pointlightLuminosity.push_back(luminosity);
	pointlightRadius.push_back(radius);


	currentPointLight++;
}
void LightShader::AddSpotLight(float key, sf::Vector2f vertex0, sf::Vector2f vertex1, sf::Vector2f vertex2) { 
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
	m_shader.setUniform("palette_size", static_cast<float>(palette.get_size()));
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

void LightShader::update(sf::RenderWindow& win, sf::Clock& clock, float key, sf::Vector2f position) {
	//m_palette.set_position({400.f, 400.f});
	m_shader.setUniform("u_size", static_cast<float>(m_palette.get_size()));
	m_shader.setUniform("u_key", static_cast<float>(key));
	m_shader.setUniform("palette", m_palette.get_texture().getTexture());
	m_shader.setUniform("texture", m_subject);
	m_shader.setUniform("u_radius", 32.f + 0.2f * sin(16.f * clock.getElapsedTime().asSeconds()));
	m_shader.setUniform("u_luminosity", 3.f);
	m_shader.setUniform("u_position", position);
	auto const num_verts{4};
	sf::Glsl::Vec2 verts[num_verts] = {{0.0f, 0.0f}, {1200.f, 0.0f}, {0.0f, 1200.f}, {1200.0f, 1200.f}};
	auto movement = sf::Vector2f{8.f * sin(clock.getElapsedTime().asSeconds()), 0.f};
	for (auto i{0}; i < num_verts; ++i) { m_shader.setUniform(std::string{"u_mask_" + std::to_string(i + 1)}, verts[i] + movement); }
}

void Shader::render(sf::RenderWindow& win, sf::Vector2f offset) {
	auto palette{sf::Sprite{m_palette.get_texture().getTexture()}};
	palette.setScale({64.f, 64.f});
	auto tile{sf::Sprite{m_subject}};
	// win.draw(palette);
	tile.setPosition(m_palette.get_position() - sf::Vector2f{tile.getLocalBounds().size} + offset);
	win.draw(tile, &m_shader);
}

} // namespace nightside