#pragma once

#include "nightside/shader/Palette.hpp"

#include <SFML/Graphics.hpp>

namespace nightside {

	struct PointLight {
		float key;
		sf::Vector2f position;
		int luminosity;
		float radius;
		//sf::Vector2f offset; //i dont think this is used in the shaders
	};
	struct SpotLight {
		float key;
		float size;
		sf::Vector2f vertices[3]; //could potentially replace this with a TriangleStruct
		//sf::Vector2f offset; //i dont think this is used in the shaders
	};

	struct ImprovedSpotLight {
		float key; //what is key? needs a better name
		float radius;
		float angle; //how wide the spot light is
		float intensity;
		float cutoffDistance;
		//you could potentially do a color shift and a palette shift.
		sf::Vector2f direction;
		sf::Vector3f colorShift;

	};

	class Finder;
	class LightShader {
	  public:
		LightShader(Finder& finder);

		void ClearPointLights();
		void ClearSpotLights();

		void AddPointLight(float key, sf::Vector2f position, int luminosity, float radius);
		void AddPointLight(PointLight pointlight) { AddPointLight(pointlight.key, pointlight.position, pointlight.luminosity, pointlight.radius); }
		void AddSpotLight(float key, sf::Vector2f vertex0, sf::Vector2f vertex1, sf::Vector2f vertex2);
		void AddSpotLight(SpotLight spotlight) { AddSpotLight(spotlight.key, spotlight.vertices[0], spotlight.vertices[1], spotlight.vertices[2]); }
		

		void Finalize();
		void Submit(sf::RenderWindow& win, Palette& palette, sf::Texture const& tex, sf::Vector2f offset = sf::Vector2f{0.f, 0.f});

	  private:
		//if youd like some additional safety, add a boolean, create a BeginShader function, set the boolean to true, and turn it off at the end of submit
		//you could potentially use a wrapper object as well, that calls Begin, AddLight or whatever on construction, then finalize and submit on deconstruction

		int currentPointLight = 0;
		int currentSpotLight = 0;

		std::vector<float> pointLightKeys{};
		std::vector<sf::Vector2f> pointlightPosition{};
		std::vector<int> pointlightLuminosity{};
		std::vector<float> pointlightRadius{};

		std::vector<float> spotlightKey{};
		std::vector<sf::Vector2f> spotlight_vertex0{};
		std::vector<sf::Vector2f> spotlight_vertex1{};
		std::vector<sf::Vector2f> spotlight_vertex2{};

		sf::Shader m_shader{};
	};

} // namespace nightside