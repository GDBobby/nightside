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
void LightShader::AddPointLight(sf::Vector2f position, int luminosity, float radius, float att_c, float att_l, float att_q){
	if (currentPointLight >= (MAX_POINT_LIGHTS - 1)) { 
		printf("already have maximum lights\n");
		return;
	}

	pointlightPosition.push_back(position);
	pointlightLuminosity.push_back(luminosity);
	pointlightRadius.push_back(radius);
	pointlightAttenuation_constant.push_back(att_c);
	pointlightAttenuation_linear.push_back(att_l);
	pointlightAttenuation_quadratic.push_back(att_q);



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
	m_shader.setUniformArray("pointlight_position", pointlightPosition.data(), pointlightPosition.size());

	m_shader.setUniformArray("pointlight_luminence", pointlightLuminosity.data(), pointlightLuminosity.size());

	m_shader.setUniformArray("pointlight_radius", pointlightRadius.data(), pointlightRadius.size());
	m_shader.setUniformArray("pointlight_attenuation_constant", pointlightAttenuation_constant.data(), pointlightAttenuation_constant.size());
	m_shader.setUniformArray("pointlight_attenuation_linear", pointlightAttenuation_linear.data(), pointlightAttenuation_linear.size());
	m_shader.setUniformArray("pointlight_attenuation_quadratic", pointlightAttenuation_quadratic.data(), pointlightAttenuation_quadratic.size());

	//m_shader.setUniform("spotlight_count", currentSpotLight + 1);
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