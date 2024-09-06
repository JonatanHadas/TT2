#ifndef _KEYSET_H
#define _KEYSET_H

#include "controller.h"

#include <SDL.h>

#include <vector>
#include <iostream>

using namespace std;

struct KeySet{
	SDL_Scancode left, right, forward, back, shoot;
	
	void serialize(ostream& output) const;
	static KeySet deserialize(istream& input);
};

class KeyController : public Controller{
	KeyState state;
	KeySet key_set;
public:
	KeyController(const KeySet& key_set);
	
	void handle_event(const SDL_Event& event);
	KeyState get_state();
};

class KeySetManager{
	vector<KeySet> keysets;
	vector<int> ids;
	
	
	int next_id;
	
	const char* filename;
	
	void load();
	void save() const;
	
	int get_index(int id) const;
public:
	KeySetManager(const char* filename);
	
	const KeySet& get_keys(int id) const;
	void set_keys(int id, const KeySet& keys);
	
	int get_new();
	void remove(int id);
};

#endif
