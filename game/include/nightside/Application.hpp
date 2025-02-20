
#pragma once

#include "shader/LightShader.hpp"

#include <SFML/Graphics.hpp>

struct KeyState {
	enum State {
		Up,
		Down,
	};
	State state{Up};
	sf::Keyboard::Key key;

	bool Update();
};

namespace nightside {

class Application {
  public:
	Application();
	void run();
	void shutdown();

  private:
	sf::RenderWindow window{};
};

} // namespace nightside