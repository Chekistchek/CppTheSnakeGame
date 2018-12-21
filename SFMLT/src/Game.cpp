#include <SFML/Graphics.hpp>
#include <windows.h>
#include <iostream> // test
#include <algorithm>
#include <memory>
#include <string>
#include "Game.h"
#include "Snake.h"
#include "Fruit.h"
#include "Wall.h"

namespace Bezhok {
	using std::vector;
	using std::ios_base;
	using std::make_shared;
	using std::ifstream;
	using std::string;
	using std::to_string;
	using std::getline;

	Game::Game()
	{
		// add smart pointers
		m_data = make_shared<Game_Data>();
		m_data->wall = make_shared<Wall>(m_data);
		m_data->snake = make_shared<Snake>(m_data);
		m_data->fruit = make_shared<Fruit>(m_data);

		//m_data->objects.push_back(make_shared<Wall>());

		m_data->window.create(sf::VideoMode(WIDTH, HEIGTH), "Snake");
	
		fstream stats_file;
		stats_file.open(m_fname, ios_base::binary | ios_base::in);
		if (stats_file.is_open()) {
			stats_file.read((char*)m_stats, sizeof(m_stats));
			stats_file.close();
		}
	}

	Game::~Game()
	{
		fstream stats_file;
		stats_file.open(m_fname, ios_base::binary | ios_base::out | ios_base::trunc);
		if (stats_file.is_open()) {
			stats_file.write((char*)m_stats, sizeof(m_stats));
			stats_file.close();
		}
		else {
			//error
		}
	}

	void Game::handle()
	{
		sf::Event e;
		while (m_data->window.pollEvent(e))
		{
			GameObject::direction dir = m_data->snake->get_direction();

			switch (e.type)
			{
				// closed window
			case sf::Event::Closed:
				m_data->window.close();
				break;

				// pressed key
			case sf::Event::KeyPressed:
				switch (e.key.code)
				{
				case sf::Keyboard::A:
				case sf::Keyboard::Left:
					//can not move forward or back
					if (dir == GameObject::direction::top || dir == GameObject::direction::bottom)
						m_data->snake->set_direction(GameObject::direction::left);
					break;

				case sf::Keyboard::D:
				case sf::Keyboard::Right:
					if (dir == GameObject::direction::top || dir == GameObject::direction::bottom)
						m_data->snake->set_direction(GameObject::direction::right);
					break;

				case sf::Keyboard::S:
				case sf::Keyboard::Down:
					if (dir == GameObject::direction::left || dir == GameObject::direction::right)
						m_data->snake->set_direction(GameObject::direction::bottom);
					break;

				case sf::Keyboard::W:
				case sf::Keyboard::Up:
					if (dir == GameObject::direction::left || dir == GameObject::direction::right)
						m_data->snake->set_direction(GameObject::direction::top);
					break;

				case sf::Keyboard::Space:
					e.key.code = sf::Keyboard::Unknown;
					m_pause = m_pause ? false : true;
					break;
				default:
					break;
				}

				break;

			default:
				break;
			}
		}
	}

	void Game::add2stats(int points)
	{
		// shift array
		for (int i = 0; i < sizeof(m_stats) / sizeof(int); ++i) {
			if (points > m_stats[i]) {
				int temp = m_stats[i];

				m_stats[i] = m_data->snake->m_points;

				int prev = temp;
				int curr;
				for (int j = i + 1; j < sizeof(m_stats) / sizeof(int); ++j) {
					curr = m_stats[j];
					m_stats[j] = prev;
					prev = curr;
				}
				break;
			}
		}
		//
	}

	void Game::init_map(const string& fname)
	{
		vector<Wall::block> temp;

		// read level's map
		ifstream file(fname.c_str());
		if (file.is_open()) {
			string line;
			for (int y = 0; y < Game::BLOCKS_COUNT_Y && getline(file, line); ++y) {
				for (int i = 0, x = 0; x < Game::BLOCKS_COUNT_X && i < line.size(); ++x, ++i) {
					if (line[i] == '0') {
						temp.push_back(Wall::block{ x,y });
					}
					else if (line[i] == '\t') {
						x += 3;
					}
				}
			}
			file.close();
		}
		else {
			//error
		}
		m_data->wall->init(temp);
	}

	void Game::run()
	{
		string name = "levels/level2.txt"; // for example
		init_map(name);
		m_pause = true;

		while (m_data->window.isOpen())
		{
			handle();

			// delay todo
			Sleep(100);

			// logic starts
			bool life = true;
			if (!m_pause) {
				life = m_data->snake->update();
			}

			// new frame start
			m_data->window.clear(sf::Color::White);
			m_data->snake->draw();
			m_data->fruit->draw();
			m_data->wall->draw();

			if (m_pause) {
			// menu
			}
			m_data->window.display();
			
			if (!life) {
				add2stats(m_data->snake->m_points);
				m_data->snake->reset();
				m_data->fruit->reset();

				// output stats
				for (int& x : m_stats) {
					std::cout << x << std::endl;
				}
				std::cout << "\n\n";

				m_pause = true;
			}
		}
	}
}