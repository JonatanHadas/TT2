#include "maze.h"

#include "../../utils/utils.h"

Maze empty_maze(int w, int h){
	return Maze(
		vector<vector<bool>>(w, vector<bool>(h - 1, false)),
		vector<vector<bool>>(w - 1, vector<bool>(w, false))
	);
}

#include <iostream>

Maze expand_tree(int w, int h){
	vector<vector<bool>>
		hwalls(w, vector<bool>(h - 1, true)),
		vwalls(w - 1, vector<bool>(h, true)),
		visited(w, vector<bool>(h, false));
	
	int x = rand_range(0, w);
	int y = rand_range(0, h);
	visited[x][y] = true;
	unsigned int cnt = w*h - 1;
	while(cnt > 0){
		int d1 = rand_range(0,2);
		int d2 = rand_range(0,2);
		int dx = (d1 + d2) - 1, dy = (d1 - d2);
		
		int new_x = x + dx, new_y = y + dy;
		if(new_x < 0 || new_x >= w || new_y < 0 || new_y >= h) continue;
		
		if(!visited[new_x][new_y]){
			cnt--;
			if(dx) vwalls[dx < 0 ? new_x : x][y] = false;
			else hwalls[x][dy < 0 ? new_y: y] = false;
		}
		visited[new_x][new_y] = true;
		x = new_x; y = new_y;
	}
	
	int num = rand_range((w+h)/2, (w+h)*3/2);
	for(int i = 0; i < num; i++){
		if(rand_range(0, 2)){
			hwalls[rand_range(0, w)][rand_range(0, h - 1)] = false;
		} else {
			vwalls[rand_range(0, w - 1)][rand_range(0, h)] = false;
		}
	}
	
	return Maze(std::move(hwalls), std::move(vwalls));
}

Maze generate_maze(MazeGeneration algorithm, int w, int h){
	switch(algorithm){
	case MazeGeneration::EXPAND_TREE:
		return expand_tree(w, h);
	case MazeGeneration::NONE:
	default:
		return empty_maze(w, h);
	}
}
