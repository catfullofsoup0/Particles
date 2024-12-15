#include "Engine.h"
//hi
Engine::Engine() {
    m_Window.create(VideoMode::getDesktopMode(), "Particle Fireworks");
}

void Engine::run() {
    Clock clock;
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete. Starting engine..." << endl;

    while (m_Window.isOpen()) {
        Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();

        input();    
        update(dtAsSeconds);
        draw();
    }
}

void Engine::input() {
    Event event;
    while (m_Window.pollEvent(event)) {
        if (event.type == Event::Closed ||
            (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
            m_Window.close();
        }

        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            for (int i = 0; i < 5; ++i) {
                int numPoints = rand() % 26 + 25; // Random points [25:50]
                Vector2i position = Mouse::getPosition(m_Window);
                m_particles.emplace_back(m_Window, numPoints, position);
            }
        }
    }
}

void Engine::update(float dtAsSeconds) {
    // Iterate through m_particles and update each one
    for (auto it = m_particles.begin(); it != m_particles.end(); ) {
        // If the particle's TTL is still valid, update it
        if (it->getTTL() > 0.0) {
            it->update(dtAsSeconds);
            ++it; // Move to the next particle
        }
        else {
            // If TTL is expired, remove the particle and update the iterator
            it = m_particles.erase(it);
        }
    }
}


void Engine::draw() {
    m_Window.clear();
    for (const auto& particle : m_particles) {
        m_Window.draw(particle);
    }
    m_Window.display();
}
