#include "game.h"

#include "../../utils/utils.h"

#include "maze.h"

Game::Game(
	MazeGeneration maze_generation
) :
	maze_generation(maze_generation) {
		
	new_round();
}

void Game::new_round(){
	round = make_unique<Round>(*this, maze_generation);
};

const Maze& Game::get_maze() const{
	return round->get_maze();
}

Round::Round(Game& game, MazeGeneration maze_generation) :
	game(game),
	maze(generate_maze(maze_generation, rand_range(5, 12), rand_range(5, 12))) {

}

const Maze& Round::get_maze() const{
	return maze;
}
