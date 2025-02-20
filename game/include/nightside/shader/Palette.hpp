
#pragma once

#include <SFML/Graphics.hpp>

#include <vector>
#include <string_view>

namespace nightside {

constexpr static auto f_swatch_dimensions_v{sf::Vector2f{1.f, 1.f}};

class Finder;

class Palette {
  public:
	Palette(std::string_view source, Finder& finder, sf::Vector2f dimensions = {f_swatch_dimensions_v});
	void render(sf::RenderWindow& win);
	void set_position(sf::Vector2f to_position);
	sf::RenderTexture& const get_texture();
	[[nodiscard]] auto get_position() const -> sf::Vector2f { return m_position; }
	[[nodiscard]] auto get_dimensions() const -> sf::Vector2f { return m_dimensions; }
	[[nodiscard]] auto get_size() const -> std::size_t{ return m_swatches.size(); }

  private:
	std::vector<sf::Color> m_swatches{};
	sf::RenderTexture m_texture{};
	sf::Vector2f m_dimensions{};
	sf::Vector2f m_position{};
};

} // namespace nightside