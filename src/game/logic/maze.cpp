#include "maze.h"

#include "../../utils/utils.h"

#include<deque>
#include<utility>

Maze empty_maze(int w, int h){
	return Maze(
		vector<vector<bool>>(w, vector<bool>(h - 1, false)),
		vector<vector<bool>>(w - 1, vector<bool>(w, false))
	);
}

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

const Direction NO_WAY = {
	.dx = 0,
	.dy = 0,
	.distance = -1
};

void bfs(const Maze& maze, int end_x, int end_y, vector<vector<Direction>>& results){
	deque<pair<int, int>> queue;
	queue.push_back({end_x, end_y});
	results[end_x][end_y].distance = 0;
	
	while(!queue.empty()){
		auto [x, y] = queue.front();
		queue.pop_front();
		
		if(!maze.has_hwall_below(x, y) && results[x][y + 1].distance == -1){
			results[x][y + 1].dy = -1;
			results[x][y + 1].distance = results[x][y].distance + 1;
			queue.push_back({x, y + 1});
		}
		if(!maze.has_hwall_below(x, y - 1) && results[x][y - 1].distance == -1){
			results[x][y - 1].dy = 1;
			results[x][y - 1].distance = results[x][y].distance + 1;
			queue.push_back({x, y - 1});
		}
		if(!maze.has_vwall_right(x, y) && results[x + 1][y].distance == -1){
			results[x + 1][y].dx = -1;
			results[x + 1][y].distance = results[x][y].distance + 1;
			queue.push_back({x + 1, y});
		}
		if(!maze.has_vwall_right(x - 1, y) && results[x - 1][y].distance == -1){
			results[x - 1][y].dx  = 1;
			results[x - 1][y].distance = results[x][y].distance + 1;
			queue.push_back({x - 1, y});
		}
	}
};

MazeMap::MazeMap(const Maze& maze) : directions(
		maze.get_w(),
		vector<vector<vector<Direction>>>(
			maze.get_h(),
			vector<vector<Direction>>(
				maze.get_w(),
				vector<Direction>(
					maze.get_h(),
					NO_WAY
				)
			)
		)
	) {

	for(int end_x = 0; end_x < maze.get_w(); end_x++){
		for(int end_y = 0; end_y < maze.get_h(); end_y++){
			bfs(maze, end_x, end_y, directions[end_x][end_y]);
		};
	}
}

const Direction& MazeMap::get_direction(
	int start_x, int start_y,
	int end_x, int end_y
) const{
	if(end_x >= 0 && end_x < directions.size()){
		const auto& end_row = directions[end_x];
		if(end_y >= 0 && end_y < end_row.size()){
			const auto& end = end_row[end_y];
			if(start_x >= 0 && start_x < end.size()){
				const auto& row = end[start_x];
				if(start_y >= 0 && start_y < row.size()){
					return row[start_y];
				}
			}
		}
	}
	return NO_WAY;
}
