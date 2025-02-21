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
void LightShader::AddSpotLight(sf::Vector2f position, sf::Vector2f direction, int luminosity, float radius, float att_c, float att_l, float att_q, float cutoff, float outerCutoff) {

	if (currentSpotLight >= (MAX_POINT_LIGHTS - 1)) {
		printf("already have maximum lights\n");
		return;
	}
	spotlightPosition.push_back(position);
	spotlightDirection.push_back(direction);
	spotlightLuminosity.push_back(luminosity);
	spotlightRadius.push_back(radius);
	spotlightAttenuation_constant.push_back(att_c);
	spotlightAttenuation_linear.push_back(att_l);
	spotlightAttenuation_quadratic.push_back(att_q);
	spotlight_cutoff.push_back(cutoff);
	spotlight_outerCutoff.push_back(outerCutoff);

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
	
	m_shader.setUniform("spotlight_count", currentSpotLight + 1);
	m_shader.setUniformArray("spotlight_position", spotlightPosition.data(), spotlightPosition.size());
	m_shader.setUniformArray("spotlight_direction", spotlightDirection.data(), spotlightDirection.size());
	m_shader.setUniformArray("spotlight_luminence", spotlightLuminosity.data(), spotlightLuminosity.size());
	m_shader.setUniformArray("spotlight_radius", spotlightRadius.data(), spotlightRadius.size());
	m_shader.setUniformArray("spotlight_attenuation_constant", spotlightAttenuation_constant.data(), spotlightAttenuation_constant.size());
	m_shader.setUniformArray("spotlight_attenuation_linear", spotlightAttenuation_linear.data(), spotlightAttenuation_linear.size());
	m_shader.setUniformArray("spotlight_attenuation_quadratic", spotlightAttenuation_quadratic.data(), spotlightAttenuation_quadratic.size());
	m_shader.setUniformArray("spotlight_cutoff", spotlight_cutoff.data(), spotlight_cutoff.size());
	m_shader.setUniformArray("spotlight_outerCutoff", spotlight_outerCutoff.data(), spotlight_outerCutoff.size());


}

void LightShader::Submit(sf::RenderWindow& win, Palette& palette, sf::Sprite const& sprite) {
	m_shader.setUniform("palette_size", static_cast<int>(palette.get_size()));
	m_shader.setUniform("palette", palette.get_texture().getTexture());
	m_shader.setUniform("texture", sprite.getTexture());

	sf::Sprite palette_sprite{palette.get_texture().getTexture()};

	win.draw(sprite, &m_shader);

}

void LightShader::ClearPointLights() {
	currentPointLight = 0;
	pointlightPosition.clear();
	pointlightLuminosity.clear();
	pointlightRadius.clear();
	pointlightAttenuation_constant.clear();
	pointlightAttenuation_linear.clear();
	pointlightAttenuation_quadratic.clear();
}

void LightShader::ClearSpotLights() {
	currentSpotLight = 0;
	spotlightPosition.clear();
	spotlightDirection.clear();
	spotlightLuminosity.clear();
	spotlightRadius.clear();
	spotlightAttenuation_constant.clear();
	spotlightAttenuation_linear.clear();
	spotlightAttenuation_quadratic.clear();
	spotlight_cutoff.clear();
	spotlight_outerCutoff.clear();
}

} // namespace nightside